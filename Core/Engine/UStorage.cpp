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
#include "UStorage.h"
#include "ULibrary.h"

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

UInstancesStorageElement::UInstancesStorageElement(const UEPtr<UContainer> &object, bool useflag)
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
 ClearObjectsStorage();
 ClearClassesStorage();
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
UId UStorage::AddClass(UEPtr<UComponent> classtemplate, const UId &classid)
{
 UEPtr<UStorage> storage=classtemplate->GetStorage();
 if(storage)
  storage->PopObject(classtemplate);

 UId id=classid;
 if(id == ForbiddenId)
  id=LastClassId+1;

 if(ClassesStorage.find(id) != ClassesStorage.end())
  throw EClassIdAlreadyExist(id);

 classtemplate->SetLogger(Logger);
 if(!classtemplate->Build())
  return ForbiddenId;

 ClassesStorage[id]=classtemplate;
 classtemplate->SetClass(id);
 LastClassId=id;

 // ��������!!! ��� ������������, �����-�� ���.
// ClassesDescription[FindClassName(id)]=classtemplate->NewDescription();

 return id;
}

// ��������� ������� ������ ������� � ���������
UId UStorage::AddClass(UEPtr<UComponent> classtemplate, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(classname);

 UId id=AddClass(classtemplate,classid);
 ClassesLookupTable[classname]=id;
 ClassesDescription[classname]=classtemplate->NewDescription();
 ClassesDescription[classname]->SetClassNameValue(classname);
 return id;
}

// ������� ������� ������ ������� �� ���������
// ���� 'force' == true �� ������������� ������� �� ���������
// ��� ������� ����� ������
void UStorage::DelClass(const UId &classid, bool force)
{
 UObjectsStorageCIterator temp=ObjectsStorage.find(classid);

 if(!force)
 {
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
UEPtr<UComponent> UStorage::GetClass(const UId &classid) const
{
 UClassesStorageCIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 return I->second;
}

UEPtr<UComponent> UStorage::GetClass(const std::string &class_name) const
{
 UId id=FindClassId(class_name);
 return GetClass(id);
}

// ���������� ����� �������
int UStorage::GetNumClasses(void) const
{
 return ClassesStorage.size();
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
void UStorage::FreeClassesStorage(void)
{
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageCIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() == 0)
  {
   DelClass(I->first);
   break;
  }
 }
}

// ������� ��� ������� ������� �� ���������
void UStorage::ClearClassesStorage(void)
{
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() != 0)
   throw EObjectStorageNotEmpty(I->first);
 }

 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
 {
  if(I->second)
   delete I->second.Get();
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
// ������ ���������� ���������� ��������
// --------------------------
// ��������� ������ �� ���������
// ���������� ��������� �� ��������� ������ �� ����� ������
// ��������� ������ ���������� ��� ������� � ���������
// ���� 'Activity' ������� ������������ � true
// ���� ���������� ������� �� ���������� �� ��������� � �����������
// � ���������
UEPtr<UComponent> UStorage::TakeObject(const UId &classid, const UEPtr<UComponent> &prototype)
{
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 UClassStorageElement tmpl=tmplI->second;
 UEPtr<UContainer> classtemplate=dynamic_pointer_cast<UContainer>(tmpl);

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
   UEPtr<UContainer> obj=element->Object;

   if(obj)
   {
    obj->Default();
    if(!prototype)
	 classtemplate->Copy(obj,this);
	else
	 dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj,this);

    obj->SetActivity(true);
    element->UseFlag=true;
   }
   return static_pointer_cast<UComponent>(obj);
  }
 }


 // ���� ���������� ������� �� �����
 UEPtr<UContainer> obj=classtemplate->New();
 PushObject(classid,obj);
 obj->SetLogger(Logger);
 obj->Default();

 // � ������, ���� ������ ��������� ��������������� ��� ����� �� ���������...
 if(!prototype)
  classtemplate->Copy(obj,this);
 else
  // � ������, ���� ������ ��������� �� ��������� ��� ����� ����� ��������
  // �������
  dynamic_pointer_cast<const UContainer>(prototype)->Copy(obj,this);

 obj->SetActivity(true);

 return static_pointer_cast<UComponent>(obj);
}

UEPtr<UComponent> UStorage::TakeObject(const NameT &classname, const UEPtr<UComponent> &prototype)
{
 return TakeObject(FindClassId(classname),prototype);
}


// ���������� Id ������, ���������� ������� 'object'
UId UStorage::FindClass(UEPtr<UComponent> object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}

// ��������� ���������� �� ������ 'object' � ���������
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

// ��������� ��������� ����� �������� � ���������
int UStorage::CalcNumObjects(void) const
{
 int result=0;

 for(UObjectsStorageCIterator I=ObjectsStorage.begin(),J=ObjectsStorage.end(); I!=J; ++I)
  result+=I->second.size();

 return result;
}

int UStorage::CalcNumObjects(const UId &classid) const
{
 UObjectsStorageCIterator instances=ObjectsStorage.find(classid);

 if(instances == ObjectsStorage.end())
  throw EClassIdNotExist(classid);

 return instances->second.size();
}

size_t UStorage::CalcNumObjects(const string &classname) const
{
 return CalcNumObjects(FindClassId(classname));
}


// �������� ��� ��������� ������� �� ���������
void UStorage::FreeObjectsStorage(void)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
				 								instances != iend; ++instances)
 {
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J;)
  {
   if(!I->UseFlag)
   {
	J=I; ++J;
	UEPtr<UContainer> object=I->Object;
	PopObject(instances,I);
	delete object;
	I=J;
   }
   else
    ++I;
  }
  instances->second.clear();
 }
}

