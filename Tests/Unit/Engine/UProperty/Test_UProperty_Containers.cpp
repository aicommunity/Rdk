#include <gtest/gtest.h>
#include "../../../Deploy/Include/rdk.h"
#include <vector>
#include <string>

using namespace RDK;

// Test class for container properties
class ContainerTestComponent : public UNet
{
public:
    UProperty<std::vector<int>, ContainerTestComponent, ptPubOutput> IntVectorOutput;
    UProperty<std::vector<int>, ContainerTestComponent, ptPubInput> IntVectorInput;
    UProperty<MDMatrix<double>, ContainerTestComponent, ptPubOutput> MatrixOutput;
    UProperty<MDMatrix<double>, ContainerTestComponent, ptPubInput> MatrixInput;
    UProperty<std::vector<std::string>, ContainerTestComponent, ptPubParameter> StringVector;

    ContainerTestComponent() : 
        IntVectorOutput("IntVectorOutput", this),
        IntVectorInput("IntVectorInput", this),
        MatrixOutput("MatrixOutput", this),
        MatrixInput("MatrixInput", this),
        StringVector("StringVector", this)
    {
    }
};

class UPropertyContainersTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        comp1 = std::make_unique<ContainerTestComponent>();
        comp2 = std::make_unique<ContainerTestComponent>();
    }

    void TearDown() override
    {
        comp1.reset();
        comp2.reset();
    }

    std::unique_ptr<ContainerTestComponent> comp1;
    std::unique_ptr<ContainerTestComponent> comp2;
};

// Test std::vector<int> property
TEST_F(UPropertyContainersTest, IntVectorProperty)
{
    std::vector<int> testVec = {1, 2, 3, 4, 5};
    comp1->IntVectorOutput = testVec;
    
    EXPECT_EQ(comp1->IntVectorOutput()->size(), 5);
    EXPECT_EQ(comp1->IntVectorOutput()[0], 1);
    EXPECT_EQ(comp1->IntVectorOutput()[4], 5);
}

// Test MDMatrix property
TEST_F(UPropertyContainersTest, MDMatrixProperty)
{
    MDMatrix<double> matrix(3, 3);
    matrix(0, 0) = 1.0;
    matrix(1, 1) = 2.0;
    matrix(2, 2) = 3.0;
    
    comp1->MatrixOutput = matrix;
    
    EXPECT_EQ(comp1->MatrixOutput()->GetRows(), 3);
    EXPECT_EQ(comp1->MatrixOutput()->GetCols(), 3);
    EXPECT_DOUBLE_EQ((*comp1->MatrixOutput)(0, 0), 1.0);
    EXPECT_DOUBLE_EQ((*comp1->MatrixOutput)(1, 1), 2.0);
    EXPECT_DOUBLE_EQ((*comp1->MatrixOutput)(2, 2), 3.0);
}

// Test vector connection
TEST_F(UPropertyContainersTest, VectorConnection)
{
    std::vector<int> testVec = {10, 20, 30};
    comp1->IntVectorOutput = testVec;
    comp2->IntVectorInput.Connect(&comp1->IntVectorOutput);
    
    EXPECT_EQ(comp2->IntVectorInput()->size(), 3);
    EXPECT_EQ(comp2->IntVectorInput()[0], 10);
    EXPECT_EQ(comp2->IntVectorInput()[1], 20);
    EXPECT_EQ(comp2->IntVectorInput()[2], 30);
}

// Test matrix connection
TEST_F(UPropertyContainersTest, MatrixConnection)
{
    MDMatrix<double> matrix(2, 2);
    matrix(0, 0) = 1.5;
    matrix(0, 1) = 2.5;
    matrix(1, 0) = 3.5;
    matrix(1, 1) = 4.5;
    
    comp1->MatrixOutput = matrix;
    comp2->MatrixInput.Connect(&comp1->MatrixOutput);
    
    EXPECT_EQ(comp2->MatrixInput()->GetRows(), 2);
    EXPECT_EQ(comp2->MatrixInput()->GetCols(), 2);
    EXPECT_DOUBLE_EQ((*comp2->MatrixInput)(0, 0), 1.5);
    EXPECT_DOUBLE_EQ((*comp2->MatrixInput)(1, 1), 4.5);
}

// Test vector modification
TEST_F(UPropertyContainersTest, VectorModification)
{
    std::vector<int> vec = {1, 2, 3};
    comp1->IntVectorOutput = vec;
    
    // Modify through property
    comp1->IntVectorOutput->push_back(4);
    EXPECT_EQ(comp1->IntVectorOutput()->size(), 4);
    EXPECT_EQ(comp1->IntVectorOutput()[3], 4);
}

// Test string vector property
TEST_F(UPropertyContainersTest, StringVectorProperty)
{
    std::vector<std::string> strVec = {"one", "two", "three"};
    comp1->StringVector = strVec;
    
    EXPECT_EQ(comp1->StringVector()->size(), 3);
    EXPECT_EQ(comp1->StringVector()[0], "one");
    EXPECT_EQ(comp1->StringVector()[2], "three");
}

// Test empty vector
TEST_F(UPropertyContainersTest, EmptyVector)
{
    std::vector<int> emptyVec;
    comp1->IntVectorOutput = emptyVec;
    
    EXPECT_EQ(comp1->IntVectorOutput()->size(), 0);
    EXPECT_TRUE(comp1->IntVectorOutput()->empty());
}

// Test large vector performance
TEST_F(UPropertyContainersTest, LargeVector)
{
    std::vector<int> largeVec(10000);
    for (size_t i = 0; i < largeVec.size(); ++i)
    {
        largeVec[i] = static_cast<int>(i);
    }
    
    comp1->IntVectorOutput = largeVec;
    comp2->IntVectorInput.Connect(&comp1->IntVectorOutput);
    
    EXPECT_EQ(comp2->IntVectorInput()->size(), 10000);
    EXPECT_EQ(comp2->IntVectorInput()[0], 0);
    EXPECT_EQ(comp2->IntVectorInput()[9999], 9999);
}

// Test matrix resize
TEST_F(UPropertyContainersTest, MatrixResize)
{
    MDMatrix<double> matrix(2, 2);
    comp1->MatrixOutput = matrix;
    
    comp1->MatrixOutput->Resize(5, 5);
    EXPECT_EQ(comp1->MatrixOutput()->GetRows(), 5);
    EXPECT_EQ(comp1->MatrixOutput()->GetCols(), 5);
}

