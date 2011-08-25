/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UACONTAINERENVIRONMENT_CPP
#define UACONTAINERENVIRONMENT_CPP

#include "UAContainerEnvironment.h"

namespace RDK {

// --------------------------
// Constructors & destructors
// --------------------------
UAContainerEnvironment::UAContainerEnvironment(void)
{
}

UAContainerEnvironment::~UAContainerEnvironment(void)
{
}
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
UAContainerStorage* UAContainerEnvironment::GetStorage(void)
{
 return dynamic_cast<UAContainerStorage*>(Storage);
}

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
/*bool UAContainerEnvironment::SetStorage(UContainerStorage *storage)
{
 if(!storage)
  return false;

 if(Storage == storage)
  return true;

 if(!DestroyModel())
  return false;

 Storage=storage;
 Init=true;

 return true;
}        */
/*
// Возвращает указатель на модель
UAComponent* UAContainerEnvironment::GetModel(void)
{
 return Model;
}
  */
// Создает новую модель из хранилища по id класса
bool UAContainerEnvironment::CreateModel(const UId& classid)
{
 return UAEnvironment::CreateModel(classid);
}

// Создает новую модель из хранилища по имени класса
bool UAContainerEnvironment::CreateModel(const NameT& classname)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=0;
 Model=GetStorage()->TakeObject(classname);
 if(Model)
 {
  return true;
 }

 return false;
}
                    /*
// Уничтожает текущую модель
bool UAContainerEnvironment::DestroyModel(void)
{
 if(!Model)
  return true;

 Model=0;
 CurrentComponent=0;

 return true;
}

// Возвращает библиотеку по индексу
ULibrary* UAContainerEnvironment::GetClassLibrary(int index)
{
 return ClassLibraryList[index];
}
                    */
// Возвращает библиотеку по имени
UALibrary* UAContainerEnvironment::GetClassLibrary(const string &name)
{
 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  UALibrary *lib=dynamic_cast<UALibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == name)
   return lib;
 }

 return 0;
}

// Возвращает имя библиотеки по индексу
const string& UAContainerEnvironment::GetClassLibraryName(int index)
{
 return ClassLibraryList[index]->GetName();
}

// Возвращает версию библиотеки по индексу
const string& UAContainerEnvironment::GetClassLibraryVersion(int index)
{
 return ClassLibraryList[index]->GetVersion();
}

                /*
// Возвращает число библиотек
size_t UAContainerEnvironment::GetNumClassLibraries(void) const
{
 return ClassLibraryList.size();
}                 */

// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool UAContainerEnvironment::AddClassLibrary(UALibrary *library)
{
 if(!library)
  return false;

 UALibrary *newlib=dynamic_cast<UALibrary*>(library);

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  UALibrary *lib=dynamic_cast<UALibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == newlib->GetName())
   return false;
 }


 return UAEnvironment::AddClassLibrary(library);
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool UAContainerEnvironment::DelClassLibrary(int index)
{
 return UAEnvironment::DelClassLibrary(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UAContainerEnvironment::DelClassLibrary(const string &name)
{
 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  UALibrary *lib=dynamic_cast<UALibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == name)
   return DelClassLibrary(i);
 }
 return true;
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UAContainerEnvironment::DelAllClassLibraries(void)
{
 return UAEnvironment::DelAllClassLibraries();
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool UAContainerEnvironment::BuildStorage(void)
{
 if(!IsStoragePresent())
  return false;

 if(!DestroyModel())
  return false;

 GetStorage()->ClearObjectsStorage();

 if(!GetStorage()->ClearClassesStorage())
  return false;

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ClassLibraryList[i]->Upload(GetStorage());
  UALibrary *lib=dynamic_cast<UALibrary *>(ClassLibraryList[i]);
  if(lib)
  {
   CompletedClassNames.insert(CompletedClassNames.end(),
                             lib->GetComplete().begin(),
                             lib->GetComplete().end());
   IncompletedClassNames.insert(IncompletedClassNames.end(),
                             lib->GetIncomplete().begin(),
                             lib->GetIncomplete().end());
  }
 }
 return true;
}
// --------------------------

// --------------------------
// Методы управления контроллерами
// --------------------------
// --------------------------

// --------------------------
// Операторы доступа к данным среды
// --------------------------
// Возвращает указатель на текущий компонент модели
UAComponent* UAContainerEnvironment::operator () (void)
{
 return CurrentComponent;
}
// --------------------------

// --------------------------
// Данные графического интерфеса пользователя
// --------------------------
// --------------------------

// --------------------------
// Методы инициализации среды
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к флагам состояний
// --------------------------
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// --------------------------

// --------------------------
// Вспомогательные методы инициализации среды
// --------------------------
// Загружает динамическую библиотеку с экземпляром хранилища и возвращает
// указатель на него.
// В базовой версии не делает ничего и возвращает 0.
UAContainerStorage* UAContainerEnvironment::LoadStorageLibrary(const string &filename)
{
 return 0;
}

// Выгружает динамическую библиотеку с экземпляром хранилища.
// В базовой версии не делает ничего и возвращает true.
bool UAContainerEnvironment::UnLoadStorageLibrary(void)
{
 return true;
}

// Загружает динамическую библиотеку с набором образцов классов
// в виде экземпляра хранилища и возвращает указатель на него.
// В базовой версии не делает ничего и возвращает 0.
UAContainerStorage* UAContainerEnvironment::LoadClassLibrary(const string &libname, const string &filename)
{
 return 0;
}

// Выгружает динамическую библиотеку с набором образцов классов.
// В базовой версии не делает ничего и возвращает true.
bool UAContainerEnvironment::UnLoadClassLibrary(const string &libname)
{
 return true;
}
// --------------------------

// --------------------------
// Методы управления моделью
// --------------------------
// --------------------------

}
#endif
