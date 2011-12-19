#ifndef UCONTAINER_DESCRIPTION_CPP
#define UCONTAINER_DESCRIPTION_CPP

#include "UContainerDescription.h"

namespace RDK {

// �������� ����� ����������
map<std::string, UParameterDescription> UContainerDescription::CommonParameters;

// --------------------------
// ������������ � �����������
// --------------------------
UContainerDescription::UContainerDescription(void)
{

}

UContainerDescription::UContainerDescription(const UContainerDescription &copy)
 : UComponentDescription(copy),
   Parameters(copy.Parameters)
{

}

UContainerDescription::~UContainerDescription(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� ������ �����������
// --------------------------
// �������� ������ ���������
const UParameterDescription& UContainerDescription::GetCommonParameter(const string &name)
{
 map<std::string, UParameterDescription>::const_iterator I=CommonParameters.find(name);

 // ��������! ����� ���������� ���� �� �������

 return I->second;
}

bool UContainerDescription::SetCommonParameter(const string &name, const UParameterDescription& value)
{
 CommonParameters[name]=value;

 return true;
}

// ��������� ������� ������ ��������� � �������� ������
bool UContainerDescription::CheckCommonParameter(const string &name)
{
 map<std::string, UParameterDescription>::const_iterator I=CommonParameters.find(name);

 return I != CommonParameters.end();
}
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// �������� ���������
const UParameterDescription& UContainerDescription::GetParameter(const string &name) const
{
 map<std::string, UParameterDescription>::const_iterator I=Parameters.find(name);

 if(I == Parameters.end())
 {
  I=CommonParameters.find(name);
 }

 return I->second;
}

bool UContainerDescription::SetParameter(const string &name, const UParameterDescription& value, bool force)
{
 if(CheckCommonParameter(name) && !force)
  return true;

// I->second=value;
 Parameters[name]=value;

 return true;
}

// ������� �������� �� ����� ��������, ���� �� ���� � ����� ���������
bool UContainerDescription::RemoveCommonDuplicatesParameter(const string &name)
{
 map<std::string, UParameterDescription>::const_iterator I=CommonParameters.find(name);

 if(I != CommonParameters.begin())
  Parameters.erase(name);

 return true;
}

// ������� ��� ��������� �� ����� ��������, ���� ��� ���� � ����� ���������
bool UContainerDescription::RemoveCommonDuplicatesParameters(void)
{
 map<std::string, UParameterDescription>::iterator I,J;
 for(J=Parameters.begin();J != Parameters.end();)
 {
  if(CommonParameters.find(J->first) != CommonParameters.end())
  {
   I=J; ++I;
   Parameters.erase(J);
   J=I;
  }
  else
   ++J;
 }

}
// --------------------------

// --------------------------
// ������ ����� ������ �������
// --------------------------
// ��������� ������ ������ � XML
bool UContainerDescription::Save(Serialize::USerStorageXML &xml)
{
 if(!UComponentDescription::Save(xml))
  return false;

 xml.AddNode("Parameters");
 map<std::string, UParameterDescription>::const_iterator I=Parameters.begin();
 while(I != Parameters.end())
 {
  xml.AddNode(I->first);

  xml.AddNode("Header");
  xml.SetNodeText(I->second.Header);
  xml.SelectUp();

  xml.SelectUp();
  ++I;
 }
 xml.SelectUp();
 return true;
}

// ��������� ������ ������ �� XML
bool UContainerDescription::Load(Serialize::USerStorageXML &xml)
{
 if(!UComponentDescription::Load(xml))
  return false;

 if(!xml.SelectNode("Parameters"))
  return false;

 Parameters.clear();
 int num_parameters=xml.GetNumNodes();
 for(int i=0;i<num_parameters;i++)
// map<std::string, UParameterDescription>::iterator I=Parameters.begin();
// while(I != Parameters.end())
 {
  if(!xml.SelectNode(i))
  {
//   ++I;
   continue;
  }

  std::string nodename=xml.GetNodeName();
  if(xml.SelectNode("Header"))
  {
   Parameters[nodename].Header=xml.GetNodeText();
   xml.SelectUp();
  }

  xml.SelectUp();
//  ++I;
 }
 xml.SelectUp();
 return true;
}
// --------------------------


// --------------------------
// ������ ����� ������ ����� ������
// --------------------------
// ��������� ������ ������ � XML
bool UContainerDescription::SaveCommon(Serialize::USerStorageXML &xml)
{
 xml.AddNode("Parameters");
 map<std::string, UParameterDescription>::const_iterator I=CommonParameters.begin();
 while(I != CommonParameters.end())
 {
  xml.AddNode(I->first);

  xml.AddNode("Header");
  xml.SetNodeText(I->second.Header);
  xml.SelectUp();

  xml.SelectUp();
  ++I;
 }
 xml.SelectUp();
 return true;
}

// ��������� ������ ������ �� XML
bool UContainerDescription::LoadCommon(Serialize::USerStorageXML &xml)
{
 if(!xml.SelectNode("Parameters"))
  return false;

// map<std::string, UParameterDescription>::iterator I=CommonParameters.begin();
// while(I != CommonParameters.end())
 CommonParameters.clear();
 int num_parameters=xml.GetNumNodes();
 for(int i=0;i<num_parameters;i++)
 {
  if(!xml.SelectNode(i))
  {
//   ++I;
   continue;
  }

  std::string nodename=xml.GetNodeName();
  if(xml.SelectNode("Header"))
  {
   CommonParameters[nodename].Header=xml.GetNodeText();
   xml.SelectUp();
  }

  xml.SelectUp();
//  ++I;
 }
 xml.SelectUp();
 return true;
}
// --------------------------


}
#endif

