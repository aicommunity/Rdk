#include "UBasePropCreatorMatrix.h"
#include "UComponent.h"


namespace RDK {

// Основная функция создания свойств всех возможных типов (добавляется в Storage)
bool UBasePropCreatorMatrix::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreatorMatrix>(serstorage,mock_unet);
}



}
