#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UConnector.h"

namespace RDK {

template<typename T, typename OwnerT>
class UPropertyInputPreBase: public UVProperty<T,OwnerT>
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
mutable T Local;

/// ���� ������� �����������
bool IsConnectedFlag;

/// ��������� �� ������������ �����
UIProperty *ConnectedOutput;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputPreBase(OwnerT * const owner, int input_type)
 : UVProperty<T,OwnerT>(owner, (T*)0), ConnectedOutput(0)
{
 IsConnectedFlag=false;
 UVBaseDataProperty<T>::IoType=input_type;
};
// -----------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ��������� ����� ������ � �����
void ApplyOutputUpdateTime(void) const
{
 if(ConnectedOutput)
  this->UpdateTime=ConnectedOutput->GetUpdateTime();
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputBase: public UPropertyInputPreBase<T,OwnerT>, virtual public UIPropertyInput
{
protected:
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputBase(const string &name, OwnerT * const owner, int input_type)
 : UPropertyInputPreBase<T,OwnerT>(owner, input_type)
{
 dynamic_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
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

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const
{
 return UPropertyInputPreBase<T,OwnerT>::GetName();
};

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const
{
 return UPropertyInputPreBase<T,OwnerT>::GetOwnerName();
};

virtual ULongTime GetUpdateTime(void) const
{
 return UPropertyInputPreBase<T,OwnerT>::GetUpdateTime();
}

// -----------------------------
// �������� ������� ������ ��� ��������� ������
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 bool res=UVProperty<T,OwnerT>::AttachTo(prop);
 if(res)
 {
  this->PData=const_cast<T*>(&this->ExternalDataSource->GetData());
  UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag=true;
 }
 return res;
}

void DetachFrom(void)
{
 *this->PData=UPropertyInputBase<T,OwnerT,type>::Local;
 UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag=false;
 UVProperty<T,OwnerT>::DetachFrom();
}
// -----------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� ������������� ����������
virtual std::string GetItemName(void) const
{
 return UIPropertyInput::GetItemName();
}

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(void) const
{
 return UIPropertyInput::GetItemFullName();
}

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(void) const
{
 return UIPropertyInput::GetItemOutputName();
}

/// �������������� ������
virtual void Init(UItem* item, const std::string &output_name)
{
 UIPropertyInput::Init(item, output_name);
}

/// ���������������� ������
virtual void UnInit(void)
{
 UIPropertyInput::UnInit();
}
// --------------------------
};

template<typename T, typename OwnerT>
class UVPropertyInputBase: public UPropertyInputPreBase<T,OwnerT>, virtual public UIPropertyInput
{
protected:
/// ������� ���������, ������������ � ����������� ����
mutable T** ExternalPData;

/// ��������� ����������, �������������� ���� ��� ��������� �����������
mutable T* LocalExternalPData;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyInputBase(OwnerT * const owner, T** data, int input_type)
 : UPropertyInputPreBase<T,OwnerT>(owner, input_type), ExternalPData(0), LocalExternalPData(0)
{
 this->PData=&this->Local;
 if(data)
  ExternalPData=data;
 else
  ExternalPData=&LocalExternalPData;
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

/// ��������� ��������� PData
virtual void UpdatePData(void* data)
{
 if(data)
  this->PData=*reinterpret_cast<T**>(data);
 ExternalPData=reinterpret_cast<T**>(data);
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� ������������� ����������
virtual std::string GetItemName(void) const
{
 return UIPropertyInput::GetItemName();
}

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(void) const
{
 return UIPropertyInput::GetItemFullName();
}

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(void) const
{
 return UIPropertyInput::GetItemOutputName();
}
// --------------------------
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
 UPropertyInputBase<T*,OwnerT,type>::Local=&LocalValue;
 this->PData=&(this->Local);
};

/// Deprecated
UPropertyInput(const string &name, OwnerT * const owner, int index)
 : UPropertyInputBase<T*,OwnerT,type>(name, owner, ipSingle | ipComp)
{
 UPropertyInputBase<T*,OwnerT,type>::Local=&LocalValue;
 this->PData=&(this->Local);
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag)
  return *this->PData;

 return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value, UIProperty* output)
{
 if(*this->PData != UPropertyInputBase<T*,OwnerT,type>::Local)
  return false;

 if(value)
 {
  *this->PData=reinterpret_cast<T*>(value);
  this->ConnectedOutput=output;
  UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag=true;
  this->ResetUpdateTime();
  return true;
 }

 return false;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, void* value)
{
 if(*this->PData == value)
 {
  *this->PData=UPropertyInputBase<T*,OwnerT,type>::Local;
  UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag=false;
  this->ConnectedOutput=0;
  return true;
 }

 return false;
}

bool operator ! (void) const
{
 return (UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag)?false:true;
};

T* operator -> (void) const
{
 return (UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag)?*this->PData:UPropertyInputBase<T*,OwnerT,type>::Local;
};

T& operator * (void)
{
 return (UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag)?**this->PData:LocalValue;
};

operator T* (void) const
{
 return (UPropertyInputBase<T*,OwnerT,type>::IsConnectedFlag)?*this->PData:UPropertyInputBase<T*,OwnerT,type>::Local;
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
 this->PData=&(this->Local);
};

/// Deprecated
UPropertyInputData(const string &name, OwnerT * const owner, int index)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, ipSingle | ipData)
{
 this->PData=&(this->Local);
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag;//(this->PData)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag)
  return this->PData;
 return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value, UIProperty* output)
{
 if(value)
 {
  this->PData=reinterpret_cast<T*>(value);
  UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag=true;
  this->ConnectedOutput=output;
  this->ResetUpdateTime();
  return true;
 }
 return false;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, void* value)
{
 if(this->PData == value)
 {
  this->PData=&(this->Local);
  UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag=false;
  this->ConnectedOutput=0;
  return true;
 }
 return false;
}

bool operator ! (void) const
{ return (UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag)?false:true; };

T* operator -> (void) const
{
 this->ApplyOutputUpdateTime();
 return (UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag)?this->PData:&(this->Local);
};

T& operator * (void)
{
 this->ApplyOutputUpdateTime();
 return (UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag)?*this->PData:UPropertyInputBase<T,OwnerT,type>::Local;
};

operator T* (void) const
{
 this->ApplyOutputUpdateTime();
 return (UPropertyInputBase<T,OwnerT,type>::IsConnectedFlag)?this->PData:&(this->Local);
}
// --------------------------

/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
 return (this->ConnectedOutput)?this->ConnectedOutput->GetUpdateTime()>this->UpdateTime:true;
}

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
UVPropertyInputData(OwnerT * const owner, T **data)
 : UVPropertyInputBase<T,OwnerT>(owner, data, ipSingle | ipData)
{
 this->PData=&(this->Local);
};

/// Deprecated
UVPropertyInputData(OwnerT * const owner, T **data, int index)
 : UVPropertyInputBase<T,OwnerT>(owner, data, ipSingle | ipData)
{
 this->PData=&(this->Local);
};

// -----------------------------


// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return (UVPropertyInputBase<T,OwnerT>::IsConnectedFlag)?true:false;
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(UVPropertyInputBase<T,OwnerT>::IsConnectedFlag)
  return this->PData;
 return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, void* value, UIProperty* output)
{
 if(value)
 {
  this->PData=reinterpret_cast<T*>(value);
  *UVPropertyInputBase<T,OwnerT>::ExternalPData=this->PData;
  this->ConnectedOutput=output;
  UVPropertyInputBase<T,OwnerT>::IsConnectedFlag=true;
  this->ResetUpdateTime();
  return true;
 }
 return false;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, void* value)
{
 if(this->PData==value)
 {
  this->PData=&(this->Local);
  *UVPropertyInputBase<T,OwnerT>::ExternalPData=0;
  this->ConnectedOutput=0;
  UVPropertyInputBase<T,OwnerT>::IsConnectedFlag=false;
  return true;
 }
 return false;
}

