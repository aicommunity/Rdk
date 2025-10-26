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
// ������� ������ ������������ �������� ������������� ������
// class UInstancesStorageElement
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement::UInstancesStorageElement(void)
 : UseFlag(false)
{
}

UInstancesStorageElement::UInstancesStorageElement(const UInstancesStorageElement &copy)
 : Object(copy.Object), UseFlag(copy.UseFlag)
{
}

UInstancesStorageElement::UInstancesStorageElement(const std::shared_ptr<UContainer> &object, bool useflag)
 : Object(object), UseFlag(useflag)
{

}


UInstancesStorageElement::~UInstancesStorageElement(void)
{
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UInstancesStorageElement& UInstancesStorageElement::operator = (const UInstancesStorageElement &copy)
{
 // ��������� �� ������
 Object=copy.Object;

 // ������� ���� �������� �� ������
 UseFlag=copy.UseFlag;

 return *this;
}

// ��������� ���������
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
// ������
// --------------------------
// ������������ � �����������
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

  // �������� ���� ���������
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
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �������������� Id �������
UId UStorage::GetLastClassId(void) const
{
 return LastClassId;
}
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
// ���������� Id ������ �� ��� �����
const UId& UStorage::FindClassId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);
 return I->second;
}

// ���������� ��� ������ �� ��� Id
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
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
UId UStorage::AddClass(std::shared_ptr<UComponentAbstractFactory> factory, const UId &classid)
{
// std::shared_ptr<UStorage> storage=classtemplate->GetStorage();
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

 // ��������!!! ��� ������������, �����-�� ���.
// ClassesDescription[FindClassName(id)]=classtemplate->NewDescription();

 return id;
}

// ��������� ������� ������ ������� � ���������
UId UStorage::AddClass(std::shared_ptr<UComponentAbstractFactory> factory, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(classname);

 UId id=AddClass(factory,classid);
 ClassesLookupTable[classname]=id;

 // ������ ClassDescription �� �����������
// ClassesDescription[classname]=factory->NewDescription();
// ClassesDescription[classname]->SetClassNameValue(classname);
 return id;
}

// ������� ������� ������ ������� �� ���������
// ���� 'force' == true �� ������������� ������� �� ���������
// ��� ������� ����� ������
void UStorage::DelClass(const UId &classid, bool force)
{
 UObjectsStorageIterator temp=ObjectsStorage.find(classid);

 if(!force)
 {
  FreeObjectsStorageByClass(classid);
  // ���� ����� ������� � ������ �� �������� ��������
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
  delete element.get();

 UClassesDescriptionIterator J=ClassesDescription.find(name);

 if(J != ClassesDescription.end())
 {
  if(J->second)
   J->second.reset();

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

 std::shared_ptr<ULibrary> lib=FindCollection(name);
 if(lib)
  lib->RemoveClassFromCompletedList(name);
}

// ��������� ������� ������� ������ ������� � ���������
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

// ���������� ������� ������
std::shared_ptr<UComponentAbstractFactory> UStorage::GetComponentFactory(const UId &classid) const
{
 UClassesStorageCIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 return I->second;
}

std::shared_ptr<UComponentAbstractFactory> UStorage::GetComponentFactory(const std::string &class_name) const
{
 UId id=FindClassId(class_name);
 return GetComponentFactory(id);
}

// ���������� ����� �������
int UStorage::GetNumClasses(void) const
{
 return int(ClassesStorage.size());
}

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UStorage::GetClassIdList(std::vector<UId> &buffer) const
{
 buffer.resize(0);
 buffer.reserve(ClassesStorage.size());
 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
  buffer.push_back(I->first);
}

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
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



// ������� ��� �� ������������ ������� ������� �� ���������
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

// ������� ��� ������� ������� �� ���������
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
	 // shared_ptr сам управляет памятью, не нужно вызывать delete
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
   delete I->second.get();
 }
 ClassesDescription.clear();
 LastClassId=0;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ��������
// --------------------------
// ��������� ������ �� ���������
// ���������� ��������� �� ��������� ������ �� ����� ������
// ��������� ������ ���������� ��� ������� � ���������
// ���� 'Activity' ������� ������������ � true
// ���� ���������� ������� �� ���������� �� ��������� � �����������
// � ���������
std::shared_ptr<UContainer> UStorage::TakeObject(const UId &classid, const std::shared_ptr<UContainer> &prototype)
{
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 UClassStorageElement tmpl=tmplI->second;
 
 // Проверяем валидность tmpl
 if(!tmpl)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Invalid class template for classid: ")+std::to_string(classid));
  return 0;
 }

 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  UInstancesStorageElement* element=0;// ��������!! instances->FindFree();
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
   std::shared_ptr<UContainer> obj=element->Object;

   if(obj)
   {
    element->UseFlag=true;
    obj->Default();
    if(!prototype)
     tmpl->ResetComponent(std::shared_ptr<UContainer>(obj.get()));
    else
      prototype->Copy(obj,get_shared_from_this());

    obj->Activity = true;
   }
   return obj;
  }
 }


 // ���� ���������� ������� �� �����
 std::shared_ptr<UContainer> obj;
 if(prototype)
 {
  if(!tmpl)
  {
   if(Logger)
    Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Invalid template for prototype creation, classid: ")+std::to_string(classid));
   return 0;
  }
  obj=tmpl->Prototype(prototype);
 }
 else
 {
  if(!tmpl)
  {
   if(Logger)
    Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Invalid template for new object creation, classid: ")+std::to_string(classid));
   return 0;
  }
  obj=tmpl->New();
 }

 if(!obj)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Class factory doesn't return object: ")+FindClassName(classid));
  return 0;
 }

 PushObject(classid,obj);
 if (obj) {
     // Временно закомментируем SetLogger для диагностики
     // obj->SetLogger(safe_shared_cast<ULoggerEnv>(Logger.get()));
     // obj->Activity = true;
 }

 return obj;
}

