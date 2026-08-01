#include "URdkDomainAccess.h"
#include "ULLMLinkIdentity.h"
#include "ULLMModelLinkWalker.h"
#include "URdkApplicationCommands.h"
#include "../Gui/ILLMPresentationSink.h"
#include "../Policy/ULLMPathPolicy.h"
#include "../Tools/ApplicationToolHelpers.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <rdk_application.h>
#include <rdk_engine_support.h>
#include <rdk_init.h>

#include "../../Core/Engine/UEngine.h"
#include "../../Core/Engine/UEnvironment.h"
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/ULibrary.h"
#include "../../Core/Engine/UContainer.h"
#include "../../Core/Engine/UNet.h"
#include "../../Core/Engine/UEnvSupport.h"
#include "../../Core/Math/MDMatrix.h"
#include "../../Core/Math/MDVector.h"

#include <regex>
#include <typeinfo>

namespace RDK::LLM {

namespace {

constexpr unsigned int kPubParameterMask = ptPubParameter;

struct ParsedPropertyMeta {
    std::string name;
    std::string type;
    std::string value;
};

std::string truncateValueRepr(const std::string& raw, size_t max_len = 256)
{
    if(raw.empty())
        return "";
    static const std::regex matrix_re(R"(<Matrix\s+(\d+)\s*x\s*(\d+)\s*>)", std::regex::icase);
    std::smatch m;
    if(std::regex_search(raw, m, matrix_re) && m.size() >= 3)
        return "<Matrix " + m[1].str() + "x" + m[2].str() + ">";
    if(raw.size() > max_len)
        return raw.substr(0, max_len) + "...";
    return raw;
}

std::vector<ParsedPropertyMeta> parsePropertiesExXml(const std::string& xml)
{
    std::vector<ParsedPropertyMeta> out;
    if(xml.empty())
        return out;
    static const std::regex node_re(
        "<([A-Za-z_][\\w.]*)[^>]*\\bPType=\"(\\d+)\"[^>]*>([^<]*)</\\1>");
    for(std::sregex_iterator it(xml.begin(), xml.end(), node_re), end; it != end; ++it)
    {
        ParsedPropertyMeta meta;
        meta.name = (*it)[1].str();
        meta.type = "ptype_" + (*it)[2].str();
        meta.value = (*it)[3].str();
        out.push_back(std::move(meta));
    }
    return out;
}

std::vector<ParsedPropertyMeta> catalogFromLookupList(const std::string& lookup_csv)
{
    std::vector<ParsedPropertyMeta> out;
    if(lookup_csv.empty())
        return out;
    std::stringstream ss(lookup_csv);
    std::string token;
    while(std::getline(ss, token, ','))
    {
        const size_t colon = token.find(':');
        const std::string name =
            colon == std::string::npos ? token : token.substr(0, colon);
        if(name.empty())
            continue;
        ParsedPropertyMeta meta;
        meta.name = name;
        meta.type = "parameter";
        out.push_back(std::move(meta));
    }
    return out;
}

bool lookupListContainsProperty(const std::string& lookup_csv, const std::string& property_name)
{
    if(lookup_csv.empty() || property_name.empty())
        return false;
    std::stringstream ss(lookup_csv);
    std::string token;
    while(std::getline(ss, token, ','))
    {
        const size_t colon = token.find(':');
        const std::string name =
            colon == std::string::npos ? token : token.substr(0, colon);
        if(name == property_name)
            return true;
    }
    return false;
}

bool componentHasProperty(RDK::UEngine* eng, const std::string& long_name,
                          const std::string& property_name)
{
    if(!eng || property_name.empty())
        return false;
    constexpr unsigned int kMasks[] = {kPubParameterMask,
                                       static_cast<unsigned int>(ptPubState),
                                       static_cast<unsigned int>(ptPubInput),
                                       static_cast<unsigned int>(ptPubOutput)};
    for(unsigned int mask : kMasks)
    {
        const char* list = eng->Model_GetComponentPropertiesLookupList(long_name.c_str(), mask);
        if(!list)
            continue;
        if(lookupListContainsProperty(list, property_name))
            return true;
    }
    return false;
}

void refreshDiagramPresentation(ILLMPresentationSink* sink)
{
    if(!sink)
        return;
    // Must not run while holding channel engine/model lock: sink blocks on GUI thread,
    // which reloads the diagram via Model_GetComponentsNameList (same mutex).
    LLMPresentationEvent ev;
    ev.effect = LLMPresentationEffect::DiagramRefresh;
    sink->apply(ev);
}

} // namespace

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

DomainSessionInfo URdkDomainAccess::sessionInfo() const
{
    DomainSessionInfo info;
    RDK::UELockPtr<RDK::UEngine> lock = RDK::GetEngineLockTimeout(0, 100);
    info.engine_ready = static_cast<bool>(lock);
    info.project_loaded = m_app && m_app->GetProjectOpenFlag();
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
                                               int max_components,
                                               const std::string& root_long_name) const
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
    RDK::UContainer* model_root = model.Get();
    RDK::UContainer* walk_root = model_root;
    std::string parent_for_walk;
    std::string applied_root_long_name;
    if(!root_long_name.empty())
    {
        RDK::UEPtr<RDK::UContainer> subtree =
            model->GetComponent(root_long_name, true);
        if(!subtree)
            return {DomainStatusCode::ComponentNotFound,
                    "Component not found: " + root_long_name};
        walk_root = subtree.Get();
        parent_for_walk = subtree->GetLongName(model_root);
        applied_root_long_name = parent_for_walk;
    }
    nlohmann::json components = nlohmann::json::array();
    int count = 0;
    walkContainer(walk_root, components, count, max_components, parent_for_walk, model_root);
    out["channel_index"] = channel_index;
    if(!applied_root_long_name.empty())
        out["root_long_name"] = applied_root_long_name;
    out["components"] = components;
    ModelLinkWalkOptions link_opts;
    link_opts.limit = modelLinkSnapshotMaxLinks();
    link_opts.offset = 0;
    link_opts.count_all = true;
    const ModelLinkWalkResult link_walk = walkModelLinks(walk_root, model_root, link_opts);
    nlohmann::json links = nlohmann::json::array();
    for(const LinkQuad& q : link_walk.links)
        links.push_back(linkQuadToJson(q));
    out["links"] = std::move(links);
    out["links_truncated"] = link_walk.truncated;
    out["truncated"] = (count >= max_components);
    out["max_components_applied"] = max_components;
    return {};
}

