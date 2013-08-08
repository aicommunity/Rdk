/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UTREE_H
#define UTREE_H


#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

namespace RDK {

using namespace std;

template <class T>
class UTree
{                   
public:
// Дополнительные типы данных
typedef string NameT;
typedef int UIdT;
typedef vector<UIdT> ULongIdT;
typedef vector<UIdT> ULongIdIterator;

protected: // Статические данные
// Разделительный символ в длинных именах узлов дерева
char Separator;

protected: // Данные
// Id узла
UIdT Id;

// имя узла
NameT Name;

// данные узла
T Data;

// Указатель на корень узла
UTree<T> *Root;

// Список ветвей узла
vector<UTree<T>* > SubTree;

// Указатель на текущий подузел
mutable UTree<T> *Current;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UTree(void)
{
 Root=0;
 Current=this;
 Name="Root";
 Separator='.';
};

UTree(const UTree<T> &node)
{
 UTree<T> *newnode;

 Root=0;
 Current=this;
 Name=node.Name;
 Data=node.Data;

 SubTree.resize(node.SubTree.size());

 for(size_t i=0;i<SubTree.size();i++)
  {
   newnode=0;
   newnode=new UTree<T>;
   if(!newnode)
    throw NoFreeMemException(this);

   *newnode=*node.SubTree[i];
   newnode->Root=this;
   SubTree[i]=newnode;
  }

 if(node.Current)
  Current=&(operator [](node.Current->GetFullId(node)));
};

virtual ~UTree(void)
{
 Clear();
};
// --------------------------

// -------------------------
// Методы доступа к статическим данным
// -------------------------
char GetSeparator(void)
{ return Separator; };
// -------------------------

// -------------------------
// Методы управления статическими данными
// -------------------------
void SetSeparator(char separator)
{ Separator=separator; };
// -------------------------

// -------------------------
// Методы доступа к физическим данным узла
// -------------------------
// Возвращает id узла
const UIdT& GetId(void) const
{ return Id; };

// Возвращает имя узла
const string& GetName(void) const
{ return Name; };

// Возвращает указатель на корень узла
UTree<T>* GetRoot(void)
{ return Root; };

// Возвращает число поддеревьев узла
size_t GetSubTreeSize(void) const
{ return SubTree.size(); };

// Возвращает указатель на поддерево с полным именем 'fullname'
UTree<T>* FindSubTree (const string &fullname)
{
 int i;
 size_t j;

 i=fullname.find_first_of(Separator);
 if(i == string::npos)
  {
   for(j=0;j<SubTree.size();j++)
    if(SubTree[j]->Name == fullname)
    {
     Current=SubTree[j];
     return Current;
    }

   Current=this;
   return 0;
  }
 else
  {
   for(j=0;j<SubTree.size();j++)
    if(fullname.compare(0,i,SubTree[j]->Name) == 0)
    {
     Current=&((*SubTree[j])[fullname.substr(i+1,fullname.size()-i-1)]);
     return Current;
    }

   Current=this;
   return 0;
  }
};

// Возвращает указатель на поддерево с полным именем 'fullid'
UTree<T>* FindSubTree (const ULongIdT &fullid)
{
 UTree<T>* comp=0,*comp2=0;

 if(fullid.size() == 0)
  return 0;

 for(size_t i=0;i<SubTree.size();i++)
  if(SubTree[i]->Id == fullid[0])
  {
   comp=SubTree[i];
   break;
  }

 for(size_t i=1;i<fullid.size();i++)
  {
   if(!comp)
    return 0;

   comp2=comp;
   comp=0;

   for(size_t j=0;j<SubTree.size();j++)
    if(comp2->SubTree[j]->Id == fullid[i])
    {
     comp=comp2->SubTree[j];
     break;
    }
  }

 if(!comp)
 {
  Current=this;
  return 0;
 }

 Current=comp;
 return comp;
};
// -------------------------

// -------------------------
// Методы доступа к виртуальным данным узла
// -------------------------
// Возвращает список кратких имен ветвей данного узла
// Содержимое списка-буфера не очищается
void GetSubTrees(vector<NameT> &buffer)
{
 for(size_t i=0;i<SubTree.size();i++)
  buffer.push_back(SubTree[i]->Name);
};

// Возвращает список id ветвей данного узла
// Содержимое списка-буфера не очищается
void GetSubTrees(vector<UIdT> &buffer)
{
 for(size_t i=0;i<SubTree.size();i++)
  buffer.push_back(SubTree[i]->Id);
};

// Возвращает полный id данного узла по
// указателю на корневой узел 'root'
// Если 'root' == 0, то возвращает полное
// имя, начиная от корня всего дерева
// Возвращает пустой id, если root -
// не является корнем данного узла.
// Имя узла 'root' не включается.
ULongIdT GetFullId(UTree<T> *root)
{
 ULongIdT id;

 if(!Root)
  {
   if(!root)
    id.push_back(Id);

   return id;
  }

 if(Root == root)
 {
  id.push_back(Id);
  return id;
 }

 UTree<T> *comp=Root;
 while(comp)
 {
  id.push_back(comp->Id);
  comp=comp->Root;
 }

 reverse(id.begin(),id.end());
 return id;
};

// Возвращает полное имя данного узла по
// указателю на корневой узел 'root'
// Если 'root' == 0, то возвращает полное
// имя, начиная от корня всего дерева
// Возвращает пустую строку, если root -
// не является корнем данного узла.
// Имя узла 'root' не включается.
string GetFullName(UTree<T> *root)
{
 string s;

 if(!Root)
  {
   if(!root)
    return Name;
   else
    return "";
  }

 if(Root == root)
  return Name;

 s=Root->GetFullName(root);
 if(!s.size())
  return "";

 return s+string(1,Separator)+Name;
};
// -------------------------

// -------------------------
// Методы настройки параметров узла
// -------------------------
// Устанавливает id узла
bool SetId(const UIdT& id)
{
 if(!Root)
  {
   Id=id;
   return true;
  }

 for(size_t i=0;i<Root->SubTree.size();i++)
  if(Root->SubTree[i]->Id == id)
   return false;

 Id=id;
 return true;
};

// Устанавливает имя узла
bool SetName(const string& name)
{
 if(name == ""
 || name.find_first_not_of(' ') == string::npos
 || name.find_first_of(Separator) != string::npos)
  return false;

 if(!Root)
  {
   Name=name;
   return true;
  }

 for(size_t i=0;i<Root->SubTree.size();i++)
  if(Root->SubTree[i]->Name == name)
   return false;

 Name=name;
 return true;
};

// Устанавливает указатель на корень узла
void SetRoot(UTree<T> *root)
{ Root=root; };
// -------------------------

// -------------------------
// Методы управления деревом
// -------------------------
// Удаляет все узлы
void Clear(void)
{
 for(size_t i=0;i<SubTree.size();i++)
  delete SubTree[i];
 SubTree.clear();
};

// Создает новый узел с именем node.Name без сортировки по имени.
// Добавления не происходит, если узел с данным именем
// уже существует, или имя добавляемого узла недопустимо.
// После создания в узел копируются данные из узла tree
// Возвращает true в случае успешного создания узла
bool Add(UTree<T> *node)
{
 if(!node->Name.size()
 || node->Name.find_first_not_of(' ') == string::npos
 || node->Name.find_first_of(Separator) != string::npos)
  return false;

 for(size_t i=0;i<SubTree.size();i++)
  if(SubTree[i]->Name == node->Name)
   return false;

 node->Id=SubTree.size()+1;
 SubTree.push_back(node);
 SubTree[SubTree.size()-1]->Root=this;
 return true;
};

// Создает новый узел с именем node.Name с сортировкой по имени.
// Добавления не происходит, если узел с данным именем
// уже существует, или имя добавляемого узла недопустимо.
// После создания в узел копируются данные из узла tree
// Возвращает true в случае успешного создания узла
bool AddSorted(UTree<T> *node)
{
 if(node->Name == ""
 || node->Name.find_first_not_of(' ') == string::npos
 || node->Name.find_first_of(Separator) != string::npos)
  return false;

 size_t j = SubTree.size();

 for(size_t i=0;i<SubTree.size();i++)
  {
   if(SubTree[i]->Name == node->Name)
    return false;

   if(SubTree[i]->Name > node->Name)
    { j=i; break; }
  }

 node->Id=SubTree.size()+1;
 if(j == SubTree.size())
  SubTree.push_back(node);
 else
  SubTree.insert(SubTree.begin()+j,node);

 SubTree[j]->Root=this;
 return true;
}


// Удаление узла с именем 'name' вместе со всем его поддеревом
bool Del(const string &name)
{
 for(size_t i=0;i<SubTree.size();i++)
  if(SubTree[i]->Name == name)
   {
    delete SubTree[i];
    SubTree.erase(SubTree.begin()+i);
    return true;
   }

 return false;
};

// Сортирует ветвь.
void Sort(void)
{
 SubTree.sort();
}

// Создает ветвь дерева по длинному имени 'name'
// Текущий указатель устанавливается на новый узел
// Возвращает указатель на новый узел
UTree<T>* CreateBranch(const string &name)
{
 int i;
 size_t j;

 i=name.find_first_of(Separator);
 if(i == string::npos)
  {
   for(j=0;j<SubTree.size();j++)
    if(SubTree[j]->Name == name)
    {
     Current=SubTree[j];
     return Current;
    }

   UTree<T>* node= new UTree<T>;
   node->Name=name.substr(0,i);
   if(!AddSorted(node))
   {
    delete node;
    return 0;
   }
   else
   {
    Current=node;
    return Current;
   }
  }
 else
  {
   for(j=0;j<SubTree.size();j++)
    if(name.compare(0,i,SubTree[j]->Name) == 0)
    {
     Current=SubTree[j]->CreateBranch(name.substr(i+1));
     return Current;
    }

   UTree<T>* node= new UTree<T>;
   node->Name=name.substr(0,i);
   if(!AddSorted(node))
   {
    delete node;
    return 0;
   }
   else
   {
    Current=node->CreateBranch(name.substr(i+1));
    return Current;
   }
  }
}
// -------------------------

// ---------------------------------------
// Перегруженные операторы для узла дерева
// ---------------------------------------
bool operator == (const UTree<T> &node) const
{ return (Name == node.Name); };

bool operator != (const UTree<T> &node) const
{ return !(*this == node); };

bool operator < (const UTree<T> &node) const
{ return (Name<node.Name)?true:false; };

bool operator > (const UTree<T> &node) const
{ return (Name>node.Name)?true:false; };

// Оператор присваивания
UTree<T>& operator = (const UTree<T> &node)
{
 Data=node.Data;
 Name=node.Name;
 Current=this;

 Clear();

 UTree<T> *newnode;

 SubTree.resize(node.SubTree.size());

 for(size_t i=0;i<SubTree.size();i++)
  {
   newnode=0;
   newnode=new UTree<T>;
   if(!newnode)
    throw NoFreeMemException(this);

   *newnode=*node.SubTree[i];
   newnode->Root=this;
   SubTree[i]=newnode;
  }

 if(node.Current)
  Current=&(operator [](node.Current->GetFullId(node)));

 return *this;
};

// Возвращает ссылку на поддерево с полным именем 'fullname'
// Генерирует исключение UTreeExNoNode, если узел не найден.
UTree<T>& operator [] (const string &fullname)
{
 if(!FindSubTree(fullname))
  throw NoNodeException(this, fullname);

 return *Current;
};

// Возвращает ссылку на поддерево с полным именем 'fullname'
// Генерирует исключение UTreeExNoNode, если узел не найден.
UTree<T>& operator [] (const ULongIdT &fullid)
{
 if(!FindSubTree(fullid))
  throw NoNodeException(this, fullid);

 return *Current;
};

// Возвращает ссылку на элемент поддерева текущего узла
// по индексу 'index'.
// Не проверяет корректность индекса, в случае
// некорректного значения результат не определен
UTree<T>& operator [] (int index)
{
 Current=SubTree[index];
 return *Current;
};

const UTree<T>& operator [] (int index) const
{
 Current=SubTree[index];
 return *Current;
};

// Возвращает ссылку на данные элемента поддерева текущего узла
// по индексу 'index'.
// Не проверяет корректность индекса, в случае
// некорректного значения результат не определен
T& operator () (int index)
{
 Current=SubTree[index];
 return Current->Data;
};

// Возвращает ссылку на данные поддерева с полным id 'fullid'
// Генерирует исключение UTreeExNoNode, если узел не найден.
T& operator () (const ULongIdT &fullid)
{
 return operator [] (fullid).Data;
};

// Возвращает ссылку на данные поддерева с полным именем 'fullname'
// Генерирует исключение UTreeExNoNode, если узел не найден.
T& operator () (const string &fullname)
{
 return operator [] (fullname).Data;
};

// Возвращает ссылку на данные этого узла
T& operator ~ (void)
{ return Data; };
const T& operator ~ (void) const
{ return Data; };

// Возвращает ссылку на данные текущего узла
// Если текущий узел не задан, возвращает данные этого узла
T& operator () (void)
{ return (Current)?Current->Data:Data; };
// ---------------------------------------

// ---------------------------------------
// Исключения
// ---------------------------------------
// Базовый класс исключений
class Exception
{
public: // Параметры исключения
// Указатель на источник исключения
UTree<T> *Owner;

public: // Методы
Exception(UTree<T> *owner)
 { Owner=owner; };

~Exception(void) {};
};

// Исключение, формируемое при невозможности выделения памяти
class NoFreeMemException: public Exception
{
public: // Параметры исключения

public: // Методы
NoFreeMemException(UTree<T> *owner)
 : Exception(owner) { };

~NoFreeMemException(void) {};
};

// Исключение, формируемое при отсутствии запрашиваемого узла
class NoNodeException: public Exception
{
public: // Параметры исключения
// Короткое имя отсутствующего узла
string NodeName;

// Короткий id отсутствующего узла
UIdT NodeId;

public: // Методы
NoNodeException(UTree<T> *owner, const string &name)
 : Exception(owner)
{ NodeName=name; };

NoNodeException(UTree<T> *owner, const UIdT &id)
 : Exception(owner)
{ NodeId=id; };

~NoNodeException(void) {};
};
// ---------------------------------------
};

} // end namespace Utility
#endif


