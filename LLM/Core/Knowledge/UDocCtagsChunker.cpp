#include "UDocCtagsChunker.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace RDK::LLM {

namespace {

std::string trim(const std::string& s)
{
    std::size_t a = 0;
    while(a < s.size() && std::isspace(static_cast<unsigned char>(s[a])))
        ++a;
    std::size_t b = s.size();
    while(b > a && std::isspace(static_cast<unsigned char>(s[b - 1])))
        --b;
    return s.substr(a, b - a);
}

std::string toLowerAscii(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

bool looksLikeControlStatement(const std::string& signature)
{
    const std::string lower = toLowerAscii(signature);
    return lower.find("if(") != std::string::npos || lower.find("if (") != std::string::npos
           || lower.find("for(") != std::string::npos || lower.find("for (") != std::string::npos
           || lower.find("while(") != std::string::npos
           || lower.find("while (") != std::string::npos
           || lower.find("switch(") != std::string::npos
           || lower.find("switch (") != std::string::npos
           || lower.find("catch(") != std::string::npos
           || lower.find("catch (") != std::string::npos;
}

int countChar(const std::string& s, const char ch)
{
    return static_cast<int>(std::count(s.begin(), s.end(), ch));
}

std::string summarizeSignature(const std::string& signature)
{
    std::string out = trim(signature);
    if(out.size() > 160)
        out = out.substr(0, 157) + "...";
    return out.empty() ? "source_chunk" : out;
}

} // namespace

std::vector<UDocCtagsChunker::SourceChunk> UDocCtagsChunker::chunkFile(
    const std::filesystem::path& file_path, const int max_chunk_lines)
{
    std::ifstream in(file_path);
    if(!in)
        return {};

    std::vector<std::string> lines;
    std::string line;
    while(std::getline(in, line))
        lines.push_back(line);

    std::vector<SourceChunk> out;
    const int n = static_cast<int>(lines.size());
    int i = 0;
    while(i < n)
    {
        const std::string line_trim = trim(lines[static_cast<size_t>(i)]);
        if(line_trim.empty() || line_trim[0] == '#')
        {
            ++i;
            continue;
        }

        if(line_trim.find('(') == std::string::npos)
        {
            ++i;
            continue;
        }

        std::string signature = line_trim;
        int signature_start = i;
        int j = i;
        bool saw_open_brace = line_trim.find('{') != std::string::npos;
        while(!saw_open_brace && j + 1 < n && (j - i) < 6)
        {
            const std::string next_trim = trim(lines[static_cast<size_t>(j + 1)]);
            if(next_trim.empty())
            {
                ++j;
                continue;
            }
            signature += " " + next_trim;
            ++j;
            if(next_trim.find('{') != std::string::npos)
                saw_open_brace = true;
            if(next_trim.find(';') != std::string::npos)
                break;
        }

        if(!saw_open_brace || signature.find(';') != std::string::npos
           || looksLikeControlStatement(signature))
        {
            ++i;
            continue;
        }

        int brace_depth = 0;
        int k = i;
        bool started = false;
        for(; k < n; ++k)
        {
            const std::string& ln = lines[static_cast<size_t>(k)];
            const int opens = countChar(ln, '{');
            const int closes = countChar(ln, '}');
            if(opens > 0)
                started = true;
            brace_depth += opens;
            brace_depth -= closes;
            if(started && brace_depth <= 0)
                break;
        }
        if(!started || k >= n)
        {
            ++i;
            continue;
        }

        const int chunk_start = signature_start + 1;
        const int chunk_end = std::min(k + 1, chunk_start + std::max(8, max_chunk_lines) - 1);
        std::ostringstream excerpt;
        for(int ln = chunk_start; ln <= chunk_end; ++ln)
            excerpt << lines[static_cast<size_t>(ln - 1)] << '\n';

        SourceChunk chunk;
        chunk.title = summarizeSignature(signature);
        chunk.excerpt = excerpt.str();
        chunk.start_line = chunk_start;
        if(!chunk.excerpt.empty())
            out.push_back(std::move(chunk));

        i = k + 1;
    }

    return out;
}

} // namespace RDK::LLM
