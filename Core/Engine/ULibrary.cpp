/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UALIBRARY_CPP
#define UALIBRARY_CPP

#include "ULibrary.h"
#include "UNet.h"
#include "UMockUNet.h"
#include "UComponentFactory.h"
#include <glog/logging.h>

namespace RDK {

/// ������ ���������� ����������� ���������
//std::list<ULibrary*> ULibrary::LibraryList;


// --------------------------
// ������������ � �����������
// --------------------------
ULibrary::ULibrary(const string &name, const string &version, int type, int revision)
: Name(name), Version(version), Revision(revision), Type(type), Storage(0)
{
}

ULibrary::ULibrary(const string &name, const string &version, const RDK::UVersion &core_version, int type, int revision)
: Name(name), Version(version), Revision(revision), Type(type), Storage(0)
{
 CoreVersion=std::make_shared<RDK::UVersion>(core_version);
}

ULibrary::~ULibrary(void)
{
 if(CoreVersion)
 {
  CoreVersion.reset();
 }
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ������������ ������������
// --------------------------
/*
/// ���������� ��������� ���������� ����������� ���������
const std::list<ULibrary*>& ULibrary::GetLibraryList(void)
{
 return LibraryList;
}

/// ������� ��������� ���������
void ULibrary::ClearLibraryList(void)
{
 LibraryList.clear();
}

/// ��������� ������� ���������� �� �� �����
bool ULibrary::CheckLibrary(const std::string &name)
{
 std::list<ULibrary*>::const_iterator I=LibraryList.begin();
 std::list<ULibrary*>::const_iterator J=LibraryList.end();
 for(;I!=J;++I)
 {
  if((*I)->GetName() == name)
   return true;
 }
 return false;
}

/// ��������� ���������� � ������, ���� ���������� � ����� ������ ��� ��� � ������
bool ULibrary::AddUniqueLibrary(ULibrary* const lib)
{
 if(!lib)
  return false;

 if(CheckLibrary(lib->GetName()))
  return false;  /// ��������!! ����� ����� ���� warning ����������

 LibraryList.push_back(lib);
 return true;
}

/// ������� ���������� �� ������.
/// ���� ������ ���������� ����, �� ���������� ���
void ULibrary::RemoveLibrary(ULibrary* const lib)
{
 std::list<ULibrary*>::iterator I=LibraryList.begin();
 std::list<ULibrary*>::iterator J=LibraryList.end();
 for(;I!=J;++I)
 {
  if((*I) == lib)
  {
   LibraryList.erase(I);
   break;
  }
 }
} */
// --------------------------

// --------------------------
// ������ ������� � ���������� ����������
// --------------------------
// ���������� ��� ����������
const string& ULibrary::GetName(void) const
{
 return Name;
}

// ���������� ������ ����������
const string& ULibrary::GetVersion(void) const
{
 return Version;
}

/// ���������� ������� ������� �������� ������
int ULibrary::GetRevision(void) const
{
 return Revision;
}

/// ���������� ������ ����, ��������������� ��� ������ ����������
const std::shared_ptr<UVersion> ULibrary::GetCoreVersion(void) const
{
 return CoreVersion;
}


/// ��� ����������
/// 0 - ���������� ���������� (������� ������ � �����)
/// 1 - ������� ���������� (��������� �� ������� dll)
/// 2 - ����������, ��������� �� ����� ����������
int ULibrary::GetType(void) const
{
 return Type;
}

/// ����������� ���������� �� ������ ���������
const std::vector<pair<string, string> > ULibrary::GetDependencies(void) const
{
 return Dependencies;
}
// --------------------------

// --------------------------
// ������ ������� � ������ ��������
// --------------------------
/// ���������� true ���� ��������� ������������� ����� � ����� ������
bool ULibrary::IsClassNamePresent(const std::string &class_name) const
{
 for(size_t i=0;i<ClassesList.size();i++)
  if(ClassesList[i] == class_name)
   return true;
 return false;
}

/// ����� ������� ����������
const vector<string>& ULibrary::GetClassesList(void) const
{
 return ClassesList;
}

// �������� ����� ���� ������� ����������� ��������
const vector<string>& ULibrary::GetComplete(void) const
{
 return Complete;
}

// �������� ����� ���� �� ����������� ��������
const vector<string>& ULibrary::GetIncomplete(void) const
{
 return Incomplete;
}

// ������� ��� ������ Complete � Incomplete
void ULibrary::ClearIncompleteAndComplete(void)
{
    Incomplete.clear();
    Complete.clear();
}
// --------------------------

// --------------------------
// ������ ������� � ������ ����������
// --------------------------
// ��������� �������� ��������� ������� �������� �������.
// ���� ����� � �������� ������ ��� ����������, �� �� ������������.
// ���������� ����� ������� ����������� �������.
int ULibrary::Upload(UStorage *storage)
{
 /*Complete.clear();
 Incomplete.clear();
 ULibrary::Upload(storage);
 return Complete.size();
 */
 int count=0;

 Storage=storage;

 if(!Storage)
  return 0;

 Incomplete.clear();
 
 // Create class samples for all libraries
 // The order is important: BasicLib must be first, then others
 // This is handled by BuildStorage() which processes libraries in the correct order
 CreateClassSamples(Storage);
 
 count=int(Complete.size());

 //���������� ������ �� Storage ��� RunTime ���������
// if(Type != 2)
//    Storage=0;
 return count;
}
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
/// ��������� ����������� ���������� �� ������ ���������
/// � ���������� ������ ����������� ���������
/// ���������� true ���� ��� ����������� ���������� ��� ���������
bool ULibrary::CheckDependencies(UStorage *storage, std::vector<pair<string, string> > &dependencies) const
{
 if(!storage)
  return false;

 if(Dependencies.empty())
  return true;

 dependencies.clear();
 int num_libraries=storage->GetNumCollections();
 for(size_t i=0;i<Dependencies.size();i++)
 {
  bool dep_found=false;
  for(int j=0;j<num_libraries;j++)
  {
   std::shared_ptr<ULibrary> lib=storage->GetCollection(j);
   if(lib && lib->GetName() == Dependencies[i].first
	&& (Dependencies[i].second.empty() || Dependencies[i].second == lib->GetVersion()))
   {
	dep_found=true;
	break;
   }
  }
  if(!dep_found)
   dependencies.push_back(Dependencies[i]);
 }
 if(dependencies.empty())
  return true;

 return false;
}

// ��������� � ��������� ��������� �����
bool ULibrary::UploadClass(const UId &classid, std::shared_ptr<UComponent> cont)
{
 return false;//ULibrary::UploadClass(classid, cont);
}

bool ULibrary::UploadClass(const string &name, std::shared_ptr<UComponent> cont)
{
 if(!cont)
  return false;

 if(name.size() == 0)
 {
  cont.reset(); // shared_ptr handles deletion
  return false;
 }

 if(Storage->CheckClass(name))
  return true;

 std::vector<std::string>::iterator I;
 std::shared_ptr<UVirtualMethodFactory> factory;
 try
 {
  // cont->SetLogger удален - используется glog
   cont->SetStorage(Storage);
  
  // Log before Build() to track where bad_weak_ptr occurs
  std::string obj_name_build = "unknown";
  try {
   auto container_build = std::dynamic_pointer_cast<UContainer>(cont);
   if(container_build) {
    obj_name_build = container_build->GetName();
   }
  } catch (...) {
   obj_name_build = "<error>";
  }
  LOG(INFO) << "UploadClass - calling Build() for: name=" << name << " object_name=" << obj_name_build;
  
  try {
   cont->Build();
  } catch (const std::bad_weak_ptr& e) {
   LOG(ERROR) << "UploadClass - bad_weak_ptr in Build() for: name=" << name << " object_name=" << obj_name_build << " error=" << e.what();
   throw; // Re-throw to be caught by outer catch
  }
  
  LOG(INFO) << "UploadClass - Build() completed for: name=" << name << " object_name=" << obj_name_build;
  
  std::shared_ptr<UContainer> container = std::dynamic_pointer_cast<UContainer>(cont);
  std::string obj_name = "unknown";
  void* obj_addr = container.get();
  size_t use_count_before = container.use_count();
  try {
   obj_name = container->GetName();
  } catch (...) {
   obj_name = "<error>";
  }
  
  LOG(INFO) << "UploadClass - creating factory for: name=" << name 
            << " object_name=" << obj_name << " use_count=" << use_count_before 
            << " address=" << obj_addr;
  
  factory = std::make_shared<UVirtualMethodFactory>(container);
  
  size_t use_count_after = container.use_count();
  LOG(INFO) << "UploadClass - factory created: name=" << name 
            << " object_name=" << obj_name << " use_count_after=" << use_count_after;
 }
 catch(...)
 {
  if(find(Incomplete.begin(),Incomplete.end(),name) == Incomplete.end())
   Incomplete.push_back(name);
  I=find(ClassesList.begin(),ClassesList.end(),name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),name);
  if(I != Complete.end())
   Complete.erase(I);
  factory.reset(); // shared_ptr handles deletion
  return false;
 }

 if(!Storage->AddClass(factory,name))
 {
  if(find(Incomplete.begin(),Incomplete.end(),name) == Incomplete.end())
   Incomplete.push_back(name);
  I=find(ClassesList.begin(),ClassesList.end(),name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),name);
  if(I != Complete.end())
   Complete.erase(I);

  factory.reset(); // shared_ptr handles deletion
  return false;
 }

