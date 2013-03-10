/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAItemH
#define UAItemH

#include "UConnector.h"
//#include "UEInterface.h"

namespace RDK {

typedef UEPtr<UConnector> PUAConnector;

class UAConnectorVector
{
protected: // ���������
// ������ ����������
int Size;

protected: // ������
// ������ ���������
PUAConnector* Data;

protected: // ��������� ����������
// �������� ������ ����������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAConnectorVector(void);
UAConnectorVector(const UAConnectorVector &copy);
virtual ~UAConnectorVector(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// ������ ������ ���������� � ����������� ���������
void Resize(int newsize);

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
int Find(const PUAConnector &item, int index=0) const;

// ��������� � ����� ���������� �������
void Add(const PUAConnector &item);

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
bool AddUnique(const PUAConnector &item);

// ������� ������� �� �������
void Del(int index);

// ������� ������� �� ���������
void Del(PUAConnector *item);
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UAConnectorVector& operator = (const UAConnectorVector &copy);

// �������� ������� � ��������
PUAConnector& operator [] (int index);
const PUAConnector& operator [] (int index) const;

// �������� ������� � �������
PUAConnector* GetData(void);

// �������� ������� � �������
int GetSize(void) const;
// --------------------------
};

class UAConnector2DVector
{
protected: // ���������
// ������ ����������
int Size;

protected: // ������
// ������ ���������
UAConnectorVector* Data;

protected: // ��������� ����������
// �������� ������ ����������
int RealSize;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAConnector2DVector(void);
UAConnector2DVector(const UAConnector2DVector &copy);
virtual ~UAConnector2DVector(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// ������ ������ ���������� � ����������� ���������
void Resize(int newsize);

// ���� � ���������� ������ �������� ������� ������� � ������� index
// � ���������� ��� ������ ��� -1 ���� ������� �� ������
//int Find(const UAConnectorVector &item, int index=0) const;

// ��������� � ����� ���������� �������
void Add(const UAConnectorVector &item);

// ��������� � ����� ���������� ������� ������ ���� ����� ��������� ��� ���
// ���������� true � ������ ��������� ����������
//bool AddUnique(const UAConnectorVector &item);

// ������� ������� �� �������
void Del(int index);

// ������� ������� �� ���������
//void Del(UAConnectorVector *item);
// --------------------------

// --------------------------
// ������ �������
// --------------------------
// �������� ������������
UAConnector2DVector& operator = (const UAConnector2DVector &copy);

// �������� ������� � ��������
UAConnectorVector& operator [] (int index);
const UAConnectorVector& operator [] (int index) const;

// �������� ������� � �������
UAConnectorVector* GetData(void);

// �������� ������� � �������
int GetSize(void) const;
// --------------------------
};

class UItem: public UConnector
{
protected: // �������� ��������
// ������ ����������� ������������ � ������� ����� �������
UAConnector2DVector AssociatedConnectors;

protected: // ������������� ��������
// ����� �������
int NumOutputs;

// ������� ���������/���������� ������ ��������������� ���������� ����� �������
// ��� ����������� ������ connector.
bool AutoNumOutputs;

protected: // ��������� ����������. Read Only!
// ��������� �� ������ ������� ������ ������������ �����������
//vector<UConnector**> PAssociatedConnectors;

// ����� ��������� ������ ������������ �����������
//vector<size_t> NumAConnectors;

protected: // ���������� �������� ������� � ������� �������. Read only!
// ����� �������
//size_t CNumOutputs;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UItem(void);
virtual ~UItem(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
public:
// ���������� ����� ������������ ��������� item
const int& GetNumOutputs(void) const;

// ������������� ����� ������������ ��������� item
bool SetNumOutputs(const int &value);

// ������� ���������/���������� ������ ��������������� ���������� ����� �������
const bool& GetAutoNumOutputs(void) const;
bool SetAutoNumOutputs(const bool &value);
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------


public:
// ----------------------
// ���������������� ������
// ----------------------
// ������������� ����� � ����������� 'c'
virtual bool Connect(UEPtr<UConnector> c, int i_index, int c_index=-1);

// ��������� ����� ������ ����� ������� � ����������� 'c'
virtual void Disconnect(UEPtr<UConnector> c);

// ���������� ������� ����� ���������� ��� ��������� ������.
int GetNumAConnectors(int index) const;

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
virtual bool Disconnect(const UId &id);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
void DisconnectAll(void);

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void DisconnectBy(UEPtr<UContainer> brklevel);

// ����������������� ��� ����� ����� item �� ����� connectors ������� ��������
// ������ �� ����� item
virtual void BuildLinks(void);

// ���������� ��������� �� ��������� �� ������ �����������
// �� Id 'id'.
UEPtr<UConnector> GetAConnector(const UId &id, int index) const;

// ����������  ��������� �� ������ �����������.
UEPtr<UConnector> GetAConnectorByIndex(int output, int index) const;

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UConnector> &connector) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, int item_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UConnector> &connector, int item_index, int conn_index) const;

// ���������� ������ �����������
template<typename T>
ULinksListT<T>& GetLinks(ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// ���������� ������ ����������� ��������������� ���������� cont
template<typename T>
ULinksListT<T>& GetPersonalLinks(UEPtr<UContainer> cont, ULinksListT<T> &linkslist, UEPtr<UContainer> netlevel) const;

// ���������� ������ ����������� ����� ���������� � ���� �������� ���������
// � ��������� ���������� comp � ���� ��� �������� �����������
template<typename T>
ULinksListT<T>& GetFullItemLinks(ULinksListT<T> &linkslist, UEPtr<UItem> comp,
                            UEPtr<UContainer> netlevel) const;
// ----------------------

public:
// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);

// ���������� ��� ����������, ������������ � ������ ����� �������.
//virtual void ActivateAll(void);
// --------------------------

};

}
#endif