DomainStatus URdkDomainAccess::listModelLinks(nlohmann::json& out, int channel_index,
                                              const std::string& root_long_name, int offset,
                                              int limit, const ModelLinkListFilters& filters) const
{
    out = nlohmann::json::object();
    URdkDomainAccess* self = const_cast<URdkDomainAccess*>(this);
    ModelLinkWalkScope scope;
    scope.channel_index = channel_index;
    scope.root_long_name = root_long_name;
    RDK::UContainer* walk_root = nullptr;
    RDK::UContainer* model_root = nullptr;
    const DomainStatus root_st = resolveModelLinkWalkRoot(*self, scope, walk_root, model_root);
    if(!root_st.ok())
        return root_st;

    int effective_limit = limit;
    if(effective_limit < 0)
        effective_limit = modelLinkWalkDefaultPageSize();
    effective_limit = std::min(effective_limit, modelLinkWalkMaxLimit());

    ModelLinkWalkOptions opts;
    opts.offset = std::max(0, offset);
    opts.limit = effective_limit;
    opts.count_all = true;
    if(!filters.component_long_name.empty() || !filters.from_long_name.empty()
       || !filters.to_long_name.empty())
        opts.subtree_filters = filters;
    const ModelLinkWalkResult walk = walkModelLinks(walk_root, model_root, opts);

    nlohmann::json links = nlohmann::json::array();
    for(const LinkQuad& q : walk.links)
        links.push_back(linkQuadToJson(q));

    out["channel_index"] = channel_index;
    if(!root_long_name.empty())
        out["root_long_name"] = root_long_name;
    if(!filters.component_long_name.empty())
        out["component_long_name"] = filters.component_long_name;
    if(!filters.from_long_name.empty())
        out["from_long_name"] = filters.from_long_name;
    if(!filters.to_long_name.empty())
        out["to_long_name"] = filters.to_long_name;
    out["offset"] = opts.offset;
    out["limit"] = effective_limit;
    out["returned_count"] = static_cast<int>(walk.links.size());
    out["total_links_seen"] = walk.total_quads_seen;
    out["total_matching"] = walk.total_matching;
    out["no_matching_links"] = walk.total_matching == 0;
    out["truncated"] = walk.truncated;
    out["next_offset"] = walk.next_offset;
    out["links"] = std::move(links);
    return {};
}

