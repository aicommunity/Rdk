#include "UModernDiagramPortManager.h"
#include "../../Deploy/Include/rdk_init.h"
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

    const char* props = Model_GetComponentPropertiesLookupList(fullName.toStdString().c_str(), propertyTypeMask);
    if(!props)
        return result;

    QStringList propsList = QString::fromUtf8(props).split(",", Qt::SkipEmptyParts);
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
    Engine_FreeBufString(props);

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
        RDK::UEPtr<RDK::UContainer> model = RDK::GetEngine() ? RDK::GetEngine()->GetModel() : nullptr;
        if(!model)
            return result;

        RDK::UEPtr<RDK::UContainer> component;
        if(fullName.isEmpty())
            component = model;
        else
            component = model->GetComponentL(fullName.toStdString(), true);

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

    // Получаем список дочерних компонентов
    const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
    if(!compList)
        return result;

    QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
    for(const QString& comp : components)
    {
        QString nestedFullName = fullName + "." + comp;
        const char* nestedProps = Model_GetComponentPropertiesLookupList(
            nestedFullName.toStdString().c_str(), ptPubOutput | ptOutput);
        if(nestedProps)
        {
            QStringList nestedPropsList = QString::fromUtf8(nestedProps).split(",", Qt::SkipEmptyParts);
            for(const QString& prop : nestedPropsList)
            {
                QStringList parts = prop.split(":");
                if(parts.size() >= 1)
                {
                    QString propName = parts[0].trimmed();
                    UModernDiagramPort port;
                    port.isInput = false;
                    port.name = propName;
                    port.componentName = comp;
                    port.fullPath = comp + "." + propName;
                    port.displayName = nodeName + "." + comp + "." + propName;
                    port.category = UModernDiagramPortCategory::Child;
                    result.append(port);
                }
            }
            Engine_FreeBufString(nestedProps);
        }
    }
    Engine_FreeBufString(compList);

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

    // Получаем список дочерних компонентов
    const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
    if(!compList)
        return result;

    QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
    for(const QString& comp : components)
    {
        QString nestedFullName = fullName + "." + comp;
        const char* nestedProps = Model_GetComponentPropertiesLookupList(
            nestedFullName.toStdString().c_str(), ptPubInput | ptInput);
        if(nestedProps)
        {
            QStringList nestedPropsList = QString::fromUtf8(nestedProps).split(",", Qt::SkipEmptyParts);
            for(const QString& prop : nestedPropsList)
            {
                QStringList parts = prop.split(":");
                if(parts.size() >= 1)
                {
                    QString propName = parts[0].trimmed();
                    UModernDiagramPort port;
                    port.isInput = true;
                    port.name = propName;
                    port.componentName = comp;
                    port.fullPath = comp + "." + propName;
                    port.displayName = nodeName + "." + comp + "." + propName;
                    port.category = UModernDiagramPortCategory::Child;
                    result.append(port);
                }
            }
            Engine_FreeBufString(nestedProps);
        }
    }
    Engine_FreeBufString(compList);

    return result;
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadAliasInputPorts(const QString& fullName, const QString& nodeName)
{
    return loadAliasPortsFromNet(fullName, nodeName, ptInput | ptPubInput, true);
}

QVector<UModernDiagramPort> UModernDiagramPortManager::loadNestedPorts(const QString& fullName, const QString& nodeName, bool isInput, bool includeNested)
{
    QVector<UModernDiagramPort> result;

    // Получаем порты текущего компонента
    unsigned int mask = isInput ? (ptPubInput | ptInput) : (ptPubOutput | ptOutput);
    const char* propsList = Model_GetComponentPropertiesLookupList(fullName.toStdString().c_str(), mask);
    if(propsList)
    {
        QStringList props = QString::fromUtf8(propsList).split(",", Qt::SkipEmptyParts);
        for(const QString& prop : props)
        {
            QStringList parts = prop.split(":");
            if(parts.size() >= 1)
            {
                QString propName = parts[0].trimmed();
                UModernDiagramPort port;
                port.isInput = isInput;
                port.name = propName;
                port.componentName = nodeName;
                port.fullPath = propName;
                port.displayName = nodeName + "." + propName;
                result.append(port);
            }
        }
        Engine_FreeBufString(propsList);
    }

    // Добавляем алиасы свойств из UNet
    QVector<UModernDiagramPort> aliasPorts = loadAliasPortsFromNet(fullName, nodeName, mask, isInput);
    result.append(aliasPorts);

    // Если нужно включить вложенные порты
    if(includeNested)
    {
        const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
        if(compList)
        {
            QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
            for(const QString& comp : components)
            {
                QString nestedFullName = fullName + "." + comp;
                const char* nestedProps = Model_GetComponentPropertiesLookupList(
                    nestedFullName.toStdString().c_str(), mask);
                if(nestedProps)
                {
                    QStringList nestedPropsList = QString::fromUtf8(nestedProps).split(",", Qt::SkipEmptyParts);
                    for(const QString& prop : nestedPropsList)
                    {
                        QStringList parts = prop.split(":");
                        if(parts.size() >= 1)
                        {
                            QString propName = parts[0].trimmed();
                            UModernDiagramPort port;
                            port.isInput = isInput;
                            port.name = propName;
                            port.componentName = comp;
                            port.fullPath = comp + "." + propName;
                            port.displayName = nodeName + "." + comp + "." + propName;
                            result.append(port);
                        }
                    }
                    Engine_FreeBufString(nestedProps);
                }
            }
            Engine_FreeBufString(compList);
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
        const char* compList = Model_GetComponentsNameList(fullName.toStdString().c_str());
        if(compList)
        {
            QStringList components = QString::fromUtf8(compList).split(",", Qt::SkipEmptyParts);
            Engine_FreeBufString(compList);

            if(!components.isEmpty())
            {
                // Проверяем, начинается ли propertyName с любого дочернего компонента
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
    }

    // Извлекаем имя свойства
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

