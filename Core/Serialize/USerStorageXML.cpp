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
#include "USerStorageXML.h"

namespace RDK {
namespace Serialize {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
USerStorageXML::USerStorageXML(void)
{
}

USerStorageXML::~USerStorageXML(void)
{
}
// --------------------------

// --------------------------
// Методы загрузки xml
// --------------------------
// Создает новый xml в корневом узле уничтожая старые данные
bool USerStorageXML::Create(const std::string &rootname)
{
 RootNode.deleteNodeContent();
 RootNode=XMLNode::createXMLTopNode(widen2(rootname).c_str());
 if(RootNode.isEmpty())
  return false;
 CurrentNode=RootNode;
 return true;
}

// Уничтожает xml
bool USerStorageXML::Destroy(void)
{
 RootNode.deleteNodeContent();
 CurrentNode=RootNode;
 return true;
}

// Загружает xml из строки
bool USerStorageXML::Load(const std::string &str, const std::string &root)
{
 RootNode.deleteNodeContent();
 RootNode=XMLNode::parseString(widen2(str).c_str(),widen2(root).c_str());
 if(RootNode.isEmpty())
  return false;

 CurrentNode=RootNode;
 return true;
}

bool USerStorageXML::LoadToNode(const std::string &str, const std::string &root)
{
 CurrentNode.deleteNodeContent();
 CurrentNode=XMLNode::parseString(widen2(str).c_str(),widen2(root).c_str());
 if(CurrentNode.isEmpty())
  return false;

 return true;
}


// Сохраняет xml в строку
bool USerStorageXML::Save(std::string &str)
{
 wchar_t* pch=RootNode.createXMLString(true);
 if(pch)
 {
  str=narrow2(pch);
  freeXMLString(pch);
 }
 else
  str="";
 return true;
}

bool USerStorageXML::SaveFromNode(std::string &str)
{
 wchar_t* pch=CurrentNode.createXMLString(true);
 if(pch)
 {
  str=narrow2(pch);
  freeXMLString(pch);
 }
 else
  str="";
 return true;
}

// Прочесть файл с диска
// Метод не делает ничего, если FileName == ""
bool USerStorageXML::LoadFromFile(const std::string &file_name, const std::string &root)
{
 fstream file(file_name.c_str(), ios::in);
 char buffer[4096];

 string result;

 if(!file)
  return false;

 while(!file.eof())
  {
   file.getline(buffer,4096);
   result+=buffer;
  }

 file.close();
 Load(result,root);
 return true;
};

// Обновить файл на диске
// Метод не делает ничего, если FileName == ""
bool USerStorageXML::SaveToFile(const std::string &file_name)
{
 fstream file(file_name.c_str(), ios::out | ios::trunc);

 if(!file)
  return false;

 std::string result;
 Save(result);
 file.write(result.c_str(),result.size());

 file.close();
 return true;
};
// --------------------------

// --------------------------
// Методы управления позицией текущего узла
// --------------------------
// Позиционируется на корневой узел
void USerStorageXML::SelectRoot(void)
{
 CurrentNode=RootNode;
}

// Позиционируется на родительский узел
void USerStorageXML::SelectUp(void)
{
 XMLNode node=CurrentNode.getParentNode();
 if(node.isEmpty())
  return;
 CurrentNode=node;
}

// Возвращает число узлов с заданным именем
int USerStorageXML::GetNumNodes(const std::string &name)
{
 return CurrentNode.nChildNode(widen2(name).c_str());
}

// Возвращает число узлов
int USerStorageXML::GetNumNodes(void)
{
 return CurrentNode.nChildNode();
}

// Если узел с таким именем и индексом существует в текущем узле то позиционируемся на него.
// index указвает на номер повторяющегося элемента
// Если с таким именем и индексом не существует, но существует узел с таким
// именем и индексом меньшим на 1 - то создаем узел
bool USerStorageXML::SelectNode(const std::string &name, int index)
{
 XMLNode node=CurrentNode.getChildNode(widen2(name).c_str(),index);
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

// Если узел с таким именем существует в текущем узле то позиционируемся на него.
// иначе создаем такой узел
// Всегда позиционируется на 0 узел!
bool USerStorageXML::SelectNodeForce(const std::string &name)
{
 if(!SelectNode(name))
  return AddNode(name);

 return true;
}

// Аналогично SelectNodeForce, но позиционируется всегда от корневого узла,
// и поддерживает составное именование узла, с разделителем в виде '/'
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

// Возвращает имя узла
const std::string USerStorageXML::GetNodeName(void) const
{
 std::string str;
 if(CurrentNode.getName())
  str=narrow2(CurrentNode.getName());

 return str;
}

// Создает новый узел с заданным именем и позиционируется на него
bool USerStorageXML::AddNode(const std::string &name)
{
 XMLNode node=CurrentNode.addChild(widen2(name).c_str());
 if(node.isEmpty())
  return false;
 CurrentNode=node;
 return true;
}

// Переименовывает текущий узел и продолжает указывать на него
void USerStorageXML::RenameNode(const std::string &newname)
{
 CurrentNode.updateName(widen2(newname).c_str());
}

// Удаляет текущий узел и устанавливает указатель уровнем выше
void USerStorageXML::DelNode(void)
{
 XMLNode tmp=CurrentNode.getParentNode();
 if(tmp.isEmpty())
  return;
 CurrentNode.deleteNodeContent();
 CurrentNode=tmp;
}

// Удаляет содержимое узла
void USerStorageXML::DelNodeContent(void)
{
 CurrentNode.deleteNodeContent();
}

// Удаляет содержимое узла сохраняя сам узел
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
// Методы управления данными текущего элемента
// --------------------------
// Устанавливает значение атрибута узла
bool USerStorageXML::SetNodeAttribute(const std::string &name, const std::string &value)
{
 if(!CurrentNode.isAttributeSet(widen2(name).c_str()))
 {
  CurrentNode.addAttribute(widen2(name).c_str(),widen2(value).c_str());
 }
 else
 {
  CurrentNode.updateAttribute(widen2(value).c_str(),0,widen2(name).c_str());
 }
 return true;
}

// Возвращает значение атрибута узла
const std::string USerStorageXML::GetNodeAttribute(const std::string &name) const
{
 std::string str;
 const wchar_t *p=CurrentNode.getAttribute(widen2(name).c_str(),0);

 if(p)
 {
  str=narrow2(p);
 }
 return str;
}

// Удаляет заданный атрибут
void USerStorageXML::DelNodeAttribute(const std::string &name)
{
 CurrentNode.deleteAttribute(widen2(name).c_str());
}

// Устанавливает текст узла
bool USerStorageXML::SetNodeText(const std::string &text)
{
 CurrentNode.updateText(widen2(text).c_str());
 return true;
}

// Возвращает значение узла
const std::string USerStorageXML::GetNodeText(void) const
{
 if(!CurrentNode.getText())
  return string();

 return narrow2(CurrentNode.getText());
}
// --------------------------

// --------------------------
// Дополнительные методы управления данными текущего элемента как ini-файлом
// --------------------------
// Считывает данные как соответствующий тип, если данное не найдено или не приводимо в
// ожидаемый тип - оно инициализируется значением по умолчанию
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

 double res=atoi(GetNodeText());

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(const std::string &name, int node_index, double default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 double res=atoi(GetNodeText());

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(int node_index, double default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 double res=atoi(GetNodeText());

 SelectUp();
 return res;
}

bool USerStorageXML::ReadBool(const std::string &name, bool default_value)
{
 if(!SelectNode(name))
  return default_value;

 bool res=atoi(GetNodeText());

 SelectUp();
 return res;
}

bool USerStorageXML::ReadBool(const std::string &name, int node_index, bool default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 bool res=atoi(GetNodeText());

 SelectUp();
 return res;
}

bool USerStorageXML::ReadBool(int node_index, bool default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 bool res=atoi(GetNodeText());

 SelectUp();
 return res;
}

// Записывает данные как соответствующий тип
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
// Скрытые методы управления данными
// --------------------------
// --------------------------
}
}
#endif

