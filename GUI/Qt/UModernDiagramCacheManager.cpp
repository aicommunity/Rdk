#include "UModernDiagramCacheManager.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramLinkItem.h"
#include "UModernDiagramScene.h"
#include "UModernDiagramView.h"
#include "UModernDiagramCoordinateManager.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDataStream>
#include <QTimer>
#include <QCryptographicHash>
#include <QDateTime>
#include "../../Deploy/Include/rdk_init.h"
#include "../Core/Engine/UEngine.h"
#include "UEngineSelectionSync.h"
#include "UGuiTelemetry.h"

// --------------------------- ComponentCache implementation ---------------------------

UModernDiagramComponentCacheEntry* UModernDiagramComponentCache::getEntry(const QString& componentFullName)
{
    if(m_cache.contains(componentFullName))
        return &m_cache[componentFullName];
    return nullptr;
}

void UModernDiagramComponentCache::setEntry(const QString& componentFullName, const UModernDiagramComponentCacheEntry& entry)
{
    m_cache[componentFullName] = entry;
}

bool UModernDiagramComponentCache::hasEntry(const QString& componentFullName) const
{
    return m_cache.contains(componentFullName);
}

void UModernDiagramComponentCache::invalidateEntry(const QString& componentFullName)
{
    m_cache.remove(componentFullName);
}

void UModernDiagramComponentCache::clear()
{
    m_cache.clear();
}

// --------------------------- CacheManager implementation ---------------------------

UModernDiagramCacheManager::UModernDiagramCacheManager(UModernDiagramWidget* owner)
    : m_owner(owner)
{
}

void UModernDiagramCacheManager::saveSceneToCache(const QString& componentName)
{
    if(!m_owner || componentName.isEmpty() || m_owner->m_nodes.isEmpty())
        return;

    // ВАЖНО: сохраняем кэш только если элементы еще в сцене (не были удалены)
    // Проверяем, что хотя бы один узел еще в сцене
    bool hasValidNodes = false;
    for(auto* node : m_owner->m_nodes)
    {
        if(node && node->scene() == m_owner->m_scene)
        {
            hasValidNodes = true;
            break;
        }
    }
    if(!hasValidNodes)
        return;  // Элементы уже удалены, не сохраняем в кэш

    UModernDiagramSceneCache cache;
    cache.nodes = m_owner->m_nodes;
    cache.links = m_owner->m_links;
    cache.nodeByName = m_owner->m_nodeByName;
    cache.lastNodePositions = m_owner->m_lastNodePositions;
    cache.normalizationOffset = m_owner->m_coordinateManager->getNormalizationOffset();

    cache.componentNames =
        childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), componentName);

    cache.isValid = true;
    m_levelCache[componentName] = cache;
}

