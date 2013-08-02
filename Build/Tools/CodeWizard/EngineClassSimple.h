/* ***********************************************************
@Copyright @COPYRIGHT_NAME@, @COPYRIGHT_YEAR@.
E-mail:        @COPYRIGHT_EMAIL@
Url:           @COPYRIGHT_URL@

This file is part of the project: @COPYRIGHT_PROJECT_NAME@

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef @FILE_NAME@_H
#define @FILE_NAME@_H

#include "@INCLUDE_FILE_NAME@"

namespace @NAMESPACE_NAME@ {

class @CLASS_NAME@: public @INHERITANCE_NAME@
{
protected: // Свойства           

protected: // Переменные состояния

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
@CLASS_NAME@(void);
virtual ~@CLASS_NAME@(void);
// --------------------------

// ---------------------
// Методы управления параметрами
// ---------------------
// ---------------------

// ---------------------
// Методы управления переменными состояния
// ---------------------
// ---------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual @CLASS_NAME@* New(void);
// --------------------------        

// --------------------------
// Скрытые методы управления счетом 
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};


}

#endif



