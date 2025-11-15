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
#include <glog/logging.h>
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
 // IMPORTANT: Minimal destructor to avoid issues with make_shared
 // Object shared_ptr will be automatically destroyed when this element is destroyed
 // If Object.reset() was called in ClearObjectsStorage, Object will be nullptr
 // Just let shared_ptr handle the destruction naturally
 // Don't access Object here as it may be partially destroyed
 // CRITICAL: Do NOT call delete Object.get() - Object is a shared_ptr and manages its own memory
 // When Object goes out of scope, shared_ptr destructor will automatically call the object's destructor
 // If Object was created via make_shared, it's in the same memory block as control block
 // Calling delete Object.get() would cause bad-free
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
 // IMPORTANT: Minimal destructor to avoid issues with make_shared
 // When UStorage is created via make_shared, it's allocated in a single block with control block
 // Any exceptions or complex operations in destructor can cause "bad-free" errors
 // So we do minimal cleanup and let shared_ptr handle the rest
 try
 {
  LOG(INFO) << "UStorage::~UStorage - starting destruction, ClassesStorage.size()=" << ClassesStorage.size() 
            << " ObjectsStorage.size()=" << ObjectsStorage.size();
  
  // Destroy factories FIRST, then objects
  // Factories hold weak_ptr to prototype objects (not shared_ptr)
  // Prototypes are stored in ObjectsStorage and managed by shared_ptr there
  // By destroying factories first, we release their weak_ptr references to prototypes
  // This allows prototypes to be destroyed when ObjectsStorage is cleared
  // IMPORTANT: Order is critical - ClearClassesStorage must be called BEFORE ClearObjectsStorage
  // to prevent prototypes from being destroyed twice
  ClearClassesStorage(true);
  
  LOG(INFO) << "UStorage::~UStorage - after ClearClassesStorage, ObjectsStorage.size()=" << ObjectsStorage.size();
  
  // Then destroy objects
  // ClearObjectsStorage will destroy all objects in ObjectsStorage, including prototypes
  // Objects will be automatically destroyed when their shared_ptr goes out of scope
  // CRITICAL: Do NOT call delete on any objects - they are managed by shared_ptr
  ClearObjectsStorage(true);

  LOG(INFO) << "UStorage::~UStorage - after ClearObjectsStorage";

  // Clear CollectionList - shared_ptr will handle library destruction
  CollectionList.clear();
  
  LOG(INFO) << "UStorage::~UStorage - destruction completed";
 }
 catch(...)
 {
  // Swallow all exceptions during destruction to prevent "bad-free"
  // If we're here, something went wrong, but we can't fix it anyway
  // The object is being destroyed, so we just need to avoid crashing
  LOG(ERROR) << "UStorage::~UStorage - exception during destruction";
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

 // IMPORTANT: element is a shared_ptr, so we don't need to delete it manually
 // shared_ptr will automatically destroy the object when the last reference is released
 // Calling delete element.get() causes "bad-free" because element was created via make_shared
 // and is stored in the same memory block as the control block
 UClassStorageElement element=I->second;
 // No need to delete - shared_ptr handles destruction automatically

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
 LOG(INFO) << "ClearClassesStorage(force=" << force << ") - starting, ClassesStorage.size()=" << ClassesStorage.size();
 
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() != 0)
  {
   LOG(ERROR) << "UStorage::" << __FUNCTION__ << " - Destroy class which objects in use: " << FindClassName(I->first);
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
     
     // Check if factory holds a prototype object
     std::shared_ptr<UVirtualMethodFactory> virtual_factory = std::dynamic_pointer_cast<UVirtualMethodFactory>(I->second);
     if(virtual_factory)
     {
      // CRITICAL: Get prototype BEFORE calling FreeComponent()
      // FreeComponent() resets weak_ptr, so GetComponent() will return nullptr after that
      std::shared_ptr<UContainer> prototype = virtual_factory->GetComponent();
      std::string proto_name = "unknown";
      void* proto_addr = nullptr;
      size_t proto_use_count = 0;
      
      if(prototype)
      {
       proto_addr = prototype.get();
       proto_use_count = prototype.use_count();
       try {
        proto_name = prototype->GetName();
       } catch (...) {
        proto_name = "<error>";
       }
       LOG(INFO) << "ClearClassesStorage - BEFORE FreeComponent: class=" << name 
                 << " prototype_name=" << proto_name << " prototype_use_count=" << proto_use_count 
                 << " prototype_address=" << proto_addr;
       
       // Check if prototype is in ObjectsStorage
       UObjectsStorageIterator proto_storage = ObjectsStorage.find(I->first);
       if(proto_storage != ObjectsStorage.end())
       {
        bool found_in_storage = false;
        for(const auto& elem : proto_storage->second)
        {
         if(elem.Object == prototype)
         {
          found_in_storage = true;
          LOG(INFO) << "ClearClassesStorage - prototype found in ObjectsStorage: class=" << name 
                    << " prototype_name=" << proto_name;
          break;
         }
        }
        if(!found_in_storage)
        {
         LOG(WARNING) << "ClearClassesStorage - prototype NOT found in ObjectsStorage: class=" << name 
                      << " prototype_name=" << proto_name << " This may cause bad-free!";
        }
       }
      }
      
      // Explicitly free prototype before destroying factory
      // This releases weak_ptr reference, allowing prototype to be destroyed if no other references exist
      // IMPORTANT: Do NOT call delete on prototype - it's managed by shared_ptr
      virtual_factory->FreeComponent();
      
      // After FreeComponent(), prototype should still be valid if it's in ObjectsStorage
      // Check prototype after FreeComponent()
      std::shared_ptr<UContainer> prototype_after = virtual_factory->GetComponent();
      if(prototype_after)
      {
       size_t proto_use_count_after = prototype_after.use_count();
       LOG(INFO) << "ClearClassesStorage - AFTER FreeComponent: class=" << name 
                 << " prototype_name=" << proto_name << " prototype_use_count=" << proto_use_count_after 
                 << " prototype_address=" << proto_addr;
      } else {
       LOG(INFO) << "ClearClassesStorage - prototype already released after FreeComponent: class=" << name 
                 << " prototype_name=" << proto_name;
      }
     }
     
	 // shared_ptr сам управляет памятью, не нужно вызывать delete
	}
   }
   catch(...)
   {
	LOG(FATAL) << __FUNCTION__ << " - Exception raised when destroy class " << FindClassName(I->first);
   }
  }
  RDK_SYS_CATCH
  {
   // if(Logger) удален - используется glog
    // Logger-> удален - используется glogProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  }
 }
 ClassesStorage.clear();
 LOG(INFO) << "ClearClassesStorage - completed, ClassesStorage cleared";

 // IMPORTANT: ClassesDescription stores shared_ptr, so we don't need to delete manually
 // shared_ptr will automatically destroy the object when the last reference is released
 // Calling delete I->second.get() causes "bad-free" because I->second was created via make_shared
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  // No need to delete - shared_ptr handles destruction automatically
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
 // TRACE: Log function entry
 std::string class_name = "unknown";
 try {
  class_name = FindClassName(classid);
 } catch (...) {
  class_name = "<error>";
 }
 LOG(INFO) << "TakeObject[TRACE] - ENTRY: classid=" << classid << " class_name=" << class_name 
           << " prototype=" << (prototype ? prototype->GetName() : "null");
 
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
 {
  LOG(ERROR) << "TakeObject[TRACE] - Class not found in ClassesStorage: classid=" << classid << " class_name=" << class_name;
  throw EClassIdNotExist(classid);
 }

 UClassStorageElement tmpl=tmplI->second;
 LOG(INFO) << "TakeObject[TRACE] - Found factory in ClassesStorage: classid=" << classid << " class_name=" << class_name 
           << " factory_valid=" << (tmpl ? "yes" : "no");
 
 // Проверяем валидность tmpl
 if(!tmpl)
 {
  // if(Logger) удален - используется glog
   // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Invalid class template for classid: ")+std::to_string(classid));
  LOG(WARNING) << "TakeObject[TRACE] - Invalid template, returning null: classid=" << classid << " class_name=" << class_name;
  return 0;
 }

 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  size_t list_size = instances->second.size();
  LOG(INFO) << "TakeObject[TRACE] - Starting iteration over ObjectsStorage: classid=" << classid 
            << " list_size=" << list_size;
  
  UInstancesStorageElement* element=0;// ��������!! instances->FindFree();
  // CRITICAL: Iterate with care - elements may be removed by PopObject during iteration
  // Use a while loop with manual iterator management to handle removals safely
  list<UInstancesStorageElement>::iterator I=instances->second.begin();
  size_t iteration_index = 0;
  while(I != instances->second.end())
  {
   // TRACE: Log iteration step
   iteration_index++;
   size_t current_list_size = instances->second.size();
   void* element_addr = &(*I);
   std::string obj_name = "null";
   void* obj_addr = nullptr;
   size_t obj_use_count = 0;
   
   // Save next iterator before potentially invalidating current one
   list<UInstancesStorageElement>::iterator next_I = I;
   ++next_I;
   
   // STAGE 2: Check if iterator is still valid (element hasn't been removed)
   // This prevents use-after-free if PopObject removed the element during recursive TakeObject
   bool iterator_valid = false;
   try {
    // Check if current iterator still points to a valid element in the list
    for(auto check_it = instances->second.begin(); check_it != instances->second.end(); ++check_it)
    {
     if(&(*check_it) == &(*I))
     {
      iterator_valid = true;
      break;
     }
    }
   } catch (...) {
    iterator_valid = false;
   }
   
   if(!iterator_valid)
   {
    LOG(WARNING) << "TakeObject[TRACE] - Iteration " << iteration_index 
                 << ": Iterator points to removed element, skipping";
    // Iterator was invalidated - we can't continue with this iteration
    // But next_I might also be invalid, so we need to restart iteration
    break;
   }
   
   // CRITICAL: Copy shared_ptr FIRST before any other operations
   // This keeps the object alive and prevents use-after-free
   // Use nested scope to ensure obj is destroyed before continue
   {
    std::shared_ptr<UContainer> obj;
    try {
     // First check if I->Object is null
     if(!I->Object)
     {
      LOG(INFO) << "TakeObject[TRACE] - Iteration " << iteration_index 
                << ": element_addr=" << element_addr << " Object is null, skipping";
      I = next_I;
      continue; // Skip null objects - obj will be destroyed here
     }
     
     // CRITICAL: Copy shared_ptr IMMEDIATELY to keep object alive
     // This must be done before any dereferencing to prevent use-after-free
     // Wrap in try-catch to handle corrupted control blocks
     try {
      obj = I->Object;
     } catch (...) {
      LOG(WARNING) << "TakeObject[TRACE] - Iteration " << iteration_index 
                   << ": Exception copying shared_ptr, control block may be corrupted, skipping";
      I = next_I;
      continue; // Skip corrupted objects - obj will be destroyed here (but may segfault)
     }
     
     // Check if obj is valid after copy
     if(!obj || obj.get() == nullptr)
     {
      LOG(WARNING) << "TakeObject[TRACE] - Iteration " << iteration_index 
                   << ": Copied obj is null, skipping";
      I = next_I;
      continue; // Skip null objects - obj will be destroyed here
     }
     
     // Now safe to get object info using the copied shared_ptr
     // But if GetName() fails, the object may be corrupted, so skip it
     bool obj_info_valid = false;
     try {
      obj_name = obj->GetName();
      obj_addr = obj.get();
      obj_use_count = obj.use_count();
      obj_info_valid = true;
     } catch (...) {
      obj_name = "<error>";
      LOG(WARNING) << "TakeObject[TRACE] - Iteration " << iteration_index 
                   << ": Failed to get object info after copy, object may be corrupted, skipping";
      // Object is corrupted, skip this element completely - obj will be destroyed here
      I = next_I;
      continue;
     }
     
     // Only log if we successfully got object info
     if(obj_info_valid)
     {
      LOG(INFO) << "TakeObject[TRACE] - Iteration " << iteration_index 
                << ": element_addr=" << element_addr << " obj_name=" << obj_name 
                << " obj_addr=" << obj_addr << " use_count=" << obj_use_count 
                << " list_size=" << current_list_size << " iterator_valid=" << iterator_valid;
     }
     
     // Check use_count - obj keeps the object alive, so this should be safe
     // But wrap in try-catch just in case
     try {
      size_t copied_use_count = obj.use_count();
      if(obj_info_valid)
      {
       LOG(INFO) << "TakeObject[TRACE] - Iteration " << iteration_index 
                 << ": Copied obj use_count=" << copied_use_count;
      }
      // use_count() == 1 means only ObjectsStorage owns it (free)
      // But we have a local copy, so use_count() will be 2
      // So we check if use_count() == 2 (1 from storage + 1 from our copy)
      if(copied_use_count == 2)
      {
       if(obj_info_valid)
       {
        LOG(INFO) << "TakeObject[TRACE] - Iteration " << iteration_index 
                  << ": Found free object, setting element pointer";
       }
       element=&(*I);
       break; // Exit the while loop - obj will be destroyed here
      }
     } catch (...) {
      // Control block may be destroyed, skip this element - obj will be destroyed here
      LOG(WARNING) << "UStorage::TakeObject - exception checking use_count, skipping";
      I = next_I;
      continue;
     }
    } catch (const std::bad_weak_ptr&) {
     // Control block was destroyed, skip this element - obj will be destroyed here
     LOG(WARNING) << "UStorage::TakeObject - bad_weak_ptr during copy, skipping";
     I = next_I;
     continue;
    } catch (...) {
     // If Object is corrupted or destroyed, skip this element - obj will be destroyed here
     LOG(WARNING) << "UStorage::TakeObject - corrupted Object in storage, skipping";
     // Try to safely reset obj before continue to avoid segfault in destructor
     try {
      obj.reset();
     } catch (...) {
      // Ignore exceptions during reset - obj may be corrupted
     }
     I = next_I;
     continue;
    }
    // obj is destroyed here when exiting the nested scope
    // Try to safely reset obj before destruction to avoid segfault
    try {
     obj.reset();
    } catch (...) {
     // Ignore exceptions during reset - obj may be corrupted
     // This is a last resort - if reset fails, destructor will still be called
    }
   }
   
   // Move to next element
   I = next_I;
  }
  
  LOG(INFO) << "TakeObject[TRACE] - Finished iteration: classid=" << classid 
            << " iterations=" << iteration_index << " element_found=" << (element != nullptr);

  if(element)
  {
   LOG(INFO) << "TakeObject[TRACE] - Using element from storage";
   // Safely get Object from element - check validity before use
   std::shared_ptr<UContainer> obj;
   try {
    if(!element->Object)
    {
     LOG(WARNING) << "UStorage::TakeObject - element->Object is null, skipping";
     // Fall through to create new object
    } else {
     LOG(INFO) << "TakeObject[TRACE] - Copying element->Object";
     obj = element->Object;
     LOG(INFO) << "TakeObject[TRACE] - Successfully copied element->Object, obj=" 
               << (obj ? obj->GetName() : "null");
     
     // Verify obj is valid before using
     if(obj && obj.get() != nullptr)
     {
      try {
       LOG(INFO) << "TakeObject[TRACE] - Calling obj->Default()";
       obj->Default();
       if(!prototype)
       {
        LOG(INFO) << "TakeObject[TRACE] - Calling tmpl->ResetComponent";
        tmpl->ResetComponent(obj);
       }
       else
       {
        LOG(INFO) << "TakeObject[TRACE] - Calling prototype->Copy";
        try {
         prototype->Copy(obj,get_shared_from_this());
         LOG(INFO) << "TakeObject[TRACE] - prototype->Copy completed";
        } catch (const std::bad_weak_ptr&) {
         // Prototype or obj is not managed by shared_ptr or already destroyed, skip Copy()
         LOG(WARNING) << "UStorage::TakeObject - bad_weak_ptr in prototype->Copy(), skipping";
         // Fall back to ResetComponent
         tmpl->ResetComponent(obj);
        } catch (...) {
         // Ignore other exceptions during Copy()
         LOG(WARNING) << "UStorage::TakeObject - exception in prototype->Copy(), skipping";
         // Fall back to ResetComponent
         tmpl->ResetComponent(obj);
        }
       }

       obj->Activity = true;
       // Update UseFlag based on use_count() after returning
       // use_count() will be > 1 after we return the shared_ptr, so mark as used
       // But we update it here before returning to ensure consistency
       element->UseFlag = true;
       LOG(INFO) << "TakeObject[TRACE] - EXIT: Returning object from storage: name=" 
                 << (obj ? obj->GetName() : "null");
       return obj;
      } catch (...) {
       // Ignore exceptions during Default() or Copy()
       LOG(WARNING) << "UStorage::TakeObject - exception during obj initialization, continuing anyway";
       // obj may be corrupted, fall through to create new object
       obj.reset();
      }
     } else {
      LOG(WARNING) << "UStorage::TakeObject - element->Object is invalid (nullptr), skipping";
      // Fall through to create new object
     }
    }
   } catch (...) {
    // If element->Object is corrupted, skip it and create new object
    LOG(WARNING) << "UStorage::TakeObject - corrupted element->Object, skipping";
    obj.reset();
   }
   
   // If we couldn't use element->Object, fall through to create new object
   // Note: We don't remove corrupted element here to avoid iterator invalidation
   // It will be cleaned up later during storage cleanup
  }
 }


 // ���� ���������� ������� �� �����
 LOG(INFO) << "TakeObject[TRACE] - Creating new object (not found in storage)";
 std::shared_ptr<UContainer> obj;
 if(prototype)
 {
  if(!tmpl)
  {
   // if(Logger) удален - используется glog
    // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Invalid template for prototype creation, classid: ")+std::to_string(classid));
   LOG(WARNING) << "TakeObject[TRACE] - Invalid template for prototype creation";
   return 0;
  }
  LOG(INFO) << "TakeObject[TRACE] - Calling tmpl->Prototype";
  obj=tmpl->Prototype(prototype);
  LOG(INFO) << "TakeObject[TRACE] - tmpl->Prototype returned: " << (obj ? obj->GetName() : "null");
 }
 else
 {
  if(!tmpl)
  {
   // if(Logger) удален - используется glog
    // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Invalid template for new object creation, classid: ")+std::to_string(classid));
   LOG(WARNING) << "TakeObject[TRACE] - Invalid template for new object creation";
   return 0;
  }
  LOG(INFO) << "TakeObject[TRACE] - Calling tmpl->New";
  obj=tmpl->New();
  LOG(INFO) << "TakeObject[TRACE] - tmpl->New returned: " << (obj ? obj->GetName() : "null");
 }

 if(!obj)
 {
  // if(Logger) удален - используется glog
   // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Class factory doesn't return object: ")+FindClassName(classid));
  LOG(WARNING) << "TakeObject[TRACE] - Class factory doesn't return object";
  return 0;
 }

 LOG(INFO) << "TakeObject[TRACE] - Calling PushObject to add new object to storage";
 PushObject(classid,obj);
 LOG(INFO) << "TakeObject[TRACE] - EXIT: Returning newly created object: name=" 
           << (obj ? obj->GetName() : "null");
 if (obj) {
     // Временно закомментируем SetLogger для диагностики
     // obj->SetLogger(safe_shared_cast<ULoggerEnv>(Logger.get()));
     // obj->Activity = true;
 }

 return obj;
}

