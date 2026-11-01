#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include <limits>

#include "rdk.h"
#include "../Core/Math/MDMatrix.h"
#include "../Core/Math/MMatrix.h"
#include "../Core/Serialize/USerStorageXML.h"

using namespace RDK;

TEST(XmlTextEncoding, Utf8BomAndDeclarationPreserveNumericTagsAndText)
{
    const std::string body="<Project><Channels><00><Value>4000</Value></00></Channels><Text>&lt;x&gt; &amp; text</Text></Project>";
    for(const std::string& prefix:std::vector<std::string>{"","\xef\xbb\xbf","<?xml version=\"1.0\" encoding=\"UTF-8\"?>","\xef\xbb\xbf<?xml version=\"1.0\" encoding=\"UTF-8\"?>"})
    {
        SCOPED_TRACE(prefix);
        USerStorageXML storage;ASSERT_TRUE(storage.Load(prefix+body,"Project"));
        ASSERT_TRUE(storage.SelectNode("Channels"));ASSERT_TRUE(storage.SelectNode("00"));
        ASSERT_TRUE(storage.SelectNode("Value"));EXPECT_EQ(storage.GetNodeText(),"4000");
        storage.SelectRoot();ASSERT_TRUE(storage.SelectNode("Text"));EXPECT_EQ(storage.GetNodeText(),"<x> & text");
    }
}

TEST(XmlTextEncoding, BomDoesNotManufactureMissingRootOrContent)
{
    USerStorageXML storage;
    EXPECT_FALSE(storage.Load("\xef\xbb\xbf","Project"));
    EXPECT_FALSE(storage.Load("\xef\xbb\xbf<Other/>","Project"));
    ASSERT_TRUE(storage.Load("<Project><Text>old</Text></Project>","Project"));
    EXPECT_FALSE(storage.Load("\xef\xbb\xbf","Project"));
}

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

namespace {
template<class T> void ExpectExactFloatingMatrixRoundtrip()
{
    MDMatrix<T> before(2,3);
    before(0,0)=std::nextafter(T(1),T(2));
    before(0,1)=std::nextafter(T(1e-9),T(2e-9));
    before(0,2)=T(-.01234567890123456789L);
    before(1,0)=std::numeric_limits<T>::min();
    before(1,1)=std::numeric_limits<T>::max();
    before(1,2)=T(.001234567890123456789L);
    USerStorageXML source;ASSERT_TRUE(source.Create("Matrix"));source << before;
    std::string text;ASSERT_TRUE(source.Save(text));
    USerStorageXML loaded;ASSERT_TRUE(loaded.Load(text,"Matrix"));
    MDMatrix<T> after;loaded >> after;
    ASSERT_EQ(after.GetRows(),2);ASSERT_EQ(after.GetCols(),3);
    for(int r=0;r<2;++r)for(int c=0;c<3;++c)
    {
        SCOPED_TRACE(std::to_string(r)+","+std::to_string(c));
        EXPECT_EQ(after(r,c),before(r,c));
    }
}
}

TEST(MatrixNodeTextFormat, MDMatrixFloatRoundtripPreservesAllSignificantDigits)
{ ExpectExactFloatingMatrixRoundtrip<float>(); }
TEST(MatrixNodeTextFormat, MDMatrixDoubleRoundtripPreservesAllSignificantDigits)
{ ExpectExactFloatingMatrixRoundtrip<double>(); }
TEST(MatrixNodeTextFormat, MDMatrixLongDoubleRoundtripPreservesAllSignificantDigits)
{ ExpectExactFloatingMatrixRoundtrip<long double>(); }

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