void UModernDiagramCacheManager::restoreSceneFromCache(const QString& componentName)
{
    if(!m_owner || componentName.isEmpty() || !m_levelCache.contains(componentName))
        return;

    const UModernDiagramSceneCache& cache = m_levelCache[componentName];
    if(!cache.isValid)
        return;

    // Оптимизация: отключаем обновления во время восстановления
    m_owner->setUpdatesEnabled(false);
    if(m_owner->m_mainView)
        m_owner->m_mainView->setUpdatesEnabled(false);

    // Восстанавливаем данные
    m_owner->m_nodes = cache.nodes;
    m_owner->m_links = cache.links;
    m_owner->m_nodeByName = cache.nodeByName;
    m_owner->m_lastNodePositions = cache.lastNodePositions;
    m_owner->m_coordinateManager->setNormalizationOffset(cache.normalizationOffset);

    // Добавляем узлы и связи обратно в сцену
    // ВАЖНО: элементы из кэша должны быть валидными, так как мы не вызываем clearScene()
    // при восстановлении из кэша. Но на всякий случай проверяем валидность через scene()
    for(auto* node : m_owner->m_nodes)
    {
        if(node)
        {
            // Проверяем валидность указателя через проверку scene()
            // Если scene() возвращает nullptr, элемент был удален, пропускаем его
            QGraphicsScene* nodeScene = nullptr;
            try {
                nodeScene = node->scene();
            } catch (...) {
                // Указатель невалидный, пропускаем этот узел
                continue;
            }

            if(!nodeScene)
            {
                m_owner->m_scene->addItem(node);
            }
        }
    }

    for(auto* link : m_owner->m_links)
    {
        if(link)
        {
            // Проверяем валидность указателя
            QGraphicsScene* linkScene = nullptr;
            try {
                linkScene = link->scene();
            } catch (...) {
                // Указатель невалидный, пропускаем эту связь
                continue;
            }

            if(!linkScene)
            {
                m_owner->m_scene->addItem(link);
                // Восстанавливаем кэш связей для узлов
                UModernDiagramNodeItem* srcNode = link->getSourceNode();
                UModernDiagramNodeItem* dstNode = link->getDestinationNode();
                if(srcNode && m_owner->m_nodes.contains(srcNode))
                    srcNode->m_connectedLinks.append(link);
                if(dstNode && m_owner->m_nodes.contains(dstNode))
                    dstNode->m_connectedLinks.append(link);
            }
        }
    }

    // Включаем обновления обратно
    m_owner->setUpdatesEnabled(true);
    if(m_owner->m_mainView)
        m_owner->m_mainView->setUpdatesEnabled(true);
}

void UModernDiagramCacheManager::invalidateLevelCache(const QString& componentName)
{
    if(componentName.isEmpty())
    {
        // Инвалидируем все кэши
        m_levelCache.clear();
    }
    else
    {
        // Инвалидируем кэш конкретного уровня
        m_levelCache.remove(componentName);
    }
}

bool UModernDiagramCacheManager::hasLevelCache(const QString& componentName) const
{
    if(componentName.isEmpty())
        return false;
    return m_levelCache.contains(componentName) && m_levelCache[componentName].isValid;
}

const UModernDiagramSceneCache& UModernDiagramCacheManager::getLevelCache(const QString& componentName) const
{
    static UModernDiagramSceneCache emptyCache;
    if(componentName.isEmpty())
        return emptyCache;
    auto it = m_levelCache.constFind(componentName);
    if(it == m_levelCache.constEnd())
        return emptyCache;
    return it.value();
}

QString UModernDiagramCacheManager::computeComponentHash(const QString& componentFullName) const
{
    if(!m_owner || !m_owner->m_application)
        return QString();

    QCryptographicHash hash(QCryptographicHash::Sha256);

    // Добавляем имя компонента
    hash.addData(componentFullName.toUtf8());

    const int channel = Core_GetSelectedChannelIndex();
    const QStringList childComponents =
        childComponentShortNamesFromModelScope(channel, componentFullName);
    hash.addData(childComponents.join(QLatin1Char(',')).toUtf8());

    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(channel);
    if(model)
    {
        const QString inputProps =
            propertiesLookupListFromModelScope(model.Get(), componentFullName, ptPubInput | ptInput);
        if(!inputProps.isEmpty())
            hash.addData(inputProps.toUtf8());

        const QString outputProps =
            propertiesLookupListFromModelScope(model.Get(), componentFullName, ptPubOutput | ptOutput);
        if(!outputProps.isEmpty())
            hash.addData(outputProps.toUtf8());
    }

    // Добавляем список связей
    const char* xmlRaw = Model_GetComponentInternalLinks(componentFullName.toStdString().c_str(), nullptr);
    if(xmlRaw)
    {
        const int len = static_cast<int>(strlen(xmlRaw));
        hash.addData(xmlRaw, len);
        Engine_FreeBufString(xmlRaw);
    }

    return QString::fromLatin1(hash.result().toHex());
}

void UModernDiagramCacheManager::invalidateComponentCache(const QString& componentFullName)
{
    if(componentFullName.isEmpty())
    {
        // Инвалидируем весь кэш
        m_componentCache.clear();
    }
    else
    {
        // Инвалидируем кэш конкретного компонента
        m_componentCache.invalidateEntry(componentFullName);
    }
}

