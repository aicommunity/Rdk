#ifndef @NAMESPACE_NAME@_@HEADER_FILE_NAME@
#define @NAMESPACE_NAME@_@HEADER_FILE_NAME@

#include "../../Rdk/Deploy/Include/rdk.h"

namespace @NAMESPACE_NAME@ {

class RDK_LIB_TYPE @CLASS_NAME@: public RDK::ULibrary
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
@CLASS_NAME@(void);
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------
};

extern RDK_LIB_TYPE @CLASS_NAME@ @LIBRARY_NAME@;

}

#endif


