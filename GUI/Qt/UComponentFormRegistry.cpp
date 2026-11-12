#include "UComponentFormRegistry.h"

UComponentFormRegistry& UComponentFormRegistry::instance()
{
    static UComponentFormRegistry g_registry;
    return g_registry;
}

void UComponentFormRegistry::registerFormFactory(const QString& componentClassName, const UComponentFormDescriptor& descriptor)
{
    if(componentClassName.isEmpty() || !descriptor.factory)
        return;

    m_formsByClass.insert(componentClassName, descriptor);
}

bool UComponentFormRegistry::canOpen(const QString& componentClassName) const
{
    return !componentClassName.isEmpty() && m_formsByClass.contains(componentClassName);
}

bool UComponentFormRegistry::canOpen(const UComponentGuiContext& context) const
{
    return canOpen(context.componentClassName);
}

const UComponentFormDescriptor* UComponentFormRegistry::findDescriptor(const QString& componentClassName) const
{
    auto it = m_formsByClass.constFind(componentClassName);
    if(it == m_formsByClass.constEnd())
        return nullptr;
    return &it.value();
}

std::vector<QString> UComponentFormRegistry::registeredClasses() const
{
    std::vector<QString> result;
    result.reserve(static_cast<size_t>(m_formsByClass.size()));
    for(auto it = m_formsByClass.constBegin(); it != m_formsByClass.constEnd(); ++it)
        result.push_back(it.key());
    return result;
}
