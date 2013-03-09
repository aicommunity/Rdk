#ifndef UCONTAINER_DESCRIPTION_CPP
#define UCONTAINER_DESCRIPTION_CPP

#include "UContainerDescription.h"

namespace RDK {

// Описания общих свойств
std::map<std::string, UPropertyDescription> UContainerDescription::CommonProperties;

// --------------------------
// Конструкторы и деструкторы
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
// Методы управления общими свойствами
// --------------------------
// Описание общего свойства
const UPropertyDescription& UContainerDescription::GetCommonProperty(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonProperties.find(name);

 // Заглушка! Здесь исключение если не найдено

 return I->second;
}

bool UContainerDescription::SetCommonProperty(const std::string &name, const UPropertyDescription& value)
{
 CommonProperties[name]=value;

 return true;
}

// Проверяет наличие общего свойства с заданным именем
bool UContainerDescription::CheckCommonProperty(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonProperties.find(name);

 return I != CommonProperties.end();
}
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Описание свойства
const UPropertyDescription& UContainerDescription::GetProperty(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=Properties.find(name);

 if(I == Properties.end())
 {
  I=CommonProperties.find(name);
  if(I == CommonProperties.end())
  {
   UPropertyDescription descr;
   I=Properties.insert(Properties.end(),pair<std::string, UPropertyDescription>(name,descr));
  }
 }

 return I->second;
}

bool UContainerDescription::SetProperty(const std::string &name, const UPropertyDescription& value, bool force)
{
 if(CheckCommonProperty(name) && !force)
  return true;

// I->second=value;
 Properties[name]=value;

 return true;
}

// Удаляет свойство из этого описания, если он есть в общих описаниях
bool UContainerDescription::RemoveCommonDuplicatesProperty(const std::string &name)
{
 std::map<std::string, UPropertyDescription>::const_iterator I=CommonProperties.find(name);

 if(I != CommonProperties.begin())
  Properties.erase(name);

 return true;
}

// Удаляет все свойства из этого описания, если они есть в общих описаниях
bool UContainerDescription::RemoveCommonDuplicatesProperties(void)
{
 std::map<std::string, UPropertyDescription>::iterator I,J;
 for(J=Properties.begin();J != Properties.end();)
 {
  if(CommonProperties.find(J->first) != CommonProperties.end())
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
// --------------------------

// --------------------------
// Методы ввода вывода данными
// --------------------------
// Сохраняет данные класса в XML
bool UContainerDescription::Save(USerStorageXML &xml)
{
 if(!UComponentDescription::Save(xml))
  return false;

 xml.AddNode("Properties");
 std::map<std::string, UPropertyDescription>::const_iterator I=Properties.begin();
 while(I != Properties.end())
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

// Загружает данные класса из XML
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
  if(xml.SelectNode("Header"))
  {
   Properties[nodename].Header=xml.GetNodeText();
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
// Методы ввода вывода общих данных
// --------------------------
// Сохраняет данные класса в XML
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

// Загружает данные класса из XML
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
}
// --------------------------


}
#endif

