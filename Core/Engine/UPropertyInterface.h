//---------------------------------------------------------------------------

#ifndef UPropertyInterfaceH
#define UPropertyInterfaceH

#include "UTime.h"
#include "UEPtr.h"
#include "../Serialize/Serialize.h"
#include "UComponent.h"

namespace RDK {

class UNet;

// Класс сериализации свойств
class RDK_LIB_TYPE UIProperty
{
public:
virtual ~UIProperty(void);

/// Возвращает указатель на владелька свойства
virtual const UComponent* GetOwner(void) const=0;
virtual UComponent* GetOwner(void)=0;

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const=0;

// Метод возвращает строковое имя компонента-владельца свойства
virtual std::string GetOwnerName(void) const=0;

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerClassName(void) const=0;

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<USerStorage> storage, bool simplemode=false)=0;

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<USerStorage> storage, bool simplemode=false)=0;

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const=0;

// Метод изменяет тип свойства
virtual bool ChangeType(unsigned int value)=0;

// Возвращает языковой тип хранимого свойства
virtual const type_info& GetLanguageType(void) const=0;

// Метод сравнивает тип этого свойства с другим свойством
virtual bool CompareLanguageType(const UIProperty &dt) const=0;

// Метод возвращает указатель на область памяти, содержащую данные свойства
virtual const void* GetMemoryArea(void) const=0;

// Метод копирует значение данных свойства из области памяти
// штатными средствами копирования реального типа данных
// входной указатель приводится к указателю на необходимый тип данных
// TODO: possible unsafe???
virtual bool ReadFromMemory(const void *buffer)=0;

public: // Методы управления временем обновления данных свойства
/// Возвращает время обновления данных свойства (мс)
virtual ULongTime GetUpdateTime(void) const=0;

/// Устанавливает время обновления данных свойства
virtual void SetUpdateTime(ULongTime value)=0;

/// Сбрасывает время обновления до нуля
virtual void ResetUpdateTime(void)=0;

/// Обновляет время изменения данных свойства
virtual void RenewUpdateTime(void)=0;

/// Возвращает диапазон индексов входа/выхода
//virtual bool CheckRange(int index)=0;

// Диапазон индексов входов
//virtual int GetMinRange(void)=0;

//virtual int GetMaxRange(void)=0;

public: // Вспомогательные методы
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)=0;

/// Устанавливает флаг, указывающий, что это динамическое свойство, и должно быть удалено при разрушении компонента-владельца
virtual void SetDynamicPropertyFlag(void)=0;

/// Возвращает состояние флага, указывающего, что это динамическое свойство
virtual bool IsDynamicPropertyFlag(void) const=0;

// Возвращает только маску типа свойства
virtual unsigned int GetPropertyType(void) const=0;

// Возвращает только маску группы свойства
virtual unsigned int GetPropertyGroup(void) const=0;

// Возвращает строковое имя типа свойства
virtual std::string GetPropertyTypeName(void) const=0;

// Проверяет соответствие типа и группы свойства маске
virtual bool CheckMask(unsigned int mask) const=0;


/// Обновляет указатель PData
//virtual void UpdatePData(void* data)=0;

public: // Исключения
// Обращение к неинициализированным данным свойства
struct EPropertyError: public EError
{
protected: // Данные исключения
/// Имя компонента владельца
std::string OwnerName;

/// Имя свойства
std::string PropertyName;

public:
EPropertyError(const std::string &owner_name, const std::string &property_name)
: EError(), OwnerName(owner_name), PropertyName(property_name) {};
virtual ~EPropertyError(void) throw() {};

// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ")+OwnerName+std::string(":")+PropertyName;
}
};

// Обращение к неинициализированным данным свойства
struct EPropertyZeroPtr: public EPropertyError
{
public:
EPropertyZeroPtr(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// Вызов Getter завершен неудачно
struct EPropertyGetterFail: public EPropertyError
{
public:
EPropertyGetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// Вызов Setter завершен неудачно
struct EPropertySetterFail: public EPropertyError
{
public:
EPropertySetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// Вызов Setter завершен неудачно
struct EPropertyWrongIndex: public EPropertyError
{
public:
EPropertyWrongIndex(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

};

class UIPropertyOutput;

class RDK_LIB_TYPE UIPropertyInput: virtual public UIProperty
{
public:
virtual ~UIPropertyInput(void);
/// Возвращает тип свойства входа
//virtual int GetInputType(void) const=0;

public: // Методы доступа к источнику данных
/// Возвращает лимит на число подключений ко входу
/// если -1, то нет ограничений
virtual int GetNumConnectionsLimit(void) const=0;

/// Возвращает число подключений ко входу
virtual int GetNumConnections(void) const=0;

/// Возвращает указатели на свойства-приемники данных
virtual const std::vector<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const=0;

/// Возвращает указатели на свойства-источники данных
virtual const UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index) const=0;
virtual UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index)=0;

/// Возвращает полное имя подключенного компонента
//virtual std::string GetItemFullName(int c_index=-1) const=0;

/// Возвращает имя подключенного выхода
//virtual std::string GetItemOutputName(int c_index=-1) const=0;

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const=0;

// Проверяет, существует ли связь с заданным коннектором
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const=0;

/// Разрывает связь со свойством output_property
/// Если c_index == -1 то отключает все вхождения этого выхода
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property, int c_index=-1)=0;

/// Разрывает связь по индексу с_index
virtual bool DisconnectFromOutput(int c_index)=0;

/// Разрывает все связи со свойством
virtual bool DisconnectAllOutputs(void)=0;

public:
/// Финальные действия по связыванию входа со свойством output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property)=0;

/// Финальные действия по уничтожению связи со свойством output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)=0;

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// Сбрасывает указатель на данные
//virtual bool ResetPointer(int index, void* value)=0;

protected:
/// Задает лимит на число подключений ко входу
/// если -1, то нет ограничений
virtual void SetNumConnectionsLimit(int value)=0;

};

class RDK_LIB_TYPE UIPropertyOutput: virtual public UIProperty
{
public: // Методы доступа к подключенным входам
virtual ~UIPropertyOutput(void);
/// Возвращает число подключенных входов
virtual size_t GetNumSubscribers(void) const=0;

/// Возвращает указатели на свойства-приемники данных
virtual const std::vector<UEPtr<UIPropertyInput> > GetSubscribedProperties(void) const=0;

/// Возвращает указатели на свойства-источники данных
virtual const UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index) const=0;
virtual UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index)=0;

/// Устанавливает связь этого выхода со входом input_property
virtual bool ConnectToInput(UIPropertyInput *input_property)=0;

/// Разрывает связь этого выхода со входом input_property
virtual bool DisconnectFromInput(UIPropertyInput *input_property, int c_index=-1)=0;

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
virtual bool DisconnectAllInputs(void)=0;

/// Возвращает true если выход подключен к выбранному входу
virtual bool IsConnectedToInput(UIPropertyInput *input_property)=0;

/// Возвращает true если выход подключен к одному из входов выбранного компонента
virtual bool IsConnectedToComponent(UNet *component)=0;

public: // Методы управления указателем на выходные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;
};

//---------------------------------------------------------------------------
}
#endif