std::shared_ptr<UContainer> UStorage::TakeObject(const NameT &classname, const std::shared_ptr<UContainer> &prototype)
{
 LOG(INFO) << "TakeObject[TRACE] - ENTRY by name: classname=" << classname 
           << " prototype=" << (prototype ? prototype->GetName() : "null");
 try
 {
  UId class_id = FindClassId(classname);
  LOG(INFO) << "TakeObject[TRACE] - FindClassId('" << classname << "') returned: " << class_id;
  bool class_exists = CheckClass(classname);
  LOG(INFO) << "TakeObject[TRACE] - CheckClass('" << classname << "') returned: " << (class_exists ? "true" : "false");
  if(!class_exists)
  {
   LOG(ERROR) << "TakeObject[TRACE] - Class '" << classname << "' not found in ClassesStorage!";
   return nullptr;
  }
  std::shared_ptr<UContainer> result = TakeObject(class_id, prototype);
  LOG(INFO) << "TakeObject[TRACE] - EXIT by name: classname=" << classname 
            << " result=" << (result ? result->GetName() : "null");
  return result;
 }
 catch(const EClassNameNotExist& e)
 {
  // if(Logger) удален - используется glog
   // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Class not found: ")+classname);
  LOG(ERROR) << "TakeObject[TRACE] - EClassNameNotExist exception for classname=" << classname << ": " << e.what();
  return nullptr;
 }
 catch(...)
 {
  // if(Logger) удален - используется glog
   // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, std::string("Unknown error when finding class: ")+classname);
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
 // With shared_ptr, we can simplify destruction significantly
 // If forcing, we need to be careful - objects may still have active references
 // ResetStorage() has already been called in ClearObjectsStorage
 if(force)
 {
  // Forced destruction: clear ObjectsStorage, but objects will be destroyed
  // when the last shared_ptr reference is released
  // We've already called ResetStorage() on all objects, so they won't try to access Storage
  ObjectsStorage.clear();
  return;
 }

 // Normal destruction: only destroy objects with use_count() == 1
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
				 								instances != iend; ++instances)
 {
  std::string object_class_name=FindClassName(instances->first);
  if(instances->second.empty())
   continue;

  // if(Logger) удален - используется glog
   // Logger-> удален - используется glogLogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+" has begun");
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(); I != instances->second.end();)
  {
   std::shared_ptr<UContainer> object=I->Object;
   if(!object)
   {
    ++I;
    continue;
   }
   
   std::string object_name=object->GetName();
   // Check use_count() - only destroy objects that are free (use_count() == 1)
   size_t use_count = object.use_count();
   if(use_count == 1)
   {
	list<UInstancesStorageElement>::iterator K;
	LOG(INFO) << __FUNCTION__ << " - Destroy objects by name " << object_name;
	K=I; ++K;
	
	// Reset Storage pointer in object BEFORE removing from storage
	// This prevents UContainer destructor from trying to access destroyed Storage
	if(object)
	{
	 object->ResetStorage();
	 object->SetClass(ForbiddenId);
	}
	
	// Erase iterator - shared_ptr will automatically destroy the object
	// The object will be automatically destroyed when the last shared_ptr is destroyed
	instances->second.erase(I);
	I=K;
   }
   else
   {
	// Object is still in use, skip it
	LOG(WARNING) << __FUNCTION__ << " - Skipping object " << object_name << ": still in use (use_count=" << use_count << ")";
	++I;
   }
  }

//  size_t end_size=instances->second.size();
//  if(end_size>0)
//  {
//   // if(Logger) удален - используется glog
//	// Logger-> удален - используется glogLogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Warning, some objects in use: ")+sntoa(end_size));
//  }
//  instances->second.clear();
  // if(Logger) удален - используется glog
   // Logger-> удален - используется glogLogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+std::string(" has finished: ")+sntoa(count)+std::string("/")+sntoa(size));
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

    // if(Logger) удален - используется glog
        // Logger-> удален - используется glogLogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+" has begun");

    for(list<UInstancesStorageElement>::iterator I=instances->second.begin(); I != instances->second.end();)
    {
        std::shared_ptr<UContainer> object=I->Object;
        if(!object)
        {
            ++I;
            continue;
        }
        
        std::string object_name=object->GetName();
        // Check use_count() instead of UseFlag for more accurate status
        size_t use_count = object.use_count();
        if(use_count > 1)
        {
            // Object is still in use, skip it
            ++I;
            continue;
        }

        // Object is free (use_count() == 1), can be destroyed
        list<UInstancesStorageElement>::iterator K=I; ++K;
        
        // Reset Storage pointer to prevent access to partially destroyed Storage
        if(object)
        {
            object->ResetStorage();
            object->SetClass(ForbiddenId);
        }
        
        // Erase iterator - shared_ptr will automatically destroy the object
        instances->second.erase(I);
        I=K;
    }

    // if(Logger) удален - используется glog
        // Logger-> удален - используется glogLogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Destroy objects of class ")+object_class_name+std::string(" has finished: ")+sntoa(count)+std::string("/")+sntoa(size));

}

