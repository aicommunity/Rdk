#ifndef @NAMESPACE_NAME@_@CLASS_NAME@_CPP
#define @NAMESPACE_NAME@_@CLASS_NAME@_CPP

#include "@HEADER_FILE_NAME@"

namespace @NAMESPACE_NAME@ {

@CLASS_NAME@ @LIBRARY_NAME@;



// --------------------------
// ������������ � �����������
// --------------------------
@CLASS_NAME@::@CLASS_NAME@(void)
 : RDK::ULibrary("@LIBRARY_NAME@","1.0")
{
}
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void @CLASS_NAME@::CreateClassSamples(RDK::UStorage *storage)
{
 RDK::UContainer *cont(0);
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


