#include <gtest/gtest.h>

#include "rdk.h"
#include "../Core/Math/MDMatrix.h"
#include "../Core/Math/MMatrix.h"
#include "../Core/Serialize/USerStorageXML.h"

using namespace RDK;

namespace {

void ExpectCleanMatrixNodeText(const std::string& text)
{
    EXPECT_FALSE(text.empty());
    EXPECT_NE('\n', text.front());
    EXPECT_NE('\t', text.back());
}

} // namespace

TEST(MatrixNodeTextFormat, MDMatrixSaveProducesCleanNodeText)
{
    USerStorageXML storage;
    MDMatrix<double> matrix(2, 3);
    matrix(0, 0) = 1.5;
    matrix(0, 1) = 2.5;
    matrix(0, 2) = 3.5;
    matrix(1, 0) = 4.5;
    matrix(1, 1) = 5.5;
    matrix(1, 2) = 6.5;

    storage.Create("Matrix");
    storage << matrix;

    const std::string text = storage.GetNodeText();
    ExpectCleanMatrixNodeText(text);
    EXPECT_NE(text.find('\t'), std::string::npos);
    EXPECT_NE(text.find('\n'), std::string::npos);
    EXPECT_EQ(text.find('\n'), text.rfind('\n'));
}

TEST(MatrixNodeTextFormat, MDMatrixLegacyNodeTextStillLoads)
{
    USerStorageXML storage;
    storage.Create("Matrix");
    storage.SetNodeAttribute("Type", std::string("MDMatrix<") + typeid(double).name() + ">");
    storage.SetNodeAttribute("Rows", "2");
    storage.SetNodeAttribute("Cols", "3");
    storage.SetNodeText("\n1\t2\t3\t\n4\t5\t6\t");

    MDMatrix<double> matrix;
    storage >> matrix;

    EXPECT_EQ(matrix.GetRows(), 2);
    EXPECT_EQ(matrix.GetCols(), 3);
    EXPECT_DOUBLE_EQ(matrix(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(matrix(0, 2), 3.0);
    EXPECT_DOUBLE_EQ(matrix(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(matrix(1, 2), 6.0);
}

TEST(MatrixNodeTextFormat, MDMatrixEmptyIsEmptyText)
{
    USerStorageXML storage;
    MDMatrix<double> matrix(0, 0);

    storage.Create("Matrix");
    storage << matrix;

    EXPECT_TRUE(storage.GetNodeText().empty());
}

TEST(MatrixNodeTextFormat, MMatrixFixedSizeUsesCleanFormat)
{
    USerStorageXML storage;
    MMatrix<double, 2, 2> matrix;
    matrix.Data[0][0] = 1.0;
    matrix.Data[0][1] = 2.0;
    matrix.Data[1][0] = 3.0;
    matrix.Data[1][1] = 4.0;

    storage.Create("Matrix");
    storage << matrix;

    const std::string text = storage.GetNodeText();
    ExpectCleanMatrixNodeText(text);
}

TEST(MatrixNodeTextFormat, NormalizeMatrixPropertyTextStripsLegacyArtifacts)
{
    const std::string legacy = "\n1\t2\t3\t\n4\t5\t6\t";
    const std::string normalized = NormalizeMatrixPropertyText(legacy);
    ExpectCleanMatrixNodeText(normalized);
    EXPECT_EQ(normalized, "1\t2\t3\n4\t5\t6");
}
