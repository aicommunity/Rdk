#ifndef UBASEPROPCREATOR_H
#define UBASEPROPCREATOR_H

#include "UCreatorPropTemplate.h"

extern std::vector<std::string> ForbiddenInputs;
extern std::vector<std::string> ForbiddenOutputs;

// Отвечает за создание функции для формирования базовых свойств
class UBasePropCreator
{
public:
    UBasePropCreator();

    static bool BaseCrPropMock(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static void CreateProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

    // Проверяет тип на встроенные типы данных и создает, если это так
    template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
    static bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, RDK::UMockUNet* mock_unet);

};

#endif // UBASEPROPCREATOR_H