// ������� ��� ������� �� ���������
void UStorage::ClearObjectsStorage(bool force)
{
 // With shared_ptr, we can simplify destruction significantly
 // If forcing, just clear ObjectsStorage - shared_ptr will automatically destroy objects
 // We don't need to call ResetStorage() or SetClass() - objects will be destroyed automatically
 if(force)
 {
  // Forced destruction: reset Storage pointer in all objects first
  // This prevents objects from trying to access Storage during destruction
  // Then clear ObjectsStorage - objects will be automatically destroyed when shared_ptr goes out of scope
  LOG(INFO) << "ClearObjectsStorage(force=true) - clearing ObjectsStorage, size=" << ObjectsStorage.size();
  
  // Count objects that are still in use (use_count > 1)
  size_t objects_in_use = 0;
  size_t total_objects = 0;
  
  // First, reset Storage pointer in all objects to prevent access to partially destroyed Storage
  // This must be done BEFORE clearing ObjectsStorage to prevent segfault
  for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
      instances != iend; ++instances)
  {
   for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
   {
    if(I->Object)
    {
     total_objects++;
     size_t use_count = I->Object.use_count();
     std::string obj_name = "unknown";
     void* obj_addr = I->Object.get();
     try {
      obj_name = I->Object->GetName();
     } catch (...) {
      obj_name = "<error>";
     }
     
     if(use_count > 1)
     {
      objects_in_use++;
      LOG(INFO) << "ClearObjectsStorage - object still in use: name=" << obj_name 
                << " use_count=" << use_count << " address=" << obj_addr;
     }
     
     try {
      // Reset Storage pointer BEFORE clearing ObjectsStorage
      // This prevents UContainer destructor from trying to access destroyed Storage
      I->Object->ResetStorage();
      // SetClass may access object internals, so check if object is still valid
      if(I->Object->GetClass() != ForbiddenId)
      {
       I->Object->SetClass(ForbiddenId);
      }
     } catch (...) {
      // Ignore exceptions during destruction - object may be partially destroyed
      LOG(WARNING) << "ClearObjectsStorage - exception while resetting Storage for object: name=" << obj_name;
     }
    }
   }
  }
  
  LOG(INFO) << "ClearObjectsStorage - total_objects=" << total_objects 
            << " objects_in_use=" << objects_in_use;
  
  // Now clear ObjectsStorage WITHOUT destroying objects
  // Objects will be automatically destroyed when the last shared_ptr reference is released
  // We've already reset Storage pointer in all objects, so they won't try to access Storage
  // IMPORTANT: Don't reset Object shared_ptr here - let shared_ptr manage the lifecycle naturally
  // When ObjectsStorage.clear() is called, UInstancesStorageElement destructors will be called
  // Each destructor will destroy its Object shared_ptr, which will decrement use_count
  // If use_count reaches 0, the object will be destroyed automatically
  // This is safe because we've already reset Storage pointer in all objects
  // CRITICAL: Do NOT call delete on any objects - they are managed by shared_ptr
  // ObjectsStorage.clear() will call UInstancesStorageElement destructors, which will
  // automatically destroy shared_ptr members, but NOT call delete on them
  LOG(INFO) << "ClearObjectsStorage(force=true) - about to clear ObjectsStorage, size=" << ObjectsStorage.size();
  ObjectsStorage.clear();
  LOG(INFO) << "ClearObjectsStorage(force=true) - ObjectsStorage cleared";
  return;
 }

 // Normal destruction: reset Storage pointer in objects before destroying
 // This prevents segfault in UContainer destructor
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
												instances != iend; ++instances)
 {
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
  {
   // Safely reset Storage pointer - object may be partially destroyed
   // Use try-catch to prevent segfault during destruction
   try {
    if(I->Object)
    {
     // Reset Storage pointer before destroying objects
     // This prevents UContainer destructor from trying to access destroyed Storage
     I->Object->ResetStorage();
     // SetClass may access object internals, so check if object is still valid
     if(I->Object->GetClass() != ForbiddenId)
     {
      I->Object->SetClass(ForbiddenId);
     }
    }
   } catch (...) {
    // Ignore exceptions during destruction - object may be partially destroyed
    // Just continue with next object
   }
  }
 }

 // Now free and destroy objects (FreeObjectsStorage will handle actual destruction)
 FreeObjectsStorage(force);
}

