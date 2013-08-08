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
// �������������� ���� ������
typedef string NameT;
typedef int UIdT;
typedef vector<UIdT> ULongIdT;
typedef vector<UIdT> ULongIdIterator;

protected: // ����������� ������
// �������������� ������ � ������� ������ ����� ������
char Separator;

protected: // ������
// Id ����
UIdT Id;

// ��� ����
NameT Name;

// ������ ����
T Data;

// ��������� �� ������ ����
UTree<T> *Root;

// ������ ������ ����
vector<UTree<T>* > SubTree;

// ��������� �� ������� �������
mutable UTree<T> *Current;

public: // ������
// --------------------------
// ������������ � �����������
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
// ������ ������� � ����������� ������
// -------------------------
char GetSeparator(void)
{ return Separator; };
// -------------------------

// -------------------------
// ������ ���������� ������������ �������
// -------------------------
void SetSeparator(char separator)
{ Separator=separator; };
// -------------------------

// -------------------------
// ������ ������� � ���������� ������ ����
// -------------------------
// ���������� id ����
const UIdT& GetId(void) const
{ return Id; };

// ���������� ��� ����
const string& GetName(void) const
{ return Name; };

// ���������� ��������� �� ������ ����
UTree<T>* GetRoot(void)
{ return Root; };

// ���������� ����� ����������� ����
size_t GetSubTreeSize(void) const
{ return SubTree.size(); };

// ���������� ��������� �� ��������� � ������ ������ 'fullname'
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

// ���������� ��������� �� ��������� � ������ ������ 'fullid'
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
// ������ ������� � ����������� ������ ����
// -------------------------
// ���������� ������ ������� ���� ������ ������� ����
// ���������� ������-������ �� ���������
void GetSubTrees(vector<NameT> &buffer)
{
 for(size_t i=0;i<SubTree.size();i++)
  buffer.push_back(SubTree[i]->Name);
};

// ���������� ������ id ������ ������� ����
// ���������� ������-������ �� ���������
void GetSubTrees(vector<UIdT> &buffer)
{
 for(size_t i=0;i<SubTree.size();i++)
  buffer.push_back(SubTree[i]->Id);
};

// ���������� ������ id ������� ���� ��
// ��������� �� �������� ���� 'root'
// ���� 'root' == 0, �� ���������� ������
// ���, ������� �� ����� ����� ������
// ���������� ������ id, ���� root -
// �� �������� ������ ������� ����.
// ��� ���� 'root' �� ����������.
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

// ���������� ������ ��� ������� ���� ��
// ��������� �� �������� ���� 'root'
// ���� 'root' == 0, �� ���������� ������
// ���, ������� �� ����� ����� ������
// ���������� ������ ������, ���� root -
// �� �������� ������ ������� ����.
// ��� ���� 'root' �� ����������.
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
// ������ ��������� ���������� ����
// -------------------------
// ������������� id ����
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

// ������������� ��� ����
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

// ������������� ��������� �� ������ ����
void SetRoot(UTree<T> *root)
{ Root=root; };
// -------------------------

// -------------------------
// ������ ���������� �������
// -------------------------
// ������� ��� ����
void Clear(void)
{
 for(size_t i=0;i<SubTree.size();i++)
  delete SubTree[i];
 SubTree.clear();
};

// ������� ����� ���� � ������ node.Name ��� ���������� �� �����.
// ���������� �� ����������, ���� ���� � ������ ������
// ��� ����������, ��� ��� ������������ ���� �����������.
// ����� �������� � ���� ���������� ������ �� ���� tree
// ���������� true � ������ ��������� �������� ����
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

// ������� ����� ���� � ������ node.Name � ����������� �� �����.
// ���������� �� ����������, ���� ���� � ������ ������
// ��� ����������, ��� ��� ������������ ���� �����������.
// ����� �������� � ���� ���������� ������ �� ���� tree
// ���������� true � ������ ��������� �������� ����
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


// �������� ���� � ������ 'name' ������ �� ���� ��� ����������
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

// ��������� �����.
void Sort(void)
{
 SubTree.sort();
}

// ������� ����� ������ �� �������� ����� 'name'
// ������� ��������� ��������������� �� ����� ����
// ���������� ��������� �� ����� ����
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
// ������������� ��������� ��� ���� ������
// ---------------------------------------
bool operator == (const UTree<T> &node) const
{ return (Name == node.Name); };

bool operator != (const UTree<T> &node) const
{ return !(*this == node); };

bool operator < (const UTree<T> &node) const
{ return (Name<node.Name)?true:false; };

bool operator > (const UTree<T> &node) const
{ return (Name>node.Name)?true:false; };

// �������� ������������
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

// ���������� ������ �� ��������� � ������ ������ 'fullname'
// ���������� ���������� UTreeExNoNode, ���� ���� �� ������.
UTree<T>& operator [] (const string &fullname)
{
 if(!FindSubTree(fullname))
  throw NoNodeException(this, fullname);

 return *Current;
};

// ���������� ������ �� ��������� � ������ ������ 'fullname'
// ���������� ���������� UTreeExNoNode, ���� ���� �� ������.
UTree<T>& operator [] (const ULongIdT &fullid)
{
 if(!FindSubTree(fullid))
  throw NoNodeException(this, fullid);

 return *Current;
};

// ���������� ������ �� ������� ��������� �������� ����
// �� ������� 'index'.
// �� ��������� ������������ �������, � ������
// ������������� �������� ��������� �� ���������
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

// ���������� ������ �� ������ �������� ��������� �������� ����
// �� ������� 'index'.
// �� ��������� ������������ �������, � ������
// ������������� �������� ��������� �� ���������
T& operator () (int index)
{
 Current=SubTree[index];
 return Current->Data;
};

// ���������� ������ �� ������ ��������� � ������ id 'fullid'
// ���������� ���������� UTreeExNoNode, ���� ���� �� ������.
T& operator () (const ULongIdT &fullid)
{
 return operator [] (fullid).Data;
};

// ���������� ������ �� ������ ��������� � ������ ������ 'fullname'
// ���������� ���������� UTreeExNoNode, ���� ���� �� ������.
T& operator () (const string &fullname)
{
 return operator [] (fullname).Data;
};

// ���������� ������ �� ������ ����� ����
T& operator ~ (void)
{ return Data; };
const T& operator ~ (void) const
{ return Data; };

// ���������� ������ �� ������ �������� ����
// ���� ������� ���� �� �����, ���������� ������ ����� ����
T& operator () (void)
{ return (Current)?Current->Data:Data; };
// ---------------------------------------

// ---------------------------------------
// ����������
// ---------------------------------------
// ������� ����� ����������
class Exception
{
public: // ��������� ����������
// ��������� �� �������� ����������
UTree<T> *Owner;

public: // ������
Exception(UTree<T> *owner)
 { Owner=owner; };

~Exception(void) {};
};

// ����������, ����������� ��� ������������� ��������� ������
class NoFreeMemException: public Exception
{
public: // ��������� ����������

public: // ������
NoFreeMemException(UTree<T> *owner)
 : Exception(owner) { };

~NoFreeMemException(void) {};
};

// ����������, ����������� ��� ���������� �������������� ����
class NoNodeException: public Exception
{
public: // ��������� ����������
// �������� ��� �������������� ����
string NodeName;

// �������� id �������������� ����
UIdT NodeId;

public: // ������
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


