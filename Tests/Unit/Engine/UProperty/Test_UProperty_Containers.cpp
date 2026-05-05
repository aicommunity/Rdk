#include <gtest/gtest.h>
#include "rdk.h"
#include <vector>
#include <string>

using namespace RDK;

class ContainerTestComponent : public UNet
{
public:
    UProperty<std::vector<int>, ContainerTestComponent, ptPubParameter> IntVector;
    UProperty<MDMatrix<double>, ContainerTestComponent, ptPubParameter> Matrix;
    UProperty<std::vector<std::string>, ContainerTestComponent, ptPubParameter> StringVector;

    ContainerTestComponent()
        : IntVector("IntVector", this), Matrix("Matrix", this), StringVector("StringVector", this)
    {
    }
};

TEST(UPropertyContainersCompat, VectorAndMatrixRoundTrip)
{
    ContainerTestComponent comp;

    std::vector<int> vec = {1, 2, 3};
    comp.IntVector = vec;
    ASSERT_EQ(comp.IntVector.size(), 3u);
    EXPECT_EQ(comp.IntVector[0], 1);
    EXPECT_EQ(comp.IntVector[2], 3);

    MDMatrix<double> m(2, 2);
    m(0, 0) = 1.0;
    m(1, 1) = 2.0;
    comp.Matrix = m;

    EXPECT_EQ(comp.Matrix->GetRows(), 2);
    EXPECT_EQ(comp.Matrix->GetCols(), 2);
    EXPECT_DOUBLE_EQ((*comp.Matrix)(0, 0), 1.0);
    EXPECT_DOUBLE_EQ((*comp.Matrix)(1, 1), 2.0);
}

TEST(UPropertyContainersCompat, StringVectorRoundTrip)
{
    ContainerTestComponent comp;
    comp.StringVector = std::vector<std::string>{"one", "two"};

    ASSERT_EQ(comp.StringVector.size(), 2u);
    EXPECT_EQ(comp.StringVector[0], "one");
    EXPECT_EQ(comp.StringVector[1], "two");
}
