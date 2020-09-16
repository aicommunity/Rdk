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
#include "UComponentFactory.h"

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
 CoreVersion=new RDK::UVersion(core_version);
}

ULibrary::~ULibrary(void)
{
 if(CoreVersion)
 {
  delete CoreVersion;
  CoreVersion=0;
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
const UEPtr<RDK::UVersion> ULibrary::GetCoreVersion(void) const
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
 CreateClassSamples(Storage);
 count=int(Complete.size());

 //Storage=0;
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
   UEPtr<ULibrary> lib=storage->GetCollection(j);
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
bool ULibrary::UploadClass(const UId &classid, UEPtr<UComponent> cont)
{
 return false;//ULibrary::UploadClass(classid, cont);
}

bool ULibrary::UploadClass(const string &name, UEPtr<UComponent> cont)
{
 if(!cont)
  return false;

 if(name.size() == 0)
 {
  delete cont;
  return false;
 }

 if(Storage->CheckClass(name))
  return true;

 std::vector<std::string>::iterator I;
 UEPtr<UVirtualMethodFactory> factory;
 try
 {
  cont->SetLogger(Storage->GetLogger());
  cont->SetStorage(Storage);
  cont->Build();
  factory = new UVirtualMethodFactory(cont);
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
  delete factory;
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

  delete factory;
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

bool ULibrary::UploadClass(const std::string &class_name, const std::string &component_name, UComponent* (*funcPointer)(void))
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
 UEPtr<UComponentFactoryMethod> factory = new UComponentFactoryMethod(Storage,funcPointer,component_name);

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

  delete factory;
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
// --------------------------




// --------------------------
// ������������ � �����������
// --------------------------
URuntimeLibrary::URuntimeLibrary(const string &name, const string &version)
 : ULibrary(name,version,2)
{

}

URuntimeLibrary::~URuntimeLibrary(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// �������� ��������� ����������
const USerStorageXML& URuntimeLibrary::GetCurrentComponentStruct(void) const
{
    return CurrentComponentStruct;
}

/// ��������� �������� ��������� �� ������ � ������ �����
bool URuntimeLibrary::LoadCompDescriptions()
{
    // ����� ����������
    std::string lib_path = "../../../RTlibs/" + Name;

    // ������ �� ���� ������������ xml ������ � �����
    // � ������� �� ������ � ������ ClassesStructures
    std::vector<std::string> comp_descriptions;

    if(RDK::FindFilesList(lib_path,"*.xml",true,comp_descriptions))
        return false;

    ClassesStructures.resize(comp_descriptions.size());

    for(int i = 0 ; i < comp_descriptions.size(); i++)
    {
        // ������� �������� �����
        CurrentComponentStruct.LoadFromFile(lib_path+"/"+comp_descriptions[i],"");
        // ������ �������� � ������
        CurrentComponentStruct.Save(ClassesStructures[i]);
    }
    CurrentComponentStruct.Destroy();
    return true;
}

/// ��������� ����� ��������� (���������� � ����)
bool URuntimeLibrary::AddNewClass(const std::string &new_class_name, UContainer *newclass)
{
    UEPtr<UComponent> p = newclass;
    UEPtr<UNet> cont = dynamic_pointer_cast<UNet>(p);
     std::string buff;
    CurrentComponentStruct.Destroy();

    // ���������� XML � ���������� ����� ���� RTname � ������
    // TODO ��� ������ ���������� ������ XML ����
    cont->SaveComponent(&CurrentComponentStruct, true, ptPubParameter);
    CurrentComponentStruct.SelectNode(cont->GetName());
    CurrentComponentStruct.SetNodeAttribute("RTname", new_class_name);
    CurrentComponentStruct.Save(buff);

    //std::string class_name = CurrentComponentStruct.GetNodeAttribute("Class");
    //cont=dynamic_pointer_cast<UNet>(Storage->TakeObject(class_name));

    std::string lib_path = "../../../RTlibs/" + Name;
    CurrentComponentStruct.SaveToFile(lib_path+"/"+new_class_name+".xml");

    UEPtr<UContainer> cont_1 = CreateClassSample(Storage, CurrentComponentStruct);


    if(UploadClass(new_class_name, cont_1))
        ClassesStructures.push_back(buff);


    CurrentComponentStruct.Destroy();

    return true;
}

/// �������� ������������ ���������
bool URuntimeLibrary::ReplaceClass(const std::string &new_class_name, UContainer *newclass)
{
    // �������� ���. ������
    DelClass(new_class_name);

    // ���������� ������
    AddNewClass(new_class_name,newclass);

    return true;
}

/// ������� ����� �� ��������� � Storage
bool URuntimeLibrary::DelClass(const std::string &class_name)
{
    // �������� ���. ������
    if(Storage->CheckClass(class_name))
        Storage->DelClass(Storage->FindClassId(class_name));

    CurrentComponentStruct.Destroy();

    for(auto it = ClassesStructures.begin(); it != ClassesStructures.end(); ++it)
    {
        CurrentComponentStruct.Load(*it,"");
        std::string search_name = CurrentComponentStruct.GetNodeAttribute("RTname");
        if(class_name == search_name)
        {
            ClassesStructures.erase(it);
            break;
        }
    }
    CurrentComponentStruct.Destroy();
}

bool URuntimeLibrary::DeleteOwnDirectory(void)
{
    // ����� ����������
    std::string lib_path = "../../../RTlibs/" + Name;
    RDK::DeleteDirectory(lib_path.c_str());
}
// --------------------------

// --------------------------
/// ������� ��������� �� �������� xml
UEPtr<UContainer> URuntimeLibrary::CreateClassSample(UStorage *storage, USerStorageXML &xml)
{
 UEPtr<UNet> cont;

 if(!storage)
    return 0;

 std::string class_name=xml.GetNodeAttribute("Class");

 cont=dynamic_pointer_cast<UNet>(storage->TakeObject(class_name));

 if(!cont)
    return 0;

 if(!cont->LoadComponent(&xml,true))
 {
     //log
    storage->ReturnObject(cont);
    return 0;
 }

 return cont;
}

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void URuntimeLibrary::CreateClassSamples(UStorage *storage)
{
    int num_classes = ClassesStructures.size();

    for(int i=0;i<num_classes;i++)
    {
        try
        {
            CurrentComponentStruct.Destroy();
            CurrentComponentStruct.Load(ClassesStructures[i],"");

            UEPtr<UContainer> cont=CreateClassSample(storage, CurrentComponentStruct);

            std::string class_name=CurrentComponentStruct.GetNodeAttribute("RTname");

            UploadClass(class_name,cont);
        }
        catch(UException &)
        {
            // smth
        }
    }
    CurrentComponentStruct.Destroy();
}
// --------------------------


}

#endif


