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

namespace RDK {

/// ������ ���������� ����������� ���������
std::list<ULibrary*> ULibrary::LibraryList;


// --------------------------
// ������������ � �����������
// --------------------------
ULibrary::ULibrary(const string &name, const string &version)
{
// if(!LibraryList)
//  LibraryList=new std::list<ULibrary*>;

 Name=name;
 Version=version;

// AddUniqueLibrary(this);
}

ULibrary::~ULibrary(void)
{
// RemoveLibrary(this);

/* if(LibraryList && LibraryList->empty())
 {
  delete LibraryList.Get();
  LibraryList=0;
 }*/
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ������������ ������������
// --------------------------
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
/*
 if(!LibraryList)
 {
  LibraryList=new std::list<ULibrary*>;
 }
  */
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
/*
 if(LibraryList->empty())
 {
  delete LibraryList;
  LibraryList=0;
 }*/
}
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
// --------------------------

// --------------------------
// ������ ������� � ������ ��������
// --------------------------
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

 // ClassSamples.clear();
 Complete.clear();
 Incomplete.clear();
 CreateClassSamples(Storage);
 count=Complete.size();

 Storage=0;
 return count;
}
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
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

 if(!Storage->AddClass(cont,name))
 {
  Incomplete.push_back(name);
  delete cont;
  return false;
 }

 Complete.push_back(name);
 return true;
}



}

#endif


