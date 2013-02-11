#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

class UPropertyIOBase: public UIPropertyIO
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
 return (MinRange == index && MaxRange<0) | (index>=MinRange && index<=MaxRange);
 //((Type == ipDataSingle || Type == ipCompSingle) && MinRange == index);
}

// �������� �������� ������
int GetMinRange(void)
{ return MinRange; };

int GetMaxRange(void)
{ return MaxRange; };
// -----------------------------

};

template<typename T, typename OwnerT, unsigned int type>
class UPropertyInputBase: protected ULProperty<T*,OwnerT,type>, public UPropertyIOBase
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : ULProperty<T*,OwnerT,type>(name, owner), UPropertyIOBase(min_range, input_type, max_range)
{ };
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
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
 if(!this->v)
  return 0;

 return this->v;
};

T& operator * (void)
{
 return *this->v;
};

operator T* (void) const
{
 return this->v;
}

		   /*
// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
 return true;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
 return true;
}           */
  /*
// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}   */
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UPropertyInputBase<T,OwnerT,type>
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInput(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle | ipComp, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipComp, max_range)
{ };
// -----------------------------


virtual void Init(void)
{

}

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


virtual void Init(void)
{
 if(Owner && MinRange>=0)
 {
  Owner->SetInputDataInfo(MinRange,new UDataInfo<T>);
 }
}

};
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type>
class UPropertyInputCBase: public UCLProperty<std::vector<T*>,OwnerT,type>, public UPropertyIOBase
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UCVLProperty<std::vector<T*>,OwnerT,type>(name, owner, (T * const)0), UPropertyIOBase(min_range, input_type, max_range)
{ };
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 return this->v[index-MinRange];
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value)
{
 this->v[index-MinRange]=reinterpret_cast<T*>(value);
 return true;
}

T* operator [] (int i)
{
 return this->v[i];
}

const T* operator [] (int i) const
{
 return this->v[i];
}
/*
bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 if(!this->PData)
  return 0;

 return this->PData;
};

T& operator * (void)
{
 return *this->PData;
};

operator T* (void) const
{
 return this->PData;
}

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}               */
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
UPropertyInputC(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle | ipComp, int max_range=-1)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipComp, max_range)
{ };
// -----------------------------


virtual void Init(void)
{

}

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
UPropertyInputCData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle, int max_range=-1)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{ };
// -----------------------------


virtual void Init(void)
{
 if(Owner) //&& MinRange>=0)
 {
  int max_range=(MaxRange<0)?MinRange:MaxRange;
  this->v.resize(max_range-MinRange+1);
  for(int i=MinRange;i<=max_range;i++)
  {
   Owner->SetInputDataInfo(i,new UDataInfo<T>);
  }
 }
}

};



// -----------------------------------------------------------------------------
// Output properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type>
class UPropertyOutputBase: protected ULProperty<T,OwnerT,type>, public UPropertyIOBase
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : ULProperty<T,OwnerT,type>(name, owner), UPropertyIOBase(min_range, input_type, max_range)
{ };
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
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

virtual void Init(void)
{
 if(Owner && MinRange>=0)
 {
  Owner->SetOutputDataAsPointer(MinRange,&this->v);
  Owner->SetOutputDataInfo(MinRange,new UDataInfo<T>);
 }
};

};

template<typename T, typename OwnerT, unsigned int type>
class UPropertyOutputCBase: protected UCLProperty<std::vector<T>,OwnerT,type>, public UPropertyIOBase
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UCLProperty<std::vector<T>,OwnerT,type>(name, owner), UPropertyIOBase(min_range, input_type, max_range)
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


}
#endif
