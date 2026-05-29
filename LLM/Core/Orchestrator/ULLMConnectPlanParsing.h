#ifndef RDK_ULLM_CONNECT_PLAN_PARSING_H
#define RDK_ULLM_CONNECT_PLAN_PARSING_H

#include <optional>
#include <string>
#include <vector>

namespace RDK::LLM {

enum class ConnectRemainingScope {
    SessionDelta,
    ModelGraph,
};

enum class ConnectTopology {
    Sequential,
    Chain,
    Tree,
};

enum class ConnectGoalKind {
    None,
    ExplicitPairs,
    ExplicitPorts,
    CountOnly,
    RemainingSessionDelta,
    AnalogousToPrevious,
};

struct ConnectEndpointRef {
    std::string token;
    std::string property;
    bool looks_like_long_name = false;
};

struct ConnectLinkSpec {
    ConnectEndpointRef from;
    ConnectEndpointRef to;
};

struct ParsedConnectGoal {
    ConnectGoalKind kind = ConnectGoalKind::None;
    std::vector<ConnectLinkSpec> explicit_links;
    int link_count = 0;
    bool wants_remaining = false;
    bool wants_analogous = false;
    ConnectRemainingScope remaining_scope = ConnectRemainingScope::SessionDelta;
    ConnectTopology topology = ConnectTopology::Sequential;
    std::optional<std::string> hub_token;
    bool wants_internal_semantics_hint = false;
};

bool isConnectGoalText(const std::string& text);
bool isDisconnectGoalText(const std::string& text);
ParsedConnectGoal parseConnectGoal(const std::string& goal_en);

} // namespace RDK::LLM

#endif
