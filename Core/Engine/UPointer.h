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
#include <vector>
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
// �������� ��������� - используем weak_ptr для безопасного доступа без владения
std::weak_ptr<T> Source;

// �������� ���������
OwnerT* Owner;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEPointer(const string &name, OwnerT * const owner)
 : Owner(owner)
{
 Source.reset();
 // UEPointer is created as a member variable, not via shared_ptr
 // So we cannot use shared_from_this() in constructor - use safe_shared_cast instead
 // This is safe because UEPointer is always owned by its Owner container
 reinterpret_cast<UContainer* const>(Owner)->AddLookupPointer(
  name,
  RDK::safe_shared_cast<UIPointer>(this)
 );
};
// --------------------------

// Legacy std::shared_ptr method
std::shared_ptr<UContainer> const GetUEPtr(void) const
{ 
 auto ptr = Source.lock();
 return ptr ? std::static_pointer_cast<UContainer>(ptr) : std::shared_ptr<UContainer>();
};

// Implement UIPointer API
// CRITICAL: Returns weak_ptr - shared_ptr exists only in UStorage
std::weak_ptr<UContainer> Get(void) const override
{ 
 // Source is already weak_ptr<T>, convert to weak_ptr<UContainer>
 std::shared_ptr<T> locked = Source.lock();
 if(!locked)
  return std::weak_ptr<UContainer>();
 std::shared_ptr<UContainer> cont = std::static_pointer_cast<UContainer>(locked);
 return std::weak_ptr<UContainer>(cont);
};

// CRITICAL: Parameter is weak_ptr - shared_ptr exists only in UStorage
virtual void Del(std::weak_ptr<UContainer> source)
{
 if(source.expired())
  return;
 std::shared_ptr<UContainer> source_locked = source.lock();
 if(Source.lock() == source_locked)
  Source.reset();
}

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
// CRITICAL: Parameter is weak_ptr - shared_ptr exists only in UStorage
virtual int Find(std::weak_ptr<const UContainer> cont) const
{ 
 if(cont.expired())
  return -1;
 std::shared_ptr<const UContainer> cont_locked = cont.lock();
 auto ptr = Source.lock();
 return (ptr && ptr == cont_locked) ? 0 : -1;
};

// CRITICAL: Parameter is weak_ptr - shared_ptr exists only in UStorage
virtual void Set(std::weak_ptr<UContainer> source)
{
 if(source.expired())
  return;
 std::shared_ptr<UContainer> source_locked = source.lock();
 if(source_locked)
  Source = std::static_pointer_cast<T>(source_locked);
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

// ������ ���������� - используем weak_ptr для безопасного доступа без владения
std::vector<std::weak_ptr<T>> Sources;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UCPointer(const string &name, OwnerT * const owner)
 : Owner(owner)
{ 
 Sources.clear(); 
 // UCPointer is created as a member variable, not via shared_ptr
 // So we cannot use shared_from_this() in constructor - use safe_shared_cast instead
 // This is safe because UCPointer is always owned by its Owner container
 reinterpret_cast<UContainer* const>(Owner)->AddLookupPointer(name, RDK::safe_shared_cast<UIPointer>(this)); 
};
virtual ~UCPointer(void)
{
 Sources.clear();
};
// --------------------------

// CRITICAL: Returns weak_ptr - shared_ptr exists only in UStorage
std::weak_ptr<UContainer> Get(void) const override
{ 
 if(Sources.empty())
  return std::weak_ptr<UContainer>();
 // Sources[0] is already weak_ptr<T>, return it as weak_ptr<UContainer>
 // We need to lock, cast, then convert back to weak_ptr
 std::shared_ptr<T> locked = Sources[0].lock();
 if(!locked)
  return std::weak_ptr<UContainer>();
 std::shared_ptr<UContainer> cont = std::static_pointer_cast<UContainer>(locked);
 return std::weak_ptr<UContainer>(cont);
};

std::shared_ptr<UContainer> const Get(size_t index) const
{ 
 if(index >= Sources.size())
  return std::shared_ptr<UContainer>();
 auto ptr = Sources[index].lock();
 return ptr ? std::static_pointer_cast<UContainer>(ptr) : std::shared_ptr<UContainer>();
};

// CRITICAL: Parameter is weak_ptr - shared_ptr exists only in UStorage
virtual void Set(std::weak_ptr<UContainer> source)
{
 if(source.expired())
  return;
 if(Find(source) >=0)
  return;

 std::shared_ptr<UContainer> source_locked = source.lock();
 if(source_locked)
  Sources.push_back(std::static_pointer_cast<T>(source_locked));
}

// CRITICAL: Parameter is weak_ptr - shared_ptr exists only in UStorage
virtual void Del(std::weak_ptr<UContainer> source)
{
 if(source.expired())
  return;
 int index=Find(source);

 if(index<0)
  return;

 Sources.erase(Sources.begin() + index);
}

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
// CRITICAL: Parameter is weak_ptr - shared_ptr exists only in UStorage
virtual int Find(std::weak_ptr<const UContainer> cont) const
{
 if(cont.expired())
  return -1;
 std::shared_ptr<const UContainer> cont_locked = cont.lock();
 if(!cont_locked)
  return -1;
 
 for(size_t i=0; i<Sources.size(); i++)
 {
  auto ptr = Sources[i].lock();
  if(ptr && ptr == cont_locked)
   return int(i);
 }
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
