/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UControllerH
#define UControllerH

#include <vector>
#include "UModule.h"

namespace RDK {

class UAContainer;

class UController: public RDK::UModule
{
protected: // Параметры
// Флаг разрешения обновления интерфейса
bool Enabled;

protected: // Данные
// Указатель на компоненты
std::vector<UAContainer*> Components;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UController(void);
virtual ~UController(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Флаг разрешения обновления интерфейса
bool IsEnabled(void) const;
bool IsEnabled(bool value);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает число компонент к которым подключен контроллер
size_t GetNumComponents(void) const;

// Возвращает компонент к которому подключен контроллер по индексу
UAContainer* GetComponents(size_t index);

// Возвращает индекс компонента по указателю
int FindComponent(UAContainer* component) const;
// --------------------------

// --------------------------
// Методы управления интерфейсом
// --------------------------
public:
// Связывает интерфейс с компонентом
virtual bool Link(UAContainer* component, bool forchilds=false);

// Отвязывает интерфейс от компонента
virtual bool UnLink(int index, bool forchilds=false);
virtual bool UnLink(UAContainer* component, bool forchilds=false);
virtual bool UnLinkAll(bool forchilds=false);

// Обновляет интерфейс
virtual bool Update(void);
// --------------------------

// --------------------------
// Скрытые методы управления интерфейсом
// --------------------------
protected:
// Связывает интерфейс с компонентом
virtual bool ALink(UAContainer* component, bool forchilds=false)=0;

// Отвязывает интерфейс от компонента
virtual bool AUnLink(int index, bool forchilds=false)=0;

// Обновляет интерфейс
virtual bool AUpdate(void)=0;
// --------------------------
};

}

#endif
