/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEnvSupportH
#define UEnvSupportH

#include <string>
#include <vector>
#include "../Utilities/UException.h"

namespace RDK {

typedef int UId;
extern const UId ForbiddenId;

typedef std::string NameT;
extern const NameT ForbiddenName;

/* *********************************************************************** */
/* *********************************************************************** */
// ������ id ���������
class RDK_LIB_TYPE UIdVector
{
protected: // ���������
// ������ �������
int Size;

protected: // ������
// ������ id
mutable UId* Buffer;

// �������� ������ �������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UIdVector(void);
UIdVector(const UIdVector &copy);
UIdVector(UId id1);
UIdVector(UId id1, UId id2, ...);
virtual ~UIdVector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int GetSize(void) const;
int size(void) const;

// �������� ������ ������� � ����������� ������� ������
void Resize(int newsize);

// ������� ������
void Clear(void);

// ���������� ��������� �� ������ ������
UId* GetBuffer(void);

// ��������� ������� � ����� �������
// ���������� ������ ��������
int Add(UId id);
// --------------------------

// --------------------------
// ������ ����� ������ ���������������
// --------------------------
// !!! ����������
UIdVector& DecodeFromString(const std::string &str);
std::string& EncodeToString(std::string &str) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UIdVector& operator = (const UIdVector &copy);

// �������� �������
UId& operator [] (int index);
const UId& operator [] (int index) const;

// ��������� ���������
bool operator == (const UIdVector &copy) const;
bool operator != (const UIdVector &copy) const;
bool operator < (const UIdVector &copy) const;
// --------------------------

// --------------------------
// ����������
// --------------------------
// ��������� �������� ����������
class EILongId;

// ������ ������������� ���������� id
class EDecodeFail;
// --------------------------
};

typedef UIdVector ULongId;


// --------------------------
// ����������
// --------------------------
// ��������� �������� ����������
class RDK_LIB_TYPE UIdVector::EILongId
{
};

// ������ ������������� ���������� id
class RDK_LIB_TYPE UIdVector::EDecodeFail: public EError, public UIdVector::EILongId
{
public:
// ������ � �������
std::string StringId;

// ������� ������ � ������
int Position;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//EDecodeFail(void);
EDecodeFail(const std::string &stringid, int position);
virtual ~EDecodeFail(void) throw();
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};
// --------------------------
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// ������ id ���������
class RDK_LIB_TYPE ULongIdVector
{
protected: // ���������
// ������ �������
int Size;

protected: // ������
// ������ ����������
mutable ULongId* Buffer;

// �������� ������ �������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
ULongIdVector(void);
ULongIdVector(const ULongIdVector &copy);
virtual ~ULongIdVector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int GetSize(void) const;

// �������� ������ ������� � ����������� ������� ������
void Resize(int newsize);

// ������� ������
void Clear(void);

// ���������� ��������� �� ������ ������
ULongId* GetBuffer(void);

// ��������� ������� � ����� �������
// ���������� ������ ��������
int Add(const ULongId &id);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
ULongIdVector& operator = (const ULongIdVector &copy);

// �������� �������
ULongId& operator [] (int index);
const ULongId& operator [] (int index) const;
// --------------------------
};
/* *********************************************************************** */
/* *********************************************************************** */

//extern ULongId ULongIdemp;


/* *********************************************************************** */
/* *********************************************************************** */
// �������� ����� �� ������ ����� ����� ���������
template<typename T>
struct ULinkSideT
{
// Id �������
T Id;

// ������ ������������ �����/������
int Index;

// ��� ������������ �����/������
std::string Name;

// --------------------------
// ������������ � �����������
// --------------------------
ULinkSideT(void);
ULinkSideT(const T &id, int index);
ULinkSideT(const T &id, const std::string &property_name);
ULinkSideT(const T &id, const std::string &property_name, int index);
ULinkSideT(const ULinkSideT &link);
virtual ~ULinkSideT(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
bool operator < (const ULinkSideT<T> &linkside2) const;
bool operator == (const ULinkSideT<T> &linkside2) const;
bool operator != (const ULinkSideT<T> &linkside2) const;
//friend bool operator < (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2);
//friend bool operator == (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2);
//friend bool operator != (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2);
// --------------------------
};

typedef ULinkSideT<ULongId> ULinkSide;
typedef ULinkSideT<std::string> UStringLinkSide;


/* *********************************************************************** */
/* *********************************************************************** */

// �������� ��������� �����
template<typename T>
struct ULinkT
{
 ULinkSideT<T> Item;
 std::vector<ULinkSideT<T> > Connector;

// --------------------------
// ������������ � �����������
// --------------------------
ULinkT(void);
ULinkT(const ULinkSideT<T> &item, const ULinkSideT<T> &conn);
ULinkT(const T &item_id, int item_index,const T &conn_id, int conn_index);
ULinkT(const ULinkT &link);
virtual ~ULinkT(void);
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// ���� �������� ������� � ���������� connector � ���������� ������ ��� ������������� �����, ���� ��
// �������
int FindConnector(const ULinkSideT<T> &connector);
// --------------------------


// --------------------------
// ���������
// --------------------------
//friend bool operator < (const ULinkT<T> &link1, const ULinkT<T> &link2);
//friend bool operator == (const ULinkT<T> &link1, const ULinkT<T> &link2);
//friend bool operator != (const ULinkT<T> &link1, const ULinkT<T> &link2);
// --------------------------
};

typedef ULinkT<ULongId> ULink;
typedef ULinkT<std::string> UStringLink;


/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */

// ������ ������
template<typename T>
class ULinksListT
{
protected: // ������
// ������
mutable std::vector<ULinkT<T> > Data;

// ������ �������
int Size;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
ULinksListT(void);
ULinksListT(const ULinksListT<T> &copy);
virtual ~ULinksListT(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������� ������
void Clear(void);

// �������� ������ ������� � ����������� ������� ������
void Resize(int newsize);

// ��������� ������� � ����� �������
// ���������� ������ ��������
int Add(const ULinkT<T> &link);

// ���������� ������� ��� � ������������
int Merge(const ULinkT<T> &link);

// �������� ������� (���� ������� �� ���������� - �� ���������)
int Set(const ULinkT<T> &link);

// ������� ������������ ������� �� �������
void Del(int index);

// ���� �������� ������� � ���������� ������ ��� ������������� �����, ���� ��
// �������
int Find(const ULinkT<T> &link);

// ���� �������� ������� � ���������� item � ���������� ������ ��� ������������� �����, ���� ��
// �������
int FindItem(const ULinkSideT<T> &item);

// ���� �������� ������� � ���������� connector � ���������� ������ ��� ������������� �����, ���� ��
// �������
int FindConnector(const ULinkSideT<T> &connector);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
int GetSize(void) const;
int size(void) const;

// ���������� ��������� �� ������ ������
ULinkT<T>* GetData(void);
// --------------------------


// --------------------------
// ���������
// --------------------------
// �������� ������������
ULinksListT<T>& operator = (const ULinksListT<T> &copy);

// �������� �������
ULinkT<T>& operator [] (int index);
const ULinkT<T>& operator [] (int index) const;
// --------------------------
};


template<typename CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const UIdVector &data)
{
 size_t size=data.GetSize();

 if(size > 0)
 {
  for(size_t i=0;i<size;i++)
  {
   stream<<data[i];
   if(i != size-1)
	stream<<".";
  }
 }

 return stream;
}

typedef ULinksListT<ULongId> ULinksList;
typedef ULinksListT<std::string> UStringLinksList;
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
template<typename T>
ULinkSideT<T>::ULinkSideT(void)
 : Index(0)
{
}

template<typename T>
ULinkSideT<T>::ULinkSideT(const T &id, int index)
 : Id(id), Index(index)
{
}

template<typename T>
ULinkSideT<T>::ULinkSideT(const T &id, const std::string &property_name)
 :Id(id), Index(-1), Name(property_name)
{
}

template<typename T>
ULinkSideT<T>::ULinkSideT(const T &id, const std::string &property_name, int index)
 :Id(id), Index(index), Name(property_name)
{
}


template<typename T>
ULinkSideT<T>::ULinkSideT(const ULinkSideT<T> &link)
{
 Id=link.Id;
 Index=link.Index;
 Name=link.Name;
}

template<typename T>
ULinkSideT<T>::~ULinkSideT(void)
{
}
// --------------------------

// --------------------------
// ���������
// --------------------------
/*template<typename T>
bool operator < (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2)
{
 return (linkside1.Id<linkside2.Id)
	|| (linkside1.Id == linkside2.Id && linkside1.Index < linkside2.Index);
}

template<typename T>
bool operator == (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2)
{
 return (linkside1.Id == linkside2.Id) & (linkside1.Index == linkside2.Index);
}

template<typename T>
bool operator != (const ULinkSideT<T> &linkside1, const ULinkSideT<T> &linkside2)
{
 return (!(linkside1 == linkside2));
} */

template<typename T>
bool ULinkSideT<T>::operator < (const ULinkSideT<T> &linkside2) const
{
 return (Id<linkside2.Id)
	|| (Id == linkside2.Id && Name < linkside2.Name) ||
	   (Id == linkside2.Id && Name == linkside2.Name && Index<linkside2.Index);
}

template<typename T>
bool ULinkSideT<T>::operator == (const ULinkSideT<T> &linkside2) const
{
 return (Id == linkside2.Id) & (Name == linkside2.Name) & (Index == linkside2.Index);
}

template<typename T>
bool ULinkSideT<T>::operator != (const ULinkSideT<T> &linkside2) const
{
 return (!(*this == linkside2));
}
// --------------------------
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
// --------------------------
// ������������ � �����������
// --------------------------
template<typename T>
ULinkT<T>::ULinkT(void)
{
}

template<typename T>
ULinkT<T>::ULinkT(const ULinkSideT<T> &item, const ULinkSideT<T> &conn)
{
 Item=item;
 Connector.push_back(conn);
}

template<typename T>
ULinkT<T>::ULinkT(const T &item_id, int item_index,const T &conn_id, int conn_index)
{
 Item.Id=item_id;
 Item.Index=item_index;
 Connector.resize(1);
 Connector[0].Id=conn_id;
 Connector[0].Index=conn_index;
}

template<typename T>
ULinkT<T>::ULinkT(const ULinkT<T> &link)
{
 Item=link.Item;
 Connector=link.Connector;
}

template<typename T>
ULinkT<T>::~ULinkT(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���� �������� ������� � ���������� connector � ���������� ������ ��� ������������� �����, ���� ��
// �������
template<typename T>
int ULinkT<T>::FindConnector(const ULinkSideT<T> &connector)
{
  for(size_t j=0;j<Connector.size();j++)
   if(Connector[j] == connector)
	return j;

 return -1;
}
// --------------------------



// --------------------------
// ���������
// --------------------------
template<typename T>
bool operator < (const ULinkT<T> &link1, const ULinkT<T> &link2)
{
 return (link1.Item<link2.Item) & (link1.Connector<link2.Connector);
}

template<typename T>
bool operator == (const ULinkT<T> &link1, const ULinkT<T> &link2)
{
 return (link1.Item == link2.Item) & (link1.Connector == link2.Connector);
}

template<typename T>
bool operator != (const ULinkT<T> &link1, const ULinkT<T> &link2)
{
 return !(link1 == link2);
}
// --------------------------


/******************************************************************************/
// --------------------------
// ������������ � �����������
// --------------------------
template<typename T>
ULinksListT<T>::ULinksListT(void)
{
 Size=0;
}

template<typename T>
ULinksListT<T>::ULinksListT(const ULinksListT<T> &copy)
{
 Size=0;

 *this=copy;
}

template<typename T>
ULinksListT<T>::~ULinksListT(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������� ������
template<typename T>
void ULinksListT<T>::Clear(void)
{
 Data.clear();
 Size=0;
}

// �������� ������ ������� � ����������� ������� ������
template<typename T>
void ULinksListT<T>::Resize(int newsize)
{
 Data.resize(newsize);
 Size=Data.size();
}

// ��������� ������� � ����� �������
// ���������� ������ ��������
template<typename T>
int ULinksListT<T>::Add(const ULinkT<T> &link)
{
 Data.push_back(link);
 ++Size;
 return Size-1;
}

// ���������� ������� ��� � ������������
template<typename T>
int ULinksListT<T>::Merge(const ULinkT<T> &link)
{
 int id=FindItem(link.Item);

 if(id < 0)
  return Add(link);

 for(size_t j=0;j<link.Connector.size();j++)
 {
  typename std::vector<ULinkSideT<T> >::iterator I=find(Data[id].Connector.begin(),Data[id].Connector.end(),link.Connector[j]);
  if(I == Data[id].Connector.end())
  {
   Data[id].Connector.push_back(link.Connector[j]);
  }
 }
 return id;
}

// �������� ������� (���� ������� �� ���������� - �� ���������)
template<typename T>
int ULinksListT<T>::Set(const ULinkT<T> &link)
{
 int id=FindItem(link.Item);

 if(id < 0)
  return Add(link);

 Data[id]=link;
 return id;
}


// ������� ������������ ������� �� �������
template<typename T>
void ULinksListT<T>::Del(int index)
{
 if(index<0 || index>=Size)
  return;

 Data.erase(Data.begin()+index);
 --Size;
}

// ���� �������� ������� � ���������� ������ ��� ������������� �����, ���� ��
// �������
template<typename T>
int ULinksListT<T>::Find(const ULinkT<T> &link)
{
 if(!Size)
  return -1;
 ULinkT<T> *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
  if(*pdata == link)
   return i;

 return -1;
}

// ���� �������� ������� � ���������� item � ���������� ������ ��� ������������� �����, ���� ��
// �������
template<typename T>
int ULinksListT<T>::FindItem(const ULinkSideT<T> &item)
{
 if(!Size)
  return -1;

 ULinkT<T> *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
  if(pdata->Item == item)
   return i;

 return -1;
}

// ���� �������� ������� � ���������� connector � ���������� ������ ��� ������������� �����, ���� ��
// �������
template<typename T>
int ULinksListT<T>::FindConnector(const ULinkSideT<T> &connector)
{
 if(!Size)
  return -1;

 ULinkT<T> *pdata=&Data[0];
 for(int i=0;i<Size;i++,pdata++)
 {
  for(size_t j=0;j<pdata->Connector.size();j++)
   if(pdata->Connector[j] == connector)
	return i;
 }

 return -1;
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
template<typename T>
int ULinksListT<T>::GetSize(void) const
{
 return Size;
}

template<typename T>
int ULinksListT<T>::size(void) const
{
 return Size;
}


// ���������� ��������� �� ������ ������
template<typename T>
ULinkT<T>* ULinksListT<T>::GetData(void)
{
 if(!Size)
  return 0;

 return &Data[0];
}
// --------------------------


// --------------------------
// ���������
// --------------------------
// �������� ������������
template<typename T>
ULinksListT<T>& ULinksListT<T>::operator = (const ULinksListT<T> &copy)
{
 Data=copy.Data;
 Size=copy.Size;

 return *this;
}

// �������� �������
template<typename T>
ULinkT<T>& ULinksListT<T>::operator [] (int index)
{
 return Data[index];
}

template<typename T>
const ULinkT<T>& ULinksListT<T>::operator [] (int index) const
{
 return Data[index];
}
// --------------------------
/* *********************************************************************** */
/* *********************************************************************** */

}

#endif
