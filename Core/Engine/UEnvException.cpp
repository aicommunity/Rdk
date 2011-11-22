#ifndef UENV_EXCEPTION_CPP
#define UENV_EXCEPTION_CPP

#include "UEnvException.h"

namespace RDK {


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UIContainerException::UIContainerException(void)
{

}

UIContainerException::UIContainerException(const UAContainer *cont)
{
 if(!cont)
  return;

 // Короткое имя компонента в котором сгенерировано исключение
 Name=cont->GetName();

 // Короткий идентификатор компонента в котором сгенерировано исключение
 Id=cont->GetId();

 // Полное имя владельца компонента в котором сгенерировано исключение
 if(cont->GetOwner())
 {
  cont->GetOwner()->GetFullName(OwnerName);

  // Полный идентификатор владельца компонента в котором сгенерировано исключение
  OwnerId=cont->GetOwner()->GetFullId();
 }

 if(cont->GetMainOwner())
 {
  // Полное имя главного владельца компонента в котором сгенерировано исключение
  cont->GetMainOwner()->GetFullName(MainOwnerName);

  // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
  MainOwnerId=cont->GetMainOwner()->GetFullId();
 }
}


UIContainerException::UIContainerException(const UIContainerException &copy)
{
 // Короткое имя компонента в котором сгенерировано исключение
 Name=copy.Name;

 // Короткий идентификатор компонента в котором сгенерировано исключение
 Id=copy.Id;

 // Полное имя владельца компонента в котором сгенерировано исключение
 OwnerName=copy.OwnerName;

 // Полный идентификатор владельца компонента в котором сгенерировано исключение
 OwnerId=copy.OwnerId;

 // Полное имя главного владельца компонента в котором сгенерировано исключение
 MainOwnerName=copy.MainOwnerName;

 // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
 MainOwnerId=copy.MainOwnerId;
}

UIContainerException::~UIContainerException(void)
{

}
// --------------------------


// class UEnvFatalException: public UFatalException, public UIContainerException
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEnvFatalException::UEnvFatalException(void)
{

}

UEnvFatalException::UEnvFatalException(const UAContainer *cont)
 : UFatalException(), UIContainerException(cont)
{

}

UEnvFatalException::UEnvFatalException(const UEnvFatalException &copy)
 : UFatalException(copy), UIContainerException(copy)
{

}

UEnvFatalException::~UEnvFatalException(void)
{

}
// --------------------------



}
#endif