DomainStatus URdkDomainAccess::linkExistsInModel(const LinkQuad& quad, int channel_index,
                                                 const std::string& root_long_name,
                                                 bool& out_exists) const
{
    out_exists = false;
    URdkDomainAccess* self = const_cast<URdkDomainAccess*>(this);
    ModelLinkWalkScope scope;
    scope.channel_index = channel_index;
    scope.root_long_name = root_long_name;
    RDK::UContainer* walk_root = nullptr;
    RDK::UContainer* model_root = nullptr;
    const DomainStatus root_st = resolveModelLinkWalkRoot(*self, scope, walk_root, model_root);
    if(!root_st.ok())
        return root_st;

    ModelLinkWalkOptions opts;
    opts.match_quad = quad;
    opts.stop_on_first_match = true;
    opts.limit = 1;
    opts.offset = 0;
    const ModelLinkWalkResult walk = walkModelLinks(walk_root, model_root, opts);
    out_exists = walk.found_match;
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

DomainStatus URdkDomainAccess::listRegisteredClasses(nlohmann::json& out,
                                                     const std::string& library_filter) const
{
    out = nlohmann::json::object();
    std::vector<std::string> names;
    DomainStatus st = listRegisteredClassNames(names);
    if(!st.ok())
        return st;

    RDK::UELockPtr<RDK::UStorage> storage_lock = RDK::GetStorageLock();
    RDK::UStorage* storage = storage_lock.Get();

    nlohmann::json classes = nlohmann::json::array();
    for(const std::string& class_name : names)
    {
        std::string library;
        if(storage)
        {
            RDK::UEPtr<RDK::ULibrary> lib = storage->FindCollection(class_name);
            if(lib)
                library = lib->GetName();
        }
        if(!library_filter.empty() && library != library_filter)
            continue;
        nlohmann::json item = {{"class_name", class_name}};
        if(!library.empty())
            item["library"] = library;
        classes.push_back(std::move(item));
    }
    out["classes"] = classes;
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
    RDK::UEPtr<RDK::UContainer> found = model->GetComponentL(long_name.c_str(), true);
    if(!found)
        return {DomainStatusCode::ComponentNotFound, "Component not found: " + long_name};
    out["long_name"] = found->GetLongName(model);
    out["short_name"] = found->GetName();
    out["class_name"] = found->GetCompClassName();
    return {};
}

namespace {

bool componentMatchesHint(const nlohmann::json& component, const std::string& hint)
{
    if(!component.is_object() || hint.empty())
        return false;
    const std::string ln = component.value("long_name", "");
    const std::string sn = component.value("short_name", "");
    if(sn == hint || ln == hint)
        return true;
    const std::string suffix = "/" + hint;
    return !ln.empty() && ln.size() >= suffix.size()
           && ln.compare(ln.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace

DomainStatus URdkDomainAccess::resolveComponentLongName(const std::string& hint,
                                                        int channel_index,
                                                        std::string& out_long_name,
                                                        const std::string& parent_scope) const
{
    out_long_name.clear();
    if(hint.empty())
        return {DomainStatusCode::ComponentNotFound, "Empty component hint"};

    nlohmann::json direct;
    if(findComponentByLongName(hint, direct, channel_index).ok()
       && direct.contains("long_name"))
    {
        out_long_name = direct["long_name"].get<std::string>();
        return {};
    }

    nlohmann::json snap;
    const DomainStatus st = listNetSnapshot(snap, channel_index, 5000, parent_scope);
    if(!st.ok())
        return st;

    const nlohmann::json* best = nullptr;
    for(const auto& component : snap.value("components", nlohmann::json::array()))
    {
        if(!componentMatchesHint(component, hint))
            continue;
        if(!best)
            best = &component;
        else
        {
            const std::string ln = component.value("long_name", "");
            if(ln.size() > best->value("long_name", "").size())
                best = &component;
        }
    }
    if(!best || !best->contains("long_name"))
    {
        return {DomainStatusCode::ComponentNotFound,
                "Component not found under scope \"" + parent_scope + "\": " + hint};
    }
    out_long_name = (*best)["long_name"].get<std::string>();
    return {};
}

DomainStatus URdkDomainAccess::getComponentProperties(const std::string& long_name,
                                                      nlohmann::json& out,
                                                      int channel_index,
                                                      const std::vector<std::string>& property_names) const
{
    DomainStatus st = findComponentByLongName(long_name, out, channel_index);
    if(!st.ok())
        return st;

    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};

    std::unordered_map<std::string, ParsedPropertyMeta> by_name;
    auto merge_catalog = [&](const std::vector<ParsedPropertyMeta>& entries) {
        for(const ParsedPropertyMeta& meta : entries)
            by_name[meta.name] = meta;
    };

    const char* props_xml =
        eng->Model_GetComponentPropertiesEx(long_name.c_str(), kPubParameterMask);
    if(props_xml)
        merge_catalog(parsePropertiesExXml(props_xml));

    constexpr unsigned int kMasks[] = {kPubParameterMask,
                                       static_cast<unsigned int>(ptPubState),
                                       static_cast<unsigned int>(ptPubInput),
                                       static_cast<unsigned int>(ptPubOutput)};
    for(unsigned int mask : kMasks)
    {
        const char* lookup =
            eng->Model_GetComponentPropertiesLookupList(long_name.c_str(), mask);
        if(lookup)
            merge_catalog(catalogFromLookupList(lookup));
    }

    const bool include_values = !property_names.empty();
    if(include_values)
    {
        for(const std::string& requested : property_names)
        {
            if(by_name.find(requested) == by_name.end())
            {
                return {DomainStatusCode::PropertyNotFound,
                        "Unknown property_name for component: " + requested
                            + ". Call get_component_properties without property_names to list "
                              "available properties."};
            }
        }
    }

    nlohmann::json properties = nlohmann::json::array();
    auto emit_property = [&](const ParsedPropertyMeta& meta, bool with_value) {
        nlohmann::json item = {{"name", meta.name}, {"type", meta.type}};
        if(with_value)
        {
            std::string value_repr;
            if(!meta.value.empty())
                value_repr = truncateValueRepr(meta.value);
            else
            {
                std::string raw;
                bool found = false;
                getPropertyValue(long_name, meta.name, channel_index, raw, found);
                if(found)
                    value_repr = truncateValueRepr(raw);
            }
            item["value_repr"] = value_repr;
        }
        else
            item["value_repr"] = "";
        properties.push_back(std::move(item));
    };

    if(include_values)
    {
        for(const std::string& requested : property_names)
            emit_property(by_name.at(requested), true);
    }
    else
    {
        for(const auto& entry : by_name)
            emit_property(entry.second, false);
    }

    out["properties"] = properties;
    return {};
}

DomainStatus URdkDomainAccess::listComponentPubPorts(const std::string& long_name,
                                                     int channel_index,
                                                     std::vector<std::string>& outputs,
                                                     std::vector<std::string>& inputs) const
{
    outputs.clear();
    inputs.clear();
    nlohmann::json found;
    DomainStatus st = findComponentByLongName(long_name, found, channel_index);
    if(!st.ok())
        return st;

    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};

    auto append_unique = [](std::vector<std::string>& dest, const std::vector<ParsedPropertyMeta>& entries) {
        for(const ParsedPropertyMeta& meta : entries)
        {
            if(meta.name.empty())
                continue;
            if(std::find(dest.begin(), dest.end(), meta.name) == dest.end())
                dest.push_back(meta.name);
        }
    };

    const char* out_list =
        eng->Model_GetComponentPropertiesLookupList(long_name.c_str(), static_cast<unsigned int>(ptPubOutput));
    if(out_list)
        append_unique(outputs, catalogFromLookupList(out_list));

    const char* in_list =
        eng->Model_GetComponentPropertiesLookupList(long_name.c_str(), static_cast<unsigned int>(ptPubInput));
    if(in_list)
        append_unique(inputs, catalogFromLookupList(in_list));

    return {};
}

DomainStatus URdkDomainAccess::addComponent(const std::string& class_name,
                                            const std::string& parent_long_name,
                                            const std::string& short_name,
                                            int channel_index,
                                            std::string& out_long_name)
{
    out_long_name.clear();
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};

    {
        RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLock(channel_index);
        if(!eng)
            return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
        RDK::UELockPtr<RDK::UStorage> storLock = RDK::GetStorageLock(channel_index);
        if(!storLock)
            return {DomainStatusCode::NotInitialized, "Storage lock unavailable"};
        RDK::UEnvironment* env = eng->GetEnvironment();
        if(!env)
            return {DomainStatusCode::NotInitialized, "Environment not available"};
        RDK::UEPtr<RDK::UContainer> model = env->GetModel();
        if(!model)
            return {DomainStatusCode::ProjectNotLoaded, "Model not loaded on channel"};
        RDK::UStorage* stor = storLock.Get();

        RDK::UEPtr<RDK::UContainer> parent;
        if(parent_long_name.empty())
            parent = model;
        else
            parent = model->GetComponentL(parent_long_name.c_str(), true);
        if(!parent)
        {
            return {DomainStatusCode::ComponentNotFound,
                    "add_component: parent container not found: " + parent_long_name};
        }

        RDK::UEPtr<RDK::UContainer> cont =
            dynamic_pointer_cast<RDK::UContainer>(stor->TakeObject(class_name.c_str()));
        if(!cont)
        {
            return {DomainStatusCode::LinkFailed,
                    "add_component failed to instantiate class " + class_name};
        }

        if(!short_name.empty())
            cont->SetName(short_name.c_str());

        if(!parent->AddComponent(cont))
        {
            stor->ReturnObject(cont);
            return {DomainStatusCode::LinkFailed,
                    "add_component failed for class " + class_name + " under parent \""
                        + parent_long_name + "\""};
        }

        if(!cont->IsInit())
            cont->Init();
        else
            cont->Reset();

        std::string buffer;
        out_long_name = cont->GetLongName(model.Get(), buffer);
    }
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::removeComponent(const std::string& long_name, int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    const int rc = MModel_DelComponent(channel_index, "", long_name.c_str());
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "remove_component failed for " + long_name + " (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::getPropertyValue(const std::string& long_name,
                                              const std::string& property_name,
                                              int channel_index,
                                              std::string& out_value,
                                              bool& found) const
{
    found = false;
    out_value.clear();
    const char* raw =
        MModel_GetComponentPropertyValue(channel_index, long_name.c_str(), property_name.c_str());
    if(raw && raw[0])
    {
        out_value = raw;
        found = true;
    }
    return {};
}

DomainStatus URdkDomainAccess::setProperty(const std::string& long_name,
                                           const std::string& property_name,
                                           const std::string& value,
                                           int channel_index,
                                           std::string* previous_value_out)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    {
        RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
        if(!eng)
            return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
        if(!componentHasProperty(eng.Get(), long_name, property_name))
        {
            return {DomainStatusCode::PropertyNotFound,
                    "Unknown property_name \"" + property_name + "\" for component " + long_name
                        + ". Use get_component_properties to list available properties."};
        }
    }
    if(previous_value_out)
    {
        bool had = false;
        getPropertyValue(long_name, property_name, channel_index, *previous_value_out, had);
        if(!had)
            previous_value_out->clear();
    }
    const int rc =
        MModel_SetComponentPropertyValue(channel_index, long_name.c_str(), property_name.c_str(),
                                         value.c_str());
    if(rc != 0)
        return {DomainStatusCode::InvalidPropertyValue,
                "set_property failed for " + property_name + " on " + long_name};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::cloneComponent(const std::string& long_name,
                                              const std::string& new_short_name,
                                              int channel_index,
                                              std::string& out_long_name)
{
    out_long_name.clear();
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "clone_component: long_name is required"};

    {
        RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLock(channel_index);
        if(!eng)
            return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
        RDK::UELockPtr<RDK::UStorage> storLock = RDK::GetStorageLock(channel_index);
        if(!storLock)
            return {DomainStatusCode::NotInitialized, "Storage lock unavailable"};
        RDK::UEnvironment* env = eng->GetEnvironment();
        if(!env)
            return {DomainStatusCode::NotInitialized, "Environment not available"};
        RDK::UEPtr<RDK::UContainer> model = env->GetModel();
        if(!model)
            return {DomainStatusCode::ProjectNotLoaded, "Model not loaded on channel"};

        RDK::UEPtr<RDK::UNet> component =
            RDK::dynamic_pointer_cast<RDK::UNet>(model->GetComponentL(long_name.c_str(), true));
        if(!component)
            return {DomainStatusCode::ComponentNotFound,
                    "clone_component: component not found: " + long_name};

        RDK::UEPtr<RDK::UNet> owner = RDK::dynamic_pointer_cast<RDK::UNet>(component->GetOwner());
        if(!owner)
            return {DomainStatusCode::LinkFailed, "clone_component: owner not found"};

        RDK::UStorage* stor = storLock.Get();
        RDK::UEPtr<RDK::UNet> new_component = RDK::dynamic_pointer_cast<RDK::UNet>(
            stor->TakeObject(component->GetClass(), component.Get()));
        if(!new_component)
            return {DomainStatusCode::LinkFailed, "clone_component: TakeObject failed"};

        if(new_short_name.empty())
            new_component->Name = component->GetName();
        else
            new_component->Name = new_short_name;

        if(!owner->AddComponent(new_component))
        {
            stor->ReturnObject(new_component);
            return {DomainStatusCode::LinkFailed, "clone_component: AddComponent failed"};
        }

        RDK::MVector<double, 3> coord = new_component->GetCoord();
        coord(0) += 1;
        coord(1) += 1;
        new_component->Coord = coord;

        std::string buffer;
        out_long_name = new_component->GetLongName(model.Get(), buffer);
    }
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::moveComponent(const std::string& long_name,
                                             const std::string& target_parent_long_name,
                                             int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty() || target_parent_long_name.empty())
        return {DomainStatusCode::InvalidPropertyValue,
                "move_component requires long_name and target_parent_long_name"};

    const int rc = MModel_MoveComponent(channel_index, long_name.c_str(),
                                        target_parent_long_name.c_str());
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "move_component failed (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::renameComponent(const std::string& long_name,
                                               const std::string& new_short_name,
                                               int channel_index,
                                               std::string& out_long_name)
{
    out_long_name.clear();
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "rename_component: long_name is required"};
    if(new_short_name.empty())
        return {DomainStatusCode::InvalidPropertyValue, "rename_component: new_short_name is required"};

    {
        RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLock(channel_index);
        if(!eng)
            return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
        RDK::UEnvironment* env = eng->GetEnvironment();
        if(!env)
            return {DomainStatusCode::NotInitialized, "Environment not available"};
        RDK::UEPtr<RDK::UContainer> model = env->GetModel();
        if(!model)
            return {DomainStatusCode::ProjectNotLoaded, "Model not loaded on channel"};

        RDK::UEPtr<RDK::UContainer> comp = model->GetComponentL(long_name.c_str(), true);
        if(!comp)
            return {DomainStatusCode::ComponentNotFound,
                    "rename_component: component not found: " + long_name};

        try
        {
            if(!comp->SetName(new_short_name.c_str()))
                return {DomainStatusCode::InvalidPropertyValue, "rename_component: SetName failed"};
        }
        catch(const RDK::UException& ex)
        {
            return {DomainStatusCode::InvalidPropertyValue,
                    std::string("rename_component: ") + ex.what()};
        }
        catch(const std::exception& ex)
        {
            return {DomainStatusCode::InvalidPropertyValue,
                    std::string("rename_component: ") + ex.what()};
        }

        std::string buffer;
        out_long_name = comp->GetLongName(model.Get(), buffer);
    }
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::reorderComponent(const std::string& long_name, int step,
                                                int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "reorder_component: long_name is required"};
    if(step == 0)
        return {DomainStatusCode::InvalidPropertyValue, "reorder_component: step must be non-zero"};

    const int rc = MModel_ChangeComponentPosition(channel_index, long_name.c_str(), step);
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "reorder_component failed (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::exportComponentToFile(const std::string& long_name,
                                                     const std::string& file_path,
                                                     int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "export_component: long_name is required"};
    if(file_path.empty())
        return {DomainStatusCode::InvalidPropertyValue, "export_component: file_path is required"};

    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(file_path, m_app, path_err))
        return {DomainStatusCode::PolicyDenied, path_err};

    const int rc =
        MModel_SaveComponentToFile(channel_index, long_name.c_str(), file_path.c_str(), 0xFFFFFFFFu);
    if(rc != 0)
        return {DomainStatusCode::IOError,
                "export_component failed (code " + std::to_string(rc) + ")"};
    return {};
}

