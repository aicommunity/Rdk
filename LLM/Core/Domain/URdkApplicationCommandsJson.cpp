#include "URdkApplicationCommandsJson.h"

#include <vector>

#include <rdk_application.h>
#include "../../../Core/Engine/TProjectLoadDiagnostics.h"

namespace RDK::LLM {

nlohmann::json diagnosticsToJson(const RDK::TProjectLoadDiagnostics& d)
{
    nlohmann::json j;
    j["is_valid"] = d.isValid;
    j["model_exists"] = d.modelExists;
    j["model_empty"] = d.modelEmpty;
    j["channels_loaded"] = d.channelsLoaded;
    j["channels_total"] = d.channelsTotal;
    j["errors"] = d.errors;
    j["warnings"] = d.warnings;
    j["missing_files"] = d.missingFiles;
    j["failed_channels"] = d.failedChannels;
    j["components_count"] = d.componentsCount;
    return j;
}

} // namespace RDK::LLM