bool operator ! (void) const
{ return (UVPropertyInputBase<T,OwnerT>::IsConnectedFlag)?false:true; };

T* operator -> (void) const
{
 this->ApplyOutputUpdateTime();
 return (UVPropertyInputBase<T,OwnerT>::IsConnectedFlag)?this->PData:&(this->Local);
};

T& operator * (void)
{
 this->ApplyOutputUpdateTime();
 return (UVPropertyInputBase<T,OwnerT>::IsConnectedFlag)?*this->PData:UVPropertyInputBase<T,OwnerT>::Local;
};

operator T* (void) const
{
 this->ApplyOutputUpdateTime();
 return (UVPropertyInputBase<T,OwnerT>::IsConnectedFlag)?this->PData:&(this->Local);
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
std::vector<T*> Local;

/// ��������� �� ������������ �����
std::vector<UIProperty*> ConnectedOutput;

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
// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return !this->v.empty();
}

// ����� ���������� �� ������
int GetNumPointers(void) const
{
 return int(this->v.size());
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
 if(int(this->v.size())<=index)
 #if defined(__BORLANDC__) && !defined(__clang__)
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
bool SetPointer(int index, void* value, UIProperty* output)
{
 if(index<0)
  return false;

 if(int(this->v.size())<=index)
 {
  size_t new_size=index+1;
  this->v.resize(new_size);
  size_t old_size=Local.size();
  for(size_t i=new_size;i<old_size;i++)
   delete Local[i];
  Local.resize(new_size);
  ConnectedOutput.resize(new_size,0);

  for(size_t i=old_size;i<new_size;i++)
   Local[i]=new T;
 }
 this->v[index]=reinterpret_cast<T*>(value);
 ConnectedOutput[index]=output;
 return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, void* value)
{
 if(int(this->v.size())>index && index >=0)
 {
  delete Local[index];
  Local.erase(Local.begin()+index);
  this->v.erase(this->v.begin()+index);
  this->ConnectedOutput.erase(ConnectedOutput.begin()+index);
  return true;
 }
 return false;
}

/// ������� ��� ���������
void ClearAllPointers(void)
{
 if(this->v.empty())
  return;

 for(size_t i=0;i<this->v.size();i++)
 {
  delete Local[i];
 }
 Local.clear();
 ConnectedOutput.clear();
 this->v.clear();
}

T* operator [] (int i)
{
 if(int(this->v.size())<=i)
 #if defined(__BORLANDC__) && !defined(__clang__)
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	0,int(this->v.size()),i);
 #endif

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

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� ������������� ����������
virtual UItem* GetItem(int index)
{
 if(int(this->v.size())>index && index >=0)
 {
  return reinterpret_cast<UItem*>(ConnectedOutput[index]->GetOwner());
 }
 return 0;
}

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int index) const
{
 if(int(this->v.size())>index && index >=0)
 {
  return ConnectedOutput[index]->GetName();
 }
 return std::string();
}

