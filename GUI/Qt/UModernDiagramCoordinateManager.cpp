#include "UModernDiagramCoordinateManager.h"
#include "UEngineSelectionSync.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramCacheManager.h" // For UModernDiagramComponentCacheEntry

#include <sstream>
#include <QDateTime>
#include "../../Deploy/Include/rdk_init.h"
#include "../../Deploy/Include/rdk_cpp_init.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UXMLEnvSerialize.h"
#include "UGuiTelemetry.h"

UModernDiagramCoordinateManager::UModernDiagramCoordinateManager(UModernDiagramWidget* owner)
    : m_owner(owner)
{
}

QPointF UModernDiagramCoordinateManager::scenePosFromKernel(const QPointF& kernel) const
{
    QPointF result = kernel * m_coordScale;
    // Логирование для отладки преобразования координат (только при необходимости, чтобы не было спама)
    // Раскомментируйте при необходимости:
    // QString logMsg = QString("[UModernDiagramCoordinateManager::scenePosFromKernel] kernel=(%1, %2) -> scene=(%3, %4), scale=%5")
    //     .arg(kernel.x()).arg(kernel.y()).arg(result.x()).arg(result.y()).arg(m_coordScale);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    return result;
}

QPointF UModernDiagramCoordinateManager::kernelPosFromScene(const QPointF& scene) const
{
    QPointF result = scene / m_coordScale;
    // Логирование для отладки преобразования координат (только при необходимости, чтобы не было спама)
    // Раскомментируйте при необходимости:
    // QString logMsg = QString("[UModernDiagramCoordinateManager::kernelPosFromScene] scene=(%1, %2) -> kernel=(%3, %4), scale=%5")
    //     .arg(scene.x()).arg(scene.y()).arg(result.x()).arg(result.y()).arg(m_coordScale);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    return result;
}

bool UModernDiagramCoordinateManager::loadCoord(const QString& fullName, QPointF& outPos) const
{
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!model)
        return false;

    std::string coordBuf;
    if(!propertyValueFromModelScope(model.Get(), fullName, QByteArrayLiteral("Coord"), coordBuf)
       || coordBuf.empty())
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[UModernDiagramCoordinateManager::loadCoord] Coordinates not found for '%1'")
        //     .arg(fullName);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return false;
    }
    // fallback: если пришла строка "x y z" без XML
    {
        std::istringstream iss(coordBuf);
        double x,y,z;
        if(iss >> x >> y >> z)
        {
            QPointF kernel(x,y);
            outPos = kernel;
            return true;
        }
    }
    RDK::USerStorageXML xml;
    bool ok = xml.Load(coordBuf, "Coord");
    if(!ok)
    {
        // DEBUG: Commented out to reduce log flood
        // QString logMsg = QString("[UModernDiagramCoordinateManager::loadCoord] Error loading XML coordinates for '%1'")
        //     .arg(fullName);
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return false;
    }
    RDK::MVector<double,3> pos;
    xml >> pos;
    QPointF kernel(pos[0], pos[1]);
    outPos = kernel; // возвращаем ядровые координаты, сцену вычисляем выше

    // DEBUG: Commented out to reduce log flood - Logging for debugging coordinate loading
    // QString logMsg = QString("[UModernDiagramCoordinateManager::loadCoord] Loaded coordinates for '%1' (XML): kernel=(%2, %3)")
    //     .arg(fullName)
    //     .arg(kernel.x()).arg(kernel.y());
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    return true;
}

