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
/*
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
explicit UIdVector(UId id1);
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
UIdVector& operator = (const UId &copy);

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
*/

// �������� ����� �� ������ ����� ����� ���������
struct ULinkSide
{
// Id �������
std::string ComponentName;

// ��� ������������ �����/������
std::string PropertyName;

// ������ ������������ �����/������ (�����������)
int Index;

// --------------------------
// ������������ � �����������
// --------------------------
ULinkSide(void);
ULinkSide(const std::string &comp_name, const std::string &property_name);
ULinkSide(const std::string &comp_name, const std::string &property_name, int index);
ULinkSide(const ULinkSide &link);
virtual ~ULinkSide(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
bool operator < (const ULinkSide &linkside2) const;
bool operator == (const ULinkSide &linkside2) const;
bool operator != (const ULinkSide &linkside2) const;
// --------------------------
};

/* *********************************************************************** */
/* *********************************************************************** */

// �������� ��������� �����
struct ULink
{
 ULinkSide Item;
 std::vector<ULinkSide> Connector;

// --------------------------
// ������������ � �����������
// --------------------------
ULink(void);
ULink(const ULinkSide &item, const ULinkSide &conn);
//ULink(const std::string &item_id, int item_index,const T &conn_id, int conn_index);
ULink(const ULink &link);
virtual ~ULink(void);
// --------------------------


// --------------------------
// ������ ���������� �������
// --------------------------
// ���� �������� ������� � ���������� connector � ���������� ������ ��� ������������� �����, ���� ��
// �������
int FindConnector(const ULinkSide &connector);
// --------------------------
};

/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */

// ������ ������
class ULinksList
{
protected: // ������
// ������
mutable std::vector<ULink> Data;

// ������ �������
int Size;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
ULinksList(void);
ULinksList(const ULinksList &copy);
virtual ~ULinksList(void);
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
int Add(const ULink &link);

// ���������� ������� ��� � ������������
int Merge(const ULink &link);

// �������� ������� (���� ������� �� ���������� - �� ���������)
int Set(const ULink &link);

// ������� ������������ ������� �� �������
void Del(int index);

// ���� �������� ������� � ���������� ������ ��� ������������� �����, ���� ��
// �������
int Find(const ULink &link);

// ���� �������� ������� � ���������� item � ���������� ������ ��� ������������� �����, ���� ��
// �������
int FindItem(const ULinkSide &item);

// ���� �������� ������� � ���������� connector � ���������� ������ ��� ������������� �����, ���� ��
// �������
int FindConnector(const ULinkSide &connector);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
int GetSize(void) const;
int size(void) const;

// ���������� ��������� �� ������ ������
ULink* GetData(void);
// --------------------------


// --------------------------
// ���������
// --------------------------
// �������� ������������
ULinksList& operator = (const ULinksList &copy);

// �������� �������
ULink& operator [] (int index);
const ULink& operator [] (int index) const;
// --------------------------
};

/*
template<typename CharT>
std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT>& stream, const UIdVector &data)
 : Item(item)
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
  */
/* *********************************************************************** */
/* *********************************************************************** */

}

#endif
