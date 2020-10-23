#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H
#include "UStorage.h"
#include "UMockUNet.h"
#include "../../Deploy/Include/rdk.h"

extern std::vector<std::string> ForbiddenInputs;
extern std::vector<std::string> ForbiddenOutputs;


// Отвечает на создание функции для добавления базовых свойств
class UBasePropCreator
{
public:
    UBasePropCreator();

    static bool AddFuncCrPropFunc(RDK::UStorage * storage);

    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Проверяет тип на встроенные типы данных и создает, если это так
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Создает свойство с необходимым типом данных
    // Необходиом инстанцировать данную при необхоидмых Т
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};



// Класс для частичной специализации метода CreatePropertyByType. Вызывается в CreateProperty
// В который в качестве значения шаблона передается ТИП данных
// если тип подразумевает шаблон внутри (std::vector<typename>), то передать UnKnow - заглушку и переопределить метод
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);
};

// Класс заглушка для параметризации типов, где неизвестен некоторый внутренний тип
class UnKnow
{
private:
    UnKnow() {}
};

// Класс заглушка для "простого" вектора
class simpleVector
{
private:
    simpleVector() {}
};


#endif // UBASEPROPCREATOR_H
