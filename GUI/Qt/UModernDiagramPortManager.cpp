#include "UModernDiagramPortManager.h"
#include "UEngineSelectionSync.h"
#include "../../Deploy/Include/rdk_init.h"
#include "../../Deploy/Include/rdk_cpp_init.h"
#include "../Core/Engine/UEngine.h"
#include "../Core/Engine/UNet.h"
#include "rdk_application.h"

#include <QStringList>

// Вспомогательный метод для загрузки портов из свойств
QVector<UModernDiagramPort> UModernDiagramPortManager::loadPortsFromProperties(
    const QString& fullName,
    const QString& nodeName,
    unsigned int propertyTypeMask,
    bool isInput,
    UModernDiagramPortCategory category,
    bool filterOwn)
{
    QVector<UModernDiagramPort> result;

    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!model)
        return result;

    const QString props = propertiesLookupListFromModelScope(model.Get(), fullName, propertyTypeMask);
    if(props.isEmpty())
        return result;

    QStringList propsList = props.split(",", Qt::SkipEmptyParts);
    for(const QString& prop : propsList)
    {
        QStringList parts = prop.split(":");
        if(parts.size() >= 1)
        {
            QString propName = parts[0].trimmed();

            // Для собственных портов фильтруем по отсутствию точки
            if(filterOwn && propName.contains('.'))
                continue;

            UModernDiagramPort port;
            port.isInput = isInput;
            port.name = propName;
            port.componentName = nodeName;
            port.fullPath = propName;
            port.displayName = propName;
            port.category = category;
            result.append(port);
        }
    }

    return result;
}