// ������� ��� ������� �� ���������
void UStorage::ClearObjectsStorage(void)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
												instances != iend; ++instances)
 {
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
   I->Object->Free();
 }

 FreeObjectsStorage();
}

// �������� ��� ������� ��������� ������ �� ���������
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
// ������ ���������� ��������� �������
// --------------------------
// ���������� XML �������� ������
const UEPtr<UContainerDescription> UStorage::GetClassDescription(const std::string &classname) const
{
 UClassesDescriptionCIterator I=ClassesDescription.find(classname);

 if(I == ClassesDescription.end())
  throw EClassNameNotExist(classname);

 return I->second;
}

// ������������� XML �������� ������
// ����� � ��������� ������ ������������
void UStorage::SetClassDescription(const std::string &classname, const UEPtr<UContainerDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(FindClassId(classname));

 if(I == ClassesStorage.end())
  throw EClassNameNotExist(classname);

 ClassesDescription[classname]=description;
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

// ���������� ���������� �� �������
UEPtr<ULibrary> UStorage::GetCollection(int index)
{
 return CollectionList[index];
}

// ���������� ����� ���������
int UStorage::GetNumCollections(void) const
{
 return int(CollectionList.size());
}

// ���������� ���������� �� �����
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
		   /*
// ��������������� ��������� ����� ������� ������ � ���������
bool UStorage::AddClass(UContainer *newclass)
{
 UId classid=newclass->GetClass();
 if(!AddClass(newclass,classid) == ForbiddenId)
  return false;

 return true;
}            */

/// ��������������� ��������� ����� ������� ������ � ���������
bool UStorage::AddClassToCollection(const std::string &new_class_name, UContainer *newclass, URuntimeLibrary *library)
{
 library->UploadClass(std::string("T")+newclass->GetName(),newclass);
 return true;
}

/// ������� ����� ���������� � �������� ������
bool UStorage::CreateRuntimeCollection(const std::string &lib_name)
{
 URuntimeLibrary* lib=new URuntimeLibrary(lib_name,"");
 return AddCollection(lib);
}

/// ��������� runtime-���������� �� ������
bool UStorage::LoadRuntimeCollection(const std::string &buffer, bool force_build)
{
 USerStorageXML xml;
 xml.Load(buffer,"Library");
 std::string lib_name=xml.GetNodeAttribute("Name");
 if(lib_name.empty())
  return false;

 if(GetCollection(lib_name) != 0)
  return false;

 URuntimeLibrary* lib=new URuntimeLibrary(lib_name,"");
 lib->SetClassesStructure(xml);
 AddCollection(lib,force_build);

 return true;
}

/// ��������� runtime-���������� � ������
bool UStorage::SaveRuntimeCollection(const std::string &lib_name, std::string &buffer)
{
 UEPtr<URuntimeLibrary> lib=dynamic_pointer_cast<URuntimeLibrary>(GetCollection(lib_name));
 return SaveRuntimeCollection(lib,buffer);
}

bool UStorage::SaveRuntimeCollection(URuntimeLibrary *library, std::string &buffer)
{
 library->UpdateClassesStructure();
 library->GetClassesStructure().Save(buffer);
 return true;
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

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool UStorage::DelCollection(int index)
{
 if(index < 0 || index >= int(CollectionList.size()))
  return false;
 std::vector<ULibrary*>::iterator I=CollectionList.begin()+index;
 if((*I)->GetType() == 2)
  delete *I;
 CollectionList.erase(I);
 DelAbandonedClasses();
 return true;
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UStorage::DelCollection(const string &name)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib && lib->GetName() == name)
   return DelCollection(i);
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

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UStorage::BuildStorage(void)
{
 for(size_t i=0;i<CollectionList.size();i++)
 {
  UEPtr<ULibrary> lib=CollectionList[i];
  if(lib)
  {
   Logger->LogMessage(RDK_EX_DEBUG, std::string("Adding components from ")+lib->GetName()+" collection...");
   CollectionList[i]->Upload(this);
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


/// ��������� ������ ������������ ������ ���������� �� ���������
/// ����� �� ������� ���������� ������ ���������, � ������ ��������� ���
void UStorage::FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies)
{
 UEPtr<UContainer> class_data=dynamic_pointer_cast<UContainer>(GetClass(class_name));
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
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
void UStorage::PushObject(const UId &classid, UEPtr<UContainer> object)
{
 UEPtr<UComponent> classtemplate=ClassesStorage.find(classid)->second;

 UInstancesStorage &instances=ObjectsStorage[classid];

 UInstancesStorageElement element(object,true);
 list<UInstancesStorageElement>::iterator instI=instances.insert(instances.end(),element);
 object->ObjectIterator=&(*instI);
 object->SetClass(classid);

 object->SetStorage(this);
}

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
UId UStorage::PopObject(UEPtr<UContainer> object)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances == ObjectsStorage.end())
  throw EClassIdNotExist(object->GetClass());

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == object)
   return PopObject(instances, I);
 }

 return ForbiddenId;
}

// ���������� ������ � ������ ���������
void UStorage::MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage)
{
 newstorage->PushObject(PopObject(object),object);
}

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
void UStorage::ReturnObject(UEPtr<UComponent> object)
{
 UEPtr<UContainer> obj=dynamic_pointer_cast<UContainer>(object);
 if(obj->ObjectIterator)
  obj->ObjectIterator->UseFlag=false;
 obj->Activity=false;
 obj->BreakOwner();
}

// � ������ ������ ���������� ForbiddenId
UId UStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
{
 UEPtr<UContainer> object=object_iterator->Object;

 instance_iterator->second.erase(object_iterator);

 UId classid=object->GetClass();
 object->ObjectIterator=0;
 object->SetStorage(0);
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