 if(find(ClassesList.begin(),ClassesList.end(),name) == ClassesList.end())
  ClassesList.push_back(name);
 if(find(Complete.begin(),Complete.end(),name) == Complete.end())
  Complete.push_back(name);
 I=find(Incomplete.begin(),Incomplete.end(),name);
 if(I != Incomplete.end())
  Incomplete.erase(I);

 return true;
}

bool ULibrary::UploadClass(const std::string &class_name, const std::string &component_name, UContainer* (*funcPointer)(void))
{
 if(!funcPointer)
  return false;

 if(class_name.size() == 0)
 {
  return false;
 }

 if(Storage->CheckClass(class_name))
  return true;

 std::vector<std::string>::iterator I;
 // Storage is a raw pointer, but UComponentFactoryMethod expects shared_ptr
 // We need to create a shared_ptr with non-owning deleter to pass to factory
 // The factory will not own the Storage - it's just for reference
 std::shared_ptr<UStorage> storage_ptr(Storage, [](UStorage*){}); // Non-owning deleter - Storage is owned by ULibrary
 std::shared_ptr<UComponentFactoryMethod> factory = std::make_shared<UComponentFactoryMethod>(storage_ptr, funcPointer, component_name);

 if(!Storage->AddClass(factory,class_name))
 {
  if(find(Incomplete.begin(),Incomplete.end(),class_name) == Incomplete.end())
   Incomplete.push_back(class_name);
  I=find(ClassesList.begin(),ClassesList.end(),class_name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),class_name);
  if(I != Complete.end())
   Complete.erase(I);

  factory.reset(); // shared_ptr handles deletion
  return false;
 }

 if(find(ClassesList.begin(),ClassesList.end(),class_name) == ClassesList.end())
  ClassesList.push_back(class_name);
 if(find(Complete.begin(),Complete.end(),class_name) == Complete.end())
  Complete.push_back(class_name);
 I=find(Incomplete.begin(),Incomplete.end(),class_name);
 if(I != Incomplete.end())
  Incomplete.erase(I);

 return true;
}

