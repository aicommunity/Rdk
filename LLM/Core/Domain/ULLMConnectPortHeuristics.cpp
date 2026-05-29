#include "ULLMConnectPortHeuristics.h"

#include <algorithm>

namespace RDK::LLM {

bool isGenericLinkPortName(const std::string& value)
{
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return lower == "input" || lower == "output";
}

std::optional<std::string> pickPreferredOutputPort(const std::vector<std::string>& outputs)
{
    if(outputs.empty())
        return std::nullopt;
    if(outputs.size() == 1)
        return outputs.front();
    for(const std::string& port : outputs)
    {
        if(port == "Output")
            return port;
    }
    for(const std::string& port : outputs)
    {
        if(port.find("ExcSynapse1") != std::string::npos)
            return port;
    }
    for(const std::string& port : outputs)
    {
        if(port.find("LTZone") != std::string::npos)
            return port;
    }
    return outputs.front();
}

std::optional<std::string> pickPreferredInputPort(const std::vector<std::string>& inputs)
{
    if(inputs.empty())
        return std::nullopt;
    if(inputs.size() == 1)
        return inputs.front();
    for(const std::string& port : inputs)
    {
        if(port == "Input")
            return port;
    }
    for(const std::string& port : inputs)
    {
        if(port.find("Soma1.ExcSynapse1") != std::string::npos)
            return port;
    }
    for(const std::string& port : inputs)
    {
        if(port.find("ExcSynapse1") != std::string::npos)
            return port;
    }
    return inputs.front();
}

} // namespace RDK::LLM
