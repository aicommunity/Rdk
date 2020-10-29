#ifndef UCREATORPROPTEMPLATE_H
#define UCREATORPROPTEMPLATE_H
#include "UStorage.h"
#include "UMockUNet.h"
#include "../../Deploy/Include/rdk.h"

// ����� ��� ��������� ������������� ������ ������ CreatePropertyByType.
// ��������������� ������������� ������� ������ � �����������, ��� ������������ �������-���������� �������
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};


#endif // UCREATORPROPTEMPLATE_H
