#include "UBasePropCreatorVector.h"
#include "UComponent.h"


namespace RDK {

// �������� ������� �������� ������� ���� ��������� ����� (����������� � Storage)
bool UBasePropCreatorVector::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreatorVector>(serstorage,mock_unet);
}



}
