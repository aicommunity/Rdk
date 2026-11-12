#include <gtest/gtest.h>

#include <cstdlib>
#include <fstream>

#include "NmsdkBuiltinKnowledgeCatalog.h"

#ifndef CMAKE_SOURCE_DIR
#define CMAKE_SOURCE_DIR "."
#endif

TEST(LLMKnowledgeCatalog, LoadedLibrariesSingleSource)
{
    const auto libs = NmsdkBuiltinKnowledgeCatalog::loadedLibraries();
    ASSERT_EQ(libs.size(), 5u);
    EXPECT_EQ(libs.front().library_id, "Rdk-BasicLib");
    EXPECT_EQ(libs.front().cl_desc_folder, "BasicLibrary");
}

TEST(LLMKnowledgeCatalog, WriteLlmsTxt)
{
    ::setenv("NMSDK_ROOT", CMAKE_SOURCE_DIR, 1);
    NmsdkBuiltinKnowledgeCatalog catalog(nullptr);
    const std::filesystem::path out_dir =
        std::filesystem::temp_directory_path() / "nmsdk_llms_txt_test";
    std::filesystem::create_directories(out_dir / "Docs");

    ASSERT_TRUE(catalog.writeLlmsTxt(out_dir));
    const std::filesystem::path llms = out_dir / "Docs/llms.txt";
    ASSERT_TRUE(std::filesystem::is_regular_file(llms));

    std::ifstream in(llms);
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_NE(content.find("Rdk-HardwareLib"), std::string::npos);
    EXPECT_NE(content.find("rdk.core"), std::string::npos);

    std::filesystem::remove_all(out_dir);
}
