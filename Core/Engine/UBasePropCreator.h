#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H

#include "UCreatorPropTemplate.h"

extern std::vector<std::string> ForbiddenInputs;
extern std::vector<std::string> ForbiddenOutputs;

// �������� �� �������� ������� ��� ������������ ������� �������
class UBasePropCreator
{
public:
    UBasePropCreator();

    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ��������� ��� �� ���������� ���� ������ � �������, ���� ��� ���
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

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
