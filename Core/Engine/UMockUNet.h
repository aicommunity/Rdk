#ifndef UMOCKUNET_H
#define UMOCKUNET_H

#include "UNet.h"

namespace RDK {

class RDK_LIB_TYPE UMockUNet: public UNet
{
protected: // �������� ��������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------

UMockUNet(RDK::USerStorageXML *serstorage);
virtual ~UMockUNet(void);

// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UMockUNet* New(void);

private:
static std::string ForbiddenInputs;
static std::string ForbiddenOutputs;

USerStorageXML ClassDesriptionXML;

// ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void CreateProperty(RDK::USerStorageXML* serstorage);

// ��������� ��� �� ���������� ���� ������ � �������, ���� ��� ���
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet);

// ������� �������� � ����������� ����� ������
// ���������� �������������� ������ ��� ����������� �
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet);

};

// ����� ��� ��������� ������������� ������ CreatePropertyByType. ���������� � CreateProperty
// � ������� � �������� �������� ������� ���������� ��� ������
// ���� ��� ������������� ������ ������ (std::vector<typename>), �� �������� int - �������� � �������������� �����
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet);
};

}

#endif