DomainStatus URdkDomainAccess::importComponentFromFile(const std::string& parent_long_name,
                                                       const std::string& file_path,
                                                       int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(file_path.empty())
        return {DomainStatusCode::InvalidPropertyValue, "import_component: file_path is required"};

    std::string path_err;
    if(!ULLMPathPolicy::isAllowed(file_path, m_app, path_err))
        return {DomainStatusCode::PolicyDenied, path_err};

    const int rc =
        MModel_LoadComponentFromFile(channel_index, parent_long_name.c_str(), file_path.c_str());
    if(rc != 0)
        return {DomainStatusCode::IOError,
                "import_component failed (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::calculateComponent(const std::string& long_name, int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "calculate_component: long_name is required"};

    const int rc = MEnv_Calculate(channel_index, long_name.c_str());
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "calculate_component failed (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::resetComponent(const std::string& long_name, int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "reset_component: long_name is required"};

    const int rc = MEnv_Reset(channel_index, long_name.c_str());
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "reset_component failed (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::defaultComponent(const std::string& long_name,
                                                bool include_subcomponents, int channel_index)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "default_component: long_name is required"};

    const int rc = MEnv_Default(channel_index, long_name.c_str(), include_subcomponents);
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "default_component failed (code " + std::to_string(rc) + ")"};
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::selectComponent(const std::string& long_name, int channel_index,
                                               bool navigate_parent)
{
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "select_component: long_name is required"};

    std::string scope = long_name;
    if(navigate_parent)
    {
        const size_t dot = scope.find_last_of('.');
        if(dot == std::string::npos)
            return {DomainStatusCode::InvalidPropertyValue,
                    "select_component: component has no parent scope"};
        scope = scope.substr(0, dot);
    }

    nlohmann::json found;
    if(!findComponentByLongName(scope, found, channel_index).ok() && scope != "Model")
    {
        // Allow navigating to Model / empty root even if find fails on synthetic names.
        if(!scope.empty())
            return {DomainStatusCode::ComponentNotFound,
                    "select_component: component not found: " + scope};
    }

    if(m_sink)
        m_sink->navigateToDiagramScope(scope, channel_index);
    return {};
}

