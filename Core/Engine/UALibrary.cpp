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

#include "UALibrary.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UALibrary::UALibrary(const string &name, const string &version)
{
 Name=name;
 Version=version;
}

UALibrary::~UALibrary(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ���������� ����������
// --------------------------
// ���������� ��� ����������
const string& UALibrary::GetName(void) const
{
 return Name;
}

// ���������� ������ ����������
const string& UALibrary::GetVersion(void) const
{
 return Version;
}
// --------------------------

// --------------------------
// ������ ������� � ������ ��������
// --------------------------
// �������� ����� ���� ������� ����������� ��������
const vector<string>& UALibrary::GetComplete(void) const
{
 return Complete;
}

// �������� ����� ���� �� ����������� ��������
const vector<string>& UALibrary::GetIncomplete(void) const
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
int UALibrary::Upload(UAStorage *storage)
{
 /*Complete.clear();
 Incomplete.clear();
 UALibrary::Upload(storage);
 return Complete.size();
 */
 int count=0;

 Storage=dynamic_cast<UAContainerStorage*>(storage);

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
bool UALibrary::UploadClass(const UId &classid, UEPtr<UAComponent> cont)
{
 return false;//UALibrary::UploadClass(classid, cont);
}

bool UALibrary::UploadClass(const string &name, UEPtr<UAComponent> cont)
{
 if(!cont)
  return false;

 if(name.size() == 0)
 {
  delete cont;
  return false;
 }

 UAContainerStorage *storage=dynamic_cast<UAContainerStorage *>(Storage);
 if(!storage)
 {
  if(!Storage->AddClass(cont,name))
  {
   Incomplete.push_back(name);
   delete cont;
   return false;
  }
 }
 else
 {
  if(!storage->AddClass(cont,name))
  {
   Incomplete.push_back(name);
   delete cont;
   return false;
  }
 }

 Complete.push_back(name);
 return true;
}



}

#endif


