#ifndef UPropertyImplementationBase_CPP
#define UPropertyImplementationBase_CPP

#include "UPropertyImplementationBase.h"
#include "UNet.h"
//#include "UPropertyOutput.cpp"
//#include "UPropertyInput.cpp"

namespace RDK {

//class UIPropertyInputBase: public UIPropertyIO
/// ������������ � �����������
UIPropertyInputBase::UIPropertyInputBase(void)
{
 SetNumConnectionsLimit(1);
}

UIPropertyInputBase::~UIPropertyInputBase(void)
{
}


/// ���������� ��� �������� �����

int UIPropertyInputBase::GetInputType(void) const
{
 return InputType;
}

/// ���������� ����� �� ����� ����������� �� �����
/// ���� -1, �� ��� �����������
int UIPropertyInputBase::GetNumConnectionsLimit(void) const
{
 return NumConnectionsLimit;
}

/// ���������� ����� ����������� �� �����
int UIPropertyInputBase::GetNumConnections(void) const
{
 return int(ConnectedProperties.size());
}

/// ���������� ��������� �� ��������-��������� ������
const std::vector<UEPtr<UIPropertyOutput> > UIPropertyInputBase::GetConnectedProperties(void) const
{
 return ConnectedProperties;
}

/// ���������� ��������� �� ��������-��������� ������
const UEPtr<UIPropertyOutput> UIPropertyInputBase::GetConnectedProperty(int c_index) const
{
 return ConnectedProperties[c_index];
}

UEPtr<UIPropertyOutput> UIPropertyInputBase::GetConnectedProperty(int c_index)
{
 return ConnectedProperties[c_index];
}
/*
/// ���������� ������ ��� ������������� ����������
std::string UIPropertyInputBase::GetItemFullName(int c_index) const
{
 if(ConnectedProperties.empty())
  return "";

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetOwner()->GetFullName();
 }

 return "";
}

// ���������� ��� ������������� ������
std::string UIPropertyInputBase::GetItemOutputName(int c_index) const
{
 if(ConnectedProperties.empty())
  return "";

 std::list<UEPtr<UIPropertyOutput> >::const_iterator I=ConnectedProperties.begin();
 int i=0;
 for(;I != ConnectedProperties.end();++I,++i)
 {
  if(i == c_index)
   return (*I)->GetName();
 }

 return "";
} */

/// ���������� true ���� ���� ����� �����������
bool UIPropertyInputBase::IsConnected(void) const
{
 return (ConnectedProperties.empty())?false:true;
}

// ���������, ���������� �� ����� � �������� �����������
bool UIPropertyInputBase::IsConnectedTo(const UIPropertyOutput *output_property) const
{
 std::vector<UEPtr<UIPropertyOutput> >::const_iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);
 return I == ConnectedProperties.end();
}

/// ��������� ����� �� ��������� output_property
bool UIPropertyInputBase::Disconnect(UIPropertyOutput *output_property, int c_index)
{
 if(!output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 return output_property->Disconnect(this,c_index);
}

/// ��������� ����� �� ������� �_index
bool UIPropertyInputBase::Disconnect(int c_index)
{
 if(c_index<0 || c_index >=ConnectedProperties.size())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property index not found: ")+sntoa(c_index));
  return false;
 }
 return ConnectedProperties[c_index]->Disconnect(this,c_index);
}


/// ��������� ��� ����� �� ���������
bool UIPropertyInputBase::DisconnectAll(void)
{
 bool res(true);
 std::vector<UEPtr<UIPropertyOutput> >::iterator I=ConnectedProperties.begin(),J;

 while(I != ConnectedProperties.end())
 {
  J=I; ++J;
  res &=(*I)->Disconnect(this);
  I=J;
 }
 return res;
}
/// ��������� �������� �� ���������� ����� �� ��������� output_property
bool UIPropertyInputBase::ConnectToOutput(UIPropertyOutput *output_property)
{

 // TODO: ��� ��������, �� ����� �� ����
 if(NumConnectionsLimit == 0)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. NumConnectionLimit set to zero."));
  return false;
 }

 if(NumConnectionsLimit>0 && ConnectedProperties.size() == NumConnectionsLimit)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Connector already in use."));
  return false;
 }

 if(!CompareLanguageType(*output_property))
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Language types are incompatible. ")+output_property->GetLanguageType().name()+std::string(" != ")+GetLanguageType().name());
  return false;
 }

 // TODO: ��� ��� ����������
 // TODO: ��� ��� ����������� ����������� ������
 ConnectedProperties.push_back(output_property);
 return true;
}

