#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

enum { ipSingle, ipRange, ipList } UInputPropertyTypes

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UInputProperty: public UVLProperty, UIInputProperty
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
 : UVLProperty<T,OwnerT>(owner, (T const *)0), InputType(input_type), MinRange(min_range), MaxRange(max_range)
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
 return PData;
}

// ������������� ��������� �� ������ �����
bool SetPointer(void const * value)
{
 PData=reinterpret_cast<T const *>(value);
 return true;
}

T* operator -> (void) const
{
 if(!PData)
  throw EUsingZeroPtr();

 return PData;
};
// --------------------------
};


};
#endif
