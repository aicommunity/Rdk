#include "UBasePropCreator.h"
#include "UComponent.h"


namespace RDK {

// Основная функция создания свойств всех возможных типов (добавляется в Storage)
bool UBasePropCreator::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreator>(serstorage,mock_unet);
}

}