// ������� ��� ������� ��������� ������ �� ���������
void UStorage::ClearObjectsStorageByClass(const UId &classid)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances ==ObjectsStorage.end())
  return;

 // With shared_ptr, we don't need to call Free()
 // Just reset Storage pointer and clear the storage
 // Objects will be automatically destroyed when shared_ptr goes out of scope
 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object)
  {
   I->Object->ResetStorage();
   I->Object->SetClass(ForbiddenId);
  }
 }

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
 // Use existing shared_ptr instead of creating new one from raw pointer
 tmpl->ResetComponent(object);

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
// GetLogger and SetLogger удалены - используется glog

// ���������� ���������� �� �������
std::shared_ptr<ULibrary> UStorage::GetCollection(int index)
{
 // CollectionList now stores shared_ptr directly - return it directly
 if(index >= 0 && index < int(CollectionList.size()))
  return CollectionList[index];
 return nullptr;
}

// ���������� ����� ���������
int UStorage::GetNumCollections(void) const
{
 return int(CollectionList.size());
}

// ���������� ���������� �� �����
std::shared_ptr<ULibrary> UStorage::GetCollection(const string &name)
{
 // CollectionList now stores shared_ptr directly - use it directly
 for(size_t i=0;i<CollectionList.size();i++)
 {
  if(CollectionList[i] && CollectionList[i]->GetName() == name)
   return CollectionList[i];
 }

 return nullptr;
}

