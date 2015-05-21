#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputBase: protected UVProperty<T,OwnerT>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
mutable T Local;

/// ���� ������� �����������
bool IsConnectedFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputBase(const string &name, OwnerT * const owner, int input_type)
 : UVProperty<T,OwnerT>(owner, (T*)0)
{
 IsConnectedFlag=false;
 this->PData=&Local;
 reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
 UVBaseDataProperty<T>::IoType=input_type;
// UVBaseDataProperty<T*>::MinRange=min_range;
// UVBaseDataProperty<T*>::MaxRange=max_range;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}// --------------------------
};

template<typename T, typename OwnerT>
class UVPropertyInputBase: public UVProperty<T,OwnerT>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
mutable T Local;

/// ���� ������� �����������
bool IsConnectedFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyInputBase(OwnerT * const owner, T* data, int input_type)
 : UVProperty<T,OwnerT>(owner, data)
{
 UVBaseDataProperty<T>::IoType=input_type;
 IsConnectedFlag=false;
 this->PData=&Local;
};
// -----------------------------


// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UPropertyInputBase<T*,OwnerT,type>
{
protected:
mutable T LocalValue;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInput(const string &name, OwnerT * const owner)
 : UPropertyInputBase<T*,OwnerT,type>(name, owner, ipSingle | ipComp)
{
 Local=&LocalValue;
 this->PData=&Local;
};

/// Deprecated
UPropertyInput(const string &name, OwnerT * const owner, int index)
 : UPropertyInputBase<T*,OwnerT,type>(name, owner, ipSingle | ipComp)
{
 Local=&LocalValue;
 this->PData=&Local;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return IsConnectedFlag;
// return (this->PData)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(IsConnectedFlag)
  return *this->PData;

 return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 if(value)
 {
  *this->PData=reinterpret_cast<T*>(value);
  IsConnectedFlag=true;
 }
 else
 {
  *this->PData=Local;
  IsConnectedFlag=false;
 }
 return true;
}

bool operator ! (void) const
{ return (IsConnectedFlag)?false:true; };

T* operator -> (void) const
{
 return (IsConnectedFlag)?*this->PData:Local;
};

T& operator * (void)
{
 return (IsConnectedFlag)?**this->PData:*Local;
};

operator T* (void) const
{
 return (IsConnectedFlag)?*this->PData:Local;
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UPropertyInputBase<T,OwnerT,type>
{
protected:
//mutable T Local;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputData(const string &name, OwnerT * const owner)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, ipSingle | ipData)
{
 this->PData=&Local;
};

/// Deprecated
UPropertyInputData(const string &name, OwnerT * const owner, int index)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, ipSingle | ipData)
{
 this->PData=&Local;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return IsConnectedFlag;//(this->PData)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(IsConnectedFlag)
  return this->PData;
 return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 if(value)
 {
  this->PData=reinterpret_cast<T*>(value);
  IsConnectedFlag=true;
 }
 else
 {
  this->PData=&Local;
  IsConnectedFlag=false;
 }
 return true;
}

bool operator ! (void) const
{ return (IsConnectedFlag)?false:true; };

T* operator -> (void) const
{
 return (IsConnectedFlag)?this->PData:&Local;
};

T& operator * (void)
{
 return (IsConnectedFlag)?*this->PData:Local;
};

operator T* (void) const
{
 return (IsConnectedFlag)?this->PData:&Local;
}
// --------------------------
/*
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{ };
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return (this->PData)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 return this->PData;
}

// ������� ��������
virtual const T& GetData(void) const
{
 return (this->PData)?*this->PData:this->v;
};

virtual void SetData(const T &value)
{
 if(this->PData)
 {
  if(*this->PData == value)
   return;

  if(this->Owner)
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
	throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

   *this->PData=value;
   return;
  }

  *this->PData=value;
 }
 else
  ULProperty<T,OwnerT,type>::SetData(value);
};

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 return (this->PData)?this->PData:&this->v;
};

T& operator * (void)
{
 return (this->PData)?*this->PData:this->v;
};

operator T* (void) const
{
 return (this->PData)?this->PData:&this->v;
}
// --------------------------
*/
};


template<typename T, typename OwnerT>
class UVPropertyInputData: public UVPropertyInputBase<T,OwnerT>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyInputData(OwnerT * const owner, T *data)
 : UVPropertyInputBase<T,OwnerT>(owner, data, ipSingle | ipData)
{
 this->PData=&Local;
};

/// Deprecated
UVPropertyInputData(OwnerT * const owner, int index)
 : UVPropertyInputBase<T,OwnerT>(owner, data, ipSingle | ipData)
{
 this->PData=&Local;
};

// -----------------------------


// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return (IsConnectedFlag)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(IsConnectedFlag)
  return this->PData;
 return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 if(value)
 {
  this->PData=reinterpret_cast<T*>(value);
  IsConnectedFlag=true;
 }
 else
 {
  this->PData=&Local;
  IsConnectedFlag=false;
 }
 return true;
}

