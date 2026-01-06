/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UASTORAGE_CPP
#define UASTORAGE_CPP

#include <string.h>
#include "../../Deploy/Include/rdk_version.h"
#include "UStorage.h"
#include "ULibrary.h"
#include "../../Deploy/Include/rdk_exceptions.h"
#include "UEnvException.h"
#include "../../Deploy/Include/rdk_init.h"
#include <future>
#include <mutex>
#include <unordered_set>

namespace RDK {

/* *********************************************************************** */
/* *********************************************************************** */
// Элемент списка существующих объектов определенного класса
// class UInstancesStorageElement
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorageElement::UInstancesStorageElement(void)
 : UseFlag(false)
{
}

UInstancesStorageElement::UInstancesStorageElement(const UInstancesStorageElement &copy)
 : Object(copy.Object), UseFlag(copy.UseFlag)
{
}

UInstancesStorageElement::UInstancesStorageElement(const UEPtr<UContainer> &object, bool useflag)
 : Object(object), UseFlag(useflag)
{

}


UInstancesStorageElement::~UInstancesStorageElement(void)
{
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UInstancesStorageElement& UInstancesStorageElement::operator = (const UInstancesStorageElement &copy)
{
 // Указатель на объект
 Object=copy.Object;

 // Признак того свободен ли объект
 UseFlag=copy.UseFlag;

 return *this;
}

// Операторы сравнения
bool UInstancesStorageElement::operator < (const UInstancesStorageElement &value)
{
 return Object<value.Object;
}

bool UInstancesStorageElement::operator > (const UInstancesStorageElement &value)
{
 return Object>value.Object;
}

bool UInstancesStorageElement::operator <= (const UInstancesStorageElement &value)
{
 return Object<=value.Object;
}

bool UInstancesStorageElement::operator >= (const UInstancesStorageElement &value)
{
 return Object>=value.Object;
}

bool UInstancesStorageElement::operator == (const UInstancesStorageElement &value)
{
 return Object == value.Object;
}

bool UInstancesStorageElement::operator != (const UInstancesStorageElement &value)
{
 return Object != value.Object;
}
// --------------------------

/* *************************************************************************** */
// Class UStorage
/* *************************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UStorage::UStorage(void)
{
 LastClassId=0;
 FuncProgressBarCallback=0;
}

UStorage::~UStorage(void)
{
 try
 {
  ClearObjectsStorage(true);
  ClearClassesStorage(true);

  // Быстрое удаление всех библиотек без вызова DelAbandonedClasses() в цикле
  // Это критично для производительности - избегаем O(n*m*k) сложности
  for(int i = 0; i < int(CollectionList.size()); i++)
  {
   if(CollectionList[i])
   {
    if(CollectionList[i]->GetType() == 2)
     delete CollectionList[i];
   }
  }
  CollectionList.clear();
  
  // Один вызов DelAbandonedClasses() в конце вместо вызова для каждой библиотеки
  DelAbandonedClasses();

 }
 catch(EObjectStorageNotEmpty &ex)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
 }
 catch(UException &ex)
 {
  if(Logger)
   Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, ex.what());
 }
 catch(std::exception &ex)
 {
  if(Logger)
   Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, ex.what());
 }
 catch(...)
 {
  if(Logger)
   Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Unknown excception");
 }
}
// --------------------------

// --------------------------
// Методы управления свойствами
// --------------------------
// Возвращает последний использованный Id классов
UId UStorage::GetLastClassId(void) const
{
 return LastClassId;
}
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает Id класса по его имени
const UId& UStorage::FindClassId(const NameT &name) const
{
 auto I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);
 return I->second;
}

// Возвращает имя класса по его Id
const NameT UStorage::FindClassName(const UId &id) const
{
 // Специальная обработка для ForbiddenId - возвращаем понятное сообщение
 // вместо исключения, чтобы избежать каскадных ошибок
 if(id == ForbiddenId)
 {
  return std::string("(ForbiddenId/Uninitialized)");
 }
 
 for(auto I=ClassesLookupTable.begin(),
									J=ClassesLookupTable.end();I != J;++I)
 {
  if(I->second == id)
   return I->first;
 }
 throw EClassIdNotExist(id);
}
// --------------------------


// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
UId UStorage::AddClass(UEPtr<UComponentAbstractFactory> factory, const UId &classid)
{
// UEPtr<UStorage> storage=classtemplate->GetStorage();
// if(storage)
//  storage->PopObject(classtemplate);

 UId id=classid;
 if(id == ForbiddenId)
  id=LastClassId+1;

 if(ClassesStorage.find(id) != ClassesStorage.end())
  throw EClassIdAlreadyExist(id);

 ClassesStorage[id]=factory;
 factory->SetClassId(id);
 LastClassId=id;

 // Заглушка!!! Это некоррректно, имени-то нет.
// ClassesDescription[FindClassName(id)]=classtemplate->NewDescription();

 return id;
}

// Добавляет образец класса объекта в хранилище
UId UStorage::AddClass(UEPtr<UComponentAbstractFactory> factory, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(classname);

 UId id=AddClass(factory,classid);
 ClassesLookupTable[classname]=id;

 // теперь ClassDescription не сохраняется
// ClassesDescription[classname]=factory->NewDescription();
// ClassesDescription[classname]->SetClassNameValue(classname);
 return id;
}

// Удаляет образец класса объекта из хранилища
// Если 'force' == true то принудительно удаляет из хранилища
// все объекты этого класса
void UStorage::DelClass(const UId &classid, bool force)
{
 UObjectsStorageIterator temp=ObjectsStorage.find(classid);

 if(!force)
 {
  FreeObjectsStorageByClass(classid);
  // Если после очистки у класса не осталось объектов
  if(temp != ObjectsStorage.end() && temp->second.empty())
      ObjectsStorage.erase(temp);
  if(temp != ObjectsStorage.end() && temp->second.size() > 0)
   throw EObjectStorageNotEmpty(classid);
 }
 else
 {
  // КРИТИЧНО: При принудительном удалении класса сначала очищаем UseFlag
  // для всех объектов этого класса, которые больше не используются
  if(temp != ObjectsStorage.end())
  {
   // Удалено избыточное логирование - создавало спам в DEBUG логах
   
   for(list<UInstancesStorageElement>::iterator I=temp->second.begin(), 
       J=temp->second.end(); I!=J; ++I)
   {
    if(I->UseFlag && I->Object)
    {
     try
     {
      UEPtr<UContainer> owner = I->Object->GetOwner();
      bool activity = I->Object->Activity;
      
      // Если объект не имеет владельца и не активен, очищаем UseFlag
      if(!owner && !activity)
      {
       I->UseFlag = false;
       // Удалено избыточное логирование - создавало спам в DEBUG логах
      }
     }
     catch(...)
     {
      // В случае ошибки оставляем UseFlag как есть
     }
    }
   }
  }
  
  ClearObjectsStorageByClass(classid);
 }

 UClassesStorageIterator I=ClassesStorage.find(classid);
 std::string name;
 
 // КРИТИЧНО: Получаем имя класса перед удалением из ClassesStorage
 // Если класс уже удален, используем альтернативный способ
 try
 {
  name=FindClassName(classid);
 }
 catch(...)
 {
  // Если класс уже не существует, используем пустое имя
  name = std::string("(unknown)");
  if(Logger)
   Logger->LogMessageEx(RDK_EX_WARNING, __FUNCTION__, 
    std::string("Class with id ") + sntoa(classid) + 
    std::string(" not found in ClassesLookupTable"));
 }

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);
 else
  throw EClassIdNotExist(classid);

 UClassStorageElement element=I->second;
 if(element)
  delete element.Get();

 UClassesDescriptionIterator J=ClassesDescription.find(name);

 if(J != ClassesDescription.end())
 {
  if(J->second)
   delete J->second;

  ClassesDescription.erase(J);
 }


 for(auto lookup_iter=ClassesLookupTable.begin(),
							  lookup_end=ClassesLookupTable.end(); lookup_iter!=lookup_end; ++lookup_iter)
 {
  if(lookup_iter->second == classid)
   {
    ClassesLookupTable.erase(lookup_iter);
   break;
  }
 }

 UEPtr<ULibrary> lib=FindCollection(name);
 if(lib)
  lib->RemoveClassFromCompletedList(name);
}

// Проверяет наличие образца класса объекта в хранилище
bool UStorage::CheckClass(const UId &classid) const
{
 if(ClassesStorage.find(classid) == ClassesStorage.end())
  return false;

 return true;
}

bool UStorage::CheckClass(const string &classname) const
{
 auto I=ClassesLookupTable.find(classname);
 if(I == ClassesLookupTable.end())
  return false;
 return true;

}

// Возвращает образец класса
UEPtr<UComponentAbstractFactory> UStorage::GetComponentFactory(const UId &classid) const
{
 UClassesStorageCIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 return I->second;
}

UEPtr<UComponentAbstractFactory> UStorage::GetComponentFactory(const std::string &class_name) const
{
 UId id=FindClassId(class_name);
 return GetComponentFactory(id);
}

// Возвращает число классов
int UStorage::GetNumClasses(void) const
{
 return int(ClassesStorage.size());
}

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
void UStorage::GetClassIdList(std::vector<UId> &buffer) const
{
 buffer.resize(0);
 buffer.reserve(ClassesStorage.size());
 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
  buffer.push_back(I->first);
}

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
void UStorage::GetClassNameList(vector<NameT> &buffer) const
{
 auto I=ClassesLookupTable.begin();
 auto J=ClassesLookupTable.end();
 buffer.resize(0);
 buffer.reserve(ClassesLookupTable.size());

 for(auto lookup_iter=ClassesLookupTable.begin(),
									lookup_end=ClassesLookupTable.end(); lookup_iter!=lookup_end; ++lookup_iter)
 {
  buffer.push_back(lookup_iter->first);
 }
}



// Удаляет все не используемые образцы классов из хранилища
void UStorage::FreeClassesStorage(bool force)
{
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageCIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() == 0)
  {
   DelClass(I->first);
  }
 }
}

// Удаляет все образцы классов из хранилища
void UStorage::ClearClassesStorage(bool force)
{
 // Оптимизация: объединяем проверку и удаление в один проход
 UClassesStorageIterator I = ClassesStorage.begin();
 while(I != ClassesStorage.end())
 {
  // Проверка наличия объектов (если нужно)
  UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() != 0)
  {
   if(Logger)
    Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Destory class which objecst in use: ")+FindClassName(I->first));
   if(!force)
   {
    throw EObjectStorageNotEmpty(I->first);
   }
   // При force пропускаем проверку и продолжаем удаление
  }
  
  // Удаление класса
  RDK_SYS_TRY
  {
   try
   {
    if(I->second)
    {
     std::string name=FindClassName(I->first);
     delete I->second.Get();
    }
   }
   catch(...)
   {
    if(Logger)
     Logger->LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("Exception raised when destroy class ")+FindClassName(I->first));
   }
  }
  RDK_SYS_CATCH
  {
   if(Logger)
    Logger->ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  }
  
  // Безопасное удаление из map (erase возвращает следующий итератор)
  I = ClassesStorage.erase(I);
 }
 
 // ClassesStorage уже очищен в цикле выше

 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  if(I->second)
   delete I->second.Get();
 }
 ClassesDescription.clear();
 LastClassId=0;
}
// --------------------------

// --------------------------
// Методы управления хранилищем объектов
// --------------------------
// Методы управления хранилищем объектов
// --------------------------
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
UEPtr<UComponent> UStorage::TakeObject(const UId &classid, const UEPtr<UComponent> &prototype)
{
 // Защита от использования неинициализированных классов
 if(classid == ForbiddenId)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, 
    std::string("Attempt to take object with ForbiddenId (0) - class not initialized"));
  throw EClassIdNotExist(classid);
 }
 
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, 
    std::string("Class with id ") + sntoa(classid) + std::string(" not found in Storage"));
  throw EClassIdNotExist(classid);
 }

 UClassStorageElement tmpl=tmplI->second;

 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  UInstancesStorageElement* element=0;// Заглушка!! instances->FindFree();
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
              J=instances->second.end(); I!=J; ++I)
  {
   if(I->UseFlag == false)
   {
   element=&(*I);
   break;
   }
  }

  if(element)
  {
   UEPtr<UContainer> obj=element->Object;

   if(obj)
   {
    element->UseFlag=true;
    
    // КРИТИЧНО: Сохраняем ClassId перед операциями, которые могут его изменить
    UId saved_class_id = obj->GetClass();
    if(saved_class_id == ForbiddenId || saved_class_id != classid)
     saved_class_id = classid; // Используем правильный classid если текущий невалидный
    
    obj->Default();
    if(!prototype)
     tmpl->ResetComponent(static_pointer_cast<UComponent>(obj));
    else
     dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj,this);

    // КРИТИЧНО: Восстанавливаем ClassId ПОСЛЕ всех операций, которые могут его изменить
    UId current_class_id = obj->GetClass();
    if(current_class_id == ForbiddenId || current_class_id != classid)
    {
     // Восстанавливаем ClassId из параметра classid
     obj->SetClass(classid);
     // Удалено избыточное логирование - создавало спам в DEBUG логах
    }

    obj->Activity = true;
   }
   return static_pointer_cast<UComponent>(obj);
  }
 }


 // Если свободного объекта не нашли
 UEPtr<UContainer> obj;
 if(prototype)
 {
  obj=dynamic_pointer_cast<UContainer>(tmpl->Prototype(prototype));
 }
 else
 {
  obj=dynamic_pointer_cast<UContainer>(tmpl->New());
 }

 if(!obj)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Class factory doesn't return object: ")+FindClassName(classid));
  return 0;
 }

 PushObject(classid,obj);
 obj->SetLogger(Logger);
 obj->Activity = true;

 return static_pointer_cast<UComponent>(obj);
}

UEPtr<UComponent> UStorage::TakeObject(const NameT &classname, const UEPtr<UComponent> &prototype)
{
 return TakeObject(FindClassId(classname),prototype);
}


// Возвращает Id класса, отвечающий объекту 'object'
UId UStorage::FindClass(UEPtr<UComponent> object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}

// Проверяет существует ли объект 'object' в хранилище
bool UStorage::CheckObject(UEPtr<UContainer> object) const
{
 if(!object)
  return false;

 UObjectsStorageCIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances != ObjectsStorage.end())
 {
  for(list<UInstancesStorageElement>::const_iterator I=instances->second.begin(),
								J=instances->second.end(); I!=J; ++I)
  {
   if(I->Object == object)
	return true;
  }
 }
 return false;
}

// Ищет фабрику, непосредственно хранящую заданный компонент
UVirtualMethodFactory* UStorage::FindVirualMethodFactory(UEPtr<UContainer> object)
{
 if(!object)
  return 0;

 UClassesStorageCIterator instances=ClassesStorage.begin();
 for(;instances != ClassesStorage.end();++instances)
 {
  UEPtr<UVirtualMethodFactory> virtual_factory=dynamic_pointer_cast<UVirtualMethodFactory>(instances->second);
  if(virtual_factory)
  {
   if(virtual_factory->GetComponent() == object)
	return virtual_factory;
  }
 }
 return 0;
}

// Вычисляет суммарное число объектов в хранилище
int UStorage::CalcNumObjects(void) const
{
 int result=0;

 for(UObjectsStorageCIterator I=ObjectsStorage.begin(),J=ObjectsStorage.end(); I!=J; ++I)
  result+=int(I->second.size());

 return result;
}

int UStorage::CalcNumObjects(const UId &classid) const
{
 UObjectsStorageCIterator instances=ObjectsStorage.find(classid);

 if(instances == ObjectsStorage.end())
  throw EClassIdNotExist(classid);

 return int(instances->second.size());
}

size_t UStorage::CalcNumObjects(const string &classname) const
{
 return CalcNumObjects(FindClassId(classname));
}


// Удаляет все свободные объекты из хранилища
void UStorage::FreeObjectsStorage(bool force)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
				 								instances != iend; ++instances)
 {
  // КРИТИЧНО: Проверяем существование класса перед попыткой получить его имя
  // Если класс уже удален, используем альтернативный способ получения информации
  std::string object_class_name;
  UId class_id = instances->first;
  
  try
  {
   // Проверяем, существует ли класс в ClassesStorage
   if(ClassesStorage.find(class_id) != ClassesStorage.end())
   {
    object_class_name = FindClassName(class_id);
   }
   else
   {
    // Класс уже удален, используем альтернативное имя
    object_class_name = std::string("(deleted class id=") + sntoa(class_id) + std::string(")");
    if(Logger)
     Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, 
      std::string("Class with id ") + sntoa(class_id) + 
      std::string(" already deleted, using alternative name"));
   }
  }
  catch(...)
  {
   // В случае ошибки используем альтернативное имя
   object_class_name = std::string("(unknown class id=") + sntoa(class_id) + std::string(")");
   if(Logger)
    Logger->LogMessageEx(RDK_EX_WARNING, __FUNCTION__, 
     std::string("Failed to get class name for id ") + sntoa(class_id) + 
     std::string(", using alternative name"));
  }
  
  if(instances->second.empty())
   continue;

  size_t count=0;

  // Удалено избыточное логирование - создавало спам в INFO логах
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(); I != instances->second.end();)
  {
   std::string object_name=I->Object->GetName();
   UEPtr<UContainer> object=I->Object;
   
   // КРИТИЧНО: Сначала восстанавливаем ClassId и проверяем UseFlag ПЕРЕД логированием ошибки
   bool actually_in_use = false;
   bool class_id_restored = false;
   UId restored_class_id = ForbiddenId;
   
   if(I->UseFlag)
   {
	try
	{
	 if(object)
	 {
	  // КРИТИЧНО: Проверяем валидность ClassId объекта и восстанавливаем из ключа контейнера
	  UId object_class_id = object->GetClass();
	  UId container_class_id = instances->first; // Ключ контейнера в ObjectsStorage
	  
	  // Если ClassId = ForbiddenId или не соответствует ключу контейнера, восстанавливаем его
	  if(object_class_id == ForbiddenId || object_class_id != container_class_id)
	  {
	   // Восстанавливаем ClassId из ключа контейнера
	   object->SetClass(container_class_id);
	   class_id_restored = true;
	   restored_class_id = container_class_id;
	   // Удалено избыточное логирование - создавало спам в WARNING логах
	   object_class_id = container_class_id;
	  }
	  else
	  {
	   restored_class_id = object_class_id;
	  }
	  
	  // Проверяем существование класса
	  if(ClassesStorage.find(object_class_id) == ClassesStorage.end())
	  {
	   // Класс объекта уже удален, объект в невалидном состоянии
	   if(Logger)
	    Logger->LogMessageEx(RDK_EX_WARNING, __FUNCTION__, 
	     std::string("Object ") + object_name + 
	     std::string(" has class id ") + sntoa(object_class_id) + 
	     std::string(" which no longer exists - class was deleted"));
	  }
	  
	  UEPtr<UContainer> owner = object->GetOwner();
	  // Если объект имеет владельца, он может быть в использовании
	  if(owner)
	   actually_in_use = true;
	   
	  // Дополнительная проверка: если Activity=false и нет владельца,
	  // объект скорее всего не используется, можно очистить UseFlag
	  if(!actually_in_use && !object->Activity)
	  {
	   // Автоматически очищаем UseFlag для объектов без активных ссылок
	   I->UseFlag = false;
	   // Удалено избыточное логирование - создавало спам в DEBUG логах
	  }
	 }
	}
	catch(...)
	{
	 // В случае ошибки считаем, что объект используется
	 actually_in_use = true;
	}
   }

   // Логируем ошибку только если UseFlag все еще true после проверки и очистки
   if(I->UseFlag && force)
   {
	if(Logger)
	{
	 std::string context_info = std::string("FORCED destroy objects by name ") + object_name + ": object in use!";
	 
	 // Добавляем информацию о контексте использования объекта
	 try
	 {
	  if(object)
	  {
	   UEPtr<UContainer> owner = object->GetOwner();
	   if(owner)
	   {
	    std::string owner_name = owner->GetName();
	    context_info += std::string(" Owner=") + owner_name;
	   }
	   
	   bool activity = object->Activity;
	   context_info += std::string(" Activity=") + (activity ? "true" : "false");
	   
	   // Используем восстановленный ClassId (если был восстановлен) или текущий
	   UId current_class_id = restored_class_id != ForbiddenId ? restored_class_id : object->GetClass();
	   if(current_class_id == 0)
	    context_info += std::string(" ClassId=ForbiddenId(0)");
	   else
	   {
	    context_info += std::string(" ClassId=") + sntoa(current_class_id);
	    if(class_id_restored)
	     context_info += std::string(" (restored)");
	   }
	  }
	 }
	 catch(...)
	 {
	  context_info += " (failed to get context info)";
	 }
	 
	 Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, context_info);
	}
   }

   // Используем обновленное значение UseFlag после возможной автоматической очистки
   if(!I->UseFlag || force)
   {
	list<UInstancesStorageElement>::iterator K;
	// Удалено избыточное логирование - создавало спам в INFO логах
	K=I; ++K;
	PopObject(instances,I);
	RDK_SYS_TRY
	{
	 try
	 {
	  UVirtualMethodFactory* virtual_factory=FindVirualMethodFactory(object);
	  if(virtual_factory)
	  {
	   virtual_factory->FreeComponent();
	  }
	  delete object;
	  ++count;
	 }
	 catch(...)
	 {
	  if(Logger)
	   Logger->LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("Exception raised when object ")+object_name);
	 }
	}
	RDK_SYS_CATCH
	{
	 if(Logger)
	  Logger->ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
	}
    I=K;
   }
   else
   {
	++I;
	// Удалено избыточное логирование - создавало огромный флуд в логах (2431+ сообщений)
	// Это нормальное поведение системы - объекты не уничтожаются, если они используются
//	if(!force)
//	{
//	if(!force)
//	{
//	 if(Logger)
   }
  }

//  size_t end_size=instances->second.size();
//  if(end_size>0)
//  size_t end_size=instances->second.size();
//  if(end_size>0)
//  {
//   if(Logger)
//	Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Warning, some objects in use: ")+sntoa(end_size));
  // Удалено избыточное логирование - создавало спам в INFO логах
 }
}

// Удаляет все свободные объекты заданного класса из хранилища
void UStorage::FreeObjectsStorageByClass(const UId &classid)
{
    UObjectsStorageIterator instances=ObjectsStorage.find(classid);

    if(instances==ObjectsStorage.end())
        return;
    std::string object_class_name=FindClassName(instances->first);

    if(instances->second.empty())
        return;

    size_t count=0;

    // Удалено избыточное логирование - создавало спам в INFO логах

    for(list<UInstancesStorageElement>::iterator I=instances->second.begin(); I != instances->second.end();)
    {
        std::string object_name=I->Object->GetName();
        if(I->UseFlag)
        {
            if(Logger)
                Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Can't destroy objects by name ")+object_name+": object in use!");
        }

        if(!I->UseFlag)
        {
            list<UInstancesStorageElement>::iterator K;
            // Удалено избыточное логирование - создавало спам в INFO логах
            K=I; ++K;
            UEPtr<UContainer> object=I->Object;
            PopObject(instances,I);
            RDK_SYS_TRY
            {
                try
                {
                UVirtualMethodFactory* virtual_factory=FindVirualMethodFactory(object);
                if(virtual_factory)
                {
                    virtual_factory->FreeComponent();
                }
                delete object;
                ++count;
                }
                catch(...)
                {
                if(Logger)
                    Logger->LogMessageEx(RDK_EX_FATAL, __FUNCTION__, std::string("Exception raised when object ")+object_name);
                }
            }
            RDK_SYS_CATCH
            {
                if(Logger)
                    Logger->ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
            }
            I=K;
        }
        else
        {
           ++I;
//           if(Logger)
//               Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects by name ")+object_name+" FAILED! Object in use.");
        }
    }

    // Удалено избыточное логирование - создавало спам в INFO логах

}

// Удаляет все объекты из хранилища
void UStorage::ClearObjectsStorage(bool force)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
												instances != iend; ++instances)
 {
  std::string object_class_name=FindClassName(instances->first);
  // Удалено избыточное логирование - создавало спам в INFO логах
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
  {
   std::string object_name=I->Object->GetName();
   // Удалено избыточное логирование - создавало спам в INFO логах
   I->Object->Free();
  }

  // Удалено избыточное логирование - создавало спам в INFO логах
 }

 FreeObjectsStorage(force);
}

// Удаляет все объекты заданного класса из хранилища
void UStorage::ClearObjectsStorageByClass(const UId &classid)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances ==ObjectsStorage.end())
  return;

 // КРИТИЧНО: Перед удалением класса очищаем UseFlag для объектов без активных ссылок
 // и восстанавливаем ClassId объектов из параметра classid, чтобы они не потеряли связь с Storage
 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
 {
  UEPtr<UContainer> object=I->Object;
  
  // КРИТИЧНО: Проверяем и восстанавливаем ClassId объекта из параметра classid
  if(object)
  {
   UId object_class_id = object->GetClass();
   if(object_class_id == ForbiddenId || object_class_id != classid)
   {
    // Восстанавливаем ClassId из параметра classid
    object->SetClass(classid);
    // Удалено избыточное логирование - создавало спам в DEBUG логах
   }
  }
  
  // Очищаем UseFlag для объектов без активных ссылок (без Owner и с Activity=false)
  if(I->UseFlag && object)
  {
   try
   {
    UEPtr<UContainer> owner = object->GetOwner();
    bool activity = object->Activity;
    
    // Если объект не имеет владельца и не активен, он больше не используется
    if(!owner && !activity)
    {
     I->UseFlag = false;
     // Удалено избыточное логирование - создавало спам в DEBUG логах
    }
   }
   catch(...)
   {
    // В случае ошибки оставляем UseFlag как есть
   }
  }
  
  // Вызываем Free() для объекта
  I->Object->Free();
  
  // КРИТИЧНО: НЕ сбрасываем ClassId объекта здесь, так как объект может все еще существовать
  // и использоваться. ClassId будет сохранен до полного уничтожения объекта.
 }

 ObjectsStorage.erase(instances);
}

/// Устанавливает состояние уже выданного компонента в состояние по умолчанию
void UStorage::DefaultObject(UEPtr<UContainer> object)
{
 if(object->GetStorage() != this)
  return;

 UId classid = object->GetClass();
 RDK::MVector<double,3> coord = object->GetCoord();
 bool activity = object->GetActivity();

 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 UClassStorageElement tmpl=tmplI->second;

 object->Default();
 tmpl->ResetComponent(static_pointer_cast<UComponent>(object));

 object->Activity = activity;
 object->Coord = coord;
}
// --------------------------

// --------------------------
// Методы управления описанием классов
// --------------------------

// --------------------------
void UStorage::SetClDescPath(const std::string& value)
{
    ClDesc = value;
}

// Получение пути к папке с описаниями классов
const std::string UStorage::GetClDescPath() const
{
    return ClDesc;
}

// Получение пути к папке с описанием конкретного класса
// Также создаёт необходимые папки
const std::string UStorage::GetCreateClDescPath(const std::string& class_name)
{
    std::string path = "";

    UEPtr<ULibrary> lib = FindCollection(class_name);
    if(lib)
    {
        path = ClDesc;
        // т.к. RDK::CreateNewDirectory делает mkdir, а не mkpath, необходимо по очереди создавать папки
        if(RDK::CreateNewDirectory(path.c_str())==0)
        {
            path += lib->GetName()+"/";
            if(RDK::CreateNewDirectory(path.c_str())==0)
            {
                path += "ru-RU/";
                if(RDK::CreateNewDirectory(path.c_str())==0)
                {
                    return path;
                }
            }
        }
    }

    return "";
}

// Возвращает XML описание класса
const UEPtr<UContainerDescription> UStorage::GetClassDescription(const std::string &classname, bool no_throw) const
{
 UClassesDescriptionCIterator I=ClassesDescription.find(classname);

 if(I == ClassesDescription.end())
 {
  if(no_throw)
   return 0;
  else
   throw EClassNameNotExist(classname);
 }

 return I->second;
}

// Устанавливает XML описание класса
// Класс в хранилище должен существовать
void UStorage::SetClassDescription(const std::string &classname, const UEPtr<UContainerDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(FindClassId(classname));

 if(I == ClassesStorage.end())
  throw EClassNameNotExist(classname);

 ClassesDescription[classname]=description;
}

// Загрузка описаний классов из xml-описаний
void UStorage::LoadClassesDescription()
{
    std::vector<string> lib_names;
    RDK::FindFilesList(ClDesc, "*", false, lib_names);
    
    // Кэшируем результаты FindFilesList для каждой библиотеки, чтобы избежать двойного вызова
    struct LibFileCache {
        std::string lib_name;
        std::string lib_cl_desc_path;
        std::vector<string> cl_desc_files;
    };
    
    std::vector<LibFileCache> lib_files_cache;
    size_t total_files = 0;
    
    // Собираем все файлы заранее (кэширование FindFilesList)
    for(std::vector<string>::iterator lib_name = lib_names.begin(); lib_name != lib_names.end(); ++lib_name)
    {
        std::string lib_cl_desc_path = ClDesc + *lib_name +"/ru-RU/";
        std::vector<string> cl_desc_files;
        RDK::FindFilesList(lib_cl_desc_path, "*.xml", true, cl_desc_files);
        
        if(!cl_desc_files.empty())
        {
            // Игнорируем проверки для ускорения загрузки
        }
        
        LibFileCache cache;
        cache.lib_name = *lib_name;
        cache.lib_cl_desc_path = lib_cl_desc_path;
        cache.cl_desc_files = std::move(cl_desc_files);
        total_files += cache.cl_desc_files.size();
        lib_files_cache.push_back(std::move(cache));
    }
    
    // Структура для хранения информации о файле для параллельной загрузки
    struct FileLoadInfo {
        std::string file_path;
        std::string lib_cl_desc_path;
        std::string file_name;
    };
    
    // Собираем все пути к файлам заранее
    std::vector<FileLoadInfo> files_to_load;
    files_to_load.reserve(total_files);
    
    for(const auto& cache : lib_files_cache)
    {
        for(const auto& file_name : cache.cl_desc_files)
        {
            FileLoadInfo info;
            info.file_path = cache.lib_cl_desc_path + file_name;
            info.lib_cl_desc_path = cache.lib_cl_desc_path;
            info.file_name = file_name;
            files_to_load.push_back(std::move(info));
        }
    }
    
    // Структура для результатов параллельной загрузки
    struct LoadedFileData {
        std::string class_name;
        std::string xml_string; // Сохраняем XML как строку, чтобы избежать проблем с копированием
        bool valid;
        
        // Метод для получения XML объекта
        USerStorageXML GetXML() const {
            USerStorageXML xml;
            if(!xml_string.empty())
                xml.Load(xml_string, "ClassDescription");
            return xml;
        }
    };
    
    // Параллельно загружаем XML файлы (только чтение, безопасно)
    std::vector<std::future<LoadedFileData>> futures;
    futures.reserve(files_to_load.size());
    
    for(const auto& file_info : files_to_load)
    {
        futures.push_back(std::async(std::launch::async, [file_info]() {
            LoadedFileData result;
            result.valid = false;
            
            try
            {
                // Каждый поток работает со своей копией XML структуры
                USerStorageXML xml;
                if(!xml.LoadFromFile(file_info.file_path, "ClassDescription"))
                {
                    result.valid = false;
                    return result;
                }
                // После LoadFromFile мы находимся в корне узла ClassDescription
                
                // Проверяем, что мы в правильном узле
                std::string root_name = xml.GetNodeName();
                if(root_name != "ClassDescription")
                {
                    // Если не в ClassDescription, пытаемся найти его
                    xml.SelectRoot();
                    if(!xml.SelectNode("ClassDescription"))
                    {
                        result.valid = false;
                        return result;
                    }
                }
                
                xml.SelectNodeForce("ClassName");
                result.class_name = xml.GetNodeText();
                // Возвращаемся к корню ClassDescription (не к корню документа!)
                // SelectUp() вернет нас из ClassName обратно к ClassDescription
                xml.SelectUp();
                
                // Убеждаемся, что мы в корне ClassDescription
                xml.SelectRoot();
                
                // Сохраняем XML как строку, чтобы избежать проблем с копированием USerStorageXML
                xml.SaveFromNode(result.xml_string);
                result.valid = true;
            }
            catch(const std::exception& ex)
            {
                // Игнорируем ошибки загрузки отдельных файлов
                result.valid = false;
            }
            catch(...)
            {
                // Игнорируем другие ошибки загрузки отдельных файлов
                result.valid = false;
            }
            
            return result;
        }));
    }
    
    // Последовательно добавляем загруженные описания в Storage (с синхронизацией)
    std::mutex desc_mutex;
    size_t processed_files = 0;
    
    for(auto& future : futures)
    {
        LoadedFileData loaded_data = future.get();
        
        if(!loaded_data.valid)
        {
            continue;
        }
        
        // Синхронизируем доступ к ClassesDescription
        std::lock_guard<std::mutex> lock(desc_mutex);
        
        try
        {
            SetClassDescription(loaded_data.class_name, new RDK::UContainerDescription());
            USerStorageXML xml = loaded_data.GetXML();
            LoadClassDescription(loaded_data.class_name, xml);
        }
        catch(const EClassNameNotExist&)
        {
            // Продолжаем загрузку других классов
        }
        catch(...)
        {
            // Пробрасываем другие исключения дальше
            throw;
        }
        
        // Обновление прогресса после каждого загруженного описания класса (19-20%)
        processed_files++;
        if(FuncProgressBarCallback && total_files > 0)
        {
            int progress = 19 + (int)(processed_files / total_files);
            std::string msg = "Launching application: loading class descriptions (" + 
                             RDK::sntoa(processed_files) + "/" + RDK::sntoa(total_files) + ")...";
            FuncProgressBarCallback(progress, msg);
        }
    }
}

// Сохраняет описание класса в файл на диск
void UStorage::SaveClassDescriptionToFile(const std::string &classname)
{
    UEPtr<UContainerDescription> cl_desc = GetClassDescription(classname, true);

    if(cl_desc)
    {
        RDK::USerStorageXML xml;
        xml.Create("ClassDescription");
        cl_desc->Save(xml);

        std::string save_path = GetCreateClDescPath(classname);

        save_path+=classname+".xml";
        xml.SaveToFile(save_path);
    }
}

// Сохраняет описание класса в xml
void UStorage::SaveClassDescription(const std::string &classname,
										USerStorageXML &xml)
{
 GetClassDescription(classname)->Save(xml);
}

// Загружает описание класса из xml
void UStorage::LoadClassDescription(const std::string &classname,
										USerStorageXML &xml)
{
 UEPtr<UContainerDescription> desc = GetClassDescription(classname, true);
 if(!desc)
 {
  return;
 }
 
 // ВАЖНО: xml уже должен быть позиционирован в корне ClassDescription после LoadFromFile
 xml.SelectRoot(); // Убеждаемся, что мы в корне ClassDescription
 
 // Загружаем описание
 desc->Load(xml);
}

// Сохраняет описание всех классов в xml
void UStorage::SaveClassesDescription(USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  xml.AddNode(I->first);
  I->second->Save(xml);
  xml.SelectUp();
 }
}

// Загружает описание всех классов из xml
void UStorage::LoadClassesDescription(USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  if(!xml.SelectNode(I->first))
   continue;
  I->second->Load(xml);
  xml.SelectUp();
 }
}

// Сохраняет общее описание всех классов в xml
bool UStorage::SaveCommonClassesDescription(USerStorageXML &xml)
{
 xml.AddNode("Default");

 xml.AddNode("Properties");
 auto I=CommonDescriptions.begin();
 while(I != CommonDescriptions.end())
 {
  xml.AddNode(I->first);

  xml.AddNode("Header");
  xml.SetNodeText(I->second.Header);
  xml.SelectUp();

  xml.SelectUp();
  ++I;
 }
 xml.SelectUp();
 xml.SelectUp();
 return true;
}

// Загружает общее описание всех классов из xml
bool UStorage::LoadCommonClassesDescription(USerStorageXML &xml)
{
 if(xml.SelectNode("Default"))
 {
  if(!xml.SelectNode("Properties"))
   return false;

  CommonDescriptions.clear();
  int num_parameters=xml.GetNumNodes();
  for(int i=0;i<num_parameters;i++)
  {
   if(!xml.SelectNode(i))
   {
	continue;
   }

   std::string nodename=xml.GetNodeName();
   if(xml.SelectNode("Header"))
   {
	CommonDescriptions[nodename].Header=xml.GetNodeText();
	xml.SelectUp();
   }

   xml.SelectUp();
  }
  xml.SelectUp();

  xml.SelectUp();
 }

 UClassesDescriptionIterator I=ClassesDescription.begin();

 while(I != ClassesDescription.end())
 {
  dynamic_pointer_cast<UContainerDescription>(I->second)->RemoveCommonDuplicatesDescriptions(CommonDescriptions);
  ++I;
 }

 return true;
}
// --------------------------



// --------------------------
// Методы управления библиотеками
// --------------------------
// Методы управления библиотеками
UEPtr<UExceptionLogger> const UStorage::GetLogger(void) const
{
 return Logger;
}

bool UStorage::SetLogger(UEPtr<UExceptionLogger> logger)
{
 if(Logger == logger)
  return true;

 Logger=logger;
 return true;
}

/// Установка callback для обновления прогресса инициализации
void UStorage::SetProgressBarCallback(ProgressBarCallback callback)
{
    FuncProgressBarCallback = callback;
}

/// Получение callback для обновления прогресса инициализации
ProgressBarCallback UStorage::GetProgressBarCallback() const
{
    return FuncProgressBarCallback;
}

// Возвращает библиотеку по индексу
UEPtr<ULibrary> UStorage::GetCollection(int index)
{
 return CollectionList[index];
}

// Возвращает число библиотек
int UStorage::GetNumCollections(void) const
{
 return int(CollectionList.size());
}

// Возвращает библиотеку по имени
UEPtr<ULibrary> UStorage::GetCollection(const string &name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib && lib->GetName() == name)
   return lib;
 }

 return 0;
}

// Возвращает имя библиотеки по индексу
const string& UStorage::GetCollectionName(int index)
{
 return CollectionList[index]->GetName();
}

// Возвращает версию библиотеки по индексу
const string& UStorage::GetCollectionVersion(int index)
{
 return CollectionList[index]->GetVersion();
}

// Очищает списки Complete и Incomplete во всех библиотеках
void UStorage::ClearAllLibsClassesNameArrays(void)
{
    for(size_t i=0;i<CollectionList.size();i++)
    {
     UEPtr<ULibrary> lib=CollectionList[i];
     lib->ClearIncompleteAndComplete();
    }
}

// Возвращается строку имен библиотек конкретного типа, разделенных запятой
// Буфер 'buffer' будет очищен от предыдущих значений
void UStorage::GetLibsNameListByType(std::string &buffer, int type) const
{
    buffer.clear();
    for(size_t i=0;i<CollectionList.size();i++)
    {
        UEPtr<ULibrary> lib = CollectionList[i];
        if(lib && lib->GetType() == type)
        {
            buffer.append(lib->GetName());
            buffer.append(",");
        }
    }
    if(!buffer.empty())
         buffer.erase(buffer.length()-1);
}

/// Непосредственно добавялет новый образец класса в хранилище
bool UStorage::AddClassToCollection(const std::string &new_class_name, const std::string &new_comp_name, bool force_replace, UContainer *newclass, const std::string &lib_name)
{
    if(new_class_name.empty())
        return false;

    // Библиотека куда добавляеться класс
	URuntimeLibrary *library = 0;

    for(size_t i=0;i<CollectionList.size();i++)
    {
        UEPtr<ULibrary> lib = CollectionList[i];
        if(lib && lib->GetName() == lib_name)
        {
			library = dynamic_cast<URuntimeLibrary*>(lib.Get());
            break;
        }
    }

    // Если не найдена или это не runtime библиотека
    if(!library || (library->GetType()!=2))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" +library->GetName() +"\" doesn't exist or it isn't runtime library");
        return false;
    }

    // Проверка на существование класса
    if(CheckClass(new_class_name))
    {
        // Разрешена ли замена
        if(force_replace)
        {
            // Библиотека где класс уже существует
            URuntimeLibrary *lib = static_cast<URuntimeLibrary*>(FindCollection(new_class_name).Get());
            if(!lib)
                return false;
            try
            {
                //Сначала удаляем из нужной
                if(!lib->DelClass(new_class_name))
                    return false;
            }
            catch(EObjectStorageNotEmpty &ex)
            {
             if(Logger)
              Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, ex.what());
              return false;
            }
            catch(EClassIdNotExist &ex)
            {
             if(Logger)
              Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, ex.what());
              return false;
            }
            // Добавление в нужную
            // AddNewClass сам запишет в Logger описания ошибок
            if(!library->AddNewClass(new_class_name, new_comp_name, newclass))
                return false;
        }
        else
        {
            return false;
        }
    }
    else
    {   // AddNewClass сам запишет в Logger описания ошибок
        if(!library->AddNewClass(new_class_name, new_comp_name, newclass))
            return false;
    }

    return true;
}

/// Удаляет образец класса из RT коллекции
bool UStorage::DelClassFromCollection(const std::string &class_name, const std::string &lib_name)
{
	URuntimeLibrary *library = 0;

    for(size_t i=0;i<CollectionList.size();i++)
    {
        UEPtr<ULibrary> lib = CollectionList[i];
        if(lib && lib->GetName() == lib_name)
        {
            library = static_cast<URuntimeLibrary*>(lib.Get());
        }
    }
    // Если не найдена или это не runtime библиотека
    if(!library || (library->GetType()!=2))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" +library->GetName() +"\" doesn't exist or it isn't runtime library");
        return false;
    }
    // Удаление компонента
    try
    {
        if(!library->DelClass(class_name))
            return  false;
    }
    catch(EObjectStorageNotEmpty &ex)
    {
     if(Logger)
        Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
        return false;
    }
    catch(EClassIdNotExist &ex)
    {
     if(Logger)
        Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
        return false;
    }
    return true;
}

/// Создает новую библиотеку с заданным именем
bool UStorage::CreateRuntimeCollection(const std::string &lib_name)
{
    if(lib_name.empty())
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Empty lib name");
        return false;
    }

	if(GetCollection(lib_name) != 0)
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library with name \"" + lib_name + "\" already exists");
        return false;
    }

    //Создание папки библиотеки
    std::string lib_path = LibrariesPath + "RTlibs/" + lib_name;

	URuntimeLibrary* lib=new URuntimeLibrary(lib_name,"", lib_path);

    if(AddCollection(lib))
	{
        //Создание папки библиотеки
        if(RDK::CreateNewDirectory(lib->GetLibPath().c_str())==0)
		{
         lib->Upload(this);
            return true;
        }
        else
        {
            if(Logger)
                Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "CreateNewDirectory() failed while creating directrory for library \"" + lib_name + "\"");
            delete lib;
            return false;
        }
    }
    else
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "AddCollection() failed while adding the library \"" + lib_name + "\"");
        delete lib;
        return false;
    }
}

/// Удаляет runtime-библиотеку вместе с папкой
bool UStorage::DeleteRuntimeCollection(const std::string &lib_name)
{
    int index = -1;
    for(size_t i=0;i<CollectionList.size();i++)
    {
        UEPtr<ULibrary> lib=CollectionList[i];
        if(lib && lib->GetName() == lib_name)
        {
           index = int(i);
           break;
        }
    }

    if(index < 0 || index >= int(CollectionList.size()))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \""+lib_name+"\" not found");
        return false;
    }

    std::vector<ULibrary*>::iterator I=CollectionList.begin()+index;
    // Если имя действительно runtime-библиотеки
    if((*I)->GetType() == 2)
    {
        // Неудачная попытка удаления папки
        if(!static_cast<URuntimeLibrary*>(*I)->DeleteOwnDirectory())
        {
            if(Logger)
               Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" + lib_name + "\" isn't runtime-library");
            return false;
        }
        delete *I;
        CollectionList.erase(I);

        try
        {
        // внутри DelClass() может выбросить исключения (EObjectStorageNotEmpty или EClassIdNotExist)
        DelAbandonedClasses();
        }
        catch(EObjectStorageNotEmpty &ex)
        {
         if(Logger)
          Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
          return false;
        }
        catch(EClassIdNotExist &ex)
        {
         if(Logger)
          Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
          return false;
        }
        return true;
    }
    else
    {
        if(Logger)
           Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" + lib_name + "\" isn't runtime-library");
        return false;
    }
}

// Установка пути к папкам библиотек
void UStorage::SetLibrariesPath(const std::string& value)
{
    LibrariesPath = value;
}

// Получение пути к папкам библиотек
const std::string UStorage::GetLibrariesPath() const
{
    return LibrariesPath;
}

/// Инициализация существующих динамических библиотек
/// Вызывается в Engine один раз
void UStorage::InitRTlibs(void)
{
    // Считывание имен библиотек из папки RTlibs
    std::string lib_path = LibrariesPath + "RTlibs";

    //Создание папки, если требуется
    if(RDK::CreateNewDirectory(lib_path.c_str()))
        return;

    // Проход по всем существующим xml файлам в папке
    // с записью их данных в строки ClassesStructures
    std::vector<std::string> lib_names;

    if(RDK::FindFilesList(lib_path,"*",false,lib_names))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "FindFilesList() error");
        return;
    }

    // Пустая директория - это нормально, не ошибка
    if (lib_names.empty())
    {
        if (Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, 
                "RTlibs directory is empty, skipping library loading");
        return;
    }

    for(size_t i = 0 ; i < lib_names.size(); i++)
    {
       LoadRuntimeCollection(lib_names[i]);
       
       // Обновление прогресса после каждой загруженной библиотеки (15-17%)
       if(FuncProgressBarCallback && lib_names.size() > 0)
       {
           int progress = 15 + (int)((i + 1) * 2 / lib_names.size());
           std::string msg = "Launching application: loading library " + lib_names[i] + " (" + 
                            RDK::sntoa(i + 1) + "/" + RDK::sntoa(lib_names.size()) + ")...";
           FuncProgressBarCallback(progress, msg);
       }
    }
}

/// Загружает runtime-библиотеку по её имени
bool UStorage::LoadRuntimeCollection(const std::string &lib_name)
{
    // Создание новой runtime-библиотеки
    if(lib_name.empty())
        return false;

    // уже существует
    if(GetCollection(lib_name) != 0)
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \""+lib_name+"\" already exists");
        return false;
    }

    //Создание папки библиотеки
    std::string lib_path = LibrariesPath + "RTlibs/" + lib_name;

    URuntimeLibrary* lib = new URuntimeLibrary(lib_name,"",lib_path);

    // Загрузка описаний компонентов внутри библиотеки
    if(!lib->LoadCompDescriptions())
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" + lib_name + "\" error ");
        delete lib;
        return false;
    }

    if(AddCollection(lib))
    {
        return true;
    }
    else
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "AddCollection() failed while adding the library \"" + lib_name + "\"");
        delete lib;
        return false;
    }

    //return true;
}


// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool UStorage::AddCollection(ULibrary *library, bool force_build)
{
 if(!library)
  return false;

 UEPtr<ULibrary> newlib=dynamic_cast<ULibrary*>(library);

 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib && lib->GetName() == newlib->GetName())
   return false;
 }

 CollectionList.push_back(library);
 
 // Обновляем индекс классов -> библиотек для оптимизации поиска
 // Это позволяет FindCollection() работать за O(1) вместо O(m)
 const std::vector<std::string>& complete_classes = newlib->GetComplete();
 for(const auto& class_name : complete_classes)
 {
  ClassLibraryIndex[class_name] = newlib;
 }
 
 if(force_build)
  BuildStorage();
 return true;
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool UStorage::DelCollection(int index)
{
 if(index < 0 || index >= int(CollectionList.size()))
  return false;
 std::vector<ULibrary*>::iterator I=CollectionList.begin()+index;
 UEPtr<ULibrary> lib_to_remove = *I;
 
 // Удаляем библиотеку из индекса классов -> библиотек
 if(lib_to_remove)
 {
  const std::vector<std::string>& complete_classes = lib_to_remove->GetComplete();
  for(const auto& class_name : complete_classes)
  {
   // Удаляем только если это та же библиотека (на случай дубликатов)
   auto index_it = ClassLibraryIndex.find(class_name);
   if(index_it != ClassLibraryIndex.end() && index_it->second == lib_to_remove)
   {
    ClassLibraryIndex.erase(index_it);
   }
  }
 }
 
 if((*I)->GetType() == 2)
 {
  //static_cast<URuntimeLibrary*>(*I)->DeleteOwnDirectory();
  delete *I;
 }
 CollectionList.erase(I);
 // Убрано: DelAbandonedClasses() - вызывающий код должен сам вызывать DelAbandonedClasses() при необходимости
 // Это критично для производительности - избегаем множественных вызовов при удалении нескольких библиотек
 return true;
}

bool UStorage::InitMockLibs(void)
{
    // Папка с библиотеками-заглушками и файл
    std::string lib_path = LibrariesPath + "/MockLibs/";
    std::string lib_list_file = lib_path + "0_LibList.xml";

    USerStorageXML LibList;
    if(!LibList.LoadFromFile(lib_list_file,"LibraryList"))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_ERROR, std::string("Error while loading Library List from file: " + lib_list_file));
        return false;
    }

    USerStorageXML CompDesctips;

    // Создание библиотек поочередно
    for(int i = 0, size = LibList.GetNumNodes() ; i < size; i++)
    {
        if(!LibList.SelectNode(i))
            continue;
        std::string lib_name = LibList.GetNodeText();

        // Если такая библиотека-заглушка есть
        if(lib_name.empty() || GetCollection(lib_name) != 0)
            continue;

        UMockLibrary* lib_mock=new UMockLibrary(lib_name, "", lib_path);

        // Заполнение описаний классов
        if(!CompDesctips.LoadFromFile(lib_path+"/"+lib_name+".xml","MockLib"))
        {
            if(Logger)
                Logger->LogMessage(RDK_EX_ERROR, std::string("Error while loading Library Classes Descriptions from file: " + lib_path+"/"+lib_name+".xml"));
            delete lib_mock;
            continue;
        }

        lib_mock->LoadFromXML(CompDesctips);

        if(!AddCollection(lib_mock))
        {
            delete lib_mock;
        }
        LibList.SelectUp();
    }

    return true;

}

bool UStorage::CreateMockLibs(void)
{
    if(Logger)
        Logger->LogMessage(RDK_EX_DEBUG, std::string("Creating Mock Libraries from Static Libraries"));

    // Создание библиотек-заглушек из статических библиотек
    for(size_t i=0;i<CollectionList.size();i++)
    {
        UEPtr<ULibrary> lib=CollectionList[i];
        if(lib && lib->GetType()==0)
        {
            // Создание папки библиотеки, если требуется
            std::string lib_path = LibrariesPath + "MockLibs/";

            if(RDK::CreateNewDirectory(lib_path.c_str()))
            {
                if(Logger)
                    Logger->LogMessage(RDK_EX_ERROR, std::string("Error while creating MockLibs path :" + lib_path));
                return false;
            }

            // имя библиотеки-заглушки
            std::string lib_name = lib->GetName()+"_Mock";

            // Если такая библиотека-заглушка есть
            if(GetCollection(lib_name) != 0)
                continue;

            UMockLibrary* lib_mock=new UMockLibrary(lib_name, "", lib_path);

            // Заполнение описаний классов
            lib->FillMockLibrary(lib_mock);

            if(!AddCollection(lib_mock))
            {
                delete lib_mock;
            }
        }
    }

    return true;
}

bool UStorage::SaveMockLibs(void)
{
    if(Logger)
        Logger->LogMessage(RDK_EX_DEBUG, std::string("Starting saving Mock Libraries to files"));

    // Сохранения списка библиотек-заглушек по порядку (такой же как в CollectionList)
    USerStorageXML LibList;
    LibList.Create("LibraryList");

    std::string lib_name = "";

    // Сохранение библиотек в отдельные файлы
    for(size_t i=0;i<CollectionList.size();i++)
    {
        UEPtr<ULibrary> lib=CollectionList[i];

        if(lib && lib->GetType()==3)
        {
            // Библиотека куда добавляется класс
            UMockLibrary *library = 0;
            library = dynamic_cast<UMockLibrary*>(lib.Get());

            library->SaveLibraryToFile();

            LibList.AddNode("library");
            LibList.SetNodeAttribute("Version",lib->GetVersion());
            LibList.SetNodeAttribute("Revision",sntoa(lib->GetRevision()));
            LibList.SetNodeAttribute("CoreVersion",GetGlobalVersion().ToStringFull());
            LibList.SetNodeText(lib->GetName());
            LibList.SelectUp();
        }
    }

    std::string file_name = LibrariesPath + "MockLibs/0_LibList.xml";
    LibList.SaveToFile(file_name);
    return true;
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UStorage::DelCollection(const string &name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib && lib->GetName() == name)
   return DelCollection(int(i));
 }

 return true;
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UStorage::DelAllCollections(void)
{
 CollectionList.clear();
 DelAbandonedClasses();
 return true;
}

// Уставнока необходимого режима сборки
void UStorage::SetBuildMode(int mode)
{
    BuildMode = mode;
}

// Получение текущего режима сборки
int UStorage::GetBuildMode()
{
    return BuildMode;
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool UStorage::BuildStorage(void)
{
 ClearAllLibsClassesNameArrays();
 switch (BuildMode)
 {
 case 1:
 {
     BuildStorage(0); // сборка статических библиотек
     BuildStorage(2); // сборка runtime-библиотек
     break;
 }

 case 2:
 {
     BuildStorage(0); // сборка статических библиотек

     // сборка статических библиотек
     if(InitMockLibs())
     {
        BuildStorage(3); // сборка mock-библиотек
     }
     else
     {
         if(Logger)
             Logger->LogMessage(RDK_EX_ERROR, std::string("Mock Libraries will not be built because of error in Mock Libraries Initialization"));
     }

     BuildStorage(2); // сборка runtime-библиотек
     break;
 }
 case 3:
 {
     // Иницилазиация мок-либ
     if(InitMockLibs())
     {
        BuildStorage(3); // сборка mock-библиотек
     }
     else
     {
         if(Logger)
             Logger->LogMessage(RDK_EX_ERROR, std::string("Mock Libraries will not be built because of error in Mock Libraries Initialization"));
     }

     BuildStorage(2); // сборка runtime-библиотек
     break;
 }
 }

 return true;
}

// Заполняет хранилище данными библиотек конктретного типа
// Тип библиотеки:
// Заполняет хранилище данными библиотек конктретного типа
// Тип библиотеки:
// 0 - Внутренняя библиотека (собрана вместе с ядром)
// 1 - Внешняя библиотека (загружена из внешней dll)
bool UStorage::BuildStorage(int lib_type)
{
    // Подсчитываем количество библиотек нужного типа для прогресса
    size_t total_libs = 0;
    for(size_t i=0;i<CollectionList.size();i++)
    {
        if(CollectionList[i] && CollectionList[i]->GetType()==lib_type)
            total_libs++;
    }
    
    size_t processed_libs = 0;
    for(size_t i=0;i<CollectionList.size();i++)
    {
     UEPtr<ULibrary> lib=CollectionList[i];
     if(lib && lib->GetType()==lib_type)
     {
      GetLogger()->LogMessage(RDK_EX_DEBUG, lib->GetName()+std::string(": collection version is ")+lib->GetVersion()+std::string(" (")+sntoa(lib->GetRevision())+")");

      if(lib->GetCoreVersion())
      {
       if(!lib->GetCoreVersion()->IsEqualFull(GetGlobalVersion()))
       {
        Logger->LogMessage(RDK_EX_FATAL, lib->GetName()+std::string(" collection SKIPPED: application core version ")+GetGlobalVersion().ToStringFull()+std::string(" is incompatible lib core version ")+lib->GetCoreVersion()->ToStringFull());
        continue;
       }
      }
      else
       Logger->LogMessage(RDK_EX_WARNING, lib->GetName()+std::string(" core version compatibility DOES NOT checked."));

      Logger->LogMessage(RDK_EX_DEBUG, std::string("Adding components from ")+lib->GetName()+" collection...");
      unsigned long long total_used_memory_before(0);
      unsigned long long largest_free_block_before(0);
      ReadUsedMemoryInfo(total_used_memory_before, largest_free_block_before);

      CollectionList[i]->Upload(this);
      unsigned long long total_used_memory_after(0);
      unsigned long long largest_free_block_after(0);
      if(ReadUsedMemoryInfo(total_used_memory_after, largest_free_block_after))
       Logger->LogMessage(RDK_EX_DEBUG, lib->GetName()+std::string(" eats ")+sntoa(total_used_memory_after-total_used_memory_before)+std::string(" bytes of RAM. Largest RAM block decreased to ")+sntoa(largest_free_block_before-largest_free_block_after)+" bytes");

      Logger->LogMessage(RDK_EX_DEBUG, std::string("Successfully added [")+sntoa(lib->GetComplete().size())+std::string("]: ")+concat_strings(lib->GetComplete(),std::string(",")));
      if(!lib->GetIncomplete().empty())
       Logger->LogMessage(RDK_EX_DEBUG, std::string("Failed to add [")+sntoa(lib->GetIncomplete().size())+std::string("]: ")+concat_strings(lib->GetIncomplete(),std::string(",")));
      CompletedClassNames.insert(CompletedClassNames.end(),
                                lib->GetComplete().begin(),
                                lib->GetComplete().end());
      IncompletedClassNames.insert(IncompletedClassNames.end(),
                                lib->GetIncomplete().begin(),
                                lib->GetIncomplete().end());
      
      // Обновление прогресса после каждой обработанной библиотеки (17-19%)
      processed_libs++;
      if(FuncProgressBarCallback && total_libs > 0)
      {
          int progress = 17 + (int)(processed_libs * 2 / total_libs);
          std::string msg = "Launching application: building library " + lib->GetName() + " (" + 
                           RDK::sntoa(processed_libs) + "/" + RDK::sntoa(total_libs) + ")...";
          FuncProgressBarCallback(progress, msg);
      }
     }
    }

    return true;
}

/// Удаляет все образцы классов, для которых нет библиотек
/// а также все связанные образцы
void UStorage::DelAbandonedClasses(void)
{
 // Оптимизация: строим индекс классов -> библиотек один раз перед циклом
 // Это уменьшает сложность с O(n*m) до O(n+m), где n - классы, m - библиотеки
 std::unordered_set<std::string> classes_with_libs;
 classes_with_libs.reserve(ClassesStorage.size());
 
 // Проходим по всем библиотекам и собираем имена классов
 for(size_t i=0; i<CollectionList.size(); i++)
 {
  UEPtr<ULibrary> lib = CollectionList[i];
  if(lib)
  {
   // Получаем список классов библиотеки
   const std::vector<std::string>& complete_classes = lib->GetComplete();
   for(const auto& class_name : complete_classes)
   {
    classes_with_libs.insert(class_name);
   }
  }
 }
 
 // Теперь проходим по классам и удаляем только те, для которых нет библиотек
 UClassesStorageIterator I=ClassesStorage.begin(),J;
 while(I != ClassesStorage.end())
 {
  J=I; ++J;
  std::string class_name = FindClassName(I->first);
  // Используем индекс вместо вызова FindCollection() для каждого класса
  if(classes_with_libs.find(class_name) == classes_with_libs.end())
   DelClass(I->first,true);
  I=J;
 }
}

/// Возвращает указатель на библиотеку класса по имени класса
UEPtr<ULibrary> UStorage::FindCollection(const std::string &class_name)
{
 // Оптимизация: используем индекс для O(1) поиска вместо O(m) линейного поиска
 auto index_it = ClassLibraryIndex.find(class_name);
 if(index_it != ClassLibraryIndex.end())
 {
  return index_it->second;
 }
 
 // Fallback: если индекс не содержит класс (например, при старых данных), используем линейный поиск
 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib && lib->IsClassNamePresent(class_name))
  {
   // Обновляем индекс для будущих вызовов
   ClassLibraryIndex[class_name] = lib;
   return lib;
  }
 }
 return 0;
}

UEPtr<ULibrary> UStorage::FindCollection(const UId &classid)
{
 return FindCollection(FindClassName(classid));
}


/// Формирует список зависимостей класса компонента от библиотек
/// Метод не очищает переданный список библиотек, а только пополняет его
void UStorage::FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies)
{
 UEPtr<RDK::UVirtualMethodFactory> factory=dynamic_pointer_cast<RDK::UVirtualMethodFactory>(GetComponentFactory(class_name));
 if(!factory)
  return;

 UEPtr<UContainer> class_data=factory->GetComponent();
 if(!class_data)
  return;

 UEPtr<ULibrary> lib=FindCollection(class_name);
 if(!lib)
  return;

 std::pair<std::string,std::string> lib_dep(lib->GetName(),lib->GetVersion());
 dependencies.push_back(lib_dep);
 for(int i=0;i<class_data->GetNumComponents();i++)
  FindComponentDependencies(FindClassName(class_data->GetComponentByIndex(i)->GetClass()),dependencies);
}
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// --------------------------
// Скрытые методы управления хранилищем объектов
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
void UStorage::PushObject(const UId &classid, UEPtr<UContainer> object)
{
 UInstancesStorage &instances=ObjectsStorage[classid];

 // КРИТИЧНО: Валидация ClassId объекта перед добавлением в Storage
 if(object)
 {
  UId object_class_id = object->GetClass();
  if(object_class_id != classid)
  {
   // Если ClassId не соответствует, устанавливаем правильный
   if(Logger && object_class_id != ForbiddenId)
   {
    Logger->LogMessageEx(RDK_EX_WARNING, __FUNCTION__, 
     std::string("Object ") + object->GetName() + 
     std::string(" ClassId mismatch: expected ") + sntoa(classid) + 
     std::string(", got ") + sntoa(object_class_id) + std::string(" - correcting"));
   }
   object->SetClass(classid);
  }
 }

 UInstancesStorageElement element(object,true);
 UInstancesStorageIterator it = instances.insert(instances.end(),element);
 // Update index map
 ObjectsIndex[object] = it;
 //list<UInstancesStorageElement>::iterator instI=instances.insert(instances.end(),element);
 //object->SetObjectIterator(&(*instI));
 object->SetClass(classid);

 object->SetStorage(this);
}

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// Выводит уже созданный объект из хранилища и возвращает
UId UStorage::PopObject(UEPtr<UContainer> object)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances == ObjectsStorage.end())
  return ForbiddenId;

 // Use index map for O(1) lookup
 auto index_it = ObjectsIndex.find(object);
 if(index_it != ObjectsIndex.end())
 {
  UInstancesStorageIterator list_it = index_it->second;
  // Verify iterator is still valid
  if(list_it != instances->second.end() && list_it->Object == object)
  {
   ObjectsIndex.erase(index_it);
   return PopObject(instances, list_it);
  }
  else
  {
   // Index is stale, remove it
   ObjectsIndex.erase(index_it);
  }
 }

 // Fallback to linear search if index is missing or stale
 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == object)
  {
   ObjectsIndex.erase(object); // Remove from index if present
   return PopObject(instances, I);
  }
 }

 return ForbiddenId;
}

// Перемещает объект в другое хранилище
void UStorage::MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage)
{
 newstorage->PushObject(PopObject(object),object);
}

// Возвращает объект в хранилище
// Выбранный объект помечается как свободный в хранилище
// Возвращает объект в хранилище
void UStorage::ReturnObject(UEPtr<UComponent> object)
{
 UEPtr<UContainer> obj=dynamic_pointer_cast<UContainer>(object);

 if(!obj)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_WARNING, __FUNCTION__, 
    std::string("Attempt to return null object"));
  return;
 }

 obj->Activity = false;
 obj->BreakOwner();

 UId class_id = object->GetClass();
 
 // Если ClassId = ForbiddenId, объект уже был удален из хранилища
 if(class_id == ForbiddenId)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, 
    std::string("Object ") + obj->GetName() + std::string(" has ForbiddenId - already removed from storage"));
  return;
 }

 UObjectsStorageIterator instances=ObjectsStorage.find(class_id);
 if(instances == ObjectsStorage.end())
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, 
    std::string("Object ") + obj->GetName() + std::string(" class not found in ObjectsStorage"));
  return;
 }

 // Use index map for O(log n) lookup
 auto index_it = ObjectsIndex.find(obj);
 if(index_it != ObjectsIndex.end())
 {
  UInstancesStorageIterator list_it = index_it->second;
  // Verify iterator is still valid
  if(list_it != instances->second.end() && list_it->Object == obj)
  {
   // Update index map
   ObjectsIndex[obj] = list_it;
   // КРИТИЧНО: Очищаем UseFlag, чтобы объект мог быть переиспользован
   list_it->UseFlag=false;
   // Удалено избыточное логирование - создавало спам в INFO логах
   return;
  }
  else
  {
   // Index is stale, remove it
   ObjectsIndex.erase(index_it);
  }
 }

 // Fallback to linear search if index is missing or stale
 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
					J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == obj)
  {
   // Update index map
   ObjectsIndex[obj] = I;
   // КРИТИЧНО: Очищаем UseFlag, чтобы объект мог быть переиспользован
   I->UseFlag=false;
   // Удалено избыточное логирование - создавало спам в INFO логах
   break;
  }
 }
}

// В случае ошибки возвращает ForbiddenId
UId UStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
{
 UEPtr<UContainer> object=object_iterator->Object;

 // Remove from index map
 ObjectsIndex.erase(object);
 instance_iterator->second.erase(object_iterator);

 UId classid=object->GetClass();
 //object->SetObjectIterator(0);
 object->SetStorage(0);
 
 // НЕ сбрасываем ClassId на ForbiddenId здесь, так как объект может все еще существовать
 // и использоваться. ClassId будет сброшен только при полном уничтожении объекта
 // в деструкторе или явном удалении. Это предотвращает ошибки EClassIdNotExist
 // при попытке получить имя класса через FindClassName().
 // object->SetClass(ForbiddenId);
 
 return classid;
}
// --------------------------


// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
// Скрытые методы таблицы соответствий классов
UId UStorage::AddLookupClass(const NameT &name)
{
 if(ClassesLookupTable.find(name) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(name);

 ClassesLookupTable.insert(make_pair(name,LastClassId+1));

 return LastClassId+1;
}

// Удаляет класс с именем 'name' из таблицы соотвествий
void UStorage::DelLookupClass(const NameT &name)
{
 auto I=ClassesLookupTable.find(name);

 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);

 ClassesLookupTable.erase(I);
}
// --------------------------

// --------------------------
// Методы для работы с компонентами-заглушками (UMockUnet)
// --------------------------
// Методы для работы с компонентами-заглушками (UMockUnet)
bool UStorage::AddCrPropMockFunc(funcCrPropMock func_ptr)
{
    // Нулевой указатель
    if(func_ptr == 0)
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Trying to add null function to FunctionsCrPropMock list in Storage");
        return false;
    }
    // Если уже существует
    if(std::find(FunctionsCrPropMock.begin(), FunctionsCrPropMock.end(), func_ptr) != FunctionsCrPropMock.end())
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Trying to add function that already exists in FunctionsCrPropMock list in Storage");
        return false;
    }

    FunctionsCrPropMock.push_back(func_ptr);
    return true;
}

// Получение массива функций-создателей свойств для UMockUnet
const std::list<funcCrPropMock>& UStorage::GetFunctionsCrPropMock() const
{
    return FunctionsCrPropMock;
}

} // namespace RDK

#endif
