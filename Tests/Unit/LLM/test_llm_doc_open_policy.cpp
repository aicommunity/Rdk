#include <gtest/gtest.h>

#include "Context/ULLMDocOpenPolicy.h"

#include <filesystem>
#include <fstream>
#include <unistd.h>

using namespace RDK::LLM;
namespace fs = std::filesystem;

namespace {

fs::path makeTempRepo()
{
    const fs::path root =
        fs::temp_directory_path() / ("nmsdk_doc_open_" + std::to_string(::getpid()));
    fs::create_directories(root / "Docs");
    fs::create_directories(root / "Bin" / "Docs");
    fs::create_directories(root / "Bin" / "Help" / "en");
    fs::create_directories(root / "Libraries" / "Nmsdk-PulseLib" / "Docs");
    fs::create_directories(root / "secret");
    {
        std::ofstream(root / "Docs" / "guide.md") << "# Guide\n";
        std::ofstream(root / "Libraries" / "Nmsdk-PulseLib" / "Docs" / "pulse.md") << "# Pulse\n";
        std::ofstream(root / "Bin" / "Help" / "en" / "index.html") << "<html></html>";
        std::ofstream(root / "Bin" / "Help" / "en" / "getting-started.html") << "<html></html>";
        std::ofstream(root / "secret" / "nope.md") << "# Nope\n";
    }
    return root;
}

} // namespace

TEST(LLMDocOpenPolicy, AllowsCatalogMarkdownRoots)
{
    const fs::path root = makeTempRepo();
    EXPECT_TRUE(isAllowedMarkdownDocPath(root / "Docs" / "guide.md", root));
    EXPECT_TRUE(isAllowedMarkdownDocPath(root / "Libraries" / "Nmsdk-PulseLib" / "Docs" / "pulse.md",
                                         root));
    EXPECT_FALSE(isAllowedMarkdownDocPath(root / "secret" / "nope.md", root));
    fs::remove_all(root);
}

TEST(LLMDocOpenPolicy, UriRoundTripMarkdownAndHelpAndClass)
{
    const fs::path root = makeTempRepo();
    const DocOpenResolve md = resolveDocUri("nmsdk-doc:Docs/guide.md", root);
    ASSERT_TRUE(md.ok);
    EXPECT_EQ(md.kind, "markdown");
    EXPECT_EQ(md.doc_uri, "nmsdk-doc:Docs/guide.md");

    const DocOpenResolve help = resolveDocUri("nmsdk-help:getting-started.html", root);
    ASSERT_TRUE(help.ok);
    EXPECT_EQ(help.kind, "help");
    EXPECT_EQ(help.help_topic, "getting-started.html");

    const DocOpenResolve cls = resolveDocUri("nmsdk-class:NSPNeuron", root);
    ASSERT_TRUE(cls.ok);
    EXPECT_EQ(cls.kind, "class");
    EXPECT_EQ(cls.class_name, "NSPNeuron");

    EXPECT_FALSE(resolveDocUri("nmsdk-doc:secret/nope.md", root).ok);
    EXPECT_FALSE(resolveDocUri("http://evil", root).ok);
    fs::remove_all(root);
}

TEST(LLMDocOpenPolicy, MakeDocUri)
{
    EXPECT_EQ(makeDocUriFromRepoRelative("Rdk/LLM/Docs/x.md"), "nmsdk-doc:Rdk/LLM/Docs/x.md");
    EXPECT_EQ(makeHelpUri(""), "nmsdk-help:index.html");
    EXPECT_EQ(makeClassUri("Foo"), "nmsdk-class:Foo");
}
