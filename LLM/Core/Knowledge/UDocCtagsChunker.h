#ifndef RDK_UDOC_CTAGS_CHUNKER_H
#define RDK_UDOC_CTAGS_CHUNKER_H

#include <filesystem>
#include <string>
#include <vector>

namespace RDK::LLM {

class UDocCtagsChunker {
public:
    struct SourceChunk {
        std::string title;
        std::string excerpt;
        int start_line = 1;
    };

    /// Heuristic ctags-like function chunking for source retrieval.
    static bool isAvailable() { return true; }
    static std::vector<SourceChunk> chunkFile(const std::filesystem::path& file_path,
                                              int max_chunk_lines = 120);
};

} // namespace RDK::LLM

#endif