/// ���������� ��� ������������� ����������
virtual std::string GetItemName(int index) const
{
 if(int(this->v.size())>index && index >=0)
 {
  return ConnectedOutput[index]->GetOwner()->GetName();
 }
 return std::string();
}

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int index) const
{
 if(int(this->v.size())>index && index >=0)
 {
  return ConnectedOutput[index]->GetOwner()->GetFullName();
 }
 return std::string();
}

protected:
/// ���������� ��� ������������� ����������
virtual UItem* GetItem(void)
{
 return 0;
}

/// ���������� ��� ������������� ����������
virtual std::string GetItemName(void) const
{
 return UIPropertyInput::GetItemName();
}

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(void) const
{
 return UIPropertyInput::GetItemFullName();
}

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(void) const
{
 return UIPropertyInput::GetItemOutputName();
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

template<typename T, typename OwnerT, unsigned int type>
std::ostream& operator << (std::ostream &stream, UPropertyInputData<T,OwnerT,type> &input)
{
 using namespace std;
 stream<<"Input "<<input.GetOwnerName()<<":"<<input.GetName();
 if(input.IsConnected())
  stream<<" connected";
 else
  stream<<" NOT connected";
 stream<<endl;
 stream<<"Data:"<<endl;
 stream<<*input;
 stream<<"--------------------";
 return stream;
}


}
#endif