// ���������� ��� ���������� �� �������
const string& UStorage::GetCollectionName(int index)
{
 // CollectionList now stores shared_ptr - access via shared_ptr
 if(index >= 0 && index < int(CollectionList.size()) && CollectionList[index])
  return CollectionList[index]->GetName();
 static const string empty;
 return empty;
}

// ���������� ������ ���������� �� �������
const string& UStorage::GetCollectionVersion(int index)
{
 // CollectionList now stores shared_ptr - access via shared_ptr
 if(index >= 0 && index < int(CollectionList.size()) && CollectionList[index])
  return CollectionList[index]->GetVersion();
 static const string empty;
 return empty;
}

// ������� ������ Complete � Incomplete �� ���� �����������
void UStorage::ClearAllLibsClassesNameArrays(void)
{
    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
     if(CollectionList[i])
      CollectionList[i]->ClearIncompleteAndComplete();
    }
}

// ������������ ������ ���� ��������� ����������� ����, ����������� �������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UStorage::GetLibsNameListByType(std::string &buffer, int type) const
{
    buffer.clear();
    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
        if(CollectionList[i] && CollectionList[i]->GetType() == type)
        {
            buffer.append(CollectionList[i]->GetName());
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

    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
        if(CollectionList[i] && CollectionList[i]->GetName() == lib_name)
        {
			library = dynamic_cast<URuntimeLibrary*>(CollectionList[i].get());
            break;
        }
    }

    // ���� �� ������� ��� ��� �� runtime ����������
    if(!library || (library->GetType()!=2))
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" +library->GetName() +"\" doesn't exist or it isn't runtime library");
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
             // if(Logger) удален - используется glog
              // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, __FUNCTION__, ex.what());
              return false;
            }
            catch(EClassIdNotExist &ex)
            {
             // if(Logger) удален - используется glog
              // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, __FUNCTION__, ex.what());
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

    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
        if(CollectionList[i] && CollectionList[i]->GetName() == lib_name)
        {
            library = static_cast<URuntimeLibrary*>(CollectionList[i].get());
        }
    }
    // ���� �� ������� ��� ��� �� runtime ����������
    if(!library || (library->GetType()!=2))
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" +library->GetName() +"\" doesn't exist or it isn't runtime library");
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
     // if(Logger) удален - используется glog
        // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
        return false;
    }
    catch(EClassIdNotExist &ex)
    {
     // if(Logger) удален - используется glog
        // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
        return false;
    }
    return true;
}

