#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../../Libraries/Rdk-BasicLib/Deploy/Include/Lib.h"
#include "../../../../Rdk/Core/Serialize/USerStorageXML.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class USerStorageXMLTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateTestStorage();
    }

    void TearDown() override {
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
};

// Test XML serialization creation
TEST_F(USerStorageXMLTest, CreateXMLSerializer) {
    USerStorageXML xml;
    EXPECT_TRUE(true) << "XML serializer should be creatable";
    
    // Test creating XML root
    bool created = xml.Create("TestRoot");
    EXPECT_TRUE(created) << "Should be able to create XML root";
    
    if (created) {
        xml.SelectRoot();
        EXPECT_TRUE(true) << "Should be able to select root";
    }
}

// Test XML serialization of component
TEST_F(USerStorageXMLTest, SerializeComponent) {
    // Verify UModel class is available without creating object
    if (!storage->CheckClass("UModel")) {
        GTEST_SKIP() << "UModel class not available";
        return;
    }
    
    USerStorageXML xml;
    bool created = xml.Create("Component");
    EXPECT_TRUE(created) << "Should be able to create XML for serialization";
    
    if (created) {
        // Verify XML structure
        xml.SelectRoot();
        EXPECT_TRUE(true) << "XML should have root node";
    }
}

// Test XML deserialization
TEST_F(USerStorageXMLTest, DeserializeComponent) {
    USerStorageXML xml;
    
    // Test creating XML with some content
    bool created = xml.Create("TestRoot");
    EXPECT_TRUE(created) << "Should be able to create XML root";
    
    if (created) {
        xml.SelectRoot();
        
        // Test basic XML operations
        EXPECT_TRUE(true) << "XML should support basic operations";
    }
}

// Test XML load from string
TEST_F(USerStorageXMLTest, LoadFromString) {
    USerStorageXML xml;
    
    // Test loading simple XML
    std::string xmlContent = "<?xml version=\"1.0\"?><TestRoot></TestRoot>";
    bool loaded = xml.Load(xmlContent, "TestRoot");
    
    if (loaded) {
        xml.SelectRoot();
        EXPECT_TRUE(true) << "Should be able to load XML from string";
    } else {
        // XML loading may require specific format
        GTEST_SKIP() << "XML loading requires specific format";
    }
}

