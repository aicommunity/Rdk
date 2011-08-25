/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef USER_STORAGE_XML_CPP
#define USER_STORAGE_XML_CPP

#include <string.h>
#include "USerStorageXML.h"

namespace RDK {
namespace Serialize {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorageXML::USerStorageXML(void)
{
}

USerStorageXML::~USerStorageXML(void)
{
}
// --------------------------

// --------------------------
// ������ �������� xml
// --------------------------
// ������� ����� xml � �������� ���� ��������� ������ ������
bool USerStorageXML::Create(const std::string &rootname)
{
 RootNode.deleteNodeContent();
 RootNode=XMLNode::createXMLTopNode(rootname.c_str());
 if(RootNode.isEmpty())
  return false;
// RootNode.updateName(rootname.c_str());
 CurrentNode=RootNode;
 return true;
}

// ��������� xml �� ������
bool USerStorageXML::Load(const std::string &str, const std::string &root)
{
// RootNode.parseString(std.c_str(),"PMML");
 RootNode.deleteNodeContent();
 RootNode=XMLNode::parseString(str.c_str(),root.c_str());
 if(RootNode.isEmpty())
  return false;

 CurrentNode=RootNode;
 return true;
}

// ��������� xml � ������
bool USerStorageXML::Save(std::string &str)
{
 char* pch=RootNode.createXMLString(true);
 if(pch)
 {
  str=pch;
  freeXMLString(pch);
 }
 else
  str="";
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������� �������� ����
// --------------------------
// ��������������� �� �������� ����
void USerStorageXML::SelectRoot(void)
{
 CurrentNode=RootNode;
}

// ��������������� �� ������������ ����
void USerStorageXML::SelectUp(void)
{
 XMLNode node=CurrentNode.getParentNode();
 if(node.isEmpty())
  return;
 CurrentNode=node;
}

// ���������� ����� ����� � �������� ������
int USerStorageXML::GetNumNodes(const std::string &name)
{
 return CurrentNode.nChildNode(name.c_str());
}

// ���������� ����� �����
int USerStorageXML::GetNumNodes(void)
{
 return CurrentNode.nChildNode();
}

// ���� ���� � ����� ������ � �������� ���������� � ������� ���� �� ��������������� �� ����.
// index �������� �� ����� �������������� ��������
// ���� � ����� ������ � �������� �� ����������, �� ���������� ���� � �����
// ������ � �������� ������� �� 1 - �� ������� ����
bool USerStorageXML::SelectNode(const std::string &name, int index)
{
 XMLNode node=CurrentNode.getChildNode(name.c_str(),index);
 if(node.isEmpty())
  return false;
 CurrentNode=node;
 return true;
}

bool USerStorageXML::SelectNode(int index)
{
 XMLNode node=CurrentNode.getChildNode(index);
 if(node.isEmpty())
  return false;
 CurrentNode=node;
 return true;
}

// ���������� ��� ����
const std::string USerStorageXML::GetNodeName(void) const
{
 std::string str;
 if(CurrentNode.getName())
  str=CurrentNode.getName();

 return str;
}

// ������� ����� ���� � �������� ������ � ��������������� �� ����
bool USerStorageXML::AddNode(const std::string &name)
{
 XMLNode node=CurrentNode.addChild(name.c_str());
 if(node.isEmpty())
  return false;
 CurrentNode=node;
 return true;
}

// ��������������� ������� ���� � ���������� ��������� �� ����
void USerStorageXML::RenameNode(const std::string &newname)
{
 CurrentNode.updateName(newname.c_str());
}

// ������� ������� ���� � ������������� ��������� ������� ����
void USerStorageXML::DelNode(void)
{
 XMLNode tmp=CurrentNode.getParentNode();
 if(tmp.isEmpty())
  return;
 CurrentNode.deleteNodeContent();
 CurrentNode=tmp;
}
// --------------------------

// --------------------------
// ������ ���������� ������� �������� ��������
// --------------------------
// ������������� �������� �������� ����
bool USerStorageXML::SetNodeAttribute(const std::string &name, const std::string &value)
{
 if(!CurrentNode.isAttributeSet(name.c_str()))
 {
  CurrentNode.addAttribute(name.c_str(),value.c_str());
 }
 else
 {
  CurrentNode.updateAttribute(value.c_str(),0,name.c_str());
 }
 return true;
}

// ���������� �������� �������� ����
const std::string USerStorageXML::GetNodeAttribute(const std::string &name) const
{
 std::string str;
 const char *p=CurrentNode.getAttribute(name.c_str(),0);

 if(p)
 {
  str=p;
 }
 return str;
}

// ������� �������� �������
void USerStorageXML::DelNodeAttribute(const std::string &name)
{
 CurrentNode.deleteAttribute(name.c_str());
}

// ������������� ����� ����
bool USerStorageXML::SetNodeText(const std::string &text)
{
 CurrentNode.updateText(text.c_str());
 return true;
}

// ���������� �������� ����
const std::string USerStorageXML::GetNodeText(void) const
{
 return CurrentNode.getText();
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
// --------------------------
}
}
#endif

