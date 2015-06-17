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
#include <iostream>
#include <fstream>
#include "USerStorageXML.h"
#include "../Utilities/USupport.h"

namespace RDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
// ������� ������ �������
USerStorageXML::USerStorageXML(void)
#ifdef RDK_UNICODE_RUN
: Locale("")
#endif
{
}

USerStorageXML::~USerStorageXML(void)
{
 Destroy();
}
// --------------------------

// --------------------------
// ������ �������� xml
// --------------------------
// ������� ����� xml � �������� ���� ��������� ������ ������
bool USerStorageXML::Create(const std::string &rootname)
{
 CurrentNode.deleteNodeContent();
 RootNode.deleteNodeContent();
#ifdef RDK_UNICODE_RUN
 RootNode=XMLNode::createXMLTopNode(widen(rootname,Locale,WBuffer).c_str());
#else
 RootNode=XMLNode::createXMLTopNode(rootname.c_str());
#endif
 if(RootNode.isEmpty())
  return false;
 CurrentNode=RootNode;
 return true;
}

// ���������� xml
bool USerStorageXML::Destroy(void)
{
 RootNode.deleteNodeContent();
 CurrentNode.deleteNodeContent();
// CurrentNode=RootNode;
 return true;
}

// ��������� xml �� ������
bool USerStorageXML::Load(const std::string &str, const std::string &root)
{
 CurrentNode.deleteNodeContent();
 RootNode.deleteNodeContent();
#ifdef RDK_UNICODE_RUN
 RootNode=XMLNode::parseString(widen(str,Locale,WBuffer).c_str(),widen(root,Locale,WBuffer2).c_str());
#else
 RootNode=XMLNode::parseString(str.c_str(),root.c_str());
#endif
 if(RootNode.isEmpty())
  return false;

 CurrentNode=RootNode;
 return true;
}

bool USerStorageXML::LoadToNode(const std::string &str, const std::string &root, bool node_clear)
{
 if(node_clear)
  DelNodeInternalContent();
#ifdef RDK_UNICODE_RUN
 CurrentNode.addChild(XMLNode::parseString(widen(str,Locale,WBuffer).c_str(),widen(root,Locale,WBuffer2).c_str()));
#else
 CurrentNode.addChild(XMLNode::parseString(str.c_str(),root.c_str()));
#endif
 if(CurrentNode.isEmpty())
  return false;

 return true;
}

bool USerStorageXML::LoadToNode(USerStorageXML &node, bool node_clear)
{
 if(node_clear)
  DelNodeInternalContent();
 CurrentNode.addChild(node.RootNode.deepCopy());
 return true;
}

bool USerStorageXML::LoadFieldsToNode(USerStorageXML &node, bool node_clear)
{
 if(node_clear)
  DelNodeInternalContent();
 for(int i=0;i<node.GetNumNodes();i++)
 {
  node.SelectNode(i);
  CurrentNode.addChild(node.CurrentNode.deepCopy());
  node.SelectUp();
 }
 return true;
}

// ��������� xml � ������
bool USerStorageXML::Save(std::string &str) const
{
#ifdef RDK_UNICODE_RUN
 wchar_t* pch=RootNode.createXMLString(true);
 if(pch)
 {
  narrow(pch,Locale,str);
  freeXMLString(pch);
 }
 else
  str="";
#else
 char* pch=RootNode.createXMLString(true);
 if(pch)
 {
  str=pch;
  freeXMLString(pch);
 }
 else
  str="";
#endif
 return true;
}

bool USerStorageXML::SaveFromNode(std::string &str)
{
#ifdef RDK_UNICODE_RUN
 wchar_t* pch=CurrentNode.createXMLString(true);
 if(pch)
 {
  narrow(pch,Locale,str);
  freeXMLString(pch);
 }
 else
  str="";
#else
 char* pch=CurrentNode.createXMLString(true);
 if(pch)
 {
  str=pch;
  freeXMLString(pch);
 }
 else
  str="";
#endif
 return true;
}

// �������� ���� � �����
// ����� �� ������ ������, ���� FileName == ""
bool USerStorageXML::LoadFromFile(const std::string &file_name, const std::string &root)
{
 fstream file(file_name.c_str(), ios::in);
// char buffer[4096];

 string result;

 if(!file || !file.is_open())
  return false;

 while(!file.eof() && !file.fail())
  {
   std::string buffer;
   if(!std::getline(file,buffer))
    break;
//   file.getline(buffer,4096);
   result+=buffer;
  }

 file.close();
 Load(result,root);
 return true;
};

