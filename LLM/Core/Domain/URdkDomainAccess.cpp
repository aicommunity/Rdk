#include "URdkDomainAccess.h"

#include <sstream>

#include <rdk_application.h>
#include <rdk_engine_support.h>
#include <rdk_init.h>

#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UEnvironment.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UContainer.h"

namespace RDK::LLM {

URdkDomainAccess::URdkDomainAccess(RDK::UApplication* app)
    : m_app(app)
{
    (void)m_app;
}

RDK::UEngine* URdkDomainAccess::engine() const
{
    RDK::UELockPtr<RDK::UEngine> lock = RDK::GetEngineLockTimeout(0, 100);
    return lock.Get();
}

RDK::UEnvironment* URdkDomainAccess::environment(int channel_index) const
{
    RDK::UELockPtr<RDK::UEngine> lock = RDK::GetEngineLockTimeout(channel_index, 100);
    if(!lock)
        return nullptr;
    return lock->GetEnvironment();
}

RDK::UStorage* URdkDomainAccess::storage() const
{
    RDK::UELockPtr<RDK::UStorage> lock = RDK::GetStorageLock();
    return lock.Get();
}

DomainSessionInfo URdkDomainAccess::sessionInfo() const
{
    DomainSessionInfo info;
    RDK::UELockPtr<RDK::UEngine> lock = RDK::GetEngineLockTimeout(0, 100);
    info.engine_ready = static_cast<bool>(lock);
    info.project_loaded = info.engine_ready;
    if(lock)
        info.channel_count = RdkCoreManager.NumChannels.Get();
    return info;
}

static void walkContainer(RDK::UContainer* cont, nlohmann::json& components,
                          int& count, int max_components, const std::string& parent,
                          RDK::UContainer* root)
{
    if(!cont || count >= max_components)
        return;
    const int n = cont->GetNumComponents();
    for(int i = 0; i < n && count < max_components; ++i)
    {
        RDK::UEPtr<RDK::UContainer> child = cont->GetComponentByIndex(i);
        if(!child)
            continue;
        nlohmann::json item;
        item["long_name"] = child->GetLongName(root);
        item["short_name"] = child->GetName();
        item["class_name"] = child->GetCompClassName();
        item["parent_long_name"] = parent;
        item["child_count"] = child->GetNumComponents();
        components.push_back(item);
        ++count;
        walkContainer(child.Get(), components, count, max_components,
                      child->GetLongName(root), root);
    }
}

DomainStatus URdkDomainAccess::listNetSnapshot(nlohmann::json& out, int channel_index,
                                               int max_components) const
{
    out = nlohmann::json::object();
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
    RDK::UEnvironment* env = eng->GetEnvironment();
    if(!env)
        return {DomainStatusCode::NotInitialized, "Environment not available"};
    RDK::UEPtr<RDK::UContainer> model = env->GetModel();
    if(!model)
        return {DomainStatusCode::ProjectNotLoaded, "Model not loaded on channel"};
    nlohmann::json components = nlohmann::json::array();
    int count = 0;
    walkContainer(model.Get(), components, count, max_components, "", model.Get());
    out["channel_index"] = channel_index;
    out["components"] = components;
    out["links"] = nlohmann::json::array();
    out["truncated"] = (count >= max_components);
    out["max_components_applied"] = max_components;
    return {};
}

DomainStatus URdkDomainAccess::listRegisteredClassNames(std::vector<std::string>& out) const
{
    out.clear();
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(0, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
    const char* list = Storage_GetClassesNameList();
    if(!list || !list[0])
        return {};
    std::string s(list);
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, ','))
    {
        if(!item.empty())
            out.push_back(item);
    }
    return {};
}

DomainStatus URdkDomainAccess::findComponentByLongName(const std::string& long_name,
                                                       nlohmann::json& out,
                                                       int channel_index) const
{
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
    RDK::UEnvironment* env = eng->GetEnvironment();
    if(!env)
        return {DomainStatusCode::NotInitialized, "Environment not available"};
    RDK::UEPtr<RDK::UContainer> model = env->GetModel();
    if(!model)
        return {DomainStatusCode::ProjectNotLoaded, "Model not loaded"};
    RDK::UEPtr<RDK::UContainer> found = model->GetComponent(long_name, true);
    if(!found)
        return {DomainStatusCode::ComponentNotFound, "Component not found: " + long_name};
    out["long_name"] = found->GetLongName(model);
    out["short_name"] = found->GetName();
    out["class_name"] = found->GetCompClassName();
    return {};
}

DomainStatus URdkDomainAccess::getComponentProperties(const std::string& long_name,
                                                      nlohmann::json& out,
                                                      int channel_index) const
{
    return findComponentByLongName(long_name, out, channel_index);
}

DomainStatus URdkDomainAccess::addComponent(const std::string& class_name,
                                            const std::string& parent_long_name,
                                            const std::string& short_name,
                                            int channel_index,
                                            std::string& out_long_name)
{
    (void)parent_long_name;
    const char* err = MModel_AddComponent(channel_index, short_name.c_str(), class_name.c_str());
    if(err && err[0])
        return {DomainStatusCode::LinkFailed, std::string(err)};
    out_long_name = short_name;
    return {};
}

DomainStatus URdkDomainAccess::setProperty(const std::string& long_name,
                                           const std::string& property_name,
                                           const std::string& value,
                                           int channel_index)
{
    (void)long_name;
    (void)property_name;
    (void)value;
    (void)channel_index;
    return {DomainStatusCode::InvalidPropertyValue,
            "set_property: full property API wiring deferred (TD-011)"};
}

DomainStatus URdkDomainAccess::validateProjectDryRun(std::vector<std::string>& warnings) const
{
    warnings.clear();
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(0, 100);
    if(!eng)
        warnings.push_back("Engine not initialized");
    return {};
}

} // namespace RDK::LLM
