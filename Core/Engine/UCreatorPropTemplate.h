#ifndef UCREATORPROPTEMPLATE_H
#define UCREATORPROPTEMPLATE_H
#include "UStorage.h"
#include "UMockUNet.h"
#include "ModernSmartPointers.h"

namespace RDK {

// ����� ��� ��������� ������������� ������ ������ CreatePropertyByType.
// ��������������� ������������� ������� ������ � �����������, ��� ������������ �������-���������� �������
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static void CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet, unsigned int ptype)
    {
        std::string prop_name = serstorage->GetNodeName();
        auto p = make_ueptr<PropType<T, UMockUNet, TypeInt>>(prop_name, mock_unet);
        mock_unet->ChangeLookupPropertyType(prop_name,ptype);
        p->Load(serstorage);
    }
};


}


#endif // UCREATORPROPTEMPLATE_H
