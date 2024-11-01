/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UPointerH
#define UPointerH

#include <cstdlib>
#include <string>
#include <memory.h>
#include "../Utilities/USupport.h"
#include "UContainer.h"

namespace RDK {

class UContainer;


// ��������� �� ��������� ��������� ���������� � ������
// ��� �������� �������������
template<typename T, class OwnerT>
class UEPointer: public UPtr<T>, public UIPointer
{
//friend class OwnerT;
protected: // ��������
// �������� ���������
T* Source;

// �������� ���������
OwnerT* Owner;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEPointer(const string &name, OwnerT * const owner)
 : Owner(owner)
{ Source=0; reinterpret_cast<UContainer* const>(Owner)->AddLookupPointer(name,this); };
// --------------------------

UEPtr<UContainer> const Get(void) const
{ return Source; };

virtual void Del(UEPtr<UContainer> source)
{
 Source=0;
}

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
virtual int Find(UEPtr<const UContainer> cont) const
{ return (cont == Source)?0:-1; };

virtual void Set(UEPtr<UContainer> source)
{
 Source=static_pointer_cast<T>(source);
};

// --------------------------
// ���������
// --------------------------
// �������� ������������
UEPointer<T,OwnerT>& operator = (UEPtr<UContainer> pdata)
{
 UIPointer::operator = (pdata);
 return *this;
};
// --------------------------
};


// ��������� �� ������ ��������� ����������� ��������� � ������
// ��� �������� �������������
template<typename T, class OwnerT>
class UCPointer: public UIPointer
{
//friend class OwnerT;
protected: // ��������
// �������� ���������
OwnerT* Owner;

// ������ ����������
T** Sources;

// ������ �������
size_t Size;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UCPointer(const string &name, OwnerT * const owner)
 : Owner(owner)
{ Size=0; Sources=0; reinterpret_cast<UContainer* const>(Owner)->AddLookupPointer(name,this); };
virtual ~UCPointer(void)
{
 if(Sources)
 {
  delete[] Sources;
  Sources=0;
  Size=0;
 }
};
// --------------------------

UEPtr<UContainer> const Get(void) const
{ return *Sources; };

UEPtr<UContainer> const Get(size_t index) const
{ return Sources[index]; };

virtual void Set(UEPtr<UContainer> source)
{
 if(Find(source) >=0)
  return;

 T** sources=new T*[Size+1];
 memcpy(sources,Sources,sizeof(T*)*Size);
 delete []Sources;
 Sources=sources;
 Sources[Size]=static_pointer_cast<T>(source);
 ++Size;
}

virtual void Del(UEPtr<UContainer> source)
{
 int index=Find(source);

 if(index<0)
  return;

 T** sources=new T*[Size-1];
 memcpy(sources,Sources,sizeof(T*)*index);
 memcpy(sources+index,Sources,sizeof(T*)*(Size-index-1));
 delete []Sources;
 Sources=sources;
 --Size;
}

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
virtual int Find(UEPtr<const UContainer> cont) const
{
 T** sources=Sources;
 for(size_t i=0;i<Size;i++,sources++)
  if(*sources == cont)
   return int(i);

 return -1;
};

// --------------------------
// ���������
// --------------------------
// �������� ������������
UEPointer<T,OwnerT>& operator = (UEPtr<UContainer> pdata)
{
 UIPointer::operator = (pdata);
 return *this;
};
// --------------------------
};
}
#endif
