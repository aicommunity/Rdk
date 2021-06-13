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
}

UStorage::~UStorage(void)
{
 try
 {
  ClearObjectsStorage(true);
  ClearClassesStorage(true);

  // Удаление всех библиотек
  for(int i =0; i < int(CollectionList.size());i++)
  {
      DelCollection(i);
  }

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
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);
 return I->second;
}

// Возвращает имя класса по его Id
const NameT UStorage::FindClassName(const UId &id) const
{
 for(map<NameT,UId>::const_iterator I=ClassesLookupTable.begin(),
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
  ClearObjectsStorageByClass(classid);
 }

 UClassesStorageIterator I=ClassesStorage.find(classid);
 std::string name=FindClassName(classid);

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


 for(map<NameT,UId>::iterator I=ClassesLookupTable.begin(),
							  J=ClassesLookupTable.end(); I!=J; ++I)
 {
  if(I->second == classid)
  {
   ClassesLookupTable.erase(I);
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
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(classname);
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
 map<NameT,UId>::const_iterator I,J;

 I=ClassesLookupTable.begin();
 J=ClassesLookupTable.end();
 buffer.resize(0);
 buffer.reserve(ClassesLookupTable.size());

 for(map<NameT,UId>::const_iterator I=ClassesLookupTable.begin(),
									J=ClassesLookupTable.end(); I!=J; ++I)
 {
  buffer.push_back(I->first);
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
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() != 0)
  {
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Destory class which objecst in use: ")+FindClassName(I->first));
   if(!force)
	throw EObjectStorageNotEmpty(I->first);
   else
    break;
  }
 }

 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
 {
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
 }
 ClassesStorage.clear();

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
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
// Если свободного объекта не существует он создается и добавляется
// в хранилище
UEPtr<UComponent> UStorage::TakeObject(const UId &classid, const UEPtr<UComponent> &prototype)
{
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  throw EClassIdNotExist(classid);

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
	obj->Default();
	if(!prototype)
     tmpl->ResetComponent(static_pointer_cast<UComponent>(obj));
    else
     dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj,this);

    obj->SetActivity(true);
    element->UseFlag=true;
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
 obj->SetActivity(true);

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
  std::string object_class_name=FindClassName(instances->first);
  if(instances->second.empty())
   continue;

  size_t size=instances->second.size();
  size_t count=0;

  if(Logger)
   Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+" has begun");
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(); I != instances->second.end();)
  {
   std::string object_name=I->Object->GetName();
   if(I->UseFlag && force)
   {
	if(Logger)
	 Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("FORCED destroy objects by name ")+object_name+": object in use!");
   }

   if(!I->UseFlag || force)
   {
	list<UInstancesStorageElement>::iterator K;
	if(Logger)
	 Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects by name ")+object_name);
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
//	if(!force)
//	{
//	 if(Logger)
//	  Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects by name ")+object_name+" FAILED! Object in use.");
//	}
   }
  }

//  size_t end_size=instances->second.size();
//  if(end_size>0)
//  {
//   if(Logger)
//	Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Warning, some objects in use: ")+sntoa(end_size));
//  }
//  instances->second.clear();
  if(Logger)
   Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+std::string(" has finished: ")+sntoa(count)+std::string("/")+sntoa(size));
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

    size_t size=instances->second.size();
    size_t count=0;

    if(Logger)
        Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+" has begun");

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
            if(Logger)
                Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects by name ")+object_name);
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

    if(Logger)
        Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+std::string(" has finished: ")+sntoa(count)+std::string("/")+sntoa(size));

}

// Удаляет все объекты из хранилища
void UStorage::ClearObjectsStorage(bool force)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
												instances != iend; ++instances)
 {
  std::string object_class_name=FindClassName(instances->first);
  if(Logger)
   Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Free objects of class ")+object_class_name+" has begun");
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
  {
   std::string object_name=I->Object->GetName();
   if(Logger)
	Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Free objects by name ")+object_name);
   I->Object->Free();
  }

  if(Logger)
   Logger->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Free objects of class ")+object_class_name+" has finished");
 }

 FreeObjectsStorage(force);
}

// Удаляет все объекты заданного класса из хранилища
void UStorage::ClearObjectsStorageByClass(const UId &classid)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances ==ObjectsStorage.end())
  return;

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
  I->Object->Free();

 ObjectsStorage.erase(instances);
}
// --------------------------

