#include <gtest/gtest.h>

#include "Providers/UOllamaModelInfo.h"

TEST(OllamaModelInfo, InfersQwenFromModelfile)
{
    const std::string modelfile = "TEMPLATE \"\"\"<|im_start|>system\n{{ .System }}\n\"\"\"";
    EXPECT_EQ(RDK::LLM::inferTemplateFamilyFromModelfile(modelfile),
              RDK::LLM::OllamaChatTemplateFamily::Qwen2);
}

TEST(OllamaModelInfo, InfersLlama3FromModelfile)
{
    const std::string modelfile = "TEMPLATE \"\"\"<|begin_of_text|><|start_header_id|>system<|end_header_id|>\"\"\"";
    EXPECT_EQ(RDK::LLM::inferTemplateFamilyFromModelfile(modelfile),
              RDK::LLM::OllamaChatTemplateFamily::Llama3);
}