std::shared_ptr<UContainer> UStorage::TakeObject(const NameT &classname, const std::shared_ptr<UContainer> &prototype)
{
 try
 {
  return TakeObject(FindClassId(classname),prototype);
 }
 catch(const EClassNameNotExist& e)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Class not found: ")+classname);
  return nullptr;
 }
 catch(...)
 {
  if(Logger)
   Logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Unknown error when finding class: ")+classname);
  return nullptr;
 }
}


// ���������� Id ������, ���������� ������� 'object'
UId UStorage::FindClass(std::shared_ptr<UContainer> object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}

// ��������� ���������� �� ������ 'object' � ���������
bool UStorage::CheckObject(std::shared_ptr<UContainer> object) const
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

// ���� �������, ��������������� �������� �������� ���������
UVirtualMethodFactory* UStorage::FindVirualMethodFactory(std::shared_ptr<UContainer> object)
{
 if(!object)
  return 0;

 UClassesStorageCIterator instances=ClassesStorage.begin();
 for(;instances != ClassesStorage.end();++instances)
 {
  std::shared_ptr<UVirtualMethodFactory> virtual_factory=dynamic_pointer_cast<UVirtualMethodFactory>(instances->second);
  if(virtual_factory)
  {
   if(virtual_factory->GetComponent().get() == object.get())
        return virtual_factory.get();
  }
 }
 return 0;
}

// ��������� ��������� ����� �������� � ���������
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


// ������� ��� ��������� ������� �� ���������
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
	std::shared_ptr<UContainer> object=I->Object;
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
                object.reset();
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

// ������� ��� ��������� ������� ��������� ������ �� ���������
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
            std::shared_ptr<UContainer> object=I->Object;
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
                object.reset();
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

// ������� ��� ������� �� ���������
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

// ������� ��� ������� ��������� ������ �� ���������
void UStorage::ClearObjectsStorageByClass(const UId &classid)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances ==ObjectsStorage.end())
  return;

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
  I->Object->Free();

 ObjectsStorage.erase(instances);
}