void UModernDiagramCoordinateManager::saveCoord(const QString& fullName, const QPointF& scenePos) const
{
    QPointF kernelPos = kernelPosFromScene(scenePos);
    // Сохраняем реальные координаты, включая отрицательные
    // Не обрезаем отрицательные координаты, так как это приводит к потере информации о позиции
    // При загрузке m_normalizationOffset будет вычислен на основе минимальных координат

    // DEBUG: Commented out to reduce log flood - Logging for debugging coordinate saving
    // QString logMsg = QString("[UModernDiagramCoordinateManager::saveCoord] Saving coordinates for '%1': scenePos=(%2, %3), kernelPos=(%4, %5), m_coordScale=%6")
    //     .arg(fullName)
    //     .arg(scenePos.x()).arg(scenePos.y())
    //     .arg(kernelPos.x()).arg(kernelPos.y())
    //     .arg(m_coordScale);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);

    RDK::USerStorageXML xml;
    xml.Create("Coord");
    RDK::MVector<double,3> posVec;
    posVec[0] = kernelPos.x();
    posVec[1] = kernelPos.y();
    posVec[2] = 0.0;
    xml << posVec;
    std::string buffer;
    xml.Save(buffer);

    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(model)
        setPropertyValueFromModelScope(model.Get(), fullName, QByteArrayLiteral("Coord"), buffer);

    // Update cache with new coordinates so buildScene() uses fresh data on next load
    // m_componentCache is mutable, so we can modify it in this const method
    if(m_owner)
    {
            UModernDiagramComponentCacheEntry* cacheEntry = m_owner->m_cacheManager->getComponentCache().getEntry(fullName);
        if(cacheEntry)
        {
            cacheEntry->kernelPos = kernelPos;
            cacheEntry->hasKernelPos = true;
            cacheEntry->timestamp = QDateTime::currentMSecsSinceEpoch();
        }
        else
        {
            // If entry doesn't exist, create it with new coordinates
            UModernDiagramComponentCacheEntry newEntry;
            newEntry.kernelPos = kernelPos;
            newEntry.hasKernelPos = true;
            newEntry.timestamp = QDateTime::currentMSecsSinceEpoch();
                    m_owner->m_cacheManager->getComponentCache().setEntry(fullName, newEntry);
        }
    }
}

QPointF UModernDiagramCoordinateManager::currentMinScenePos() const
{
    if(!m_owner)
        return QPointF(0, 0);

    QPointF min(0,0);
    bool first=true;
    for(auto* n : m_owner->m_nodes)
    {
        QPointF p = n->scenePos();
        if(first)
        {
            min = p;
            first=false;
        }
        else
        {
            if(p.x() < min.x()) min.setX(p.x());
            if(p.y() < min.y()) min.setY(p.y());
        }
    }
    if(first) return QPointF(0,0);
    return min;
}

void UModernDiagramCoordinateManager::recalculateNormalizationOffset(const QPointF& pendingComponentPos, const QString& pendingComponentName)
{
    if(!m_owner)
        return;

    // Recalculate m_normalizationOffset based on kernel coordinates from the engine
    // This ensures that offset corresponds to saved coordinates, not current scene positions
    // which may be normalized and cause infinite loops
    QPointF oldOffset = m_normalizationOffset;
    QPointF minKernel(0, 0);
    bool minSet = false;
    bool coordsLoaded = false;
    int nodeCount = 0;

    // Collect all kernel coordinates from the engine (not from scene positions)
    // This avoids the infinite loop when components are moved
    for(auto* node : m_owner->m_nodes)
    {
        if(!node)
            continue;

        nodeCount++;
        QString fullName = m_owner->m_componentName.isEmpty() ? node->nodeName
                                                     : m_owner->m_componentName + "." + node->nodeName;

        // Skip the component that is being moved - we'll use pendingComponentPos for it
        if(!pendingComponentName.isEmpty() && fullName == pendingComponentName)
            continue;

        // Load kernel coordinates from the engine
        QPointF kernelPos;
        bool loaded = loadCoord(fullName, kernelPos);

        if(loaded)
        {
            coordsLoaded = true;
            if(!minSet)
            {
                minKernel = kernelPos;
                minSet = true;
            }
            else
            {
                if(kernelPos.x() < minKernel.x()) minKernel.setX(kernelPos.x());
                if(kernelPos.y() < minKernel.y()) minKernel.setY(kernelPos.y());
            }
        }
    }

    // If there's a component that is being moved, use its pending position
    if(!pendingComponentPos.isNull() && !pendingComponentName.isEmpty())
    {
        // Convert absolute scene position to kernel coordinates
        QPointF pendingKernelPos = kernelPosFromScene(pendingComponentPos);
        if(!minSet)
        {
            minKernel = pendingKernelPos;
            minSet = true;
            coordsLoaded = true;
        }
        else
        {
            if(pendingKernelPos.x() < minKernel.x()) minKernel.setX(pendingKernelPos.x());
            if(pendingKernelPos.y() < minKernel.y()) minKernel.setY(pendingKernelPos.y());
        }
    }

    // Calculate normalization offset
    QPointF minScenePos = coordsLoaded ? scenePosFromKernel(minKernel) : QPointF(0, 0);
    m_normalizationOffset = minScenePos;

    // DEBUG: Commented out to reduce log flood - Logging for debugging offset recalculation
    // QString logMsg = QString("[UModernDiagramCoordinateManager::recalculateNormalizationOffset] Recalculating offset: nodeCount=%1, oldOffset=(%2, %3), minKernel=(%4, %5), minScenePos=(%6, %7), newOffset=(%8, %9), m_coordScale=%10, pendingComponent='%11'")
    //     .arg(nodeCount)
    //     .arg(oldOffset.x()).arg(oldOffset.y())
    //     .arg(minKernel.x()).arg(minKernel.y())
    //     .arg(minScenePos.x()).arg(minScenePos.y())
    //     .arg(m_normalizationOffset.x()).arg(m_normalizationOffset.y())
    //     .arg(m_coordScale)
    //     .arg(pendingComponentName.isEmpty() ? "none" : pendingComponentName);
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
}