void UModernDiagramCacheManager::clearComponentCache()
{
    m_componentCache.clear();

    // Также удаляем файлы кэша
    QString jsonPath = getCacheFilePath("json");
    QString binPath = getCacheFilePath("bin");

    if(QFile::exists(jsonPath))
        QFile::remove(jsonPath);
    if(QFile::exists(binPath))
        QFile::remove(binPath);
}

QString UModernDiagramCacheManager::getCacheFilePath(const QString& extension) const
{
    if(!m_owner || !m_owner->m_application)
        return QString();

    QString cacheDirPath;

    // Пытаемся использовать путь к текущему открытому проекту
    QString projectPath = QString::fromLocal8Bit(m_owner->m_application->GetProjectPath().c_str());
    if(!projectPath.isEmpty())
    {
        // Используем папку текущего проекта
        QDir projectDir(projectPath);
        cacheDirPath = projectDir.absoluteFilePath(".cache");
    }
    else
    {
        // Fallback: используем старую логику, если проект не открыт
        QString configsPath = QString::fromLocal8Bit(m_owner->m_application->GetConfigsMainPath().c_str());
        QString workDir = QString::fromLocal8Bit(m_owner->m_application->GetWorkDirectory().c_str());

        // Определяем полный путь к папке конфигураций
        QString fullConfigsPath;
        if(QDir::isAbsolutePath(configsPath))
        {
            fullConfigsPath = configsPath;
        }
        else
        {
            // Относительный путь - относительно рабочей директории
            QDir workDirObj(workDir);
            fullConfigsPath = workDirObj.absoluteFilePath(configsPath);
        }

        // Получаем имя проекта из имени файла приложения или используем "default"
        QString projectName = "default";
        QString appFileName = QString::fromLocal8Bit(m_owner->m_application->GetApplicationFileName().c_str());
        if(!appFileName.isEmpty())
        {
            QFileInfo fileInfo(appFileName);
            projectName = fileInfo.baseName();
            if(projectName.isEmpty())
                projectName = "default";
        }

        // Создаем путь к папке кэша: {ConfigsMainPath}/{ProjectName}/.cache/
        QDir configsDir(fullConfigsPath);
        cacheDirPath = configsDir.absoluteFilePath(projectName + "/.cache");
    }

    // Создаем папку кэша, если она не существует
    QDir cacheDir(cacheDirPath);
    if(!cacheDir.exists())
    {
        cacheDir.mkpath(".");
    }

    // Возвращаем путь к файлу кэша
    return cacheDir.absoluteFilePath("component_cache." + extension);
}

