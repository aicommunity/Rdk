#ifndef UPropertyImplementationBase_H
#define UPropertyImplementationBase_H

#include "UPropertyInterface.h"

namespace RDK {

class UNet;

class RDK_LIB_TYPE UIPropertyInputBase: virtual public UIPropertyInput
{
private: // Данные
/// указатели на свойства-приемники данных
std::vector<UEPtr<UIPropertyOutput> > ConnectedProperties;

/// Лимит на число подключений
/// если -1, то нет ограничений
int NumConnectionsLimit;

/// Тип входа
//int InputType;

public:
/// Конструкторы и деструкторы
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

public: // Методы доступа к источнику данных
/// Возвращает лимит на число подключений ко входу
/// если -1, то нет ограничений
virtual int GetNumConnectionsLimit(void) const;

/// Возвращает число подключений ко входу
virtual int GetNumConnections(void) const;

/// Возвращает указатели на свойства-источники данных
virtual const std::vector<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const;

/// Возвращает указатели на свойства-источники данных
virtual const UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index) const;
virtual UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index);

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const;

// Проверяет, существует ли связь с заданным коннектором
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const;

/// Разрывает связь со свойством output_property
/// Если c_index == -1 то отключает все вхождения этого выхода
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property, int c_index=-1);

/// Разрывает связь по индексу с_index
virtual bool DisconnectFromOutput(int c_index);

/// Разрывает все связи со свойством
virtual bool DisconnectAllOutputs(void);

public:
/// Финальные действия по связыванию входа со свойством output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property);

/// Финальные действия по уничтожению связи со свойством output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index);

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
virtual void SetNumConnectionsLimit(int value);
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
private: // Данные
/// указатели на свойства-приемники данных
std::vector<UEPtr<UIPropertyInput> > SubscribedProperties;

public: // Конструкторы и деструкторы
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // Методы доступа к подключенным входам
/// Возвращает число подключенных входов
virtual size_t GetNumSubscribers(void) const;

/// Возвращает указатели на свойства-приемники данных
virtual const std::vector<UEPtr<UIPropertyInput> > GetSubscribedProperties(void) const;

/// Возвращает указатели на свойства-источники данных
virtual const UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index) const;
virtual UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index);

/// Устанавливает связь этого выхода со входом input_property
virtual bool ConnectToInput(UIPropertyInput *input_property);

/// Разрывает связь этого выхода со входом input_property
virtual bool DisconnectFromInput(UIPropertyInput *input_property, int c_index=-1);

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
virtual bool DisconnectAllInputs(void);

/// Возвращает true если выход подключен к выбранному входу
virtual bool IsConnectedToInput(UIPropertyInput *input_property);

/// Возвращает true если выход подключен к одному из входов выбранного компонента
virtual bool IsConnectedToComponent(UNet *component);

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;
};




}


#endif
