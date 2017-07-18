#ifndef UPropertyImplementationBase_H
#define UPropertyImplementationBase_H

#include "UPropertyInterface.h"
//#include "UPropertyOutput.h"
//#include "UPropertyInput.h"

namespace RDK {

class UNet;
//typedef UEPtr<UNet> PUAConnector;


class RDK_LIB_TYPE UIPropertyInputBase: virtual public UIPropertyInput
{
protected: // Данные
/// Данные подключенных ко входу источников
//std::vector<UCItem> ItemsList;

/// указатели на свойства-приемники данных
std::list<UEPtr<UIPropertyOutput> > ConnectedProperties;

/// Тип входа
int InputType;

public:
/// Конструкторы и деструкторы
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// Возвращает тип свойства ввода-вывода
virtual int GetInputType(void) const;

public: // Методы доступа к источнику данных
/// Возвращает число подключений ко входу
virtual int GetNumConnections(void) const;

/// Возвращает указатели на свойства-источники данных
virtual const std::list<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const;

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(int c_index=-1) const;

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(int c_index=-1) const;

/// Возвращает true если вход имеет подключение
virtual bool IsConnected(void) const;

// Проверяет, существует ли связь с заданным коннектором
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const;

/// Разрывает связь со свойством output_property
virtual bool Disconnect(UIPropertyOutput *output_property);

/// Разрывает все связи со свойством
virtual bool DisconnectAll(void);

public:
/// Финальные действия по связыванию входа со свойством output_property
virtual bool ConnectToOutput(UIPropertyOutput *output_property);

/// Финальные действия по уничтожению связи со свойством output_property
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property);

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;

/// Устанавливает указатель на данные
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// Сбрасывает указатель на данные
//virtual bool ResetPointer(int index, void* value)=0;
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
protected: // Данные
/// указатели на свойства-приемники данных
std::list<UEPtr<UIPropertyInput> > ConnectedProperties;

public: // Конструкторы и деструкторы
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // Методы доступа к подключенным входам
/// Возвращает число подключенных входов
virtual size_t GetNumConnectors(void) const;

/// Возвращает указатели на свойства-приемники данных
virtual const std::list<UEPtr<UIPropertyInput> > GetConnectedProperties(void) const;

/// Устанавливает связь этого выхода со входом input_property
virtual bool Connect(UIPropertyInput *input_property);

/// Разрывает связь этого выхода со входом input_property
virtual bool Disconnect(UIPropertyInput *input_property);

// Разрывает связь выхода этого объекта со всеми
// подключенными коннекторами.
virtual bool DisconnectAll(void);

/// Возвращает true если выход подключен к выбранному входу
virtual bool IsConnectedTo(UIPropertyInput *input_property);

/// Возвращает true если выход подключен к одному из входов выбранного компонента
virtual bool IsConnectedTo(UNet *component);

public: // Методы управления указателем на входные данные
/// Возвращает указатель на данные
//virtual void const* GetPointer(int index) const=0;
};




}


#endif
