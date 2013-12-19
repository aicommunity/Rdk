#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {
	  /*
class UPropertyIOBase: public UIProperty
{
protected:
// ��� �����
int IoType;

// �������� �������� ������
int MinRange, MaxRange;

public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyIOBase(int min_range=0, int io_type=ipDataSingle, int max_range=-1)
 : IoType(io_type), MinRange(min_range), MaxRange(max_range)
{ };
// -----------------------------

// -----------------------------
// ������ ���������� �����������
// -----------------------------
// ���
virtual int GetIoType(void) const
{
 return IoType;
}

virtual bool CheckRange(int index)
{
 if(IoType & ipSingle)
  return (MinRange == index && MaxRange<0) | (index>=MinRange && index<=MaxRange);
 else
 if(IoType & ipRange)
  return (MinRange <= index && MaxRange<0) | (index>=MinRange && index<=MaxRange);

 return false;
}

// �������� �������� ������
int GetMinRange(void)
{ return MinRange; };

int GetMaxRange(void)
{ return MaxRange; };
// -----------------------------

};
      */
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputBase: protected ULProperty<T,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
//mutable T Local;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : ULProperty<T,OwnerT,type>(name, owner)
{
 UVBaseDataProperty<T>::IoType=input_type;
 UVBaseDataProperty<T>::MinRange=min_range;
 UVBaseDataProperty<T>::MaxRange=max_range;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UPropertyInputBase<T*,OwnerT,type>
{
protected:
mutable T Local;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInput(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle | ipComp, int max_range=-1)
 : UPropertyInputBase<T*,OwnerT,type>(name, owner, min_range, input_type | ipComp, max_range)
{ };
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return (this->v)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 return this->v;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 this->v=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->v)?false:true; };

T* operator -> (void) const
{
 return (this->v)?this->v:&Local;
};

T& operator * (void)
{
 return (this->v)?*this->v:Local;
};

operator T* (void) const
{
 return (this->v)?this->v:&Local;
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UPropertyInputBase<T,OwnerT,type>
{
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
};
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCBase: public UCLProperty<std::vector<T*>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
std::vector<T> Local;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UCLProperty<std::vector<T*>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(int(this->v.size())<=index-this->MinRange)
 #ifdef __BORLANDC__
//  return 0;
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),index);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),index);
 #endif
 return this->v[index-this->MinRange];
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 if(int(this->v.size())<=index-this->MinRange)
  this->v.resize(index-this->MinRange+1);
 this->v[index-this->MinRange]=reinterpret_cast<T*>(value);
 return true;
}

T* operator [] (int i)
{
 if(int(this->v.size())<=i-this->MinRange)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #endif
// throw EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
//	this->MinRange,int(this->v.size()+this->MinRange),i);

 return (this->v[i])?this->v[i]:&Local[i];
}

const T* operator [] (int i) const
{
 if(int(this->v.size())<=i-this->MinRange)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #endif


 return (this->v[i])?this->v[i]:&Local[i];
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
UPropertyInputC(const string &name, OwnerT * const owner, int min_range, int input_type=ipRange | ipComp, int max_range=-1)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipComp, max_range)
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


virtual void SetData(const T &value)
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
UPropertyInputCData(const string &name, OwnerT * const owner, int min_range, int input_type=ipRange, int max_range=-1)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{ };
// -----------------------------
};



// -----------------------------------------------------------------------------
// Output properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputBase: protected ULProperty<T,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : ULProperty<T,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
T* operator -> (void)
{
 return &this->v;
};

T& operator * (void)
{
 return this->v;
};

operator T* (void)
{
 return &this->v;
}

T& Value(void)
{
 return this->v;
}

const T& Value(void) const
{
 return this->v;
}

T& operator [] (int i)
{
 return this->v;
}

const T& operator [] (int i) const
{
 return this->v;
}

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ ������
void const * GetPointer(int index) const
{
 return &this->v;
}

// ������������� ��������� �� ������ ������
bool SetPointer(int index, void* value)
{
 return true;
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: public UPropertyOutputBase<T,OwnerT,type>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle, int max_range=-1)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{

};
// -----------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCBase: protected UCLProperty<std::vector<T>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UCLProperty<std::vector<T>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
T& Value(int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

const T& Value(int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

T& operator [] (int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

const T& operator [] (int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCData: public UPropertyOutputCBase<T,OwnerT,type>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCData(const string &name, OwnerT * const owner, int min_range, int input_type=ipRange, int max_range=-1)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{

};
// -----------------------------
};

// -----------------------------------------------------------------------------
// Output virtual properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT>
class UVPropertyOutputBase: public UVProperty<T,OwnerT>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyOutputBase(OwnerT * const owner, T* data, int min_range, int input_type, int max_range=-1)
 : UVProperty<T,OwnerT>(owner,data)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ ������
void const * GetPointer(int index) const
{
 return this->PData;
}

// ������������� ��������� �� ������ ������
bool SetPointer(int index, void* value)
{
 return true;
}
// --------------------------
};

template<typename T, typename OwnerT>
class UVPropertyOutputData: public UVPropertyOutputBase<T,OwnerT>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyOutputData(OwnerT * const owner, T* data, int min_range, int input_type=ipSingle, int max_range=-1)
 : UVPropertyOutputBase<T,OwnerT>(owner, data, min_range, input_type | ipData, max_range)
{

};
// -----------------------------
};
		  /*
template<typename T, typename OwnerT>
class UVPropertyOutputCBase: protected UVProperty<std::vector<T>,OwnerT>, public UPropertyIOBase
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UVProperty<std::vector<T>,OwnerT,type>(name, owner), UPropertyIOBase(min_range, input_type, max_range)
{ };
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
T& Value(int i)
{
 return this->v[i];
}

const T& Value(int i) const
{
 return this->v[i];
}

T& operator [] (int i)
{
 return this->v[i];
}

const T& operator [] (int i) const
{
 return this->v[i];
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCData: public UPropertyOutputCBase<T,OwnerT,type>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle, int max_range=-1)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{

};
// -----------------------------

virtual void Init(void)
{
 if(Owner) //&& MinRange>=0)
 {
  int max_range=(MaxRange<0)?MinRange:MaxRange;
  this->v.resize(max_range-MinRange+1);
  for(int i=MinRange;i<=max_range;i++)
  {
   Owner->SetOutputDataAsPointer(i,&(this->v[i-MinRange]));
   Owner->SetOutputDataInfo(i,new UDataInfo<T>);
  }
 }
};

};
         */

}
#endif
