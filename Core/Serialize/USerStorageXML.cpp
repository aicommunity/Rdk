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
#include <locale>
#include <clocale>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "USerStorageXML.h"
#include "../Utilities/USupport.h"
#include "../System/rdk_system.h"

namespace RDK {

namespace {

using XmlText = std::basic_string<XMLCHAR>;

bool SameXmlText(XMLCSTR first, XMLCSTR second)
{
 if(!first || !second) return first == second;
 return XmlText(first) == XmlText(second);
}

std::string AsciiXmlText(XMLCSTR value)
{
 std::string result;
 if(!value) return result;
 for(; *value; ++value)
 {
  if(*value < 0 || *value > 127) return {};
  result.push_back(static_cast<char>(*value));
 }
 return result;
}

bool SamePortableXmlNode(const XMLNode &first,const XMLNode &second,
 const USerStorageXML::PortableIntegralFields &fields,std::vector<std::string> &path)
{
 if(first.isEmpty() || second.isEmpty()) return false;
 if(!SameXmlText(first.getName(),second.getName()) ||
    first.isDeclaration()!=second.isDeclaration() ||
    first.nAttribute()!=second.nAttribute() || first.nElement()!=second.nElement()) return false;
 std::map<XmlText, XMLCSTR> leftAttributes, rightAttributes;
 const auto attributes = [](const XMLNode &node, std::map<XmlText, XMLCSTR> &result) {
  for(int index=0;index<node.nAttribute();++index)
  {
   const auto attribute=node.getAttribute(index);
   if(!attribute.lpszName || !result.emplace(XmlText(attribute.lpszName),attribute.lpszValue).second) return false;
  }
  return true;
 };
 if(!attributes(first,leftAttributes) || !attributes(second,rightAttributes)) return false;
 for(const auto &attribute:leftAttributes)
 {
  const auto other=rightAttributes.find(attribute.first);
  if(other==rightAttributes.end()) return false;
  const auto name=AsciiXmlText(attribute.first.c_str());
  const bool type=name=="Type" || name=="elemType" || name=="elemVecType" ||
   name=="elemSVType" || name=="firstType" || name=="secondType" || name=="pointType" || name=="elemPointType";
  auto domain=UXmlIntegralDomain::Nominal;
  const auto field=fields.find(path);
  if(name=="Type" && field!=fields.end())domain=field->second;
  if(!(type ? SamePortableXmlType(AsciiXmlText(attribute.second),AsciiXmlText(other->second),domain,
                                 AsciiXmlText(first.getText())) :
              SameXmlText(attribute.second,other->second))) return false;
 }
 // Preserve text/child/clear-field interleaving, including mixed content.
 int rightIndex=0;
 for(int index=0;index<first.nElement();++index)
 {
  const auto left=first.enumContents(index);
  if(left.etype==eNodeAttribute) continue;
  while(rightIndex<second.nElement() && second.enumContents(rightIndex).etype==eNodeAttribute) ++rightIndex;
  if(rightIndex==second.nElement()) return false;
  const auto right=second.enumContents(rightIndex++);
  if(left.etype!=right.etype) return false;
  switch(left.etype)
  {
   case eNodeChild:
    path.push_back(AsciiXmlText(left.child.getName()));
    if(!SamePortableXmlNode(left.child,right.child,fields,path))return false;
    path.pop_back();break;
   case eNodeText: if(!SameXmlText(left.text,right.text)) return false; break;
   case eNodeClear:
    if(!SameXmlText(left.clear.lpszValue,right.clear.lpszValue) ||
       !SameXmlText(left.clear.lpszOpenTag,right.clear.lpszOpenTag) ||
       !SameXmlText(left.clear.lpszCloseTag,right.clear.lpszCloseTag)) return false;
    break;
   default: return false;
  }
 }
 while(rightIndex<second.nElement() && second.enumContents(rightIndex).etype==eNodeAttribute) ++rightIndex;
 return rightIndex==second.nElement();
}

/// Parse float from XML wire format: accept '.' and legacy locale ',' decimals.
double ParseXmlFloat(const char *text, double default_value)
{
 if(!text || !*text)
  return default_value;

 std::string normalized(text);
 for(char &ch : normalized)
 {
  if(ch == ',')
   ch = '.';
 }
 const char *previous = std::setlocale(LC_NUMERIC, nullptr);
 const std::string previousLocale = previous ? previous : "C";
 std::setlocale(LC_NUMERIC, "C");
 char *end = nullptr;
 const double value = std::strtod(normalized.c_str(), &end);
 std::setlocale(LC_NUMERIC, previousLocale.c_str());
 if(end == normalized.c_str())
  return default_value;
 return value;
}

} // namespace

bool USerStorageXML::SamePortableSerialization(const USerStorageXML &other) const
{
 return SamePortableSerialization(other,{});
}

bool USerStorageXML::SamePortableSerialization(const USerStorageXML &other,const PortableIntegralFields &fields) const
{
 std::vector<std::string> path{AsciiXmlText(RootNode.getName())};
 return SamePortableXmlNode(RootNode,other.RootNode,fields,path);
}

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Создает пустую очередь
USerStorageXML::USerStorageXML(void)
#ifdef RDK_UNICODE_RUN
: Locale(std::locale::classic()),
#else
:
#endif
 NodeNameCached(false)
, NodeTextCached(false)
, PathCached(false)
, AttributesCached(false)
{
}

USerStorageXML::~USerStorageXML(void)
{
 Destroy();
}

void USerStorageXML::InvalidateNodeCache(void)
{
 NodeNameCached = false;
 NodeTextCached = false;
 AttributesCached = false;
 CachedNodeName.clear();
 CachedNodeText.clear();
 CachedAttributes.clear();
 CachedNode = XMLNode();
 CachedAttributesNode = XMLNode();
}
// --------------------------

// --------------------------
// Методы загрузки xml
// --------------------------
// Создает новый xml в корневом узле уничтожая старые данные
bool USerStorageXML::Create(const std::string &rootname)
{
 InvalidateNodeCache();
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

// Уничтожает xml
bool USerStorageXML::Destroy(void)
{
 InvalidateNodeCache();
 RootNode.deleteNodeContent();
 CurrentNode.deleteNodeContent();
// CurrentNode=RootNode;
 return true;
}

// Загружает xml из строки
bool USerStorageXML::Load(const std::string &str, const std::string &root)
{
 InvalidateNodeCache();
 CurrentNode.deleteNodeContent();
 RootNode.deleteNodeContent();
 // XMLNode::parseString does not reliably recognize a declaration after a
 // UTF-8 BOM (unlike its file/encoding helpers). Treat the BOM as an encoding
 // signature, never as document text. Leave ordinary input allocation-free.
 std::string withoutBom;
 const std::string *xml=&str;
 if(str.compare(0,3,"\xef\xbb\xbf")==0)
 {
  withoutBom=str.substr(3);
  xml=&withoutBom;
 }
#ifdef RDK_UNICODE_RUN
 RootNode=XMLNode::parseString(widen(*xml,Locale,WBuffer).c_str(),widen(root,Locale,WBuffer2).c_str());
#else
 RootNode=XMLNode::parseString(xml->c_str(),root.c_str());
#endif
 if(RootNode.isEmpty())
  return false;

 CurrentNode=RootNode;
 return true;
}

bool USerStorageXML::LoadToNode(const std::string &str, const std::string &root, bool node_clear)
{
 InvalidateNodeCache();
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
 InvalidateNodeCache();
 if(node_clear)
  DelNodeInternalContent();
 CurrentNode.addChild(node.RootNode.deepCopy());
 return true;
}

// Move-версия для оптимизации (использует тот же код, так как XMLNode уже легковесный)
bool USerStorageXML::LoadToNode(USerStorageXML &&node, bool node_clear)
{
 return LoadToNode(node, node_clear); // Вызываем обычную версию
}

bool USerStorageXML::LoadFieldsToNode(USerStorageXML &node, bool node_clear)
{
 InvalidateNodeCache();
 if(node_clear)
  DelNodeInternalContent();
 
 // Оптимизация: сохраняем текущий узел node, чтобы не терять позицию
 XMLNode savedNode = node.CurrentNode;
 node.SelectRoot();
 
 int numNodes = node.GetNumNodes();
 for(int i=0;i<numNodes;i++)
 {
  XMLNode child = node.CurrentNode.getChildNode(i);
  if(!child.isEmpty())
   CurrentNode.addChild(child.deepCopy());
 }
 
 // Восстанавливаем позицию в исходном узле
 node.CurrentNode = savedNode;
 node.InvalidateNodeCache();
 
 // Инвалидируем кэш при изменении узла
 InvalidateNodeCache();
 return true;
}

// Move-версия для оптимизации
bool USerStorageXML::LoadFieldsToNode(USerStorageXML &&node, bool node_clear)
{
 return LoadFieldsToNode(node, node_clear); // Вызываем обычную версию
}

// Сохраняет xml в строку
bool USerStorageXML::Save(std::string &str) const
{
 return Save(str, true); // По умолчанию с форматированием для обратной совместимости
}

bool USerStorageXML::Save(std::string &str, bool formatted) const
{
#ifdef RDK_UNICODE_RUN
 wchar_t* pch=RootNode.createXMLString(formatted ? 1 : 0);
 if(pch)
 {
  narrow(pch,Locale,str);
  freeXMLString(pch);
 }
 else
  str="";
#else
 char* pch=RootNode.createXMLString(formatted ? 1 : 0);
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
 return SaveFromNode(str, true); // По умолчанию с форматированием для обратной совместимости
}

bool USerStorageXML::SaveFromNode(std::string &str, bool formatted)
{
#ifdef RDK_UNICODE_RUN
 wchar_t* pch=CurrentNode.createXMLString(formatted ? 1 : 0);
 if(pch)
 {
  narrow(pch,Locale,str);
  freeXMLString(pch);
 }
 else
  str="";
#else
 char* pch=CurrentNode.createXMLString(formatted ? 1 : 0);
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

// Прочесть файл с диска
// Метод не делает ничего, если FileName == ""
bool USerStorageXML::LoadFromFile(const std::string &file_name, const std::string &root)
{
 string result;
 bool res=RDK::LoadFile(file_name,result);
 if(!res)
  return false;

 Load(result,root);
 return true;
};

// Обновить файл на диске
// Метод не делает ничего, если FileName == ""
bool USerStorageXML::SaveToFile(const std::string &file_name)
{
 std::string result;
 Save(result);
 return SaveFile(file_name,result);
};
// --------------------------

// --------------------------
// Методы управления позицией текущего узла
// --------------------------
// Позиционируется на корневой узел
void USerStorageXML::SelectRoot(void)
{
 InvalidateNodeCache();
 CurrentNode=RootNode;
}

// Позиционируется на родительский узел
void USerStorageXML::SelectUp(void)
{
 XMLNode node=CurrentNode.getParentNode();
 if(node.isEmpty())
  return;
 // Всегда переходим на родителя. Сравнение getName() по указателям (особенно
 // при RDK_UNICODE_RUN) ложно оставляло CurrentNode на месте — следующие
 // SelectNodeForce писали не в Interfaces, а DelNodeInternalContent мог
 // снести уже сохранённых соседей. В Interface.xml оставался один хвост.
 InvalidateNodeCache();
 CurrentNode=node;
}

// Возвращает число узлов с заданным именем
int USerStorageXML::GetNumNodes(const std::string &name)
{
#ifdef RDK_UNICODE_RUN
 return CurrentNode.nChildNode(widen(name,Locale,WBuffer).c_str());
#else
 return CurrentNode.nChildNode(name.c_str());
#endif
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
#ifdef RDK_UNICODE_RUN
 XMLNode node=CurrentNode.getChildNode(widen(name,Locale,WBuffer).c_str(),index);
#else
 XMLNode node=CurrentNode.getChildNode(name.c_str(),index);
#endif
 if(node.isEmpty())
  return false;
 InvalidateNodeCache();
 CurrentNode=node;
 return true;
}

bool USerStorageXML::SelectNode(int index)
{
 XMLNode node=CurrentNode.getChildNode(index);
 if(node.isEmpty())
  return false;
 InvalidateNodeCache();
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
 // Оптимизация: используем кэш для разбиения пути
 std::vector<std::string> nodes;
 if(PathCached && CachedPath == name)
 {
  nodes = CachedPathNodes;
 }
 else
 {
  separatestring(name,nodes, '/');
  CachedPath = name;
  CachedPathNodes = nodes;
  PathCached = true;
 }

 SelectRoot();
 if(nodes.empty())
  return false;

 // Оптимизация: проверяем имя корневого узла без повторного вызова GetNodeName
 std::string rootName;
 if(!RootNode.isEmpty() && RootNode.getName())
 {
#ifdef RDK_UNICODE_RUN
  narrow(RootNode.getName(),Locale,rootName);
#else
  rootName = RootNode.getName();
#endif
 }

 if(rootName != nodes[0])
 {
  Create(nodes[0]);
  // Инвалидируем кэш пути при изменении корня
  PathCached = false;
 }
 for(size_t i=1;i<nodes.size();i++)
 {
  if(!SelectNode(nodes[i]))
   if(!AddNode(nodes[i]))
    return false;
 }
 return true;
}

// Оптимизированный batch-метод для множественного выбора узлов
bool USerStorageXML::SelectNodeBatch(const std::vector<std::string> &path)
{
 if(path.empty())
  return false;

 for(size_t i=0;i<path.size();i++)
 {
  if(!SelectNode(path[i]))
   return false;
 }
 return true;
}

// Возвращает имя узла
const std::string USerStorageXML::GetNodeName(void) const
{
 if(NodeNameCached)
  return CachedNodeName;

 std::string str;
 if(CurrentNode.isEmpty())
 {
  NodeNameCached = false;
  return str;
 }

 if(CurrentNode.getName())
#ifdef RDK_UNICODE_RUN
  narrow(CurrentNode.getName(),Locale,str);
#else
  str=CurrentNode.getName();
#endif

 // Обновляем кэш
 CachedNodeName = str;
 CachedNode = CurrentNode;
 NodeNameCached = true;

 return str;
}

// Создает новый узел с заданным именем и позиционируется на него
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
 InvalidateNodeCache();
 CurrentNode=node;
 return true;
}

// Переименовывает текущий узел и продолжает указывать на него
void USerStorageXML::RenameNode(const std::string &newname)
{
 if(CurrentNode.isEmpty())
  return;
 InvalidateNodeCache();

#ifdef RDK_UNICODE_RUN
 CurrentNode.updateName(widen(newname,Locale,WBuffer).c_str());
#else
 CurrentNode.updateName(newname.c_str());
#endif
}

// Удаляет текущий узел и устанавливает указатель уровнем выше
void USerStorageXML::DelNode(void)
{
 if(CurrentNode.isEmpty())
  return;

 XMLNode tmp=CurrentNode.getParentNode();
 if(tmp.isEmpty())
  return;
 InvalidateNodeCache();
 CurrentNode.deleteNodeContent();
 CurrentNode=tmp;
}

// Удаляет содержимое узла
void USerStorageXML::DelNodeContent(void)
{
 InvalidateNodeCache();
 CurrentNode.deleteNodeContent();
}

// Удаляет содержимое узла сохраняя сам узел
void USerStorageXML::DelNodeInternalContent(void)
{
 InvalidateNodeCache();
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
 // Кэш чтения может содержать лишь часть атрибутов, включая отсутствующие.
 // Наличие атрибута определяет XML, а не результат прежнего чтения.
 InvalidateNodeCache();
#ifdef RDK_UNICODE_RUN
 const bool attributeExists = CurrentNode.isAttributeSet(widen(name,Locale,WBuffer).c_str());

 if(!attributeExists)
 {
  CurrentNode.addAttribute(widen(name,Locale,WBuffer).c_str(),widen(value,Locale,WBuffer2).c_str());
 }
 else
 {
  CurrentNode.updateAttribute(widen(value,Locale,WBuffer).c_str(),0,widen(name,Locale,WBuffer2).c_str());
 }
#else
 const bool attributeExists = CurrentNode.isAttributeSet(name.c_str());

 if(!attributeExists)
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

// Возвращает значение атрибута узла
const std::string USerStorageXML::GetNodeAttribute(const std::string &name) const
{
 // Оптимизация: используем кэш атрибутов
 if(AttributesCached)
 {
  auto it = CachedAttributes.find(name);
  if(it != CachedAttributes.end())
   return it->second;
 }

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

 // Обновляем кэш
 if(AttributesCached)
 {
  CachedAttributes[name] = str;
 }
 else
 {
  CachedAttributes.clear();
  CachedAttributes[name] = str;
  CachedAttributesNode = CurrentNode;
  AttributesCached = true;
 }

 return str;
}

// Удаляет заданный атрибут
void USerStorageXML::DelNodeAttribute(const std::string &name)
{
 InvalidateNodeCache();
#ifdef RDK_UNICODE_RUN
 CurrentNode.deleteAttribute(widen(name,Locale,WBuffer).c_str());
#else
 CurrentNode.deleteAttribute(name.c_str());
#endif
}

// Устанавливает текст узла
bool USerStorageXML::SetNodeText(const std::string &text)
{
 InvalidateNodeCache();
#ifdef RDK_UNICODE_RUN
 CurrentNode.updateText(widen(text,Locale,WBuffer).c_str());
#else
 CurrentNode.updateText(text.c_str());
#endif
 return true;
}

// Возвращает значение узла
const std::string USerStorageXML::GetNodeText(void) const
{
 if(NodeTextCached)
  return CachedNodeText;

 std::string str;
 if(!CurrentNode.getText())
 {
  NodeTextCached = false;
  return str;
 }

#ifdef RDK_UNICODE_RUN
 str = narrow(CurrentNode.getText(),Locale,SBuffer);
#else
 str = CurrentNode.getText();
#endif

 // Обновляем кэш
 CachedNodeText = str;
 CachedNode = CurrentNode;
 NodeTextCached = true;

 return str;
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

unsigned int USerStorageXML::ReadUnsigned(const std::string &name, unsigned int default_value)
{
 if(!SelectNode(name))
  return default_value;

 unsigned int res=static_cast<unsigned int>(std::strtoull(GetNodeText().c_str(),nullptr,10));

 SelectUp();
 return res;
}

unsigned int USerStorageXML::ReadUnsigned(const std::string &name, int node_index, unsigned int default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 unsigned int res=static_cast<unsigned int>(std::strtoull(GetNodeText().c_str(),nullptr,10));

 SelectUp();
 return res;
}

unsigned int USerStorageXML::ReadUnsigned(int node_index, unsigned int default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 unsigned int res=static_cast<unsigned int>(std::strtoull(GetNodeText().c_str(),nullptr,10));

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(const std::string &name, double default_value)
{
 if(!SelectNode(name))
  return default_value;

 double res=ParseXmlFloat(GetNodeText().c_str(), default_value);

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(const std::string &name, int node_index, double default_value)
{
 if(!SelectNode(name,node_index))
  return default_value;

 double res=ParseXmlFloat(GetNodeText().c_str(), default_value);

 SelectUp();
 return res;
}

double USerStorageXML::ReadFloat(int node_index, double default_value)
{
 if(!SelectNode(node_index))
  return default_value;

 double res=ParseXmlFloat(GetNodeText().c_str(), default_value);

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

void USerStorageXML::WriteUnsigned(const std::string &name, unsigned int value)
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

 // C-locale (dot decimal): sntoa(double) via stringstream inherits global locale.
 SetNodeText(sntoa(value, 17));

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
#endif

