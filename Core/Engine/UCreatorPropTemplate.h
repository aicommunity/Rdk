#ifndef UCREATORPROPTEMPLATE_H
#define UCREATORPROPTEMPLATE_H
#include "UStorage.h"
#include "UMockUNet.h"

namespace RDK {

// ����� ��� ��������� ������������� ������ ������ CreatePropertyByType.
// ��������������� ������������� ������� ������ � �����������, ��� ������������ �������-���������� �������
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static void CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
    {
        std::string prop_name = serstorage->GetNodeName();
        PropType<T, UMockUNet, TypeInt>* p = new PropType<T, UMockUNet, TypeInt>(prop_name, mock_unet);
        p->Load(serstorage);
    }
};


}


#endif // UCREATORPROPTEMPLATE_H