/// ��������� �������� �� ����������� ����� �� ��������� output_property
bool UIPropertyInputBase::DisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(c_index<0 || c_index>=int(ConnectedProperties.size()))
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property index not found: ")+sntoa(c_index));
  return false;
 }

 if(ConnectedProperties[c_index] != output_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found by expected connected index: ")+output_property->GetName()+std::string(" c_index=")+sntoa(c_index));
  return false;
 }
 /*
 std::vector<UEPtr<UIPropertyOutput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),output_property);

 if(I == ConnectedProperties.end())
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+output_property->GetName());
  return false;
 }   */

 // TODO: ��� ��� ����������� ���������� ������
 ConnectedProperties.erase(ConnectedProperties.begin()+c_index);
 return true;
}

/// ������ ����� �� ����� ����������� �� �����
/// ���� -1, �� ��� �����������
void UIPropertyInputBase::SetNumConnectionsLimit(int value)
{
 NumConnectionsLimit=value;
}

/* *************************************************************************** */
//class UIPropertyOutputBase: public UIPropertyIO
/// ������������ � �����������
UIPropertyOutputBase::UIPropertyOutputBase(void)
{
}

UIPropertyOutputBase::~UIPropertyOutputBase(void)
{
// Connectors.clear();
// ConnectorInputNames.clear();
 ConnectedProperties.clear();
}

// ���������� ����� ������������ ������
size_t UIPropertyOutputBase::GetNumConnectors(void) const
{
 return ConnectedProperties.size();
}

/// ���������� ��������� �� ��������-��������� ������
const std::vector<UEPtr<UIPropertyInput> > UIPropertyOutputBase::GetConnectedProperties(void) const
{
 return ConnectedProperties;
}

/// ���������� ��������� �� ��������-��������� ������
const UEPtr<UIPropertyInput> UIPropertyOutputBase::GetConnectedProperty(int c_index) const
{
 return ConnectedProperties[c_index];
}

UEPtr<UIPropertyInput> UIPropertyOutputBase::GetConnectedProperty(int c_index)
{
 return ConnectedProperties[c_index];
}

/// ������������� ����� ����� ������ �� ������ input_property
bool UIPropertyOutputBase::Connect(UIPropertyInput *input_property)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connection failed. Connected property pointer is null"));
  return false;
 }

 if(!input_property->ConnectToOutput(this))
 {
  // ���������� ��������
  return false;
 }
 ConnectedProperties.push_back(input_property);
 return true;
}

/// ��������� ����� ����� ������ �� ������ input_property
/// ���� c_index == -1 �� ��������� ��� ��������� ����� ������
bool UIPropertyOutputBase::Disconnect(UIPropertyInput *input_property, int c_index)
{
 if(!input_property)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property pointer is null"));
  return false;
 }

 bool res(true);
 size_t found_counter(0);
 std::vector<UEPtr<UIPropertyInput> >::iterator I,J;

 I=ConnectedProperties.begin();
 int i(0);
 while(I != ConnectedProperties.end())
 {
  if(*I == input_property)
  {
   if(c_index == -1 || i == c_index)
   {
    J=I; ++J;
	// TODO: ��� ��� ����������� ���������� ������
	ConnectedProperties.erase(I);
	res&=input_property->DisconnectFromOutput(this,i);
    I=J;
   }
   else
   {
	++I;
    ++i;
   }
  }
 }

 if(found_counter == 0)
 {
  GetOwner()->LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Disconnected property not found in ConnectedProperties list: ")+input_property->GetName());
  return false;
 }
 return res;
}

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
bool UIPropertyOutputBase::DisconnectAll(void)
{
 bool res(true);
 std::vector<UEPtr<UIPropertyInput> >::iterator I=ConnectedProperties.begin(),J;
 while(I != ConnectedProperties.end())
 {
  J=I; ++J;
  res &= Disconnect(*I);
  I=J;
 }
 return res;
}

/// ���������� true ���� ����� ��������� � ���������� �����
bool UIPropertyOutputBase::IsConnectedTo(UIPropertyInput *input_property)
{
 std::vector<UEPtr<UIPropertyInput> >::iterator I=find(ConnectedProperties.begin(),ConnectedProperties.end(),input_property);
 if(I == ConnectedProperties.end())
  return false;

 return true;
}

/// ���������� true ���� ����� ��������� � ������ �� ������ ���������� ����������
bool UIPropertyOutputBase::IsConnectedTo(UNet *component)
{
 std::vector<UEPtr<UIPropertyInput> >::iterator I=ConnectedProperties.begin();
 for(;I != ConnectedProperties.end();I++)
  if((*I)->GetOwner() == component)
   return true;

 return false;
}



}

#endif

