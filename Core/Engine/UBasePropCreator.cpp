#include "UBasePropCreator.h"
#include "UComponent.h"


namespace RDK {

// �������� ������� �������� ������� ���� ��������� ����� (����������� � Storage)
bool UBasePropCreator::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreator>(serstorage,mock_unet);
}

}
