#include "UBasePropCreatorVector.h"
#include "UComponent.h"


namespace RDK {

// Основная функция создания свойств всех возможных типов (добавляется в Storage)
bool UBasePropCreatorVector::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreatorVector>(serstorage,mock_unet);
}



}