/// �������� ��������� ������ �� ������ ������� �����������
/// ����� ����������� � ������������� (Incomplete)
void ULibrary::RemoveClassFromCompletedList(const string &name)
{
 std::vector<std::string>::iterator I;

 I=find(ClassesList.begin(),ClassesList.end(),name);
 if(I != ClassesList.end())
  ClassesList.erase(I);

 I=find(Complete.begin(),Complete.end(),name);
 if(I != Complete.end())
  Complete.erase(I);

 I=find(Incomplete.begin(),Incomplete.end(),name);
 if(I == Incomplete.end())
  Incomplete.push_back(name);
}

/// ��������� ����������-�������� ����� XML ���������� ����������� �����������
void ULibrary::FillMockLibrary(UMockLibrary* lib)
{
    USerStorageXML ComponentStruct;
    ComponentStruct.Create("stub");

    // ������ �� ���� ������� ��������� �������
    for(vector<string>::iterator it = Complete.begin(); it !=  Complete.end(); ++it)
    {
        std::shared_ptr<UComponent> obj;
        try{
        obj = Storage->TakeObject(*it);
        }
        catch(UException &ex)
        {
            LOG(INFO) << __FUNCTION__ << " - " << ex.what();
            continue;
        }

        std::shared_ptr<UNet> cont=dynamic_pointer_cast<UNet>(obj);

        if(!cont)
            continue;

        if(!ComponentStruct.Destroy())
        {
            Storage->ReturnObject(cont);
            continue;
        }

        // ���������� XML ����� �������� ����������
        if(!cont->SaveComponent(&ComponentStruct, true, ptAny|pgPublic))
        {
            LOG(INFO) << __FUNCTION__ << " - Error while saving XML description of class " << *it;
            Storage->ReturnObject(cont);
            continue;
        }

        lib->AddNewCompDescription(ComponentStruct);
        Storage->ReturnObject(cont);
    }

    Storage->FreeObjectsStorage();

    return;
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
URuntimeLibrary::URuntimeLibrary(const string &name, const string &version, const string& path)
 : ULibrary(name,version,2), LibPath(path)
{

}

URuntimeLibrary::~URuntimeLibrary(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------

/// ��������� ���� ����������
const std::string& URuntimeLibrary::GetLibPath() const
{
    return LibPath;
}

/// ��������� �������� ��������� �� ������ � ������ �����
bool URuntimeLibrary::LoadCompDescriptions()
{
    // ������ �� ���� ������������ xml ������ � �����
    // � ������� �� ������ � ������ ClassesStructures
    std::vector<std::string> comp_descriptions;

    if(RDK::FindFilesList(LibPath,"*.xml",true,comp_descriptions))
        return false;

    ClassesStructures.resize(comp_descriptions.size());
    for(size_t i = 0 ; i < comp_descriptions.size(); i++)
    {
        // ������� �������� �����
        CurrentComponentStruct.LoadFromFile(LibPath+"/"+comp_descriptions[i],"");
        // ������ �������� � ������
        CurrentComponentStruct.Save(ClassesStructures[i]);
    }

    return true;
}

/// ��������� ����� ��������� (���������� � ����)
bool URuntimeLibrary::AddNewClass(const std::string &new_class_name, const std::string &new_comp_name, UContainer *newclass)
{
    // newclass is a raw pointer passed from outside - we need to create shared_ptr
    // But we don't own newclass, so we use non-owning deleter
    // This is a temporary shared_ptr for passing to dynamic_pointer_cast
    std::shared_ptr<UContainer> p = std::shared_ptr<UContainer>(newclass, [](UContainer*){}); // Non-owning deleter - caller owns newclass
    std::shared_ptr<UNet> cont = dynamic_pointer_cast<UNet>(p);

    if(!cont)
        return false;

    std::string buff;

    // XML ������ ����� ���� ���������.
    // � ���� ������ Destroy() �� ��������������������, �� ��������. ������� ������� Create ��������
	CurrentComponentStruct.Create("stub");
	CurrentComponentStruct.Destroy();

    // ���������� XML � ���������� ������ ���� RTname � ������
    if(!cont->SaveComponent(&CurrentComponentStruct, true, ptAny|pgPublic))
        return false;

    CurrentComponentStruct.SelectNode(cont->GetName());
    CurrentComponentStruct.RenameNode(new_comp_name);
    CurrentComponentStruct.SetNodeAttribute("RTname", new_class_name);
    CurrentComponentStruct.Save(buff);

    CurrentComponentStruct.SaveToFile(LibPath+"/"+new_class_name+".xml");

    std::shared_ptr<UContainer> cont_1 = CreateClassSample(Storage, CurrentComponentStruct);

    if(!cont_1)
        return false;

    if(!UploadClass(new_class_name, cont_1))
    {
        LOG(INFO) << __FUNCTION__ << " - UploadClass failed while uploading \"" << new_class_name << "\" class";
        return false;
    }

    ClassesStructures.push_back(buff);

    return true;
}

/// ������� ����� �� ��������� � Storage
bool URuntimeLibrary::DelClass(const std::string &class_name)
{
    if(class_name.empty())
        return false;
    // �������� ���. ������
    if(Storage->CheckClass(class_name))
    {
        // ���� ����� �� ������ ����������
        if(Storage->FindCollection(class_name).get()!= static_cast<ULibrary*>(this))
        {
            LOG(INFO) << __FUNCTION__ << " - \"" << class_name << "\" class exists in another library. Not in \"" << Name << "\"";
            return false;
        }

        // ����� ��������� ����������
        Storage->DelClass(Storage->FindClassId(class_name));
    }

    // ����� � ������� ���������
    for(vector<string>::iterator it = ClassesStructures.begin(); it != ClassesStructures.end(); ++it)
    {
        CurrentComponentStruct.Load(*it,"");
        std::string search_name = CurrentComponentStruct.GetNodeAttribute("RTname");
        if(class_name == search_name)
        {
            ClassesStructures.erase(it);
            std::string xml_path = LibPath +"/" + search_name + ".xml";
            RDK::RemoveFile(xml_path.c_str());
            break;
        }
    }
    return true;
}

/// ������� ���������� ���������� ������ �� ����� �������
bool URuntimeLibrary::DeleteOwnDirectory(void)
{
    return (!RDK::DeleteDirectory(LibPath.c_str()));
}
// --------------------------

// --------------------------
/// ������� ��������� �� �������� xml
std::shared_ptr<UContainer> URuntimeLibrary::CreateClassSample(UStorage *storage, USerStorageXML &xml)
{
 std::shared_ptr<UContainer> cont;

 if(!storage)
    return 0;

 std::string class_name=xml.GetNodeAttribute("Class");

 cont=storage->TakeObject(class_name);

 if(!cont)
    return 0;

 if(!dynamic_pointer_cast<UNet>(cont)->LoadComponent(&xml,true))
 {
    LOG(INFO) << __FUNCTION__ << " - Error while LoadComponent() from XML file for class \"" << class_name << "\"";

    storage->ReturnObject(cont);
    return 0;
 }

 return cont;
}

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void URuntimeLibrary::CreateClassSamples(UStorage *storage)
{
    size_t num_classes = ClassesStructures.size();

    for(size_t i=0;i<num_classes;i++)
    {
        try
        {
            CurrentComponentStruct.Load(ClassesStructures[i],"");

            std::shared_ptr<UContainer> cont=CreateClassSample(storage, CurrentComponentStruct);

            if(!cont)
                return;

            std::string class_name=CurrentComponentStruct.GetNodeAttribute("RTname");

            UploadClass(class_name,cont);
        }
        catch(UException &ex)
        {
            LOG(INFO) << __FUNCTION__ << " - " << ex.what();
        }
    }
}
// --------------------------



// ����������-��������

// --------------------------
// ������������ � �����������
// --------------------------
UMockLibrary::UMockLibrary(const string &name, const string &version, const string& path)
 : ULibrary(name,version,3), LibPath(path)
{

}

UMockLibrary::~UMockLibrary(void)
{

}

// ��������� �������� ���������� � ClassesStructures
bool UMockLibrary::AddNewCompDescription(USerStorageXML& descript)
{
    std::string added;
    if(!descript.Save(added))
        return false;
    ClassesStructures.push_back(added);
    return true;
}

bool UMockLibrary::SaveLibraryToFile()
{
    std::string all_comps;

    // �������� ���� �������
    for(std::vector<std::string>::iterator it = ClassesStructures.begin(); it != ClassesStructures.end(); ++it)
    {
        all_comps += (*it)+"\n";
    }

    CurrentComponentStruct.Create("a");
    CurrentComponentStruct.Destroy();

    CurrentComponentStruct.Create("MockLib");
    CurrentComponentStruct.SelectRoot();

    CurrentComponentStruct.SetNodeAttribute("Name", Name);
    CurrentComponentStruct.SetNodeAttribute("Version", Version);
    CurrentComponentStruct.SetNodeAttribute("Revision", sntoa(Revision));
    CurrentComponentStruct.SetNodeAttribute("CoreVersion",GetGlobalVersion().ToStringFull());

    // XML �� ����� ������������
    USerStorageXML CompStruct;
    CompStruct.Load(all_comps,"");

    // ���������� ����������� � XML ����������
    CurrentComponentStruct.LoadToNode(CompStruct,true);

    std::string file_name = LibPath + "/" + Name + ".xml";
    CurrentComponentStruct.SaveToFile(file_name);

    return true;
}

void UMockLibrary::LoadFromXML(USerStorageXML& xml)
{
    xml.SelectRoot();

    // �������� �������� ����������� ����������
    for(int i = 0, size = xml.GetNumNodes() ; i < size; i++)
    {
        if(!xml.SelectNode(i))
            continue;

        std::string add;

        if(!xml.SaveFromNode(add))
            continue;

        ClassesStructures.push_back(add);
        xml.SelectUp();
    }

}

/// ������� ��������� �� �������� xml
std::shared_ptr<UContainer> UMockLibrary::CreateClassSample(USerStorageXML &xml, UStorage *storage)
{
    std::string class_name=xml.GetNodeAttribute("Class");

    std::shared_ptr<UMockUNet> mock = std::make_shared<UMockUNet>(&xml, storage);

    std::shared_ptr<UContainer> cont = mock;

    if(!dynamic_pointer_cast<UNet>(cont))
    {
        mock.reset(); // shared_ptr handles deletion
        return 0;
    }

    return cont;
}

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void UMockLibrary::CreateClassSamples(UStorage *storage)
{
    int num_classes = int(ClassesStructures.size());

    for(int i=0;i<num_classes;i++)
    {
        try
        {
            CurrentComponentStruct.Load(ClassesStructures[i],"");

            std::shared_ptr<UContainer> cont=CreateClassSample(CurrentComponentStruct,Storage);

            if(!cont)
                return;
            CurrentComponentStruct.Load(ClassesStructures[i],"");

            std::string class_name=CurrentComponentStruct.GetNodeAttribute("Class");

            UploadClass(class_name,cont);
        }
        catch(UException &ex)
        {
            LOG(INFO) << __FUNCTION__ << " - " << ex.what();
        }
    }
}

}

#endif