/// ������������� ��������� ��� ��������� ���������� � ��������� �� ���������
void UStorage::DefaultObject(std::shared_ptr<UContainer> object)
{
 if(object->GetStorage().get() != this)
  return;

 UId classid = object->GetClass();
 RDK::MVector<double,3> coord = object->GetCoord();
 bool activity = object->GetActivity();

 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 UClassStorageElement tmpl=tmplI->second;

 object->Default();
 tmpl->ResetComponent(std::shared_ptr<UContainer>(object.get()));

 object->Activity = activity;
 object->Coord = coord;
}
// --------------------------

// --------------------------
// ������ ���������� ��������� �������
// --------------------------

// ��������� ���� � ����� � ���������� �������
void UStorage::SetClDescPath(const std::string& value)
{
    ClDesc = value;
}

// ��������� ���� � ����� � ���������� �������
const std::string UStorage::GetClDescPath() const
{
    return ClDesc;
}

// ��������� ���� � ����� � ��������� ����������� ������
// ����� ������ ����������� �����
const std::string UStorage::GetCreateClDescPath(const std::string& class_name)
{
    std::string path = "";

    std::shared_ptr<ULibrary> lib = FindCollection(class_name);
    if(lib)
    {
        path = ClDesc;
        // �.�. RDK::CreateNewDirectory ������ mkdir, � �� mkpath, ���������� �� ������� ��������� �����
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

// ���������� XML �������� ������
const std::shared_ptr<UContainerDescription> UStorage::GetClassDescription(const std::string &classname, bool nothrow) const
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

// ������������� XML �������� ������
// ����� � ��������� ������ ������������
void UStorage::SetClassDescription(const std::string &classname, const std::shared_ptr<UContainerDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(FindClassId(classname));

 if(I == ClassesStorage.end())
  throw EClassNameNotExist(classname);

 ClassesDescription[classname]=description;
}

// �������� �������� ������� �� xml-��������
void UStorage::LoadClassesDescription()
{
    std::vector<string> lib_names;
    RDK::FindFilesList(ClDesc, "*", false, lib_names);
    for(std::vector<string>::iterator lib_name = lib_names.begin(); lib_name != lib_names.end(); ++lib_name)
    {
        std::string lib_cl_desc_path = ClDesc + *lib_name +"/ru-RU/";

        std::vector<string> cl_desc_files;
        RDK::FindFilesList(lib_cl_desc_path, "*.xml", true, cl_desc_files);

        for(std::vector<string>::iterator cl_decs = cl_desc_files.begin(); cl_decs != cl_desc_files.end(); ++cl_decs)
        {
            USerStorageXML cl_desc_xml;
            cl_desc_xml.LoadFromFile(lib_cl_desc_path+*cl_decs,"ClassDescription");

            cl_desc_xml.SelectNodeForce("ClassName");
            std::string class_name = cl_desc_xml.GetNodeText();
            cl_desc_xml.SelectRoot();

            SetClassDescription(class_name, std::make_shared<RDK::UContainerDescription>());

            LoadClassDescription(class_name,cl_desc_xml);
        }
    }
}

// ��������� �������� ������ � ���� �� ����
void UStorage::SaveClassDescriptionToFile(const std::string &classname)
{
    std::shared_ptr<UContainerDescription> cl_desc = GetClassDescription(classname, true);

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

// ��������� �������� ������ � xml
void UStorage::SaveClassDescription(const std::string &classname,
										USerStorageXML &xml)
{
 GetClassDescription(classname)->Save(xml);
}

// ��������� �������� ������ �� xml
void UStorage::LoadClassDescription(const std::string &classname,
										USerStorageXML &xml)
{
 GetClassDescription(classname)->Load(xml);
}

// ��������� �������� ���� ������� � xml
void UStorage::SaveClassesDescription(USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  xml.AddNode(I->first);
  I->second->Save(xml);
  xml.SelectUp();
 }
}

// ��������� �������� ���� ������� �� xml
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

// ��������� ����� �������� ���� ������� � xml
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

// ��������� ����� �������� ���� ������� �� xml
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
// ������ ���������� ������������
// --------------------------
// ��������� �� ������
std::shared_ptr<ULoggerEnv> const UStorage::GetLogger(void) const
{
 return Logger;
}

bool UStorage::SetLogger(std::shared_ptr<ULoggerEnv> logger)
{
 if(Logger == logger)
  return true;

 Logger=logger;
 return true;
}

// ���������� ���������� �� �������
std::shared_ptr<ULibrary> UStorage::GetCollection(int index)
{
 return std::shared_ptr<ULibrary>(CollectionList[index], [](ULibrary*){});
}

// ���������� ����� ���������
int UStorage::GetNumCollections(void) const
{
 return int(CollectionList.size());
}

// ���������� ���������� �� �����
std::shared_ptr<ULibrary> UStorage::GetCollection(const string &name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
  if(lib && lib->GetName() == name)
   return lib;
 }

 return 0;
}

// ���������� ��� ���������� �� �������
const string& UStorage::GetCollectionName(int index)
{
 return CollectionList[index]->GetName();
}

// ���������� ������ ���������� �� �������
const string& UStorage::GetCollectionVersion(int index)
{
 return CollectionList[index]->GetVersion();
}

// ������� ������ Complete � Incomplete �� ���� �����������
void UStorage::ClearAllLibsClassesNameArrays(void)
{
    for(size_t i=0;i<CollectionList.size();i++)
    {
     std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
     lib->ClearIncompleteAndComplete();
    }
}

// ������������ ������ ���� ��������� ����������� ����, ����������� �������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UStorage::GetLibsNameListByType(std::string &buffer, int type) const
{
    buffer.clear();
    for(size_t i=0;i<CollectionList.size();i++)
    {
        std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
        if(lib && lib->GetType() == type)
        {
            buffer.append(lib->GetName());
            buffer.append(",");
        }
    }
    if(!buffer.empty())
         buffer.erase(buffer.length()-1);
}

/// ��������������� ��������� ����� ������� ������ � ���������
bool UStorage::AddClassToCollection(const std::string &new_class_name, const std::string &new_comp_name, bool force_replace, UContainer *newclass, const std::string &lib_name)
{
    if(new_class_name.empty())
        return false;

    // ���������� ���� ������������ �����
	URuntimeLibrary *library = 0;

    for(size_t i=0;i<CollectionList.size();i++)
    {
        std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
        if(lib && lib->GetName() == lib_name)
        {
			library = dynamic_cast<URuntimeLibrary*>(lib.get());
            break;
        }
    }

    // ���� �� ������� ��� ��� �� runtime ����������
    if(!library || (library->GetType()!=2))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" +library->GetName() +"\" doesn't exist or it isn't runtime library");
        return false;
    }

    // �������� �� ������������� ������
    if(CheckClass(new_class_name))
    {
        // ��������� �� ������
        if(force_replace)
        {
            // ���������� ��� ����� ��� ����������
            URuntimeLibrary *lib = static_cast<URuntimeLibrary*>(FindCollection(new_class_name).get());
            if(!lib)
                return false;
            try
            {
                //������� ������� �� ������
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
            // ���������� � ������
            // AddNewClass ��� ������� � Logger �������� ������
            if(!library->AddNewClass(new_class_name, new_comp_name, newclass))
                return false;
        }
        else
        {
            return false;
        }
    }
    else
    {   // AddNewClass ��� ������� � Logger �������� ������
        if(!library->AddNewClass(new_class_name, new_comp_name, newclass))
            return false;
    }

    return true;
}

