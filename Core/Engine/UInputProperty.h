#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

enum UInputPropertyTypes { ipSingle=1, ipRange=2, ipList=3 };

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UInputProperty: public UVLProperty<T,OwnerT,type>, public UIInputProperty
{
protected:
// ��� �����
UInputPropertyTypes InputType;

// �������� �������� ������
int MinRange, MaxRange;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UInputProperty(const string &name, OwnerT * const owner, UInputPropertyTypes input_type=ipSingle, int min_range=0, int max_range=-1)
 : UVLProperty<T,OwnerT,type>(name, owner, (T * const)0), InputType(input_type), MinRange(min_range), MaxRange(max_range)
{ };
// -----------------------------

// -----------------------------
// ������ ���������� �����������
// -----------------------------
// ��� �����
UInputPropertyTypes GetInputType(void) const
{
 return InputType;
}

bool CheckRange(int index)
{
 if(InputType == ipSingle && MinRange == index)
  return true;
 return false;
}
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ �����
void const * GetPointer(void) const
{
 return this->PData;
}

// ������������� ��������� �� ������ �����
bool SetPointer(void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 if(!this->PData)
  return 0;
//  throw EUsingZeroPtr();


 return this->PData;
};

T& operator * (void)
{
// if(!PData)
  //throw EUsingZeroPtr();

 return *this->PData;
};

operator T* (void) const
{
 return this->PData;
}

// --------------------------
};


}
#endif
