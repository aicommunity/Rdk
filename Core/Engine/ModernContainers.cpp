/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#include "ModernContainers.h"
#include <algorithm>
#include <cctype>

namespace RDK {

// ============================================================================
// String Operations Implementation
// ============================================================================

std::string_view TrimStringView(std::string_view str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos) {
        return {};
    }
    
    auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool StartsWith(std::string_view str, std::string_view prefix) {
    return str.size() >= prefix.size() && 
           str.substr(0, prefix.size()) == prefix;
}

bool EndsWith(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() && 
           str.substr(str.size() - suffix.size()) == suffix;
}

std::vector<std::string_view> SplitStringView(std::string_view str, char delimiter) {
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string_view::npos) {
        if (start != end) {
            result.emplace_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find(delimiter, start);
    }
    
    if (start < str.size()) {
        result.emplace_back(str.substr(start));
    }
    
    return result;
}

} // namespace RDK







