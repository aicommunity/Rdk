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

class UIPropertyIO;

enum { ipData=1, ipComp=2 };
enum { ipSingle=16, ipRange=32, ipList=64 };
enum { ipDataSingle=ipData|ipSingle, ipDataRange=ipData|ipRange,
	   ipDataList=ipData|ipList, ipCompSingle=ipComp|ipSingle,
	   ipCompRange=ipComp|ipRange, ipCompList=ipComp|ipList };

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
UCItem Find(const UEPtr<UAItem> item, int index=0) const;
UCItem Find(const UAItem *const item, int index=0) const;

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
UCLink GetCLink(const UEPtr<UAItem> item) const;
UCLink GetCLink(const UAItem* const item) const;
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

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UAItem> &item) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UAItem> &item, int item_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UAItem> &item, int item_index, int conn_index) const;

// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel, bool exclude_internals=false, UEPtr<UAContainer> internal_level=0) const;

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UAContainer> cont, ULinksListT<T> &linkslist, UEPtr<UAContainer> netlevel) const;
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


class UIPropertyIO
{
public:
// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� �������� �����-������
virtual int GetIoType(void) const=0;

/// ���������� �������� �������� �����/������
virtual bool CheckRange(int index)=0;
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
virtual bool SetPointer(int index, void* value)=0;

/// ��������� ������������� ���������
virtual void Init(void)=0;
// --------------------------
};

class UIPropertyInput
{
protected: // ������
/// ��������� �� ���������-�������� ������
UEPtr<UAItem> Item;

/// ��� ������ ���������-��������� ������
std::string ItemOutputName;

public:
/// ���������� ��������� �� ���������-��������
UEPtr<UAItem> GetItem(void);

/// ���������� ��� ������������� ������
const std::string& GetItemOutputName(void);
};

class UIPropertyOutput
{
protected: // ������
/// ��������� �� ����������-��������� ������
std::vector<UEPtr<UAItem> > Connectors;

/// ��� ������ ���������-��������� ������
std::vector<std::string> ConnectorInputNames;

public:
/// ���������� ����� ������������ ������
size_t GetNumConnectors(void);

/// ���������� ��������� �� ���������-��������
UEPtr<UAConnector> GetConnector(int index);

/// ���������� ��� ������������� ����� ����������-���������
const std::string& GetConnectorInputName(int index);
};

}

#include "UPropertyIO.h"
#endif