// Вспомогательный метод для загрузки портов алиасов из UNet
QVector<UModernDiagramPort> UModernDiagramPortManager::loadAliasPortsFromNet(
    const QString& fullName,
    const QString& nodeName,
    unsigned int propertyTypeMask,
    bool isInput)
{
    QVector<UModernDiagramPort> result;

    try
    {
        RDK::UELockPtr<RDK::UContainer> modelLock =
            RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
        if(!modelLock)
            return result;
        RDK::UContainer* model = modelLock.Get();

        RDK::UEPtr<RDK::UContainer> component;
        if(fullName.isEmpty())
            component = model;
        else
            component = model->GetComponentL(fullName.toUtf8().constData(), true);

        if(component)
        {
            RDK::UEPtr<RDK::UNet> net = RDK::dynamic_pointer_cast<RDK::UNet>(component);
            if(net)
            {
                std::vector<RDK::UPropertyAlias> aliases = net->GetPropertyAliasesByType(propertyTypeMask);

                for(const auto& alias : aliases)
                {
                    UModernDiagramPort port;
                    port.isInput = isInput;
                    port.name = QString::fromStdString(alias.AliasName);
                    port.componentName = nodeName;
                    port.fullPath = QString::fromStdString(alias.GetFullPropertyPath());
                    port.displayName = QString::fromStdString(alias.AliasName) + " [Alias]";
                    port.category = UModernDiagramPortCategory::Alias;
                    result.append(port);
                }
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки при получении алиасов
    }

    return result;
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadOwnOutputPorts(const QString& fullName, const QString& nodeName)
{
    return loadPortsFromProperties(fullName, nodeName, ptPubOutput | ptOutput, false, UModernDiagramPortCategory::Own, true);
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadChildOutputPorts(const QString& fullName, const QString& nodeName)
{
    QVector<UModernDiagramPort> result;
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!model)
        return result;

    const QStringList components =
        childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
    for(const QString& comp : components)
    {
        const QString nestedFullName = fullName + QLatin1Char('.') + comp;
        const QString nestedProps =
            propertiesLookupListFromModelScope(model.Get(), nestedFullName, ptPubOutput | ptOutput);
        if(nestedProps.isEmpty())
            continue;

        const QStringList nestedPropsList = nestedProps.split(",", Qt::SkipEmptyParts);
        for(const QString& prop : nestedPropsList)
        {
            const QStringList parts = prop.split(":");
            if(parts.isEmpty())
                continue;
            const QString propName = parts[0].trimmed();
            UModernDiagramPort port;
            port.isInput = false;
            port.name = propName;
            port.componentName = comp;
            port.fullPath = comp + QLatin1Char('.') + propName;
            port.displayName = nodeName + QLatin1Char('.') + comp + QLatin1Char('.') + propName;
            port.category = UModernDiagramPortCategory::Child;
            result.append(port);
        }
    }

    return result;
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadAliasOutputPorts(const QString& fullName, const QString& nodeName)
{
    return loadAliasPortsFromNet(fullName, nodeName, ptOutput | ptPubOutput, false);
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadOwnInputPorts(const QString& fullName, const QString& nodeName)
{
    return loadPortsFromProperties(fullName, nodeName, ptPubInput | ptInput, true, UModernDiagramPortCategory::Own, true);
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadChildInputPorts(const QString& fullName, const QString& nodeName)
{
    QVector<UModernDiagramPort> result;
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!model)
        return result;

    const QStringList components =
        childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
    for(const QString& comp : components)
    {
        const QString nestedFullName = fullName + QLatin1Char('.') + comp;
        const QString nestedProps =
            propertiesLookupListFromModelScope(model.Get(), nestedFullName, ptPubInput | ptInput);
        if(nestedProps.isEmpty())
            continue;

        const QStringList nestedPropsList = nestedProps.split(",", Qt::SkipEmptyParts);
        for(const QString& prop : nestedPropsList)
        {
            const QStringList parts = prop.split(":");
            if(parts.isEmpty())
                continue;
            const QString propName = parts[0].trimmed();
            UModernDiagramPort port;
            port.isInput = true;
            port.name = propName;
            port.componentName = comp;
            port.fullPath = comp + QLatin1Char('.') + propName;
            port.displayName = nodeName + QLatin1Char('.') + comp + QLatin1Char('.') + propName;
            port.category = UModernDiagramPortCategory::Child;
            result.append(port);
        }
    }

    return result;
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadAliasInputPorts(const QString& fullName, const QString& nodeName)
{
    return loadAliasPortsFromNet(fullName, nodeName, ptInput | ptPubInput, true);
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadChildInputPortsRecursive(const QString& fullName, const QString& nodeName, const QString& prefixPath)
{
    QVector<UModernDiagramPort> result;
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!model)
        return result;

    const QStringList components =
        childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
    for(const QString& comp : components)
    {
        const QString nestedFullName = fullName + QLatin1Char('.') + comp;
        const QString displayPrefix = prefixPath.isEmpty() ? comp : prefixPath + QLatin1Char('.') + comp;

        const QString nestedProps =
            propertiesLookupListFromModelScope(model.Get(), nestedFullName, ptPubInput | ptInput);
        if(!nestedProps.isEmpty())
        {
            const QStringList nestedPropsList = nestedProps.split(",", Qt::SkipEmptyParts);
            for(const QString& prop : nestedPropsList)
            {
                const QStringList parts = prop.split(":");
                if(parts.isEmpty())
                    continue;
                const QString propName = parts[0].trimmed();
                UModernDiagramPort port;
                port.isInput = true;
                port.name = propName;
                port.componentName = displayPrefix;
                port.fullPath = displayPrefix + QLatin1Char('.') + propName;
                port.displayName = nodeName + QLatin1Char('.') + displayPrefix + QLatin1Char('.') + propName;
                port.category = UModernDiagramPortCategory::Child;
                result.append(port);
            }
        }

        result.append(loadChildInputPortsRecursive(nestedFullName, nodeName, displayPrefix));
    }

    return result;
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadChildOutputPortsRecursive(const QString& fullName, const QString& nodeName, const QString& prefixPath)
{
    QVector<UModernDiagramPort> result;
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(!model)
        return result;

    const QStringList components =
        childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
    for(const QString& comp : components)
    {
        const QString nestedFullName = fullName + QLatin1Char('.') + comp;
        const QString displayPrefix = prefixPath.isEmpty() ? comp : prefixPath + QLatin1Char('.') + comp;

        const QString nestedProps =
            propertiesLookupListFromModelScope(model.Get(), nestedFullName, ptPubOutput | ptOutput);
        if(!nestedProps.isEmpty())
        {
            const QStringList nestedPropsList = nestedProps.split(",", Qt::SkipEmptyParts);
            for(const QString& prop : nestedPropsList)
            {
                const QStringList parts = prop.split(":");
                if(parts.isEmpty())
                    continue;
                const QString propName = parts[0].trimmed();
                UModernDiagramPort port;
                port.isInput = false;
                port.name = propName;
                port.componentName = displayPrefix;
                port.fullPath = displayPrefix + QLatin1Char('.') + propName;
                port.displayName = nodeName + QLatin1Char('.') + displayPrefix + QLatin1Char('.') + propName;
                port.category = UModernDiagramPortCategory::Child;
                result.append(port);
            }
        }

        result.append(loadChildOutputPortsRecursive(nestedFullName, nodeName, displayPrefix));
    }

    return result;
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadNestedPorts(const QString& fullName, const QString& nodeName, bool isInput, bool includeNested)
{
    QVector<UModernDiagramPort> result;

    unsigned int mask = isInput ? (ptPubInput | ptInput) : (ptPubOutput | ptOutput);
    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(Core_GetSelectedChannelIndex());
    if(model)
    {
        const QString propsList = propertiesLookupListFromModelScope(model.Get(), fullName, mask);
        const QStringList props = propsList.split(",", Qt::SkipEmptyParts);
        for(const QString& prop : props)
        {
            const QStringList parts = prop.split(":");
            if(parts.isEmpty())
                continue;
            const QString propName = parts[0].trimmed();
            UModernDiagramPort port;
            port.isInput = isInput;
            port.name = propName;
            port.componentName = nodeName;
            port.fullPath = propName;
            port.displayName = nodeName + QLatin1Char('.') + propName;
            result.append(port);
        }
    }

    // Добавляем алиасы свойств из UNet
    QVector<UModernDiagramPort> aliasPorts = loadAliasPortsFromNet(fullName, nodeName, mask, isInput);
    result.append(aliasPorts);

    // Если нужно включить вложенные порты
    if(includeNested && model)
    {
        const QStringList components =
            childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
        for(const QString& comp : components)
        {
            const QString nestedFullName = fullName + QLatin1Char('.') + comp;
            const QString nestedProps = propertiesLookupListFromModelScope(model.Get(), nestedFullName, mask);
            if(nestedProps.isEmpty())
                continue;
            const QStringList nestedPropsList = nestedProps.split(",", Qt::SkipEmptyParts);
            for(const QString& prop : nestedPropsList)
            {
                const QStringList parts = prop.split(":");
                if(parts.isEmpty())
                    continue;
                const QString propName = parts[0].trimmed();
                UModernDiagramPort port;
                port.isInput = isInput;
                port.name = propName;
                port.componentName = comp;
                port.fullPath = comp + QLatin1Char('.') + propName;
                port.displayName = nodeName + QLatin1Char('.') + comp + QLatin1Char('.') + propName;
                result.append(port);
            }
        }
    }

    return result;
}

UModernDiagramPortCategory UModernDiagramPortManager::determinePortCategory(
    const QString& fullName,
    const QString& nodeName,
    const QString& propertyName,
    bool isInput,
    const QVector<UModernDiagramPort>& ownPorts,
    const QVector<UModernDiagramPort>& childPorts,
    const QVector<UModernDiagramPort>& aliasPorts)
{
    // Сначала проверяем, содержит ли propertyName путь к дочернему компоненту
    if(!fullName.isEmpty())
    {
        const QStringList components =
            childComponentShortNamesFromModelScope(Core_GetSelectedChannelIndex(), fullName);
        if(!components.isEmpty())
        {
            for(const QString& comp : components)
                {
                    if(propertyName.startsWith(comp + ".") || propertyName == comp)
                    {
                        return UModernDiagramPortCategory::Child;
                    }
                }

                // Если propertyName начинается с nodeName, проверяем часть после nodeName
                if(propertyName.startsWith(nodeName + "."))
                {
                    QString afterNodeName = propertyName.mid(nodeName.length() + 1);
                    if(!afterNodeName.isEmpty())
                    {
                        QStringList parts = afterNodeName.split('.');
                        if(!parts.isEmpty())
                        {
                            QString firstPart = parts.first();
                            if(components.contains(firstPart))
                            {
                                return UModernDiagramPortCategory::Child;
                            }
                        }
                    }
                }
        }
    }

    QString propName = propertyName;
    if(propName.contains('.'))
    {
        propName = propName.split('.').last();
    }

    // Проверяем фактическое наличие свойства в каждой категории
    // Сначала проверяем алиасы (они имеют приоритет)
    for(const UModernDiagramPort& port : aliasPorts)
    {
        if(port.name == propName ||
           port.fullPath == propertyName ||
           port.fullPath.endsWith("." + propName))
        {
            return UModernDiagramPortCategory::Alias;
        }
    }

    // Затем проверяем дочерние компоненты
    for(const UModernDiagramPort& port : childPorts)
    {
        if(port.name == propName ||
           port.fullPath == propertyName ||
           port.fullPath.endsWith("." + propName) ||
           propertyName.contains(port.componentName + "." + propName))
        {
            return UModernDiagramPortCategory::Child;
        }

        if(propertyName.startsWith(port.componentName + "."))
        {
            return UModernDiagramPortCategory::Child;
        }

        if(propertyName.contains("." + port.componentName + ".") ||
           propertyName.startsWith(port.componentName + "."))
        {
            return UModernDiagramPortCategory::Child;
        }
    }

    // Наконец проверяем собственные свойства
    for(const UModernDiagramPort& port : ownPorts)
    {
        if(port.name == propName ||
           port.fullPath == propertyName ||
           port.fullPath == propName)
        {
            return UModernDiagramPortCategory::Own;
        }
    }

    // Если свойство не найдено ни в одной категории, определяем по имени
    if(propertyName.contains('.'))
    {
        if(!propertyName.startsWith(nodeName + "."))
        {
            return UModernDiagramPortCategory::Child;
        }
    }

    // По умолчанию возвращаем Own
    return UModernDiagramPortCategory::Own;
}

