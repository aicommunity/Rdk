#include "UBasePropCreatorStd.h"
#include "UComponent.h"


namespace RDK {

// �������� ������� �������� ������� ���� ��������� ����� (����������� � Storage)
bool UBasePropCreatorStd::BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet)
{
 return BaseCrPropMockTempl<UBasePropCreatorStd>(serstorage,mock_unet);
}

}