// �������� ���� �� �����
// ����� �� ������ ������, ���� FileName == ""
bool USerStorageXML::SaveToFile(const std::string &file_name)
{
 fstream file(file_name.c_str(), ios::out | ios::trunc);

 if(!file || !file.is_open())
  return false;

 std::string result;
 Save(result);
 file.write(result.c_str(),result.size());

 file.close();
 return true;
};
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
#ifdef RDK_UNICODE_RUN
 return CurrentNode.nChildNode(widen(name,Locale,WBuffer).c_str());
#else
 return CurrentNode.nChildNode(name.c_str());
#endif
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
#ifdef RDK_UNICODE_RUN
 XMLNode node=CurrentNode.getChildNode(widen(name,Locale,WBuffer).c_str(),index);
#else
 XMLNode node=CurrentNode.getChildNode(name.c_str(),index);
#endif
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

// ���� ���� � ����� ������ ���������� � ������� ���� �� ��������������� �� ����.
// ����� ������� ����� ����
// ������ ��������������� �� 0 ����!
bool USerStorageXML::SelectNodeForce(const std::string &name)
{
 if(!SelectNode(name))
  return AddNode(name);

 return true;
}

// ���������� SelectNodeForce, �� ��������������� ������ �� ��������� ����,
// � ������������ ��������� ���������� ����, � ������������ � ���� '/'
bool USerStorageXML::SelectNodeRoot(const std::string &name)
{
 std::vector<std::string> nodes;

 separatestring(name,nodes, '/');
 SelectRoot();
 if(nodes.empty())
  return false;

 if(GetNodeName() != nodes[0])
  Create(nodes[0]);
 for(size_t i=1;i<nodes.size();i++)
 {
  if(!SelectNode(nodes[i]))
   if(!AddNode(nodes[i]))
    return false;
 }
 return true;
}

// ���������� ��� ����
const std::string USerStorageXML::GetNodeName(void) const
{
 std::string str;
 if(CurrentNode.isEmpty())
  return str;

 if(CurrentNode.getName())
#ifdef RDK_UNICODE_RUN
  narrow(CurrentNode.getName(),Locale,str);
#else
  str=CurrentNode.getName();
#endif

 return str;
}

// ������� ����� ���� � �������� ������ � ��������������� �� ����
bool USerStorageXML::AddNode(const std::string &name)
{
 if(CurrentNode.isEmpty())
  return false;

#ifdef RDK_UNICODE_RUN
 XMLNode node=CurrentNode.addChild(widen(name,Locale,WBuffer).c_str());
#else
 XMLNode node=CurrentNode.addChild(name.c_str());
#endif

 if(node.isEmpty())
  return false;
 CurrentNode=node;
 return true;
}

// ��������������� ������� ���� � ���������� ��������� �� ����
void USerStorageXML::RenameNode(const std::string &newname)
{
 if(CurrentNode.isEmpty())
  return;

#ifdef RDK_UNICODE_RUN
 CurrentNode.updateName(widen(newname,Locale,WBuffer).c_str());
#else
 CurrentNode.updateName(newname.c_str());
#endif
}

// ������� ������� ���� � ������������� ��������� ������� ����
void USerStorageXML::DelNode(void)
{
 if(CurrentNode.isEmpty())
  return;

 XMLNode tmp=CurrentNode.getParentNode();
 if(tmp.isEmpty())
  return;
 CurrentNode.deleteNodeContent();
 CurrentNode=tmp;
}

// ������� ���������� ����
void USerStorageXML::DelNodeContent(void)
{
 CurrentNode.deleteNodeContent();
}

// ������� ���������� ���� �������� ��� ����
void USerStorageXML::DelNodeInternalContent(void)
{
 int num_nodes=CurrentNode.nChildNode();
 for(int i=0;i<num_nodes;i++)
 {
  SelectNode(0);
  DelNode();
 }
}
// --------------------------

// --------------------------
// ������ ���������� ������� �������� ��������
// --------------------------
// ������������� �������� �������� ����
bool USerStorageXML::SetNodeAttribute(const std::string &name, const std::string &value)
{
#ifdef RDK_UNICODE_RUN
 if(!CurrentNode.isAttributeSet(widen(name,Locale,WBuffer).c_str()))
 {
  CurrentNode.addAttribute(widen(name,Locale,WBuffer).c_str(),widen(value,Locale,WBuffer2).c_str());
 }
 else
 {
  CurrentNode.updateAttribute(widen(value,Locale,WBuffer).c_str(),0,widen(name,Locale,WBuffer2).c_str());
 }
#else
 if(!CurrentNode.isAttributeSet(name.c_str()))
 {
  CurrentNode.addAttribute(name.c_str(),value.c_str());
 }
 else
 {
  CurrentNode.updateAttribute(value.c_str(),0,name.c_str());
 }
#endif
 return true;
}

