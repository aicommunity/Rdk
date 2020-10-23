#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H
#include "UStorage.h"
#include "UMockUNet.h"
#include "../../Deploy/Include/rdk.h"

extern std::vector<std::string> ForbiddenInputs;
extern std::vector<std::string> ForbiddenOutputs;


// �������� �� �������� ������� ��� ���������� ������� �������
class UBasePropCreator
{
public:
    UBasePropCreator();

    static bool AddFuncCrPropFunc(RDK::UStorage * storage);

    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ��������� ��� �� ���������� ���� ������ � �������, ���� ��� ���
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� �������� � ����������� ����� ������
    // ���������� �������������� ������ ��� ����������� �
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};



// ����� ��� ��������� ������������� ������ CreatePropertyByType. ���������� � CreateProperty
// � ������� � �������� �������� ������� ���������� ��� ������
// ���� ��� ������������� ������ ������ (std::vector<typename>), �� �������� UnKnow - �������� � �������������� �����
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};

// ����� �������� ��� �������������� �����, ��� ���������� ��������� ���������� ���
class UnKnow
{
private:
    UnKnow() {}
};

// ����� �������� ��� "��������" �������
class simpleVector
{
private:
    simpleVector() {}
};


#endif // UBASEPROPCREATOR_H
