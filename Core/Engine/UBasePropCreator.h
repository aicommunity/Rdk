#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H

#include "UCreatorPropTemplate.h"
namespace RDK {

// Отвечает за создание функции для формирования базовых свойств
class UBasePropCreator
{

public:
    UBasePropCreator();

    static const std::vector<std::string>& GetForbiddenInputs();

    static const std::vector<std::string>& GetForbiddenOutputs();

    // Основная функция создания свойств всех возможных типов (добавляется в Storage)
    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Проверяет тип на встроенные типы данных и создает, если это так
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор с простейшими типами данным (simpleVector)
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertySimpleVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор простейших векторов std::vector<simpleVector> >
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfSimVec(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор пар
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор из MVector<T>
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа вектор из MDMatrix<T>
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyVectorOfMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа list
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyList(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MDMatrix
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMDMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MDVector
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMDVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MVector  size - 2,3,4   тип - double
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMVector(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа MMatrix [rows,cols] - [3,3][4,4] типы - double, int, bool
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMMatrix(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа std::pair
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyPair(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция для создания свойства типа std::map
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreatePropertyMap(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

};

}
#endif // UBASEPROPCREATOR_H
