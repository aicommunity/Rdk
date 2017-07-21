#ifndef UPropertyImplementationBase_H
#define UPropertyImplementationBase_H

#include "UPropertyInterface.h"
//#include "UPropertyOutput.h"
//#include "UPropertyInput.h"

namespace RDK {

class UNet;
//typedef UEPtr<UNet> PUAConnector;


class RDK_LIB_TYPE UIPropertyInputBase: virtual public UIPropertyInput
{
private: // ������
/// ��������� �� ��������-��������� ������
std::vector<UEPtr<UIPropertyOutput> > ConnectedProperties;

/// ����� �� ����� �����������
/// ���� -1, �� ��� �����������
int NumConnectionsLimit;

/// ��� �����
//int InputType;

public:
/// ������������ � �����������
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// ���������� ��� �������� �����-������
//virtual int GetInputType(void) const;

public: // ������ ������� � ��������� ������
/// ���������� ����� �� ����� ����������� �� �����
/// ���� -1, �� ��� �����������
virtual int GetNumConnectionsLimit(void) const;

/// ���������� ����� ����������� �� �����
virtual int GetNumConnections(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::vector<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index) const;
virtual UEPtr<UIPropertyOutput> GetConnectedProperty(int c_index);

/// ���������� ������ ��� ������������� ����������
//virtual std::string GetItemFullName(int c_index) const;

/// ���������� ��� ������������� ������
//virtual std::string GetItemOutputName(int c_index) const;

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const;

// ���������, ���������� �� ����� � �������� �����������
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const;

/// ��������� ����� �� ��������� output_property
/// ���� c_index == -1 �� ��������� ��� ��������� ����� ������
virtual bool Disconnect(UIPropertyOutput *output_property, int c_index=-1);

/// ��������� ����� �� ������� �_index
virtual bool Disconnect(int c_index);

/// ��������� ��� ����� �� ���������
virtual bool DisconnectAll(void);

public:
/// ��������� �������� �� ���������� ����� �� ��������� output_property
virtual bool ConnectToOutput(UIPropertyOutput *output_property);

/// ��������� �������� �� ����������� ����� �� ��������� output_property
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property, int c_index);

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
virtual void SetNumConnectionsLimit(int value);
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
private: // ������
/// ��������� �� ��������-��������� ������
std::vector<UEPtr<UIPropertyInput> > SubscribedProperties;

public: // ������������ � �����������
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // ������ ������� � ������������ ������
/// ���������� ����� ������������ ������
virtual size_t GetNumSubscribers(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::vector<UEPtr<UIPropertyInput> > GetSubscribedProperties(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index) const;
virtual UEPtr<UIPropertyInput> GetSubscribedProperty(int c_index);

/// ������������� ����� ����� ������ �� ������ input_property
virtual bool Connect(UIPropertyInput *input_property);

/// ��������� ����� ����� ������ �� ������ input_property
virtual bool Disconnect(UIPropertyInput *input_property, int c_index=-1);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual bool DisconnectAll(void);

/// ���������� true ���� ����� ��������� � ���������� �����
virtual bool IsConnectedToInput(UIPropertyInput *input_property);

/// ���������� true ���� ����� ��������� � ������ �� ������ ���������� ����������
virtual bool IsConnectedToComponent(UNet *component);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;
};




}


#endif
