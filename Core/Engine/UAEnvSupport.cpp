/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAENV_SUPPORT_CPP
#define UAENV_SUPPORT_CPP

#include <string.h>
#include <stdlib.h>
#include "UAEnvSupport.h"

namespace RDK {

UId ForbiddenId=0;
ULongId ULongIdemp;

/* *********************************************************************** */
/* *********************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UIdVector::UIdVector(void)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;
}

UIdVector::UIdVector(const UIdVector &copy)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 *this=copy;
}

UIdVector::UIdVector(UId id1, UId id2, ...)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 Add(id1);
 UId *p = &id2;
 while (*p != ForbiddenId)
 {
  Add(*p++);
 }
}

UIdVector::UIdVector(UId id1)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 Add(id1);
}

UIdVector::~UIdVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int UIdVector::GetSize(void) const
{
 return Size;
}

int UIdVector::size(void) const
{
 return Size;
}

// �������� ������ ������� � ����������� ������� ������
void UIdVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  UId *newbuffer=new UId[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ������� ������
void UIdVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// ���������� ��������� �� ������ ������
UId* UIdVector::GetBuffer(void)
{
 return Buffer;
}

// ��������� ������� � ����� �������
// ���������� ������ ��������
int UIdVector::Add(UId id)
{
 Resize(Size+1);
 Buffer[Size-1]=id;
 return Size-1;
}
// --------------------------

// --------------------------
// ������ ����� ������ ���������������
// --------------------------
UIdVector& UIdVector::DecodeFromString(const std::string &str)
{
 Resize(0);

 if(!str.size())
  return *this;

 std::size_t start=str.find_first_not_of("0123456789. ");
 std::size_t stop;
 if(start != string::npos)
  throw EDecodeFail(str,0);

 start=stop=0;
 while(start != string::npos)
 {
  start=str.find_first_of("0123456789",stop);
  if(start == string::npos)
   break;

  stop=str.find_first_of(".",start);
  try
  {
   if(stop == string::npos)
	Add(atoi(str.substr(start)));
   else
	Add(atoi(str.substr(start,stop-start)));
  }
  catch (EStrToNumber *strtonumber)
  {
   delete strtonumber;
   throw EDecodeFail(str,stop);
  }
 }

 return *this;
}

std::string& UIdVector::EncodeToString(std::string &str) const
{
 str.clear();
 for(int i=0;i<Size;i++)
 {
  str+=sntoa(Buffer[i]);
  if(i < Size-1)
   str+=".";
 }

 return str;
}
// --------------------------


// --------------------------
// ���������
// --------------------------
// �������� ������������
UIdVector& UIdVector::operator = (const UIdVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Buffer[i]=copy.Buffer[i];

 return *this;
}

// �������� �������
UId& UIdVector::operator [] (int index)
{
 return Buffer[index];
}

const UId& UIdVector::operator [] (int index) const
{
 return Buffer[index];
}

// ��������� ���������
bool UIdVector::operator == (const UIdVector &copy) const
{
 if(Size != copy.Size)
  return false;

 return !memcmp(Buffer,copy.Buffer,Size*sizeof(UId));
}

bool UIdVector::operator != (const UIdVector &copy) const
{
 return !(*this == copy);
}

bool UIdVector::operator < (const UIdVector &copy) const
{
 if(Size < copy.Size)
  return true;

 if(Size > copy.Size)
  return false;

 if(memcmp(Buffer,copy.Buffer,Size*sizeof(UId))<0)
  return true;

 return false;
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
/*UIdVector::EDecodeFail::EDecodeFail(void)
{

}
  */
UIdVector::EDecodeFail::EDecodeFail(const std::string &stringid, int position)
 : StringId(stringid), Position(position)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UIdVector::EDecodeFail::CreateLogMessage(void) const
{
 std::string result=UException::CreateLogMessage();
 result+=" StringId=";
 result+=StringId;
 result+=" Position=";
 result+=sntoa(Position);
 return result;
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */


/* *********************************************************************** */
/* *********************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
ULongIdVector::ULongIdVector(void)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;
}

ULongIdVector::ULongIdVector(const ULongIdVector &copy)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 *this=copy;
}

ULongIdVector::~ULongIdVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int ULongIdVector::GetSize(void) const
{
 return Size;
}

// �������� ������ ������� � ����������� ������� ������
void ULongIdVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  ULongId *newbuffer=new ULongId[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ������� ������
void ULongIdVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// ���������� ��������� �� ������ ������
ULongId* ULongIdVector::GetBuffer(void)
{
 return Buffer;
}

// ��������� ������� � ����� �������
// ���������� ������ ��������
int ULongIdVector::Add(const ULongId &id)
{
 Resize(Size+1);
 Buffer[Size-1]=id;
 return Size-1;
}
// --------------------------


// --------------------------
// ���������
// --------------------------
// �������� ������������
ULongIdVector& ULongIdVector::operator = (const ULongIdVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Buffer[i]=copy.Buffer[i];

 return *this;
}

// �������� �������
ULongId& ULongIdVector::operator [] (int index)
{
 return Buffer[index];
}

const ULongId& ULongIdVector::operator [] (int index) const
{
 return Buffer[index];
}
// --------------------------







}

#endif


