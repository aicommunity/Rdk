#ifndef UMOCKUNET_H
#define UMOCKUNET_H

#include "UNet.h"

namespace RDK {

class RDK_LIB_TYPE UMockUNet: public UNet
{
protected: // Основные свойства

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------

UMockUNet(RDK::USerStorageXML *serstorage);
virtual ~UMockUNet(void);

// Выделяет память для новой чистой копии объекта этого класса
virtual UMockUNet* New(void);

private:
static std::string ForbiddenInputs;
static std::string ForbiddenOutputs;

USerStorageXML ClassDesriptionXML;

// Функция вызывает необходимые фукнции в зависимости от типа (строка) свойства
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
void CreateProperty(RDK::USerStorageXML* serstorage);

// Проверяет тип на встроенные типы данных и создает, если это так
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool CreateSimpleTypeProperty(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet);

// Создает свойство с необходимым типом данных
// Необходиом инстанцировать данную при необхоидмых Т
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt>
bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet);

};

// Класс для частичной специализации метода CreatePropertyByType. Вызывается в CreateProperty
// В который в качестве значения шаблона передается ТИП данных
// если тип подразумевает шаблон внутри (std::vector<typename>), то передать int - заглушку и переопределить метод
template <template<typename, typename, unsigned int> class PropType, unsigned int TypeInt, typename T>
class CreatorProperty
{
public:
    static bool CreatePropertyByType(RDK::USerStorageXML* serstorage, UMockUNet* mock_unet);
};

}

#endif