bool UModernDiagramCacheManager::saveComponentCacheToFile(const QString& filePath, bool useBinary) const
{
    if(useBinary)
    {
        // Бинарный формат
        QFile file(filePath);
        if(!file.open(QIODevice::WriteOnly))
            return false;

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_15);

        // Заголовок: магическое число и версия
        const quint32 MAGIC = 0x4E4D5344; // "NMSD" (NeuroModeler SDK)
        const quint32 VERSION = 1;
        stream << MAGIC << VERSION;

        const QHash<QString, UModernDiagramComponentCacheEntry>& entries = m_componentCache.getAllEntries();
        stream << static_cast<quint32>(entries.size());

        for(auto it = entries.begin(); it != entries.end(); ++it)
        {
            const QString& componentName = it.key();
            const UModernDiagramComponentCacheEntry& entry = it.value();

            // Имя компонента
            stream << componentName;

            // Метаданные
            stream << entry.timestamp;
            stream << entry.hash;
            stream << entry.className;
            stream << entry.hasKernelPos;
            if(entry.hasKernelPos)
            {
                stream << entry.kernelPos.x() << entry.kernelPos.y();
            }

            // Вспомогательная функция для сериализации портов
            auto serializePorts = [&stream](const QVector<UModernDiagramPort>& ports)
            {
                stream << static_cast<quint32>(ports.size());
                for(const UModernDiagramPort& port : ports)
                {
                    stream << port.pos.x() << port.pos.y();
                    stream << port.isInput;
                    stream << port.name;
                    stream << port.fullPath;
                    stream << port.componentName;
                    stream << port.displayName;
                    stream << static_cast<quint32>(port.category);
                }
            };

            // Сериализуем все массивы портов
            serializePorts(entry.ownInputPorts);
            serializePorts(entry.childInputPorts);
            serializePorts(entry.aliasInputPorts);
            serializePorts(entry.ownOutputPorts);
            serializePorts(entry.childOutputPorts);
            serializePorts(entry.aliasOutputPorts);

            // Сериализуем portCategoryCache
            stream << static_cast<quint32>(entry.portCategoryCache.size());
            for(auto cacheIt = entry.portCategoryCache.begin(); cacheIt != entry.portCategoryCache.end(); ++cacheIt)
            {
                stream << cacheIt.key().first;  // propertyName
                stream << cacheIt.key().second; // isInput
                stream << static_cast<quint32>(cacheIt.value()); // category
            }
        }

        file.close();
        return true;
    }

    // JSON формат
    QJsonObject root;
    root["version"] = 1;

    QJsonObject components;
    const QHash<QString, UModernDiagramComponentCacheEntry>& entries = m_componentCache.getAllEntries();
    for(auto it = entries.begin(); it != entries.end(); ++it)
    {
        const QString& componentName = it.key();
        const UModernDiagramComponentCacheEntry& entry = it.value();

        QJsonObject componentObj;
        componentObj["timestamp"] = entry.timestamp;
        componentObj["hash"] = entry.hash;

        // Сохраняем данные, используемые в buildScene
        componentObj["className"] = entry.className;
        componentObj["hasKernelPos"] = entry.hasKernelPos;
        if(entry.hasKernelPos)
        {
            QJsonObject kernelPosObj;
            kernelPosObj["x"] = entry.kernelPos.x();
            kernelPosObj["y"] = entry.kernelPos.y();
            componentObj["kernelPos"] = kernelPosObj;
        }

        QJsonObject portsObj;

        // Сериализуем порты
        QJsonArray ownInputArray;
        for(const UModernDiagramPort& port : entry.ownInputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            ownInputArray.append(portObj);
        }
        portsObj["ownInput"] = ownInputArray;

        QJsonArray childInputArray;
        for(const UModernDiagramPort& port : entry.childInputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            childInputArray.append(portObj);
        }
        portsObj["childInput"] = childInputArray;

        QJsonArray aliasInputArray;
        for(const UModernDiagramPort& port : entry.aliasInputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            aliasInputArray.append(portObj);
        }
        portsObj["aliasInput"] = aliasInputArray;

        QJsonArray ownOutputArray;
        for(const UModernDiagramPort& port : entry.ownOutputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            ownOutputArray.append(portObj);
        }
        portsObj["ownOutput"] = ownOutputArray;

        QJsonArray childOutputArray;
        for(const UModernDiagramPort& port : entry.childOutputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            childOutputArray.append(portObj);
        }
        portsObj["childOutput"] = childOutputArray;

        QJsonArray aliasOutputArray;
        for(const UModernDiagramPort& port : entry.aliasOutputPorts)
        {
            QJsonObject portObj;
            portObj["name"] = port.name;
            portObj["fullPath"] = port.fullPath;
            portObj["componentName"] = port.componentName;
            portObj["displayName"] = port.displayName;
            portObj["isInput"] = port.isInput;
            portObj["category"] = static_cast<int>(port.category);
            aliasOutputArray.append(portObj);
        }
        portsObj["aliasOutput"] = aliasOutputArray;

        componentObj["ports"] = portsObj;
        components[componentName] = componentObj;
    }

    root["components"] = components;

    QJsonDocument doc(root);
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(doc.toJson());
    file.close();

    return true;
}

