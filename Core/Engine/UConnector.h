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

#include "UContainer.h"

namespace RDK {

//class UIPropertyIO;



class UItem;

// �������� ������������� �������� "UConnectedITEM"
struct RDK_LIB_TYPE UCItem
{
// ������������ �������
UItem* Item;

// ������ ������
int Index;

// ��� ������
std::string Name;

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
struct RDK_LIB_TYPE UCLink
{
// ������ ������
int Output;

// ������ �����
int Input;

// ��� ������
std::string OutputName;

// ��� �����
std::string InputName;

// --------------------------
// ������������ � �����������
// --------------------------
UCLink(void);
UCLink(const UCLink &copy);
// --------------------------
};

// ��������� - ������ ���������� �� ������������ ��������
class RDK_LIB_TYPE UCItemList
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
UCItem Find(const UEPtr<UItem> &item, int index=0) const;
UCItem Find(const UItem *const item, int index=0) const;

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

class RDK_LIB_TYPE UConnector: public UContainer
{
friend class UItem;
protected: // ��������� ������
// ������ ���������� �� ������������ �������� ���� � �������
// <��� �������� � �������� ����������, ������ ������������ ���������>
std::map<std::string, std::vector<UCItem> > ConnectedItemList;

// ��������, ������������ � ������, ����� ���������� ������ �� CItem ������� ����������
UCItem DummyItem;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UConnector(void);
virtual ~UConnector(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
/// ���������� ����� ������� � ������� ���-�� ����������
virtual int GetNumActiveInputs(void) const;

/// ���������� ����� ������� ������������ ��������� � ��������� ��������
virtual int GetNumActiveInputs(const NameT &connector_property_name) const;
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
/// ���� ������ ��������� ����
virtual NameT FindFreeInputName(void)=0;

// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& GetCItem(const NameT &connector_property_name, const UEPtr<UItem> &item, int &index) const;
const UCItem& GetCItem(const NameT &connector_property_name, int index) const;
void GetCItem(const NameT &connector_property_name, std::vector<UCItem> &buffer) const;

// ���������� ���������� �� �������� ������ � ���� item ��� -1, -1
// ���� ����� ����� �����������
virtual UCLink GetCLink(const UEPtr<UItem> &item) const;
virtual UCLink GetCLink(const UItem* const item) const;
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
/// ���� ��������-���� �� ��������� �������
virtual void FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const;

/// ���������� ������ ����� � �������� ������
virtual int FindInputIndex(const NameT &input_name) const;
// --------------------------

// ----------------------
// ���������������� ������
// ----------------------
protected:
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// ��������� ��� ����� � ��������� ���� 'na'
virtual void DisconnectFromItem(UEPtr<UItem> na);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int connected_c_index);

// ��������� �������� ����� ��������� ������������ �����
virtual bool AConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

// ��������� �������� ����� ��������� ���������� �����
virtual void ADisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

public:
// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);

virtual void DisconnectFromIndex(const NameT &connector_property_name);

// ��������� ��� ������� �����
virtual void DisconnectAllItems(void);

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void DisconnectByObject(UEPtr<UContainer> brklevel);

public:
// ���������� ��������� �����
//virtual UEInterface* GetInputInterface(int c_index);

// ���������� ��������� �����
//NameT GetInputInterfaceTypeName(int c_index);

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name);

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UItem> &item) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UItem> &item, const NameT &item_property_name, const NameT &connector_property_name) const;

// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;
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
EInputIndexNotExist(int index) : EInvalidIndex(index) {};
};
};

	   /*
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
};     */

class RDK_LIB_TYPE UIPropertyInput
{
protected: // ������
/// ��������� �� ���������-�������� ������
UEPtr<UItem> Item;

/// ��� ������ ���������-��������� ������
std::string ItemOutputName;

public:
/// ���������� ��������� �� ���������-��������
UEPtr<UItem> GetItem(void);

/// ���������� ��� ������������� ������
const std::string& GetItemOutputName(void);
};

class RDK_LIB_TYPE UIPropertyOutput
{
protected: // ������
/// ��������� �� ����������-��������� ������
std::vector<UEPtr<UItem> > Connectors;

/// ��� ������ ���������-��������� ������
std::vector<std::string> ConnectorInputNames;

public:
/// ���������� ����� ������������ ������
size_t GetNumConnectors(void);

/// ���������� ��������� �� ���������-��������
UEPtr<UConnector> GetConnector(int index);

/// ���������� ��� ������������� ����� ����������-���������
const std::string& GetConnectorInputName(int index);
};

}

#include "UPropertyIO.h"
#endif