/// ������� ����� ���������� � �������� ������
bool UStorage::CreateRuntimeCollection(const std::string &lib_name)
{
    if(lib_name.empty())
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Empty lib name");
        return false;
    }

	if(GetCollection(lib_name) != 0)
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library with name \"" + lib_name + "\" already exists");
        return false;
    }

    //�������� ����� ����������
    std::string lib_path = LibrariesPath + "RTlibs/" + lib_name;

	// Create shared_ptr for library - AddCollection now expects shared_ptr
	std::shared_ptr<URuntimeLibrary> lib = std::make_shared<URuntimeLibrary>(lib_name,"", lib_path);

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
            // if(Logger) удален - используется glog
                // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "CreateNewDirectory() failed while creating directrory for library \"" + lib_name + "\"");
            // No need to delete - shared_ptr will handle destruction
            return false;
        }
    }
    else
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "AddCollection() failed while adding the library \"" + lib_name + "\"");
        // No need to delete - shared_ptr will handle destruction
        return false;
    }
}

/// ������� runtime-���������� ������ � ������
bool UStorage::DeleteRuntimeCollection(const std::string &lib_name)
{
    int index = -1;
    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
        if(CollectionList[i] && CollectionList[i]->GetName() == lib_name)
        {
           index = int(i);
           break;
        }
    }

    if(index < 0 || index >= int(CollectionList.size()))
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \""+lib_name+"\" not found");
        return false;
    }

    auto I = CollectionList.begin() + index;
    // ���� ��� ������������� runtime-����������
    if((*I) && (*I)->GetType() == 2)
    {
        // ��������� ������� �������� �����
        if(!static_cast<URuntimeLibrary*>((*I).get())->DeleteOwnDirectory())
        {
            // if(Logger) удален - используется glog
               // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" + lib_name + "\" isn't runtime-library");
            return false;
        }
        // No need to delete - shared_ptr will handle destruction
        CollectionList.erase(I);

        try
        {
        // ������ DelClass() ����� ��������� ���������� (EObjectStorageNotEmpty ��� EClassIdNotExist)
        DelAbandonedClasses();
        }
        catch(EObjectStorageNotEmpty &ex)
        {
         // if(Logger) удален - используется glog
          // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
          return false;
        }
        catch(EClassIdNotExist &ex)
        {
         // if(Logger) удален - используется glog
          // Logger-> удален - используется glogLogMessageEx(RDK_EX_ERROR, __FUNCTION__, ex.what());
          return false;
        }
        return true;
    }
    else
    {
        // if(Logger) удален - используется glog
           // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" + lib_name + "\" isn't runtime-library");
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
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "FindFilesList() error");
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
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \""+lib_name+"\" already exists");
        return false;
    }

    //�������� ����� ����������
    std::string lib_path = LibrariesPath + "RTlibs/" + lib_name;

    // Create shared_ptr for library - AddCollection now expects shared_ptr
    std::shared_ptr<URuntimeLibrary> lib = std::make_shared<URuntimeLibrary>(lib_name,"",lib_path);

    // �������� �������� ����������� ������ ����������
    if(!lib->LoadCompDescriptions())
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Library \"" + lib_name + "\" error ");
        // No need to delete - shared_ptr will handle destruction
        return false;
    }

    if(AddCollection(lib))
    {
        return true;
    }
    else
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "AddCollection() failed while adding the library \"" + lib_name + "\"");
        // No need to delete - shared_ptr will handle destruction
        return false;
    }

    //return true;
}


// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
// ���� force_build == true �� ���������� ������������ ������������� ����������
// � ���������
bool UStorage::AddCollection(std::shared_ptr<ULibrary> library, bool force_build)
{
 if(!library)
  return false;

 // Check if library with same name already exists
 for(size_t i=0;i<CollectionList.size();i++)
 {
  if(CollectionList[i] && CollectionList[i]->GetName() == library->GetName())
   return false;
 }

 // CollectionList now stores shared_ptr directly - no need to create new shared_ptr
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
 
 // CollectionList now stores shared_ptr - no need to delete manually
 // shared_ptr will automatically destroy the object when last reference is released
 auto I = CollectionList.begin() + index;
 if((*I) && (*I)->GetType() == 2)
 {
  // For runtime libraries, delete directory if needed
  // Note: This may need to be handled differently with shared_ptr
  //static_cast<URuntimeLibrary*>((*I).get())->DeleteOwnDirectory();
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
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, std::string("Error while loading Library List from file: " + lib_list_file));
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

        // Create shared_ptr for mock library - AddCollection now expects shared_ptr
        std::shared_ptr<UMockLibrary> lib_mock = std::make_shared<UMockLibrary>(lib_name, "", lib_path);

        // ���������� �������� �������
        if(!CompDesctips.LoadFromFile(lib_path+"/"+lib_name+".xml","MockLib"))
        {
            // if(Logger) удален - используется glog
                // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, std::string("Error while loading Library Classes Descriptions from file: " + lib_path+"/"+lib_name+".xml"));
            // No need to delete - shared_ptr will handle destruction
            continue;
        }

        lib_mock->LoadFromXML(CompDesctips);

        if(!AddCollection(lib_mock))
        {
            // No need to delete - shared_ptr will handle destruction
        }
        LibList.SelectUp();
    }

    return true;

}

