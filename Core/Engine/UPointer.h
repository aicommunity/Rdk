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
#include <memory>
#include "../Utilities/USupport.h"
#include "UContainer.h"
#include "ModernSmartPointers.h"

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
{
 Source=0;
 reinterpret_cast<UContainer* const>(Owner)->AddLookupPointer(
  name,
  std::shared_ptr<UIPointer>(this, RDK::NonOwningDeleter())
 );
};
// --------------------------

// Legacy std::shared_ptr method
std::shared_ptr<UContainer> const GetUEPtr(void) const
{ return std::shared_ptr<UContainer>(Source, RDK::NonOwningDeleter()); };

// Implement UIPointer API
std::shared_ptr<UContainer> Get(void) const override
{ return std::shared_ptr<UContainer>(Source, RDK::NonOwningDeleter()); };

virtual void Del(std::shared_ptr<UContainer> source)
{
 Source=0;
}

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
virtual int Find(std::shared_ptr<const UContainer> cont) const
{ return (cont.get() == Source)?0:-1; };

virtual void Set(std::shared_ptr<UContainer> source)
{
 Source=static_pointer_cast<T>(source).get();
};

// Override UIPointer methods with std::shared_ptr
// Modern C++20 methods - already exist above

// --------------------------
// ���������
// --------------------------
// �������� ������������
UEPointer<T,OwnerT>& operator = (std::shared_ptr<UContainer> pdata)
{
 // UIPointer::operator = (pdata); // Removed - not compatible with std::shared_ptr
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
{ Size=0; Sources=0; reinterpret_cast<UContainer* const>(Owner)->AddLookupPointer(name,std::shared_ptr<UIPointer>(this, RDK::NonOwningDeleter())); };
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

std::shared_ptr<UContainer> Get(void) const override
{ return std::shared_ptr<UContainer>(*Sources, RDK::NonOwningDeleter()); };

std::shared_ptr<UContainer> const Get(size_t index) const
{ return std::shared_ptr<UContainer>(Sources[index], RDK::NonOwningDeleter()); };

virtual void Set(std::shared_ptr<UContainer> source)
{
 if(Find(source) >=0)
  return;

 T** sources=new T*[Size+1];
 memcpy(sources,Sources,sizeof(T*)*Size);
 delete []Sources;
 Sources=sources;
 Sources[Size]=std::static_pointer_cast<T>(source).get();
 ++Size;
}

virtual void Del(std::shared_ptr<UContainer> source)
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
virtual int Find(std::shared_ptr<const UContainer> cont) const
{
 T** sources=Sources;
 for(size_t i=0;i<Size;i++,sources++)
  if(*sources == cont.get())
   return int(i);

 return -1;
};

// --------------------------
// ���������
// --------------------------
// �������� ������������
UEPointer<T,OwnerT>& operator = (std::shared_ptr<UContainer> pdata)
{
 // UIPointer::operator = (pdata); // Removed - not compatible with std::shared_ptr
 return *this;
};
// --------------------------
};
}
#endif
