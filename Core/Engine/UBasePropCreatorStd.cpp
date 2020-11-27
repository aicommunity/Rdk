#include "UBasePropCreatorStd.h"
#include "UComponent.h"


namespace RDK {

// Основная функция создания свойств всех возможных типов (добавляется в Storage)
bool UBasePropCreatorStd::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreatorStd>(serstorage,mock_unet);
}

}