DomainStatus URdkDomainAccess::connectComponents(const std::string& from_long_name,
                                                 const std::string& from_property,
                                                 const std::string& to_long_name,
                                                 const std::string& to_property,
                                                 int channel_index,
                                                 bool* already_existed_out)
{
    if(already_existed_out)
        *already_existed_out = false;

    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};

    LinkQuad quad;
    quad.from_long_name = from_long_name;
    quad.from_property = from_property;
    quad.to_long_name = to_long_name;
    quad.to_property = to_property;

    nlohmann::json snap;
    const DomainStatus snap_st = listNetSnapshot(snap, channel_index);
    bool already = false;
    if(snap_st.ok() && snapshotContainsLink(snap, quad))
        already = true;
    else if(snap_st.ok() && snap.value("links_truncated", false))
    {
        bool ex = false;
        if(linkExistsInModel(quad, channel_index, "", ex).ok() && ex)
            already = true;
    }
    if(already)
    {
        if(already_existed_out)
            *already_existed_out = true;
        return {};
    }

    const int rc = MModel_CreateLinkByName(channel_index, from_long_name.c_str(),
                                           from_property.c_str(), to_long_name.c_str(),
                                           to_property.c_str());
    if(rc != 0)
    {
        std::ostringstream msg;
        msg << "connect_components failed (code " << rc << "). ";
        msg << "from_property must be a published output on \"" << from_long_name << "\" (got \""
            << from_property << "\"); to_property must be a published input on \"" << to_long_name
            << "\" (got \"" << to_property
            << "\"). Use get_component_properties to list ports, or specify exact port names "
               "(e.g. ComponentName.PortName).";
        return {DomainStatusCode::LinkFailed, msg.str()};
    }
    refreshDiagramPresentation(m_sink);
    return {};
}