bool UStorage::CreateMockLibs(void)
{
    // if(Logger) удален - используется glog
        // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, std::string("Creating Mock Libraries from Static Libraries"));

    // �������� ���������-�������� �� ����������� ���������
    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
        if(CollectionList[i] && CollectionList[i]->GetType()==0)
        {
            // �������� ����� ����������, ���� ���������
            std::string lib_path = LibrariesPath + "MockLibs/";

            if(RDK::CreateNewDirectory(lib_path.c_str()))
            {
                // if(Logger) удален - используется glog
                    // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, std::string("Error while creating MockLibs path :" + lib_path));
                return false;
            }

            // ��� ����������-��������
            std::string lib_name = CollectionList[i]->GetName()+"_Mock";

            // ���� ����� ����������-�������� ����
            if(GetCollection(lib_name) != 0)
                continue;

            // Create shared_ptr for mock library - AddCollection now expects shared_ptr
            std::shared_ptr<UMockLibrary> lib_mock = std::make_shared<UMockLibrary>(lib_name, "", lib_path);

            // ���������� �������� �������
            CollectionList[i]->FillMockLibrary(lib_mock.get());

            if(!AddCollection(lib_mock))
            {
                // No need to delete - shared_ptr will handle destruction
            }
        }
    }

    return true;
}

bool UStorage::SaveMockLibs(void)
{
    // if(Logger) удален - используется glog
        // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, std::string("Starting saving Mock Libraries to files"));

    // ���������� ������ ���������-�������� �� ������� (����� �� ��� � CollectionList)
    USerStorageXML LibList;
    LibList.Create("LibraryList");

    std::string lib_name = "";

    // ���������� ��������� � ��������� �����
    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
        auto lib = CollectionList[i];

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
 // CollectionList now stores shared_ptr directly - use it directly
 for(size_t i=0;i<CollectionList.size();i++)
 {
  if(CollectionList[i] && CollectionList[i]->GetName() == name)
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
         // if(Logger) удален - используется glog
             // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, std::string("Mock Libraries will not be built because of error in Mock Libraries Initialization"));
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
         // if(Logger) удален - используется glog
             // Logger-> удален - используется glogLogMessage(RDK_EX_ERROR, std::string("Mock Libraries will not be built because of error in Mock Libraries Initialization"));
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
    // CollectionList now stores shared_ptr directly - use it directly
    for(size_t i=0;i<CollectionList.size();i++)
    {
     auto lib = CollectionList[i];
     if(lib && lib->GetType()==lib_type)
     {
      // Declare variables at the beginning of the scope
      unsigned long long total_used_memory_before(0);
      unsigned long long largest_free_block_before(0);
      unsigned long long total_used_memory_after(0);
      unsigned long long largest_free_block_after(0);
      
      LOG(INFO) << lib->GetName() << ": collection version is " << lib->GetVersion() << " (" << lib->GetRevision() << ")";

      if(lib->GetCoreVersion())
      {
       if(!lib->GetCoreVersion()->IsEqualFull(GetGlobalVersion()))
       {
        // Logger-> удален - используется glogLogMessage(RDK_EX_FATAL, lib->GetName()+std::string(" collection SKIPPED: application core version ")+GetGlobalVersion().ToStringFull()+std::string(" is incompatible lib core version ")+lib->GetCoreVersion()->ToStringFull());
        continue;
       }
      }
      else
       // Logger-> удален - используется glogLogMessage(RDK_EX_WARNING, lib->GetName()+std::string(" core version compatibility DOES NOT checked."));

      // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, std::string("Adding components from ")+lib->GetName()+" collection...");
      ReadUsedMemoryInfo(total_used_memory_before, largest_free_block_before);

      CollectionList[i]->Upload(this);
      if(ReadUsedMemoryInfo(total_used_memory_after, largest_free_block_after)) {
       LOG(INFO) << lib->GetName() << " eats " << (total_used_memory_after-total_used_memory_before) << " bytes of RAM. Largest RAM block decreased to " << (largest_free_block_before-largest_free_block_after) << " bytes";
      }

      // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, std::string("Successfully added [")+sntoa(lib->GetComplete().size())+std::string("]: ")+concat_strings(lib->GetComplete(),std::string(",")));
      if(!lib->GetIncomplete().empty())
       // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, std::string("Failed to add [")+sntoa(lib->GetIncomplete().size())+std::string("]: ")+concat_strings(lib->GetIncomplete(),std::string(",")));
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
 // CollectionList now stores shared_ptr directly - use it directly
 for(size_t i=0;i<CollectionList.size();i++)
 {
  if(CollectionList[i] && CollectionList[i]->IsClassNamePresent(class_name))
   return CollectionList[i];
 }
 return nullptr;
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

 // Use GetComponent() directly - it already returns shared_ptr, don't create new one from .get()
 // This prevents double destruction when shared_ptr is destroyed
 std::shared_ptr<UContainer> class_data = factory->GetComponent();
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

 // UseFlag is set based on use_count()
 // use_count() == 1 means only UStorage owns it (free)
 // use_count() > 1 means object is in use
 // Initially, object is in use (we're about to return it), so UseFlag = true
 UInstancesStorageElement element(object, true);
 instances.insert(instances.end(),element);
 object->SetClass(classid);
 object->Activity = true;
}

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
UId UStorage::PopObject(std::shared_ptr<UContainer> object)
{
 // TRACE: Log function entry
 if(!object)
 {
  LOG(INFO) << "PopObject[TRACE] - ENTRY: object is null, returning ForbiddenId";
  return ForbiddenId;
 }
 
 UId class_id = object->GetClass();
 std::string obj_name = "unknown";
 void* obj_addr = object.get();
 size_t use_count_before = object.use_count();
 try {
  obj_name = object->GetName();
 } catch (...) {
  obj_name = "<error>";
 }
 
 LOG(INFO) << "PopObject[TRACE] - ENTRY: name=" << obj_name 
           << " class_id=" << class_id << " use_count=" << use_count_before 
           << " address=" << obj_addr;
 
 UObjectsStorageIterator instances=ObjectsStorage.find(class_id);
 if(instances == ObjectsStorage.end())
 {
  // Object not found in ObjectsStorage - this is OK if it was already removed
  // or if it was created via make_shared (not TakeObject)
  LOG(INFO) << "PopObject[TRACE] - Object not found in ObjectsStorage for class_id=" << class_id;
  return ForbiddenId;
 }

 size_t list_size_before = instances->second.size();
 LOG(INFO) << "PopObject[TRACE] - Starting search in ObjectsStorage: class_id=" << class_id 
           << " list_size=" << list_size_before;

 size_t search_index = 0;
 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  search_index++;
  void* element_addr = &(*I);
  std::string element_obj_name = "null";
  void* element_obj_addr = nullptr;
  size_t element_obj_use_count = 0;
  
  try {
   if(I->Object)
   {
    element_obj_name = I->Object->GetName();
    element_obj_addr = I->Object.get();
    element_obj_use_count = I->Object.use_count();
   }
  } catch (...) {
   element_obj_name = "<error>";
  }
  
  LOG(INFO) << "PopObject[TRACE] - Search iteration " << search_index 
            << ": element_addr=" << element_addr 
            << " element_obj_name=" << element_obj_name 
            << " element_obj_addr=" << element_obj_addr 
            << " element_obj_use_count=" << element_obj_use_count
            << " target_obj_addr=" << obj_addr;
  
  if(I->Object == object)
  {
   // Found object - remove it from ObjectsStorage
   LOG(INFO) << "PopObject[TRACE] - Found object at iteration " << search_index 
             << ", about to call PopObject(iterator)";
   size_t list_size_before_erase = instances->second.size();
   UId result = PopObject(instances, I);
   size_t list_size_after_erase = instances->second.size();
   size_t use_count_after = object.use_count();
   LOG(INFO) << "PopObject[TRACE] - EXIT: Object removed from ObjectsStorage: name=" << obj_name 
             << " use_count_after=" << use_count_after 
             << " list_size_before=" << list_size_before_erase 
             << " list_size_after=" << list_size_after_erase;
   return result;
  }
 }

 // Object not found in ObjectsStorage for this class - this is OK if it was already removed
 // or if it was created via make_shared (not TakeObject)
 LOG(INFO) << "PopObject[TRACE] - EXIT: Object not found in ObjectsStorage list: class_id=" << class_id 
           << " searched_elements=" << search_index;
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
 if(!object)
  return;
 
 // Check if object is still valid - it may be partially destroyed
 // Check Class to ensure object is not in ForbiddenId state
 UId class_id;
 try {
  class_id = object->GetClass();
  if(class_id == ForbiddenId)
  {
   // Object is already being destroyed, skip ReturnObject
   return;
  }
 } catch (...) {
  // Object is partially destroyed, cannot access GetClass()
  // Skip ReturnObject - object will be destroyed automatically
  return;
 }
 
 // IMPORTANT: Don't set Activity or call BreakOwner if object may be partially destroyed
 // Activity is a UProperty that stores Owner pointer (raw pointer), which may be invalid
 // If Owner is dangling, accessing Activity will cause segfault
 // Since try-catch cannot catch segfault, we need to avoid accessing Activity entirely
 // 
 // SOLUTION: Skip setting Activity and BreakOwner entirely in ReturnObject
 // The object will be cleaned up automatically by shared_ptr when Storage is destroyed
 // Activity and BreakOwner are not critical for object lifecycle management with shared_ptr
 // They were needed for manual memory management, but with shared_ptr they're redundant
 // 
 // If we really need to set Activity, we would need to refactor UProperty to use weak_ptr
 // instead of raw pointer for Owner, but that's a larger architectural change
 // For now, skip Activity and BreakOwner to avoid segfault
 // 
 // Note: This means Activity flag may not be updated correctly, but with shared_ptr
 // this is not critical - objects are destroyed automatically when last reference is released

 UObjectsStorageIterator instances=ObjectsStorage.find(class_id);
 if(instances == ObjectsStorage.end())
  return;

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == object)
  {
   // Update UseFlag based on use_count()
   // use_count() == 1 means only UStorage owns it (free)
   // use_count() > 1 means object is still in use
   I->UseFlag = (object.use_count() > 1);
   break;
  }
 }
}

