#ifndef UShareH
#define UShareH

#include "UAContainer.h"

namespace RDK {

template<typename T,class MainOwnerT>
class UShare
{
protected: // ������
// ��������� �� ����������� ������
T* Pointer;

// ��������� ����� ������ ���� ��������� �� ��������
T Data;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UShare(void)
{
 Pointer=0;
}

virtual ~UShare(void)
{
 Pointer=0;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
const T& Get(void) const
{
 return (Pointer)?*Pointer:Data;
}

void Set(const T& data)
{
 if(Pointer)
  *Pointer=data;
 else
  Data=data;
}

const T& GetLocal(void) const
{
 return Data;
}

void SetLocal(const T& data)
{
 Data=data;
}

const T& GetShared(void) const
{
 if(Pointer)
  return *Pointer;

 return Data;
}

void SetShared(const T& data)
{
 if(Pointer)
  *Pointer=data;
 else
  Data=data;
}

const T* GetPointer(void) const
{
 return Pointer;
}

void SetPointer(UAComponent *owner, T* pointer)
{
 MainOwnerT *mainowner=dynamic_cast<MainOwnerT*>(owner);

 if(mainowner)
 {
  Pointer=pointer;
  if(Pointer)
   *Pointer=Data;
 }
 else
  Pointer=0;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
UShare<T,MainOwnerT>& operator = (const UShare<T,MainOwnerT> &copy)
{
 Pointer=copy.Pointer;
 Data=copy.Data;

 return *this;
}
// --------------------------

};



}

#endif
