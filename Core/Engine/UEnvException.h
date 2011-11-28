#ifndef UENV_EXCEPTION_H
#define UENV_EXCEPTION_H

#include <string>
#include "UAEnvSupport.h"
#include "../Utilities/UException.h"

namespace RDK {

class UAContainer;

/* Базовый класс исключений */
class UIContainerException
{
public: // Данные исключения
// Короткое имя компонента в котором сгенерировано исключение
std::string Name;

// Короткий идентификатор компонента в котором сгенерировано исключение
ULongId Id;

// Полное имя владельца компонента в котором сгенерировано исключение
std::string OwnerName;

// Полный идентификатор владельца компонента в котором сгенерировано исключение
ULongId OwnerId;

// Полное имя главного владельца компонента в котором сгенерировано исключение
std::string MainOwnerName;

// Полный идентификатор главного владельца компонента в котором сгенерировано исключение
ULongId MainOwnerId;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UIContainerException(void);
UIContainerException(const UAContainer *cont);
UIContainerException(const UIContainerException &copy);
virtual ~UIContainerException(void);
// --------------------------
};

class UEnvFatalException: public UFatalException, public UIContainerException
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEnvFatalException(void);
UEnvFatalException(const UAContainer *cont);
UEnvFatalException(const UEnvFatalException &copy);
virtual ~UEnvFatalException(void);
// --------------------------
};



}

#endif

