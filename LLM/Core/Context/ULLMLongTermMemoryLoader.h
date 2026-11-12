#ifndef RDK_ULLM_LONG_TERM_MEMORY_LOADER_H
#define RDK_ULLM_LONG_TERM_MEMORY_LOADER_H

#include <filesystem>
#include <string>

namespace RDK::LLM {

std::string loadLongTermMemoryBlock(const std::filesystem::path& repository_root, int user_id,
                                    const std::string& project_xml_path);

} // namespace RDK::LLM

#endif
