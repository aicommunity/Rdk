/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAEnvSupportH
#define UAEnvSupportH

namespace RDK {

typedef int UId;
extern UId ForbiddenId;

typedef std::string NameT;
extern NameT ForbiddenName;

/* *********************************************************************** */
/* *********************************************************************** */
// ������ id ���������
class UIdVector
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
};

typedef UIdVector ULongId;
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// ������ id ���������
class ULongIdVector
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

extern ULongId ULongIdemp;


/* *********************************************************************** */
/* *********************************************************************** */
// �������� ����� �� ������ ����� ����� ���������
struct ULinkSide
{
// Id �������
ULongId Id;

// ������ ������������ �����/������
int Index;

// --------------------------
// ������������ � �����������
// --------------------------
ULinkSide(void);
ULinkSide(const ULongId &id, int index);
ULinkSide(const ULinkSide &link);
virtual ~ULinkSide(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
friend bool operator < (const ULinkSide &linkside1, const ULinkSide &linkside2);
friend bool operator == (const ULinkSide &linkside1, const ULinkSide &linkside2);
friend bool operator != (const ULinkSide &linkside1, const ULinkSide &linkside2);
// --------------------------
};

// �������� ��������� �����
struct ULink
{
 ULinkSide Item;
 ULinkSide Connector;

// --------------------------
// ������������ � �����������
// --------------------------
ULink(void);
ULink(const ULinkSide &item, const ULinkSide &conn);
ULink(const ULongId &item_id, int item_index,const ULongId &conn_id, int conn_index);
ULink(const ULink &link);
virtual ~ULink(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
friend bool operator < (const ULink &link1, const ULink &link2);
friend bool operator == (const ULink &link1, const ULink &link2);
friend bool operator != (const ULink &link1, const ULink &link2);
// --------------------------
};

// ������ ������
class ULinksList
{
protected: // ������
// ������
mutable ULink *Data;

// ������ �������
int Size;

// �������� ������ �������
int RealSize;

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

// ������� ������������ ������� �� �������
void Del(int index);

// ���� �������� ������� � ���������� ������ ��� ������������� �����, ���� ��
// �������
int Find(const ULink &link);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������
int GetSize(void) const;

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
/* *********************************************************************** */
/* *********************************************************************** */
}

#endif
