/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAConnectorH
#define UAConnectorH

#include "UAContainer.h"

namespace RDK {

class UAItem;

// �������� ������������� �������� "UConnectedITEM"
struct UCItem
{
// ������������ �������
UAItem* Item;

// ������ ������
int Index;

// --------------------------
// ������������ � �����������
// --------------------------
UCItem(void);
UCItem(const UCItem &copy);
// --------------------------

// --------------------------
// ���������
// --------------------------
bool operator == (const UCItem &value);
bool operator != (const UCItem &value);
// --------------------------
};

// �������� �������� ����� � �������� item "UConnectedLINK"
struct UCLink
{
// ������ ������
int Output;

// ������ �����
int Input;

// --------------------------
// ������������ � �����������
// --------------------------
UCLink(void);
UCLink(const UCLink &copy);
// --------------------------
};

// ��������� - ������ ���������� �� ������������ ��������
class UCItemList
{
protected: // ���������
// ������ ����������
int Size;

protected: // ������
// ������ ���������
UCItem* Data;

protected: // ��������� ����������
// �������� ������ ����������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UCItemList(void);
UCItemList(const UCItemList &copy);
virtual ~UCItemList(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// ������ ������ ���������� � ����������� ���������
void Resize(int newsize);

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ��������
UCItem Find(UEPtr<UAItem> item, int index=0) const;

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
int Find(const UCItem &item, int index=0) const;

// ��������� � ����� ���������� �������
void Add(const UCItem &item);

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
bool AddUnique(const UCItem &item);

// ������� ������� �� �������
void Del(int index);

// ������� ������� �� ���������
void Del(UCItem *item);
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UCItemList& operator = (const UCItemList &copy);

// �������� ������� � ��������
UCItem& operator [] (int index);
const UCItem& operator [] (int index) const;
//const UCItem& operator () (int index) const;

// �������� ������� � �������
UCItem* GetData(void);

// �������� ������� � �������
int GetSize(void) const;
// --------------------------
};

class UAConnector: public UAContainer
{
friend class UAItem;
private: // ��������� ������
// ������ ���������� �� ������������ �������� ����
UCItemList CItemList;

protected: // ������������� ��������
// ����� ��������� ������
int NumInputs;

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
bool AutoNumInputs;

protected: // ���������� �������� ������� � ������� ������. Read only!

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAConnector(void);
virtual ~UAConnector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ���������� ����� ������������ ��������� item
int GetNumInputs(void) const;

// ������������� ����� ������������ ��������� item
bool SetNumInputs(int value);

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
bool GetAutoNumInputs(void) const;
bool SetAutoNumInputs(bool value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& GetCItem(int c_index) const;

// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
UCLink GetCLink(UEPtr<UAItem> item) const;
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
protected:
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UAItem> na, int i_index, int &c_index);

// ��������� ����� � ��������� ���� 'na'
virtual void DisconnectFromItem(UEPtr<UAItem> na);

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
virtual void DisconnectFromIndex(int c_index);

// ��������� �������� ����� ��������� ������������ �����
virtual bool AConnectToItem(UEPtr<UAItem> na, int i_index, int c_index);

// ��������� �������� ����� ��������� ���������� �����
virtual void ADisconnectFromItem(UEPtr<UAItem> na, int i_index, int c_index);

public:
// ��������� ��� ������� �����
virtual void DisconnectAllItems(void);

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void DisconnectByObject(UEPtr<UAContainer> brklevel);

public:
// ���������� ��������� �����
//virtual UEInterface* GetInputInterface(int c_index);

// ���������� ��������� �����
//NameT GetInputInterfaceTypeName(int c_index);

// ���������, ��������� �� ����������� ��������� item � ����� ����������
virtual bool CheckItem(UEPtr<UAItem> item, int item_index, int conn_index);

// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel) const;
// ----------------------

// --------------------------
// ������ ���������� ������
// --------------------------
public:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);
// --------------------------

public: // ������ �������� ����������
// ������������ ������ �����
struct EInvalidInputIndex: public EInvalidIndex
{
EInvalidInputIndex(int index) : EInvalidIndex(index) {};
};

// ����� ���� �� ����������
struct EInputIndexNotExist: public EInvalidIndex
{
EInputIndexNotExist(void) : EInvalidIndex(-1) {};
};
};

}
#endif
