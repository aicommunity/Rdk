#ifndef UCOMPONENT_DESCRIPTION_CPP
#define UCOMPONENT_DESCRIPTION_CPP

#include "UComponentDescription.h"
//#include "UComponent.h"
#include "../Utilities/USupport.h"
#include "UStorage.h"
namespace RDK {

/* ������� ����� �������� ���������� */
// --------------------------
// ������������ � �����������
// --------------------------
UComponentDescription::IException::IException(void)
{

}
// IException(const UContainer *cont);
UComponentDescription::IException::IException(const IException &copy)
{

}

UComponentDescription::IException::~IException(void)
{

}
// --------------------------
/* ******************************** */

// --------------------------
// ������������ � �����������
// --------------------------
UComponentDescription::UComponentDescription(void)
    : Storage(NULL)
{

}

UComponentDescription::UComponentDescription(const UComponentDescription &copy)
 : 	ClassName(copy.ClassName),
	Header(copy.Header),
    Description(copy.Description),
    Storage(copy.Storage)
{

}

UComponentDescription::~UComponentDescription(void)
{

}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ��� ������
const std::string& UComponentDescription::GetClassNameValue(void) const
{
 return ClassName;
}

void UComponentDescription::SetClassNameValue(const std::string& value)
{
 if(ClassName == value)
  return;

 ClassName=value;
 CreateProperties();
}

// ��������� ������
const std::string& UComponentDescription::GetHeader(void) const
{
 return Header;
}

void UComponentDescription::SetHeader(const std::string& value)
{
 if(Header == value)
  return;

 Header=value;
}

// ��������
const std::string& UComponentDescription::GetDescription(void) const
{
 return Description;
}

void UComponentDescription::SetDescription(const std::string& value)
{
 if(Description == value)
  return;

 Description=value;
}

// ��������� ��������� �� ���������
void UComponentDescription::SetStorage(UStorage* storage)
{
    Storage = storage;
}

// �������� �������� ������� �������� (����� � ���������)
void UComponentDescription::CreateProperties()
{

}
// --------------------------

// --------------------------
// ������ ����� ������ �������
// --------------------------
// ��������� ������ ������ � XML
bool UComponentDescription::Save(USerStorageXML &xml)
{
 xml.AddNode("ClassName");
 xml.SetNodeText(ClassName);
 xml.SelectUp();

 xml.AddNode("Header");
 xml.SetNodeText(Header);
 xml.SelectUp();

 xml.AddNode("Description");
 xml.SetNodeText(Description);
 xml.SelectUp();
 return true;
}

// ��������� ������ ������ �� XML
bool UComponentDescription::Load(USerStorageXML &xml)
{
 if(xml.SelectNode("ClassName"))
 {
  ClassName=xml.GetNodeText();
  xml.SelectUp();
 }

 if(xml.SelectNode("Header"))
 {
  Header=xml.GetNodeText();
  xml.SelectUp();
 }

 if(xml.SelectNode("Description"))
 {
  Description=xml.GetNodeText();
  xml.SelectUp();
 }

 return true;
}
// --------------------------

}
#endif