DomainStatus URdkDomainAccess::getComponentClassName(const std::string& long_name,
                                                     int channel_index,
                                                     std::string& out_class_name) const
{
    out_class_name.clear();
    nlohmann::json info;
    DomainStatus st = findComponentByLongName(long_name, info, channel_index);
    if(!st.ok())
        return st;
    out_class_name = info.value("class_name", "");
    if(out_class_name.empty())
        return {DomainStatusCode::ClassNotFound,
                "Class name is not available for component " + long_name};
    return {};
}

namespace {

bool isWatchableLanguageType(const std::type_info& ti)
{
    // Mirrors UWatchTab::createSelectionDialog / UMatrixFormDialog type gate.
    return ti == typeid(double) || ti == typeid(int) || ti == typeid(RDK::MDMatrix<double>)
           || ti == typeid(RDK::MDMatrix<int>) || ti == typeid(RDK::MDVector<double>)
           || ti == typeid(RDK::MDVector<int>);
}

bool isScalarWatchableLanguageType(const std::type_info& ti)
{
    return ti == typeid(double) || ti == typeid(int);
}

} // namespace

DomainStatus URdkDomainAccess::validateWatchProperty(const std::string& long_name,
                                                     const std::string& property_name,
                                                     int channel_index, int jx, int jy) const
{
    if(long_name.empty())
        return {DomainStatusCode::ComponentNotFound, "add_watch_series: long_name is required"};
    if(property_name.empty())
        return {DomainStatusCode::PropertyNotFound, "add_watch_series: property_name is required"};
    if(jx < 0 || jy < 0)
    {
        return {DomainStatusCode::InvalidPropertyValue,
                "add_watch_series: jx/jy must be >= 0 (matrix/vector cell indices)"};
    }

    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};

    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};

    RDK::UEnvironment* env = eng->GetEnvironment();
    if(!env)
        return {DomainStatusCode::NotInitialized, "Environment not available"};
    RDK::UEPtr<RDK::UContainer> model = env->GetModel();
    if(!model)
        return {DomainStatusCode::ProjectNotLoaded, "Model not loaded on channel"};

    RDK::UEPtr<RDK::UContainer> cont = model->GetComponentL(long_name.c_str(), true);
    if(!cont)
        return {DomainStatusCode::ComponentNotFound, "Component not found: " + long_name};

    RDK::UEPtr<RDK::UIProperty> prop = cont->FindProperty(property_name);
    if(!prop)
    {
        // Fallback: published lookup lists (same catalog as set_property).
        if(!componentHasProperty(eng.Get(), long_name, property_name))
        {
            return {DomainStatusCode::PropertyNotFound,
                    "Unknown property_name \"" + property_name + "\" for component " + long_name
                        + ". Use get_component_properties / get_component_ports."};
        }
        // Property is published but FindProperty failed — still reject for Watch (need type).
        return {DomainStatusCode::InvalidPropertyValue,
                "Property \"" + property_name + "\" on " + long_name
                    + " is not accessible for Watch series (FindProperty failed)."};
    }

    const std::type_info& ti = prop->GetLanguageType();
    if(!isWatchableLanguageType(ti))
    {
        return {DomainStatusCode::InvalidPropertyValue,
                "Property \"" + property_name + "\" on " + long_name
                    + " is not numeric/matrix Watch-compatible (need int, double, "
                      "MDMatrix/MDVector of int|double)."};
    }
    if(isScalarWatchableLanguageType(ti) && (jx != 0 || jy != 0))
    {
        return {DomainStatusCode::InvalidPropertyValue,
                "Property \"" + property_name + "\" is scalar; use jx=0, jy=0"};
    }
    (void)jy;
    return {};
}