// --------------------------
// Методы управления описанием классов
// --------------------------
// Возвращает XML описание класса
const UEPtr<UContainerDescription> UStorage::GetClassDescription(const std::string &classname, bool nothrow) const
{
 UClassesDescriptionCIterator I=ClassesDescription.find(classname);

 if(I == ClassesDescription.end())
 {
  if(nothrow)
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
 GetClassDescription(classname)->Load(xml);
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
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonDescriptions.begin();
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
// Указатель на логгер
UEPtr<ULoggerEnv> const UStorage::GetLogger(void) const
{
 return Logger;
}

bool UStorage::SetLogger(UEPtr<ULoggerEnv> logger)
{
 if(Logger == logger)
  return true;

 Logger=logger;
 return true;
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

    for(size_t i = 0 ; i < lib_names.size(); i++)
    {
       LoadRuntimeCollection(lib_names[i]);
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
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
// Если force_build == true то немедленно осущетсвляет развертывание бибилотеки
// в хранилище
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
 if((*I)->GetType() == 2)
 {
  //static_cast<URuntimeLibrary*>(*I)->DeleteOwnDirectory();
  delete *I;
 }
 CollectionList.erase(I);
 DelAbandonedClasses();
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
// 0 - Внутренняя библиотека (собрана вместе с ядром)
// 1 - Внешняя библиотека (загружена из внешней dll)
// 2 - Библиотека, созданная во время выполнения
// 3 - Библиотека-заглушка (все компоненты-заглушки)
bool UStorage::BuildStorage(int lib_type)
{
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
     }
    }

    return true;
}

/// Удаляет все образцы классов, для которых нет библиотек
/// а также все связанные образцы
void UStorage::DelAbandonedClasses(void)
{
 UClassesStorageIterator I=ClassesStorage.begin(),J;
 while(I != ClassesStorage.end())
 {
  J=I; ++J;
  if(!FindCollection(I->first))
   DelClass(I->first,true);
  I=J;
 }
}

/// Возвращает указатель на библиотеку класса по имени класса
UEPtr<ULibrary> UStorage::FindCollection(const std::string &class_name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib->IsClassNamePresent(class_name))
   return lib;
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
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
// Добавляет уже созданный объект в хранилище
// Если объект уже принадлежит иному хранилищу то возвращает false
void UStorage::PushObject(const UId &classid, UEPtr<UContainer> object)
{
 UInstancesStorage &instances=ObjectsStorage[classid];

 UInstancesStorageElement element(object,true);
 instances.insert(instances.end(),element);
 //list<UInstancesStorageElement>::iterator instI=instances.insert(instances.end(),element);
 //object->SetObjectIterator(&(*instI));
 object->SetClass(classid);

 object->SetStorage(this);
}

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// В случае ошибки возвращает ForbiddenId
UId UStorage::PopObject(UEPtr<UContainer> object)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances == ObjectsStorage.end())
  return ForbiddenId;

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == object)
   return PopObject(instances, I);
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
// Флаг 'Activity' объекта выставляется в false
void UStorage::ReturnObject(UEPtr<UComponent> object)
{
 UEPtr<UContainer> obj=dynamic_pointer_cast<UContainer>(object);

 obj->SetActivity(false);
 obj->BreakOwner();

 UObjectsStorageIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances == ObjectsStorage.end())
  return;

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == object)
  {
   I->UseFlag=false;
   break;
  }
 }
}

// В случае ошибки возвращает ForbiddenId
UId UStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
{
 UEPtr<UContainer> object=object_iterator->Object;

 instance_iterator->second.erase(object_iterator);

 UId classid=object->GetClass();
 //object->SetObjectIterator(0);
 object->SetStorage(0);
 object->SetClass(ForbiddenId);
 return classid;
}
// --------------------------


// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
// Добавляет класс с именем 'name' в таблицу соответствий
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
 map<NameT,UId>::iterator I=ClassesLookupTable.find(name);

 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);

 ClassesLookupTable.erase(I);
}
// --------------------------

// --------------------------
// Методы для работы с компонентами-заглушками (UMockUnet)
// --------------------------
// Добавление функции-создателя свойств для UMockUnet в массив в Storage
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

/* *************************************************************************** */
              /*
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UStorage::EClassIdNotExist::EClassIdNotExist(UId id)
 : Id(id)
{

}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UStorage::EClassIdNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Id=")+sntoa(Id);
}
// --------------------------

// Попытка работы с классом по имени, отсутствующему в хранилище
//class UStorage::EClassNameNotExist: public EError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UStorage::EClassNameNotExist::EClassNameNotExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UStorage::EClassNameNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------

// Некорректное имя класса
// class UStorage::EInvalidClassName: public EError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UStorage::EInvalidClassName::EInvalidClassName(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UStorage::EInvalidClassName::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------



// Класс с заданным именем уже существует
//class UStorage::EClassNameAlredyExist: public EError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UStorage::EClassNameAlreadyExist::EClassNameAlreadyExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UStorage::EClassNameAlreadyExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------
       */
}


#endif

