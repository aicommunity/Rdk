#ifndef @NAMESPACE_NAME@_@CPP_FILE_NAME@
#define @NAMESPACE_NAME@_@CPP_FILE_NAME@

#include "@HEADER_FILE_NAME@"

namespace @NAMESPACE_NAME@ {

@CLASS_NAME@ @LIBRARY_NAME@;



// --------------------------
// ������������ � �����������
// --------------------------
@CLASS_NAME@::@CLASS_NAME@(void)
 : ULibrary("@LIBRARY_NAME@","1.0")
{
}
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void @CLASS_NAME@::CreateClassSamples(UStorage *storage)
{
 UContainer *cont(0);
/*
 cont=new UStatisticMatrix<int>;
 cont->SetName("StatisticIntMatrix");
 cont->Default();
 UploadClass("UStatisticIntMatrix",cont);
*/
}
// --------------------------

}

#endif