bool operator ! (void) const
{ return (IsConnectedFlag)?false:true; };

T* operator -> (void) const
{
 return (IsConnectedFlag)?this->PData:&Local;
};

T& operator * (void)
{
 return (IsConnectedFlag)?*this->PData:Local;
};

operator T* (void) const
{
 return (IsConnectedFlag)?this->PData:&Local;
}
// --------------------------
/*
// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return (this->PData)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 return this->PData;
}

// ������� ��������
virtual const T& GetData(void) const
{
 return (this->PData)?*this->PData:throw(1); // TODO
};

virtual void SetData(const T &value)
{
 if(this->PData)
 {
  if(*this->PData == value)
   return;

  if(this->Owner)
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
	throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

   *this->PData=value;
   return;
  }

  *this->PData=value;
 }
 else
  UVProperty<T,OwnerT>::SetData(value);
};

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 return (this->PData)?this->PData:throw(1); // TODO
};

T& operator * (void)
{
 return (this->PData)?*this->PData:throw(1); // TODO
};

operator T* (void) const
{
 return (this->PData)?this->PData:throw(1); // TODO
}
// --------------------------
*/
};
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCBase: public UCLProperty<std::vector<T*>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
std::vector<T*> Local;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputCBase(const string &name, OwnerT * const owner, int input_type)
 : UCLProperty<std::vector<T*>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
};

~UPropertyInputCBase(void)
{
 for(size_t i=0;i<Local.size();i++)
  delete Local[i];
}
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(int(this->v.size())<=index)
 #ifdef __BORLANDC__
//  return 0;
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),index);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),index);
 #endif
 return this->v[index];
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 int new_index=index;
 if(value)
 {
  size_t new_size=this->v.size()+1;
  new_index=new_size-1;
  this->v.resize(new_size);
  size_t old_size=Local.size();
  for(size_t i=new_size;i<old_size;i++)
   delete Local[i];
  Local.resize(new_size);

  for(size_t i=old_size;i<new_size;i++)
   Local[i]=new T;
 }
 this->v[new_index]=reinterpret_cast<T*>(value);

/*
 if(int(this->v.size())<=index)
 {
  size_t new_size=index+1;
  this->v.resize(new_size);
  size_t old_size=Local.size();
  for(size_t i=new_size;i<old_size;i++)
   delete Local[i];
  Local.resize(new_size);

  for(size_t i=old_size;i<new_size;i++)
   Local[i]=new T;
 }
 this->v[index]=reinterpret_cast<T*>(value);
*/
 return true;
}

T* operator [] (int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	0,int(this->v.size()),i);
 #endif
// throw EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
//	this->MinRange,int(this->v.size()+this->MinRange),i);

 return (this->v[i])?this->v[i]:Local[i];
}

const T* operator [] (int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif


 return (this->v[i])?this->v[i]:Local[i];
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputC: public UPropertyInputCBase<T,OwnerT,type>
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputC(const string &name, OwnerT * const owner)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipComp)
{ };

/// Deprecated
UPropertyInputC(const string &name, OwnerT * const owner, int index)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipComp)
{ };
// -----------------------------

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 return true;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 return true;
};


virtual void SetData(const std::vector<T*> &value)
{
 return;
};


};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCData: public UPropertyInputCBase<T,OwnerT,type>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputCData(const string &name, OwnerT * const owner)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{ };

/// Deprecated
UPropertyInputCData(const string &name, OwnerT * const owner, int index)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{ };

// -----------------------------
};


}
#endif
