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

// --------------------------
// ������������ � �����������
// --------------------------
ULibrary::ULibrary(const string &name, const string &version)
{
 Name=name;
 Version=version;
}

ULibrary::~ULibrary(void)
{
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

