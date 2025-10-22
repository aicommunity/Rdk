#ifndef UCREATORPROPTEMPLATE_H
#define UCREATORPROPTEMPLATE_H
#include "UStorage.h"
#include "UMockUNet.h"
#include "ModernSmartPointers.h"
#include <memory>
#include "ModernProperties.h"

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
        auto p = std::make_shared<PropType<T, UMockUNet, TypeInt>>(prop_name, mock_unet);
        mock_unet->ChangeLookupPropertyType(prop_name,ptype);
        p->Load(std::shared_ptr<USerStorage>(serstorage));
    }

    // Modern C++20 property creation (simplified without concepts)
    template<typename U = T>
    static std::shared_ptr<PropType<U, UMockUNet, TypeInt>> CreateModernProperty(
        const std::string& name,
        UMockUNet* owner,
        U&& value) {
        auto prop = std::make_shared<PropType<U, UMockUNet, TypeInt>>(name, owner);
        if constexpr (std::is_assignable_v<U&, U>) {
            prop->SetData(std::forward<U>(value));
        }
        return prop;
    }
};


}


#endif // UCREATORPROPTEMPLATE_H
