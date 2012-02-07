#ifndef UCONTAINER_DESCRIPTION_CPP
#define UCONTAINER_DESCRIPTION_CPP

#include "UContainerDescription.h"

namespace RDK {

// Описания общих параметров
std::map<std::string, UParameterDescription> UContainerDescription::CommonParameters;

// --------------------------
// Конструкторы и деструкторы
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
// Методы управления общими параметрами
// --------------------------
// Описание общего параметра
const UParameterDescription& UContainerDescription::GetCommonParameter(const std::string &name)
{
 std::map<std::string, UParameterDescription>::const_iterator I=CommonParameters.find(name);

 // Заглушка! Здесь исключение если не найдено

 return I->second;
}

bool UContainerDescription::SetCommonParameter(const std::string &name, const UParameterDescription& value)
{
 CommonParameters[name]=value;

 return true;
}

// Проверяет наличие общего параметра с заданным именем
bool UContainerDescription::CheckCommonParameter(const std::string &name)
{
 std::map<std::string, UParameterDescription>::const_iterator I=CommonParameters.find(name);

 return I != CommonParameters.end();
}
// --------------------------


// --------------------------
// Методы управления данными
// --------------------------
// Описание параметра
const UParameterDescription& UContainerDescription::GetParameter(const std::string &name) const
{
 std::map<std::string, UParameterDescription>::const_iterator I=Parameters.find(name);

 if(I == Parameters.end())
 {
  I=CommonParameters.find(name);
 }

 return I->second;
}

bool UContainerDescription::SetParameter(const std::string &name, const UParameterDescription& value, bool force)
{
 if(CheckCommonParameter(name) && !force)
  return true;

// I->second=value;
 Parameters[name]=value;

 return true;
}

// Удаляет параметр из этого описания, если он есть в общих описаниях
bool UContainerDescription::RemoveCommonDuplicatesParameter(const std::string &name)
{
 std::map<std::string, UParameterDescription>::const_iterator I=CommonParameters.find(name);

 if(I != CommonParameters.begin())
  Parameters.erase(name);

 return true;
}

// Удаляет все параметры из этого описания, если они есть в общих описаниях
bool UContainerDescription::RemoveCommonDuplicatesParameters(void)
{
 std::map<std::string, UParameterDescription>::iterator I,J;
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
 return true;
}
// --------------------------

// --------------------------
// Методы ввода вывода данными
// --------------------------
// Сохраняет данные класса в XML
bool UContainerDescription::Save(Serialize::USerStorageXML &xml)
{
 if(!UComponentDescription::Save(xml))
  return false;

 xml.AddNode("Parameters");
 std::map<std::string, UParameterDescription>::const_iterator I=Parameters.begin();
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

// Загружает данные класса из XML
bool UContainerDescription::Load(Serialize::USerStorageXML &xml)
{
 if(!UComponentDescription::Load(xml))
  return false;

 if(!xml.SelectNode("Parameters"))
  return false;

 Parameters.clear();
 int num_parameters=xml.GetNumNodes();
 for(int i=0;i<num_parameters;i++)
// std::map<std::string, UParameterDescription>::iterator I=Parameters.begin();
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
// Методы ввода вывода общих данных
// --------------------------
// Сохраняет данные класса в XML
bool UContainerDescription::SaveCommon(Serialize::USerStorageXML &xml)
{
 xml.AddNode("Parameters");
 std::map<std::string, UParameterDescription>::const_iterator I=CommonParameters.begin();
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

// Загружает данные класса из XML
bool UContainerDescription::LoadCommon(Serialize::USerStorageXML &xml)
{
 if(!xml.SelectNode("Parameters"))
  return false;

// std::map<std::string, UParameterDescription>::iterator I=CommonParameters.begin();
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