bool UModernDiagramCacheManager::loadComponentCacheFromFile(const QString& filePath, bool useBinary)
{
    if(useBinary)
    {
        // Бинарный формат
        QFile file(filePath);
        if(!file.exists() || !file.open(QIODevice::ReadOnly))
            return false;

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_5_15);

        // Проверяем заголовок
        quint32 magic, version;
        stream >> magic >> version;
        if(magic != 0x4E4D5344 || version != 1) // "NMSD" и версия 1
        {
            file.close();
            return false;
        }

        quint32 componentCount;
        stream >> componentCount;

        QHash<QString, UModernDiagramComponentCacheEntry> entries;

        for(quint32 i = 0; i < componentCount; ++i)
        {
            UModernDiagramComponentCacheEntry entry;
            QString componentName;
            stream >> componentName;

            // Метаданные
            stream >> entry.timestamp;
            stream >> entry.hash;
            stream >> entry.className;
            stream >> entry.hasKernelPos;
            if(entry.hasKernelPos)
            {
                double x, y;
                stream >> x >> y;
                entry.kernelPos = QPointF(x, y);
            }

            // Вспомогательная функция для десериализации портов
            auto deserializePorts = [&stream](QVector<UModernDiagramPort>& ports)
            {
                quint32 count;
                stream >> count;
                ports.reserve(count);
                for(quint32 j = 0; j < count; ++j)
                {
                    UModernDiagramPort port;
                    double x, y;
                    stream >> x >> y;
                    port.pos = QPointF(x, y);
                    stream >> port.isInput;
                    stream >> port.name;
                    stream >> port.fullPath;
                    stream >> port.componentName;
                    stream >> port.displayName;
                    quint32 category;
                    stream >> category;
                    port.category = static_cast<UModernDiagramPortCategory>(category);
                    ports.append(port);
                }
            };

            // Десериализуем все массивы портов
            deserializePorts(entry.ownInputPorts);
            deserializePorts(entry.childInputPorts);
            deserializePorts(entry.aliasInputPorts);
            deserializePorts(entry.ownOutputPorts);
            deserializePorts(entry.childOutputPorts);
            deserializePorts(entry.aliasOutputPorts);

            // Десериализуем portCategoryCache
            quint32 cacheSize;
            stream >> cacheSize;
            for(quint32 j = 0; j < cacheSize; ++j)
            {
                QString propertyName;
                bool isInput;
                quint32 category;
                stream >> propertyName >> isInput >> category;
                entry.portCategoryCache[QPair<QString, bool>(propertyName, isInput)] = static_cast<UModernDiagramPortCategory>(category);
            }

            entries[componentName] = entry;
        }

        if(stream.status() != QDataStream::Ok)
        {
            file.close();
            QString logMsg = QString("[UModernDiagramCacheManager] Failed to load component cache from binary file: %1 (stream error)").arg(filePath);
            MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            return false;
        }

        m_componentCache.setAllEntries(entries);
        QString logMsg = QString("[UModernDiagramCacheManager] Component cache loaded from binary file: %1 (%2 entries)")
            .arg(filePath).arg(entries.size());
        MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
        return true;
    }

    // JSON формат
    QFile file(filePath);
    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray rawData = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &error);
    if(error.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject root = doc.object();
    int version = root["version"].toInt();
    if(version != 1)
        return false;  // Неподдерживаемая версия

    QJsonObject components = root["components"].toObject();
    QHash<QString, UModernDiagramComponentCacheEntry> entries;

    for(auto it = components.begin(); it != components.end(); ++it)
    {
        const QString& componentName = it.key();
        QJsonObject componentObj = it.value().toObject();

        UModernDiagramComponentCacheEntry entry;
        entry.timestamp = componentObj["timestamp"].toVariant().toLongLong();
        entry.hash = componentObj["hash"].toString();

        // Восстанавливаем данные, используемые в buildScene
        entry.className = componentObj["className"].toString();
        entry.hasKernelPos = componentObj["hasKernelPos"].toBool(false);
        if(entry.hasKernelPos && componentObj.contains("kernelPos"))
        {
            QJsonObject kernelPosObj = componentObj["kernelPos"].toObject();
            entry.kernelPos.setX(kernelPosObj["x"].toDouble());
            entry.kernelPos.setY(kernelPosObj["y"].toDouble());
        }

        QJsonObject portsObj = componentObj["ports"].toObject();

        // Десериализуем порты
        QJsonArray ownInputArray = portsObj["ownInput"].toArray();
        for(const QJsonValue& val : ownInputArray)
        {
            QJsonObject portObj = val.toObject();
            UModernDiagramPort port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<UModernDiagramPortCategory>(portObj["category"].toInt());
            entry.ownInputPorts.append(port);
        }

        QJsonArray childInputArray = portsObj["childInput"].toArray();
        for(const QJsonValue& val : childInputArray)
        {
            QJsonObject portObj = val.toObject();
            UModernDiagramPort port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<UModernDiagramPortCategory>(portObj["category"].toInt());
            entry.childInputPorts.append(port);
        }

        QJsonArray aliasInputArray = portsObj["aliasInput"].toArray();
        for(const QJsonValue& val : aliasInputArray)
        {
            QJsonObject portObj = val.toObject();
            UModernDiagramPort port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<UModernDiagramPortCategory>(portObj["category"].toInt());
            entry.aliasInputPorts.append(port);
        }

        QJsonArray ownOutputArray = portsObj["ownOutput"].toArray();
        for(const QJsonValue& val : ownOutputArray)
        {
            QJsonObject portObj = val.toObject();
            UModernDiagramPort port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<UModernDiagramPortCategory>(portObj["category"].toInt());
            entry.ownOutputPorts.append(port);
        }

        QJsonArray childOutputArray = portsObj["childOutput"].toArray();
        for(const QJsonValue& val : childOutputArray)
        {
            QJsonObject portObj = val.toObject();
            UModernDiagramPort port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<UModernDiagramPortCategory>(portObj["category"].toInt());
            entry.childOutputPorts.append(port);
        }

        QJsonArray aliasOutputArray = portsObj["aliasOutput"].toArray();
        for(const QJsonValue& val : aliasOutputArray)
        {
            QJsonObject portObj = val.toObject();
            UModernDiagramPort port;
            port.name = portObj["name"].toString();
            port.fullPath = portObj["fullPath"].toString();
            port.componentName = portObj["componentName"].toString();
            port.displayName = portObj["displayName"].toString();
            port.isInput = portObj["isInput"].toBool();
            port.category = static_cast<UModernDiagramPortCategory>(portObj["category"].toInt());
            entry.aliasOutputPorts.append(port);
        }

        entries[componentName] = entry;
    }

    m_componentCache.setAllEntries(entries);
    QString logMsg = QString("[UModernDiagramCacheManager] Component cache loaded from JSON file: %1 (%2 entries)")
        .arg(filePath).arg(entries.size());
    MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    return true;
}

void UModernDiagramCacheManager::scheduleCacheSave()
{
    if(!m_owner)
        return;

    // Отложенное сохранение кэша через 2 секунды после последнего обновления
    // Это позволяет группировать множественные обновления и не замедлять работу
    QTimer::singleShot(2000, m_owner, [this]()
    {
        if(m_owner && m_owner->m_application)
        {
            const QHash<QString, UModernDiagramComponentCacheEntry>& entries = m_componentCache.getAllEntries();
            if(!entries.isEmpty())
            {
                // Пробуем сохранить в бинарном формате (быстрее), если не получится - в JSON
                QString binPath = getCacheFilePath("bin");
                if(!saveComponentCacheToFile(binPath, true))
                {
                    QString jsonPath = getCacheFilePath("json");
                    saveComponentCacheToFile(jsonPath, false);
                }
            }
        }
    });
}

