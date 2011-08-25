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

// ћетоды
// --------------------------
//  онструкторы и деструкторы
// --------------------------
// —оздает пустую очередь
USerStorageXML::USerStorageXML(void)
{
}

USerStorageXML::~USerStorageXML(void)
{
}
// --------------------------

// --------------------------
// ћетоды загрузки xml
// --------------------------
// —оздает новый xml в корневом узле уничтожа€ старые данные
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

// «агружает xml из строки
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

// —охран€ет xml в строку
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
// ћетоды управлени€ позицией текущего узла
// --------------------------
// ѕозиционируетс€ на корневой узел
void USerStorageXML::SelectRoot(void)
{
 CurrentNode=RootNode;
}

// ѕозиционируетс€ на родительский узел
void USerStorageXML::SelectUp(void)
{
 XMLNode node=CurrentNode.getParentNode();
 if(node.isEmpty())
  return;
 CurrentNode=node;
}

// ¬озвращает число узлов с заданным именем
int USerStorageXML::GetNumNodes(const std::string &name)
{
 return CurrentNode.nChildNode(name.c_str());
}

// ¬озвращает число узлов
int USerStorageXML::GetNumNodes(void)
{
 return CurrentNode.nChildNode();
}

// ≈сли узел с таким именем и индексом существует в текущем узле то позиционируемс€ на него.
// index указвает на номер повтор€ющегос€ элемента
// ≈сли с таким именем и индексом не существует, но существует узел с таким
// именем и индексом меньшим на 1 - то создаем узел
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

// ¬озвращает им€ узла
const std::string USerStorageXML::GetNodeName(void) const
{
 std::string str;
 if(CurrentNode.getName())
  str=CurrentNode.getName();

 return str;
}

// —оздает новый узел с заданным именем и позиционируетс€ на него
bool USerStorageXML::AddNode(const std::string &name)
{
 XMLNode node=CurrentNode.addChild(name.c_str());
 if(node.isEmpty())
  return false;
 CurrentNode=node;
 return true;
}

// ѕереименовывает текущий узел и продолжает указывать на него
void USerStorageXML::RenameNode(const std::string &newname)
{
 CurrentNode.updateName(newname.c_str());
}

// ”дал€ет текущий узел и устанавливает указатель уровнем выше
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
// ћетоды управлени€ данными текущего элемента
// --------------------------
// ”станавливает значение атрибута узла
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

// ¬озвращает значение атрибута узла
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

// ”дал€ет заданный атрибут
void USerStorageXML::DelNodeAttribute(const std::string &name)
{
 CurrentNode.deleteAttribute(name.c_str());
}

// ”станавливает текст узла
bool USerStorageXML::SetNodeText(const std::string &text)
{
 CurrentNode.updateText(text.c_str());
 return true;
}

// ¬озвращает значение узла
const std::string USerStorageXML::GetNodeText(void) const
{
 return CurrentNode.getText();
}
// --------------------------

// --------------------------
// —крытые методы управлени€ данными
// --------------------------
// --------------------------
}
}
#endif