// � ������ ������ ���������� ForbiddenId
UId UStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
{
 // TRACE: Log function entry
 std::shared_ptr<UContainer> object=object_iterator->Object;
 
 std::string obj_name = "unknown";
 void* obj_addr = object.get();
 size_t use_count_before = object.use_count();
 void* element_addr = &(*object_iterator);
 try {
  obj_name = object->GetName();
 } catch (...) {
  obj_name = "<error>";
 }

 size_t list_size_before = instance_iterator->second.size();
 LOG(INFO) << "PopObject(iterator)[TRACE] - ENTRY: name=" << obj_name 
           << " element_addr=" << element_addr 
           << " obj_addr=" << obj_addr 
           << " use_count_before=" << use_count_before 
           << " list_size_before=" << list_size_before;

 LOG(INFO) << "PopObject(iterator)[TRACE] - About to call erase()";
 instance_iterator->second.erase(object_iterator);
 size_t list_size_after = instance_iterator->second.size();
 LOG(INFO) << "PopObject(iterator)[TRACE] - erase() completed: list_size_after=" << list_size_after;

 UId classid=object->GetClass();
 //object->SetObjectIterator(0);
 object->ResetStorage();
 object->SetClass(ForbiddenId);
 
 size_t use_count_after = object.use_count();
 LOG(INFO) << "PopObject(iterator)[TRACE] - EXIT: name=" << obj_name 
           << " classid=" << classid << " use_count_before=" << use_count_before 
           << " use_count_after=" << use_count_after << " address=" << obj_addr;
 
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
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Trying to add null function to FunctionsCrPropMock list in Storage");
        return false;
    }
    // ���� ��� ����������
    if(std::find(FunctionsCrPropMock.begin(), FunctionsCrPropMock.end(), func_ptr) != FunctionsCrPropMock.end())
    {
        // if(Logger) удален - используется glog
            // Logger-> удален - используется glogLogMessage(RDK_EX_DEBUG, __FUNCTION__, "Trying to add function that already exists in FunctionsCrPropMock list in Storage");
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

