#ifndef UCONTAINER_DESCRIPTION_CPP
#define UCONTAINER_DESCRIPTION_CPP

#include <map>
#include <string>
#include "UContainerDescription.h"
#include "UXMLEnvSerialize.h"
#include "UStorage.h"

namespace RDK {

UPropertyDescription::UPropertyDescription(void)
: DataSelectionType(0), PropertyType(0)
{

}

// �������� ����� �������
//std::map<std::string, UPropertyDescription> UContainerDescription::CommonProperties;

// --------------------------
// ������������ � �����������
// --------------------------
UContainerDescription::UContainerDescription(void)
{

}

UContainerDescription::UContainerDescription(const UContainerDescription &copy)
 : UComponentDescription(copy),
   Properties(copy.Properties)
{

}

UContainerDescription::~UContainerDescription(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� ������ ����������
// --------------------------
/*
// �������� ������ ��������
const UPropertyDescription& UContainerDescription::GetCommonProperty(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonProperties.find(name);

 // ��������! ����� ���������� ���� �� �������

 return I->second;
}

bool UContainerDescription::SetCommonProperty(const std::string &name, const UPropertyDescription& value)
{
 CommonProperties[name]=value;

 return true;
}

// ��������� ������� ������ �������� � �������� ������
bool UContainerDescription::CheckCommonProperty(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonProperties.find(name);

 return I != CommonProperties.end();
}   */
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// �������� ��������
const UPropertyDescription& UContainerDescription::GetPropertyDescription(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=Properties.find(name);

 if(I == Properties.end())
 {
//  I=CommonProperties.find(name);
//  if(I == CommonProperties.end())
  {
   UPropertyDescription descr;
   I=Properties.insert(Properties.end(),std::pair<std::string, UPropertyDescription>(name,descr));
  }
 }

 return I->second;
}

bool UContainerDescription::SetPropertyDescription(const std::string &name, const UPropertyDescription& value, bool force)
{
// if(CheckCommonProperty(name) && !force)
//  return true;

 Properties[name]=value;

 return true;
}

// ������� �������� �� ����� ��������, ���� �� ���� � ����� ���������
bool UContainerDescription::RemoveCommonDuplicatesDescription(const std::map<std::string, UPropertyDescription> &common_descriptions, const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=common_descriptions.find(name);

 if(I != common_descriptions.begin())
  Properties.erase(name);

 return true;
}

// ������� ��� �������� �� ����� ��������, ���� ��� ���� � ����� ���������
bool UContainerDescription::RemoveCommonDuplicatesDescriptions(const std::map<std::string, UPropertyDescription> &common_descriptions)
{
 std::map<std::string, UPropertyDescription>::iterator I,J;
 for(J=Properties.begin();J != Properties.end();)
 {
  if(common_descriptions.find(J->first) != common_descriptions.end())
  {
   I=J; ++I;
   Properties.erase(J);
   J=I;
  }
  else
   ++J;
 }
 return true;
}

// ���������� ������ ������� (����� � ���������)
void UContainerDescription::CreateProperties()
{
    Properties.clear();
    RDK::UEPtr<RDK::UContainer> cont;
    cont = dynamic_pointer_cast<RDK::UContainer>(Storage->TakeObject(ClassName));

    if(cont)
    {
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();

        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if(i->first.empty())
                continue;

            UPropertyDescription prop_desc;
            prop_desc.Header = "";
            prop_desc.Description = "";
            prop_desc.DataSelectionType = 0;
            prop_desc.Step = "";
            prop_desc.ValueList = {"",""};

            prop_desc.PropertyType = i->second.Type;//i->second.GetPropertyType();
            prop_desc.Type = "";

            if (i->second.CheckMask(ptPubParameter))
            {
                prop_desc.Type += "| ptPubParameter | ";
            }
            if (i->second.CheckMask(ptPubOutput))
            {
                prop_desc.Type += "| ptPubOutput | ";
            }
            if (i->second.CheckMask(ptPubInput))
            {
                prop_desc.Type += "| ptPubInput | ";
            }
            if (i->second.CheckMask(ptPubState))
            {
                prop_desc.Type +="| ptPubState | ";
            }

            SetPropertyDescription(i->first, prop_desc);
        }
        Storage->ReturnObject(cont);
        Storage->FreeObjectsStorageByClass(Storage->FindClassId(ClassName));
    }
}

const std::map<std::string, UPropertyDescription>& UContainerDescription::GetProperties()
{
    return Properties;
}

const std::map<std::string, std::string>& UContainerDescription::GetFavorites()
{
    return Favorites;
}

void UContainerDescription::AddNewFavorite(const std::string& name, const std::string& path)
{
    Favorites[name] = path;
}

void UContainerDescription::DeleteFavorite(const std::string &name)
{
    Favorites.erase(name);
}

// --------------------------

// --------------------------
// ������ ����� ������ �������
// --------------------------
// ��������� ������ ������ � XML
bool UContainerDescription::Save(USerStorageXML &xml)
{
 if(!UComponentDescription::Save(xml))
  return false;

 xml.AddNode("Properties");
 std::map<std::string, UPropertyDescription>::const_iterator I=Properties.begin();
 while(I != Properties.end())
 {
  xml.AddNode(I->first);

  xml.WriteString("Header",I->second.Header);
  xml.WriteString("Description",I->second.Description);
  xml.WriteString("Type",I->second.Type);
  xml.WriteInteger("DataSelectionType",I->second.DataSelectionType);
  xml.WriteData("ValueList",I->second.ValueList);
  xml.WriteInteger("PropertyType",I->second.PropertyType);
  if(I->second.DataSelectionType == 4)
   xml.WriteString("Step",I->second.Step);

  xml.SelectUp();
  ++I;
 }
 xml.SelectUp();

 xml.AddNode("Favorites");
 std::map<std::string, std::string>::const_iterator J=Favorites.begin();
 while(J != Favorites.end())
 {
  xml.AddNode(J->first);

  xml.WriteString("Path",J->second);

  xml.SelectUp();
  ++J;
 }
 xml.SelectUp();

 return true;
}

// ��������� ������ ������ �� XML
bool UContainerDescription::Load(USerStorageXML &xml)
{
 if(!UComponentDescription::Load(xml))
  return false;

 if(!xml.SelectNode("Properties"))
  return false;

 Properties.clear();
 int num_parameters=xml.GetNumNodes();
 for(int i=0;i<num_parameters;i++)
// std::map<std::string, UPropertyDescription>::iterator I=Properties.begin();
// while(I != Properties.end())
 {
  if(!xml.SelectNode(i))
  {
//   ++I;
   continue;
  }

  std::string nodename=xml.GetNodeName();
  Properties[nodename].Header=xml.ReadString("Header",Properties[nodename].Header);
  Properties[nodename].Description=xml.ReadString("Description",Properties[nodename].Description);
  Properties[nodename].Type=xml.ReadString("Type",Properties[nodename].Type);
  Properties[nodename].DataSelectionType=xml.ReadInteger("DataSelectionType",Properties[nodename].DataSelectionType);
  Properties[nodename].PropertyType=xml.ReadInteger("PropertyType",Properties[nodename].PropertyType);
  xml.ReadData("ValueList",Properties[nodename].ValueList);
  if(Properties[nodename].DataSelectionType == 4)
   Properties[nodename].Step=xml.ReadString("Step",Properties[nodename].Step);

  xml.SelectUp();
//  ++I;
 }
 xml.SelectUp();


 if(!xml.SelectNode("Favorites"))
  return false;
 Favorites.clear();
 num_parameters=xml.GetNumNodes();
 for(int i=0;i<num_parameters;i++)
 {
  if(!xml.SelectNode(i))
  {
   continue;
  }
  std::string nodename=xml.GetNodeName();
  Favorites[nodename] = xml.ReadString("Path", "");
  xml.SelectUp();
 }
 xml.SelectUp();


 return true;
}
// --------------------------


// --------------------------
// ������ ����� ������ ����� ������
// --------------------------
/*
// ��������� ������ ������ � XML
bool UContainerDescription::SaveCommon(USerStorageXML &xml)
{
 xml.AddNode("Properties");
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonProperties.begin();
 while(I != CommonProperties.end())
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
bool UContainerDescription::LoadCommon(USerStorageXML &xml)
{
 if(!xml.SelectNode("Properties"))
  return false;

// std::map<std::string, UPropertyDescription>::iterator I=CommonProperties.begin();
// while(I != CommonProperties.end())
 CommonProperties.clear();
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
   CommonProperties[nodename].Header=xml.GetNodeText();
   xml.SelectUp();
  }

  xml.SelectUp();
//  ++I;
 }
 xml.SelectUp();
 return true;
}         */
// --------------------------


}
#endif