// ���������� �������� �������� ����
const std::string USerStorageXML::GetNodeAttribute(const std::string &name) const
{
 std::string str;
#ifdef RDK_UNICODE_RUN
 const wchar_t *p=CurrentNode.getAttribute(widen(name,Locale,WBuffer).c_str(),0);

 if(p)
  narrow(p,Locale,str);
#else
 const char *p=CurrentNode.getAttribute(name.c_str(),0);

 if(p)
  str=p;
#endif
 return str;
}

// ������� �������� �������
void USerStorageXML::DelNodeAttribute(const std::string &name)
{
#ifdef RDK_UNICODE_RUN
 CurrentNode.deleteAttribute(widen(name,Locale,WBuffer).c_str());
#else
 CurrentNode.deleteAttribute(name.c_str());
#endif
}

// ������������� ����� ����
bool USerStorageXML::SetNodeText(const std::string &text)
{
#ifdef RDK_UNICODE_RUN
 CurrentNode.updateText(widen(text,Locale,WBuffer).c_str());
#else
 CurrentNode.updateText(text.c_str());
#endif
 return true;
}

// ���������� �������� ����
const std::string USerStorageXML::GetNodeText(void) const
{
 if(!CurrentNode.getText())
  return string();

#ifdef RDK_UNICODE_RUN
 return narrow(CurrentNode.getText(),Locale,SBuffer);
#else
 return CurrentNode.getText();
#endif
}
// --------------------------

// --------------------------
// �������������� ������ ���������� ������� �������� �������� ��� ini-������
// --------------------------
// ��������� ������ ��� ��������������� ���, ���� ������ �� ������� ��� �� ��������� �
// ��������� ��� - ��� ���������������� ��������� �� ���������
const std::string USerStorageXML::ReadString(const std::string &name, const std::string &default_value)
{
 if(!SelectNode(name))
  return default_value;

 std::string res=GetNodeText();

 SelectUp();
 return res;
}

const std::string USerStorageXML::ReadString(const std::string &name, int node_index, const std::string &default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 std::string res=GetNodeText();

 SelectUp();
 return res;
}

const std::string USerStorageXML::ReadString(int node_index, const std::string &default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 std::string res=GetNodeText();

 SelectUp();
 return res;
}

int USerStorageXML::ReadInteger(const std::string &name, int default_value)
{
 if(!SelectNode(name))
  return default_value;

 int res=atoi(GetNodeText());

 SelectUp();
 return res;
}

int USerStorageXML::ReadInteger(const std::string &name, int node_index, int default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 int res=atoi(GetNodeText());

 SelectUp();
 return res;
}

int USerStorageXML::ReadInteger(int node_index, int default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 int res=atoi(GetNodeText());

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(const std::string &name, double default_value)
{
 if(!SelectNode(name))
  return default_value;

 double res=atof(GetNodeText());

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(const std::string &name, int node_index, double default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 double res=atof(GetNodeText());

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(int node_index, double default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 double res=atof(GetNodeText());

 SelectUp();
 return res;
}

bool USerStorageXML::ReadBool(const std::string &name, bool default_value)
{
 if(!SelectNode(name))
  return default_value;

 int res=atoi(GetNodeText());

 SelectUp();
 return res;
}

bool USerStorageXML::ReadBool(const std::string &name, int node_index, bool default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 int res=atoi(GetNodeText());

 SelectUp();
 return res;
}

bool USerStorageXML::ReadBool(int node_index, bool default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 int res=atoi(GetNodeText());

 SelectUp();
 return res;
}

// ���������� ������ ��� ��������������� ���
void USerStorageXML::WriteString(const std::string &name, const std::string &value)
{
 if(!SelectNode(name))
  AddNode(name);

 SetNodeText(value);

 SelectUp();
}

void USerStorageXML::WriteInteger(const std::string &name, int value)
{
 if(!SelectNode(name))
  AddNode(name);

 SetNodeText(sntoa(value));

 SelectUp();
}

void USerStorageXML::WriteFloat(const std::string &name, double value)
{
 if(!SelectNode(name))
  AddNode(name);

 SetNodeText(sntoa(value));

 SelectUp();
}

void USerStorageXML::WriteBool(const std::string &name, bool value)
{
 if(!SelectNode(name))
  AddNode(name);

 SetNodeText(sntoa(value));

 SelectUp();
}
// --------------------------



// --------------------------
// ������� ������ ���������� �������
// --------------------------
// --------------------------
}
#endif

