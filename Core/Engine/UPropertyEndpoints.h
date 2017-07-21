//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
//---------------------------------------------------------------------------

namespace RDK {

/// deprecated
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class ULProperty: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
ULProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) //ipSingle | ipData
{
};

// �������� ������������
ULProperty<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};


};

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UPropertyParam: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyParam(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// �������� ������������
UPropertyParam<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubState>
class UPropertyState: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyState(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipData
{
};

// �������� ������������
UPropertyState<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// �������� ������������
UPropertyOutputData<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInput(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipComp
{
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipDatas
{
};

};

template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputDataRange: public UPropertyRange<T,RangeT,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputDataRange(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

/// deprecated
template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputDataC: public UPropertyRange<T,RangeT,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputDataC(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

}
#endif
