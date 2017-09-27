//---------------------------------------------------------------------------

#ifndef UPropertyInterfaceH
#define UPropertyInterfaceH

#include "UTime.h"
#include "UEPtr.h"
#include "../Serialize/Serialize.h"
#include "UComponent.h"

namespace RDK {

class UNet;

// ����� ������������ �������
class RDK_LIB_TYPE UIProperty
{
public:
virtual ~UIProperty(void);

/// ���������� ��������� �� ��������� ��������
virtual const UComponent* GetOwner(void) const=0;
virtual UComponent* GetOwner(void)=0;

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const=0;

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const=0;

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerClassName(void) const=0;

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<USerStorage> storage, bool simplemode=false)=0;

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<USerStorage> storage, bool simplemode=false)=0;

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const=0;

// ����� �������� ��� ��������
virtual bool ChangeType(unsigned int value)=0;

// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const=0;

// ����� ���������� ��� ����� �������� � ������ ���������
virtual bool CompareLanguageType(const UIProperty &dt) const=0;

// ����� ���������� ��������� �� ������� ������, ���������� ������ ��������
virtual const void* GetMemoryArea(void) const=0;

// ����� �������� �������� ������ �������� �� ������� ������
// �������� ���������� ����������� ��������� ���� ������
// ������� ��������� ���������� � ��������� �� ����������� ��� ������
// TODO: possible unsafe???
virtual bool ReadFromMemory(const void *buffer)=0;

public: // ������ ���������� �������� ���������� ������ ��������
/// ���������� ����� ���������� ������ �������� (��)
virtual ULongTime GetUpdateTime(void) const=0;

/// ������������� ����� ���������� ������ ��������
virtual void SetUpdateTime(ULongTime value)=0;

/// ���������� ����� ���������� �� ����
virtual void ResetUpdateTime(void)=0;

/// ��������� ����� ��������� ������ ��������
virtual void RenewUpdateTime(void)=0;

/// ���������� �������� �������� �����/������
//virtual bool CheckRange(int index)=0;

// �������� �������� ������
//virtual int GetMinRange(void)=0;

//virtual int GetMaxRange(void)=0;

public: // ��������������� ������
// ����� ������������� �������� ��������� �� ��������-��������� ������ �� ����
// �������� � ������������ ����������
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)=0;

/// ������������� ����, �����������, ��� ��� ������������ ��������, � ������ ���� ������� ��� ���������� ����������-���������
virtual void SetDynamicPropertyFlag(void)=0;

/// ���������� ��������� �����, ������������, ��� ��� ������������ ��������
virtual bool IsDynamicPropertyFlag(void) const=0;

// ���������� ������ ����� ���� ��������
virtual unsigned int GetPropertyType(void) const=0;

// ���������� ������ ����� ������ ��������
virtual unsigned int GetPropertyGroup(void) const=0;

// ���������� ��������� ��� ���� ��������
virtual std::string GetPropertyTypeName(void) const=0;

// ��������� ������������ ���� � ������ �������� �����
virtual bool CheckMask(unsigned int mask) const=0;


/// ��������� ��������� PData
//virtual void UpdatePData(void* data)=0;

public: // ����������
// ��������� � �������������������� ������ ��������
struct EPropertyError: public EError
{
protected: // ������ ����������
/// ��� ���������� ���������
std::string OwnerName;

/// ��� ��������
std::string PropertyName;

public:
EPropertyError(const std::string &owner_name, const std::string &property_name)
: EError(), OwnerName(owner_name), PropertyName(property_name) {};
virtual ~EPropertyError(void) throw() {};

// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ")+OwnerName+std::string(":")+PropertyName;
}
};

// ��������� � �������������������� ������ ��������
struct EPropertyZeroPtr: public EPropertyError
{
public:
EPropertyZeroPtr(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// ����� Getter �������� ��������
struct EPropertyGetterFail: public EPropertyError
{
public:
EPropertyGetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// ����� Setter �������� ��������
struct EPropertySetterFail: public EPropertyError
{
public:
EPropertySetterFail(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

// ����� Setter �������� ��������
struct EPropertyWrongIndex: public EPropertyError
{
public:
EPropertyWrongIndex(const std::string &owner_name, const std::string &property_name)
: EPropertyError(owner_name, property_name) {};
};

};

class UIPropertyOutput;

class RDK_LIB_TYPE UIPropertyInput: virtual public UIProperty
{
public:
virtual ~UIPropertyInput(void);
/// ���������� ��� �������� �����
//virtual int GetInputType(void) const=0;

public: // ������ ������� � ��������� ������
/// ���������� ����� �� ����� ����������� �� �����
/// ���� -1, �� ��� �����������
virtual int GetNumConnectionsLimit(void) const=0;

/// ���������� ����� ����������� �� �����
virtual int GetNumConnections(void) const=0;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::vector<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const=0;

/// ���������� ��������� �� ��������-��������� ������
virtual const UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index) const=0;
virtual UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index)=0;

/// ���������� ������ ��� ������������� ����������
//virtual std::string GetItemFullName(int c_index=-1) const=0;

/// ���������� ��� ������������� ������
//virtual std::string GetItemOutputName(int c_index=-1) const=0;

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const=0;

// ���������, ���������� �� ����� � �������� �����������
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const=0;

/// ��������� ����� �� ��������� output_property
/// ���� c_index == -1 �� ��������� ��� ��������� ����� ������
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property, int c_index=-1)=0;

/// ��������� ����� �� ������� �_index
virtual bool DisconnectFromOutput(int c_index)=0;

/// ��������� ��� ����� �� ���������
virtual bool DisconnectAllOutputs(void)=0;

public:
/// ��������� �������� �� ���������� ����� �� ��������� output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property)=0;

/// ��������� �������� �� ����������� ����� �� ��������� output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)=0;

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// ���������� ��������� �� ������
//virtual bool ResetPointer(int index, void* value)=0;

protected:
/// ������ ����� �� ����� ����������� �� �����
/// ���� -1, �� ��� �����������
virtual void SetNumConnectionsLimit(int value)=0;

};

class RDK_LIB_TYPE UIPropertyOutput: virtual public UIProperty
{
public: // ������ ������� � ������������ ������
virtual ~UIPropertyOutput(void);
/// ���������� ����� ������������ ������
virtual size_t GetNumSubscribers(void) const=0;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::vector<UEPtr<UIPropertyInput> > GetSubscribedProperties(void) const=0;

/// ���������� ��������� �� ��������-��������� ������
virtual const UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index) const=0;
virtual UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index)=0;

/// ������������� ����� ����� ������ �� ������ input_property
virtual bool ConnectToInput(UIPropertyInput *input_property)=0;

/// ��������� ����� ����� ������ �� ������ input_property
virtual bool DisconnectFromInput(UIPropertyInput *input_property, int c_index=-1)=0;

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual bool DisconnectAllInputs(void)=0;

/// ���������� true ���� ����� ��������� � ���������� �����
virtual bool IsConnectedToInput(UIPropertyInput *input_property)=0;

/// ���������� true ���� ����� ��������� � ������ �� ������ ���������� ����������
virtual bool IsConnectedToComponent(UNet *component)=0;

public: // ������ ���������� ���������� �� �������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;
};

//---------------------------------------------------------------------------
}
#endif