DomainStatus URdkDomainAccess::breakComponentLink(const std::string& from_long_name,
                                                  const std::string& from_property,
                                                  const std::string& to_long_name,
                                                  const std::string& to_property,
                                                  int channel_index)
{
    (void)channel_index;
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    const int rc = Model_BreakLinkByName(from_long_name.c_str(), from_property.c_str(),
                                         to_long_name.c_str(), to_property.c_str());
    if(rc != 0)
        return {DomainStatusCode::LinkFailed,
                "break link failed (code " + std::to_string(rc) + ")"};
    return {};
}

DomainStatus URdkDomainAccess::loadProject(const std::string& path)
{
    if(m_commands)
    {
        const ApplicationCommandResult r = m_commands->loadConfiguration(path, "close");
        if(r.status.ok())
            applyPresentationFromCommand(m_sink, r);
        return r.status;
    }
    if(!m_app)
        return {DomainStatusCode::ProjectNotLoaded, "Application not available"};
    if(path.empty())
        return {DomainStatusCode::InvalidPropertyValue, "project_path is required"};
    if(!m_app->OpenProject(path))
        return {DomainStatusCode::ProjectNotLoaded, "OpenProject failed for " + path};
    return {};
}

DomainStatus URdkDomainAccess::saveProject(const std::string& path_optional)
{
    if(m_commands)
    {
        const ApplicationCommandResult r =
            path_optional.empty() ? m_commands->saveConfiguration()
                                  : m_commands->saveConfigurationAs(path_optional);
        if(r.status.ok())
            applyPresentationFromCommand(m_sink, r);
        return r.status;
    }
    if(!m_app)
        return {DomainStatusCode::ProjectNotLoaded, "Application not available"};
    const bool ok = path_optional.empty() ? m_app->SaveProject() : m_app->SaveProjectAs(path_optional);
    if(!ok)
        return {DomainStatusCode::ProjectNotLoaded, "SaveProject failed"};
    return {};
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