/// ������� ������� ������ �� RT ���������
bool UStorage::DelClassFromCollection(const std::string &class_name, const std::string &lib_name)
{
	URuntimeLibrary *library = 0;

    for(size_t i=0;i<CollectionList.size();i++)
    {
        std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
        if(lib && lib->GetName() == lib_name)
        {
            library = static_cast<URuntimeLibrary*>(lib.get());
        }
    }
    // ���� �� ������� ��� ��� �� runtime ����������
    if(!library || (library->GetType()!=2))
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" +library->GetName() +"\" doesn't exist or it isn't runtime library");
        return false;
    }
    // �������� ����������
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

/// ������� ����� ���������� � �������� ������
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

    //�������� ����� ����������
    std::string lib_path = LibrariesPath + "RTlibs/" + lib_name;

	URuntimeLibrary* lib=new URuntimeLibrary(lib_name,"", lib_path);

    if(AddCollection(lib))
	{
        //�������� ����� ����������
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

/// ������� runtime-���������� ������ � ������
bool UStorage::DeleteRuntimeCollection(const std::string &lib_name)
{
    int index = -1;
    for(size_t i=0;i<CollectionList.size();i++)
    {
        std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
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
    // ���� ��� ������������� runtime-����������
    if((*I)->GetType() == 2)
    {
        // ��������� ������� �������� �����
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
        // ������ DelClass() ����� ��������� ���������� (EObjectStorageNotEmpty ��� EClassIdNotExist)
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

// ��������� ���� � ������ ���������
void UStorage::SetLibrariesPath(const std::string& value)
{
    LibrariesPath = value;
}

// ��������� ���� � ������ ���������
const std::string UStorage::GetLibrariesPath() const
{
    return LibrariesPath;
}

/// ������������� ������������ ������������ ���������
/// ���������� � Engine ���� ���
void UStorage::InitRTlibs(void)
{
    // ���������� ���� ��������� �� ����� RTlibs
    std::string lib_path = LibrariesPath + "RTlibs";

    //�������� �����, ���� ���������
    if(RDK::CreateNewDirectory(lib_path.c_str()))
        return;

    // ������ �� ���� ������������ xml ������ � �����
    // � ������� �� ������ � ������ ClassesStructures
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

/// ��������� runtime-���������� �� � �����
bool UStorage::LoadRuntimeCollection(const std::string &lib_name)
{
    // �������� ����� runtime-����������
    if(lib_name.empty())
        return false;

    // ��� ����������
    if(GetCollection(lib_name) != 0)
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \""+lib_name+"\" already exists");
        return false;
    }

    //�������� ����� ����������
    std::string lib_path = LibrariesPath + "RTlibs/" + lib_name;

    URuntimeLibrary* lib = new URuntimeLibrary(lib_name,"",lib_path);

    // �������� �������� ����������� ������ ����������
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


// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
// ���� force_build == true �� ���������� ������������ ������������� ����������
// � ���������
bool UStorage::AddCollection(ULibrary *library, bool force_build)
{
 if(!library)
  return false;

 std::shared_ptr<ULibrary> newlib(dynamic_cast<ULibrary*>(library), [](ULibrary*){});

 for(size_t i=0;i<CollectionList.size();i++)
 {
  std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
  if(lib && lib->GetName() == newlib->GetName())
   return false;
 }

 CollectionList.push_back(library);
 if(force_build)
  BuildStorage();
 return true;
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
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
    // ����� � ������������-���������� � ����
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

    // �������� ��������� ����������
    for(int i = 0, size = LibList.GetNumNodes() ; i < size; i++)
    {
        if(!LibList.SelectNode(i))
            continue;
        std::string lib_name = LibList.GetNodeText();

        // ���� ����� ����������-�������� ����
        if(lib_name.empty() || GetCollection(lib_name) != 0)
            continue;

        UMockLibrary* lib_mock=new UMockLibrary(lib_name, "", lib_path);

        // ���������� �������� �������
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

    // �������� ���������-�������� �� ����������� ���������
    for(size_t i=0;i<CollectionList.size();i++)
    {
        std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
        if(lib && lib->GetType()==0)
        {
            // �������� ����� ����������, ���� ���������
            std::string lib_path = LibrariesPath + "MockLibs/";

            if(RDK::CreateNewDirectory(lib_path.c_str()))
            {
                if(Logger)
                    Logger->LogMessage(RDK_EX_ERROR, std::string("Error while creating MockLibs path :" + lib_path));
                return false;
            }

            // ��� ����������-��������
            std::string lib_name = lib->GetName()+"_Mock";

            // ���� ����� ����������-�������� ����
            if(GetCollection(lib_name) != 0)
                continue;

            UMockLibrary* lib_mock=new UMockLibrary(lib_name, "", lib_path);

            // ���������� �������� �������
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

    // ���������� ������ ���������-�������� �� ������� (����� �� ��� � CollectionList)
    USerStorageXML LibList;
    LibList.Create("LibraryList");

    std::string lib_name = "";

    // ���������� ��������� � ��������� �����
    for(size_t i=0;i<CollectionList.size();i++)
    {
        std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});

        if(lib && lib->GetType()==3)
        {
            // ���������� ���� ����������� �����
            UMockLibrary *library = 0;
            library = dynamic_cast<UMockLibrary*>(lib.get());

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

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UStorage::DelCollection(const string &name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
  if(lib && lib->GetName() == name)
   return DelCollection(int(i));
 }

 return true;
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UStorage::DelAllCollections(void)
{
 CollectionList.clear();
 DelAbandonedClasses();
 return true;
}

// ��������� ������������ ������ ������
void UStorage::SetBuildMode(int mode)
{
    BuildMode = mode;
}

// ��������� �������� ������ ������
int UStorage::GetBuildMode()
{
    return BuildMode;
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UStorage::BuildStorage(void)
{
 ClearAllLibsClassesNameArrays();
 switch (BuildMode)
 {
 case 1:
 {
     BuildStorage(0); // ������ ����������� ���������
     BuildStorage(2); // ������ runtime-���������
     break;
 }

 case 2:
 {
     BuildStorage(0); // ������ ����������� ���������

     // ������������� ���-���
     if(InitMockLibs())
     {
        BuildStorage(3); // ������ mock-���������
     }
     else
     {
         if(Logger)
             Logger->LogMessage(RDK_EX_ERROR, std::string("Mock Libraries will not be built because of error in Mock Libraries Initialization"));
     }

     BuildStorage(2); // ������ runtime-���������
     break;
 }
 case 3:
 {
     // ������������� ���-���
     if(InitMockLibs())
     {
        BuildStorage(3); // ������ mock-���������
     }
     else
     {
         if(Logger)
             Logger->LogMessage(RDK_EX_ERROR, std::string("Mock Libraries will not be built because of error in Mock Libraries Initialization"));
     }

     BuildStorage(2); // ������ runtime-���������
     break;
 }
 }

 return true;
}

// ��������� ��������� ������� ��������� ������������ ����
// ��� ����������:
// 0 - ���������� ���������� (������� ������ � �����)
// 1 - ������� ���������� (��������� �� ������� dll)
// 2 - ����������, ��������� �� ����� ����������
// 3 - ����������-�������� (��� ����������-��������)
bool UStorage::BuildStorage(int lib_type)
{
    for(size_t i=0;i<CollectionList.size();i++)
    {
     std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
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

/// ������� ��� ������� �������, ��� ������� ��� ���������
/// � ����� ��� ��������� �������
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

/// ���������� ��������� �� ���������� ������ �� ����� ������
std::shared_ptr<ULibrary> UStorage::FindCollection(const std::string &class_name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  std::shared_ptr<ULibrary> lib(CollectionList[i], [](ULibrary*){});
  if(lib->IsClassNamePresent(class_name))
   return lib;
 }
 return 0;
}

std::shared_ptr<ULibrary> UStorage::FindCollection(const UId &classid)
{
 return FindCollection(FindClassName(classid));
}


/// ��������� ������ ������������ ������ ���������� �� ���������
/// ����� �� ������� ���������� ������ ���������, � ������ ��������� ���
void UStorage::FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies)
{
 std::shared_ptr<UVirtualMethodFactory> factory=dynamic_pointer_cast<RDK::UVirtualMethodFactory>(GetComponentFactory(class_name));
 if(!factory)
  return;

 std::shared_ptr<UContainer> class_data(factory->GetComponent().get());
 if(!class_data)
  return;

 std::shared_ptr<ULibrary> lib=FindCollection(class_name);
 if(!lib)
  return;

 std::pair<std::string,std::string> lib_dep(lib->GetName(),lib->GetVersion());
 dependencies.push_back(lib_dep);
 for(int i=0;i<class_data->GetNumComponents();i++)
  FindComponentDependencies(FindClassName(class_data->GetComponentByIndex(i)->GetClass()),dependencies);
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
void UStorage::PushObject(const UId &classid, std::shared_ptr<UContainer> object)
{
 UInstancesStorage &instances=ObjectsStorage[classid];

 UInstancesStorageElement element(object,true);
 instances.insert(instances.end(),element);
 //list<UInstancesStorageElement>::iterator instI=instances.insert(instances.end(),element);
 //object->SetObjectIterator(&(*instI));
 object->SetClass(classid);

 // Временно закомментируем SetStorage для диагностики
 // std::shared_ptr<UStorage> this_shared = get_shared_from_this();
 // if (object && this_shared) {
 //     object->SetStorage(std::weak_ptr<UStorage>(this_shared));
 // }
}

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
UId UStorage::PopObject(std::shared_ptr<UContainer> object)
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

// ���������� ������ � ������ ���������
void UStorage::MoveObject(std::shared_ptr<UContainer> object, std::shared_ptr<UStorage> newstorage)
{
 newstorage->PushObject(PopObject(object),object);
}

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
void UStorage::ReturnObject(std::shared_ptr<UContainer> object)
{

 object->Activity = false;
 object->BreakOwner();

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

// � ������ ������ ���������� ForbiddenId
UId UStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
{
 std::shared_ptr<UContainer> object=object_iterator->Object;

 instance_iterator->second.erase(object_iterator);

 UId classid=object->GetClass();
 //object->SetObjectIterator(0);
 object->ResetStorage();
 object->SetClass(ForbiddenId);
 return classid;
}
// --------------------------


// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
// ��������� ����� � ������ 'name' � ������� ������������
UId UStorage::AddLookupClass(const NameT &name)
{
 if(ClassesLookupTable.find(name) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(name);

 ClassesLookupTable.insert(make_pair(name,LastClassId+1));

 return LastClassId+1;
}

// ������� ����� � ������ 'name' �� ������� �����������
void UStorage::DelLookupClass(const NameT &name)
{
 map<NameT,UId>::iterator I=ClassesLookupTable.find(name);

 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);

 ClassesLookupTable.erase(I);
}
// --------------------------

// --------------------------
// ������ ��� ������ � ������������-���������� (UMockUnet)
// --------------------------
// ���������� �������-��������� ������� ��� UMockUnet � ������ � Storage
bool UStorage::AddCrPropMockFunc(funcCrPropMock func_ptr)
{
    // ������� ���������
    if(func_ptr == 0)
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Trying to add null function to FunctionsCrPropMock list in Storage");
        return false;
    }
    // ���� ��� ����������
    if(std::find(FunctionsCrPropMock.begin(), FunctionsCrPropMock.end(), func_ptr) != FunctionsCrPropMock.end())
    {
        if(Logger)
            Logger->LogMessage(RDK_EX_DEBUG, __FUNCTION__, "Trying to add function that already exists in FunctionsCrPropMock list in Storage");
        return false;
    }

    FunctionsCrPropMock.push_back(func_ptr);
    return true;
}

// ��������� ������� �������-���������� ������� ��� UMockUnet
const std::list<funcCrPropMock>& UStorage::GetFunctionsCrPropMock() const
{
    return FunctionsCrPropMock;
}

/* *************************************************************************** */
              /*
// --------------------------
// ������������ � �����������
// --------------------------
UStorage::EClassIdNotExist::EClassIdNotExist(UId id)
 : Id(id)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UStorage::EClassIdNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Id=")+sntoa(Id);
}
// --------------------------

// ������� ������ � ������� �� �����, �������������� � ���������
//class UStorage::EClassNameNotExist: public EError
// --------------------------
// ������������ � �����������
// --------------------------
UStorage::EClassNameNotExist::EClassNameNotExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UStorage::EClassNameNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------

// ������������ ��� ������
// class UStorage::EInvalidClassName: public EError
// --------------------------
// ������������ � �����������
// --------------------------
UStorage::EInvalidClassName::EInvalidClassName(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UStorage::EInvalidClassName::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------



// ����� � �������� ������ ��� ����������
//class UStorage::EClassNameAlredyExist: public EError
// --------------------------
// ������������ � �����������
// --------------------------
UStorage::EClassNameAlreadyExist::EClassNameAlreadyExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UStorage::EClassNameAlreadyExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------
       */
}


#endif

