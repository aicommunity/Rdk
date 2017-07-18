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
protected: // ������
/// ������ ������������ �� ����� ����������
//std::vector<UCItem> ItemsList;

/// ��������� �� ��������-��������� ������
std::list<UEPtr<UIPropertyOutput> > ConnectedProperties;

/// ��� �����
int InputType;

public:
/// ������������ � �����������
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// ���������� ��� �������� �����-������
virtual int GetInputType(void) const;

public: // ������ ������� � ��������� ������
/// ���������� ����� ����������� �� �����
virtual int GetNumConnections(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::list<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const;

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int c_index=-1) const;

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int c_index=-1) const;

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const;

// ���������, ���������� �� ����� � �������� �����������
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const;

/// ��������� ����� �� ��������� output_property
virtual bool Disconnect(UIPropertyOutput *output_property);

/// ��������� ��� ����� �� ���������
virtual bool DisconnectAll(void);

public:
/// ��������� �������� �� ���������� ����� �� ��������� output_property
virtual bool ConnectToOutput(UIPropertyOutput *output_property);

/// ��������� �������� �� ����������� ����� �� ��������� output_property
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// ���������� ��������� �� ������
//virtual bool ResetPointer(int index, void* value)=0;
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
protected: // ������
/// ��������� �� ��������-��������� ������
std::list<UEPtr<UIPropertyInput> > ConnectedProperties;

public: // ������������ � �����������
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // ������ ������� � ������������ ������
/// ���������� ����� ������������ ������
virtual size_t GetNumConnectors(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::list<UEPtr<UIPropertyInput> > GetConnectedProperties(void) const;

/// ������������� ����� ����� ������ �� ������ input_property
virtual bool Connect(UIPropertyInput *input_property);

/// ��������� ����� ����� ������ �� ������ input_property
virtual bool Disconnect(UIPropertyInput *input_property);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual bool DisconnectAll(void);

/// ���������� true ���� ����� ��������� � ���������� �����
virtual bool IsConnectedTo(UIPropertyInput *input_property);

/// ���������� true ���� ����� ��������� � ������ �� ������ ���������� ����������
virtual bool IsConnectedTo(UNet *component);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;
};




}


#endif
