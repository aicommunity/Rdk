#ifndef RDK_UDOC_CTAGS_CHUNKER_H
#define RDK_UDOC_CTAGS_CHUNKER_H

// Post-MVP TD-034: function-level source chunks via ctags for scope=sources.

namespace RDK::LLM {

class UDocCtagsChunker {
public:
    /// Returns false until ctags pipeline is implemented.
    static bool isAvailable() { return false; }
};

} // namespace RDK::LLM

#endif
