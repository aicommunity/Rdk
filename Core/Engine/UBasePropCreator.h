#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H

#include "UCreatorPropTemplate.h"
namespace RDK {

// �������� �� �������� ������� ��� ������������ ������� �������
class UBasePropCreator
{

public:
    UBasePropCreator();

    static const std::vector<std::string>& GetForbiddenInputs();

    static const std::vector<std::string>& GetForbiddenOutputs();

    // �������� ������� �������� ������� ���� ��������� ����� (����������� � Storage)
    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� �������� ����������� ������� � ����������� �� ���� (������) ��������
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ��������� ��� �� ���������� ���� ������ � �������, ���� ��� ���
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� ������
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� ������ � ����������� ������ ������ (simpleVector)
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertySimpleVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� ������ ���������� �������� std::vector<simpleVector> >
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfSimVec(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� ������ ���
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� ������ �� MVector<T>
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� ������ �� MDMatrix<T>
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� list
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyList(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� MDMatrix
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� MDVector
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMDVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� MVector  size - 2,3,4   ��� - double
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� MMatrix [rows,cols] - [3,3][4,4] ���� - double, int, bool
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� std::pair
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // ������� ��� �������� �������� ���� std::map
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMap(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

};

}
#endif // UBASEPROPCREATOR_H
