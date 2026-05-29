#include "URdkDomainAccess.h"
#include "ULLMLinkIdentity.h"
#include "ULLMModelLinkWalker.h"
#include "URdkApplicationCommands.h"
#include "../Gui/ILLMPresentationSink.h"
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

#include <regex>

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
                                              int limit) const
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
    const ModelLinkWalkResult walk = walkModelLinks(walk_root, model_root, opts);

    nlohmann::json links = nlohmann::json::array();
    for(const LinkQuad& q : walk.links)
        links.push_back(linkQuadToJson(q));

    out["channel_index"] = channel_index;
    if(!root_long_name.empty())
        out["root_long_name"] = root_long_name;
    out["offset"] = opts.offset;
    out["limit"] = effective_limit;
    out["returned_count"] = static_cast<int>(walk.links.size());
    out["total_links_seen"] = walk.total_quads_seen;
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
    (void)short_name;
    const DomainSessionInfo session = sessionInfo();
    if(!session.engine_ready)
        return {DomainStatusCode::NotInitialized, "Engine not ready"};
    if(m_app && !session.project_loaded)
        return {DomainStatusCode::ProjectNotLoaded, "No configuration is open"};
    const char* added =
        MModel_AddComponent(channel_index, parent_long_name.c_str(), class_name.c_str());
    if(!added || !added[0])
    {
        return {DomainStatusCode::LinkFailed,
                "add_component failed for class " + class_name + " under parent \"" + parent_long_name
                    + "\""};
    }
    out_long_name = added;
    nlohmann::json found;
    if(findComponentByLongName(added, found, channel_index).ok()
       && found.contains("long_name"))
        out_long_name = found["long_name"].get<std::string>();
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
    RDK::UELockPtr<RDK::UEngine> eng = RDK::GetEngineLockTimeout(channel_index, 500);
    if(!eng)
        return {DomainStatusCode::NotInitialized, "Engine lock unavailable"};
    if(!componentHasProperty(eng.Get(), long_name, property_name))
    {
        return {DomainStatusCode::PropertyNotFound,
                "Unknown property_name \"" + property_name + "\" for component " + long_name
                    + ". Use get_component_properties to list available properties."};
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
