//---------------------------------------------------------------------------

#ifndef UDynamicNetH
#define UDynamicNetH
#include "UNet.h"
//---------------------------------------------------------------------------
namespace RDK {

template<class T>
class UDynamicNet: public UNet
{
public: // Parameters
/// Number of dynamic inputs
UProperty<int,UDynamicNet<T>, ptPubParameter> NumInputs;

/// Number of dynamic outputs
UProperty<int,UDynamicNet<T>, ptPubParameter> NumOutputs;

protected:
/// Pointers to dynamic IO
std::vector<UEPtr<UProperty<T,UDynamicNet<T>, ptPubInput> > > InputData;

/// Pointers to dynamic IO
std::vector<UEPtr<UProperty<T,UDynamicNet<T>, ptPubOutput> > > OutputData;

public:
// ------------------
// Constructors & destructors
// ------------------
UDynamicNet(void);
virtual ~UDynamicNet(void);
// ------------------

// ------------------
// Parameters setters
// ------------------
/// NumInputs setter
bool SetNumInputs(const int &value);

/// NumOutputs setter
bool SetNumOutputs(const int &value);
// ------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);

// ����� �������� ����� ��� ������ ��������
virtual bool Reset(void);
// --------------------------
};


// ------------------
// Constructors & destructors
// ------------------
template<class T>
UDynamicNet<T>::UDynamicNet(void)
{

}

template<class T>
UDynamicNet<T>::~UDynamicNet(void)
{

}
// ------------------

// ------------------
// Parameters setters
// ------------------
/// NumInputs setter
template<class T>
bool UDynamicNet<T>::SetNumInputs(const int &value)
{
 Ready=false;
 return true;
}

/// NumOutputs setter
template<class T>
bool UDynamicNet<T>::SetNumOutputs(const int &value)
{
 Ready=false;
 return true;
}
// ------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
template<class T>
bool UDynamicNet<T>::Default(void)
{
 NumInputs=1;
 NumOutputs=1;
 return UNet::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
template<class T>
bool UDynamicNet<T>::Build(void)
{
 while(InputData.size() < NumInputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubInput> > prop;
  prop=new UProperty<T,UDynamicNet<T>,ptPubInput>(std::string("DataInput")+sntoa(InputData.size()),this,0,0,true);
  InputData.push_back(prop);
 }

 while(InputData.size() > NumInputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubInput> > prop=InputData.back();
  delete prop;
  InputData.resize(InputData.size()-1);
 }

 while(OutputData.size() < NumOutputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubOutput> > prop;
  prop=new UProperty<T,UDynamicNet<T>,ptPubInput>(std::string("DataOutput")+sntoa(OutputData.size()),this,0,0,true);
  OutputData.push_back(prop);
 }

 while(OutputData.size() > NumOutputs)
 {
  UEPtr<UProperty<T,UDynamicNet<T>,ptPubOutput> > prop=OutputData.back();
  delete prop;
  OutputData.resize(OutputData.size()-1);
 }

 return UNet::Build();
}

// ����� �������� ����� ��� ������ ��������
template<class T>
bool UDynamicNet<T>::Reset(void)
{
 return UNet::Reset();
}
// --------------------------

}
#endif