void UModernDiagramCoordinateManager::updateNormalizationOffsetForMovement(const QPointF& newMinNormalizedPos, const QSet<UModernDiagramNodeItem*>& componentsToAdjust)
{
    if(!m_owner)
        return;

    // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Полностью блокируем обновление offset во время движения
    // Никакие параметры канвы не должны меняться с момента клика до окончания перетаскивания
    if(m_owner->m_isComponentMoving)
    {
        return; // Блокируем обновление во время движения
    }

    // Only update if the new minimum is less than current (component moved left/up)
    // This prevents components from being saved with negative kernel coordinates
    if(newMinNormalizedPos.x() >= 0 && newMinNormalizedPos.y() >= 0)
    {
        return; // No update needed
    }

    // Calculate the change in offset
    // If newMinNormalizedPos is negative, we need to adjust the offset to make it 0
    QPointF oldOffset = m_normalizationOffset;
    QPointF deltaOffset;

    // Calculate how much we need to adjust the offset
    // If normalizedPos is -10, we need to add 10 to the offset to make normalizedPos become 0
    if(newMinNormalizedPos.x() < 0)
    {
        deltaOffset.setX(-newMinNormalizedPos.x());
    }
    if(newMinNormalizedPos.y() < 0)
    {
        deltaOffset.setY(-newMinNormalizedPos.y());
    }

    // Update the offset
    QPointF newOffset = oldOffset + deltaOffset;
    m_normalizationOffset = newOffset;

    // CRITICAL FIX: Clear m_componentsWithNegativePos BEFORE updating positions
    // to prevent components from being re-added during offset update
    // This prevents cascade updates where itemChange() adds components back to the set
    m_owner->m_componentsWithNegativePos.clear();

    // Set flag to prevent recursive calls to updateNormalizationOffsetForMovement
    // when we adjust positions of all components
    m_owner->m_isUpdatingNormalizationOffset = true;

    // CRITICAL FIX: Adjust ALL components to maintain absolute scene coordinates
    // When offset increases by deltaOffset, normalizedPos must decrease by deltaOffset
    // for ALL components to keep absoluteScenePos constant: absoluteScenePos = normalizedPos + offset
    // This ensures that all components maintain their visual positions on screen
    for(UModernDiagramNodeItem* node : m_owner->m_nodes)
    {
        if(node)
        {
            QPointF currentNormalizedPos = node->pos();
            QPointF adjustedPos;

            // Calculate absolute position BEFORE offset update
            QPointF absolutePosBeforeUpdate = currentNormalizedPos + oldOffset;

            if(componentsToAdjust.contains(node))
            {
                // This component was moved into negative territory
                // Get its original absolute position (before being moved to negative)
                QPointF originalAbsolutePos = m_owner->m_originalAbsolutePositions.value(node, currentNormalizedPos + oldOffset);

                // Calculate new normalized position to maintain original absolute position
                // absolutePos = normalizedPos + offset
                // normalizedPos = absolutePos - offset
                adjustedPos = originalAbsolutePos - newOffset;

                // Clamp to 0 to prevent negative positions
                adjustedPos.setX(qMax(0.0, adjustedPos.x()));
                adjustedPos.setY(qMax(0.0, adjustedPos.y()));

                // DEBUG: Commented out to reduce log flood
                // QString logMsg = QString("[UModernDiagramCoordinateManager::updateNormalizationOffsetForMovement] Component '%1' restored: originalAbsolute=(%2, %3), newNormalized=(%4, %5), newOffset=(%6, %7), finalAbsolute=(%8, %9)")
                //     .arg(node->nodeName)
                //     .arg(originalAbsolutePos.x()).arg(originalAbsolutePos.y())
                //     .arg(adjustedPos.x()).arg(adjustedPos.y())
                //     .arg(newOffset.x()).arg(newOffset.y())
                //     .arg(adjustedPos.x() + newOffset.x()).arg(adjustedPos.y() + newOffset.y());
                // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            }
            else
            {
                // This component was NOT moved into negative territory
                // Subtract deltaOffset from normalized positions to maintain absolute scene coordinates
                double newX = currentNormalizedPos.x() - deltaOffset.x();
                double newY = currentNormalizedPos.y() - deltaOffset.y();

                // CRITICAL: Clamp to 0 to prevent negative positions
                // This ensures that after offset update, no component has negative normalized position
                adjustedPos.setX(qMax(0.0, newX));
                adjustedPos.setY(qMax(0.0, newY));

                // DEBUG: Commented out to reduce log flood - Log if a component would have become negative (or was adjusted)
                // if(newX < 0 || newY < 0)
                // {
                //     QString logMsg = QString("[UModernDiagramCoordinateManager::updateNormalizationOffsetForMovement] Component '%1' adjusted from (%2, %3) to (%4, %5) to maintain absolute coordinates")
                //         .arg(node->nodeName)
                //         .arg(currentNormalizedPos.x()).arg(currentNormalizedPos.y())
                //         .arg(adjustedPos.x()).arg(adjustedPos.y());
                //     MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
                // }
            }

            // CRITICAL: Set flag before setPos to prevent itemChange from adding components
            // to m_componentsWithNegativePos during offset update
            // The flag is already set at the beginning of the function, but we ensure it's still set
            m_owner->m_isUpdatingNormalizationOffset = true;
            node->setPos(adjustedPos);
            m_owner->m_lastNodePositions[node] = adjustedPos;
        }
    }

    // Clear flag after adjusting all positions
    m_owner->m_isUpdatingNormalizationOffset = false;

    // Update sceneRect after offset update to reflect new component positions
    m_owner->updateSceneRect();

    // DEBUG: Commented out to reduce log flood - Logging for debugging offset update
    // QString logMsg = QString("[UModernDiagramCoordinateManager::updateNormalizationOffsetForMovement] Updated offset: newMinNormalizedPos=(%1, %2), oldOffset=(%3, %4), deltaOffset=(%5, %6), newOffset=(%7, %8)")
    //     .arg(newMinNormalizedPos.x()).arg(newMinNormalizedPos.y())
    //     .arg(oldOffset.x()).arg(oldOffset.y())
    //     .arg(deltaOffset.x()).arg(deltaOffset.y())
    //     .arg(m_normalizationOffset.x()).arg(m_normalizationOffset.y());
    // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
}

