/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UADITEM_H
#define UADITEM_H

#include <typeinfo>
#include "UItem.h"
#include "UItemData.h"

namespace RDK {

class RDK_LIB_TYPE UADItem: public UItem
{
friend class UItemData;
private: // ��������� �������� ������
// ������ �������� ������
vector<UItemData> OutputData;

private: // ��������� ������� ������
// ������ ���������� �� ������� ������
//vector<UEPtr<const UItemData> > InputData;
vector<UItemData*> InputData;

protected: // ������������� ��������
// ����� ��������� ������
int NumInputs;

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
bool AutoNumInputs;

// ����� �������
int NumOutputs;

// ������� ���������/���������� ������ ��������������� ���������� ����� �������
// ��� ����������� ������ connector.
bool AutoNumOutputs;

protected: // ���������� �������� ������� � ������� ������. Read only!
// ��������� ����� ���� ������
size_t FullInputDataSize;

// ����������� ������ ������� ������
size_t MinInputDataSize;

// ������������ ������ ������� ������
size_t MaxInputDataSize;

protected: // ���������� �������� ������� � ������� �������. Read only!
// ��������� �� ������ ������� ������� ���������� �� ������� ������
UItemData* POutputData;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UADItem(void);
virtual ~UADItem(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ����� ������������ ��������� item
const int& GetNumInputs(void) const;

// ������������� ����� ������������ ��������� item
bool SetNumInputs(const int &value);

// ������� ���������/���������� ������ ��������������� ���������� ����� ������
// ��� ����������� ������ item.
const bool& GetAutoNumInputs(void) const;
bool SetAutoNumInputs(const bool &value);

// ���������� ����� ������������ ��������� item
const int& GetNumOutputs(void) const;

// ������������� ����� ������������ ��������� item
bool SetNumOutputs(const int &value);

// ������� ���������/���������� ������ ��������������� ���������� ����� �������
const bool& GetAutoNumOutputs(void) const;
bool SetAutoNumOutputs(const bool &value);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ������ �������� ������
inline const UItemData& GetOutputData(int index) const
{ return POutputData[index]; };

// ���������� ��������� �� ������ ������ InputData �� ��������� �� item
// ���������� 0 ���� citem == 0 ��� �� ������ � ������ �����������
const UEPtr<const UItemData>& GetInputData(const UEPtr<UItem> &citem) const;

// ���������� ��������� �� ������ ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
const UEPtr<const UItemData>& GetInputData(size_t index) const;

// ���������� ������ ������� ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
size_t GetInputDataSize(size_t index) const;

// ���������� ��������� ������ ���� �������� ������
size_t GetFullInputDataSize(void) const;
// --------------------------

// ----------------------
// ������ ���������� ��������� �������
// ----------------------
// ������ ������� �������� ������
inline size_t GetOutputDataSize(int index) const
{ return OutputData[index].Size; };
virtual bool SetOutputDataSize(int index, int size, bool nobuild=false);

// ���������� ������ ������� �������� ������ � ������
inline size_t GetByteOutputDataSize(int index) const
{ return OutputData[index].ByteSize; };

// ������ ���������� ������� ������� �������� ������ � ������
size_t GetOutputDataElementSize(int index) const
{ return OutputData[index].DataSize; };
bool SetOutputDataElementSize(int index, int size);

// ��������� �������� �������� ������ �������
void FillOutputData(int index, const void *data=0);

// ��������� ��� �������� ������� ��������� �������
void FillOutputData(const void *data=0);
// ----------------------

// --------------------------
// ������ �������������� ������� �  �������� ������
// --------------------------
// �������������� ����� ��� ��������� �� ������
bool SetOutputDataAsPointer(int index, void *pointer);

// ���������� ������ ������ ��� ��������� �� ������
void* GetOutputDataAsPointer(int index);

// ���������� ������ ����� ��� ��������� �� ������
void* GetInputDataAsPointer(int index);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
virtual UContainerDescription* ANewDescription(UComponentDescription* description);
// --------------------------

// ----------------------
// ������ ������� � ��������� ���������
// ----------------------
protected:
// ������ �������� ��������
vector<size_t> GetOutputDataSize(void) const;
bool SetOutputDataSize(const vector<size_t> &value);

// ������ ���������� ������� ������� ������� � ������
vector<size_t> GetOutputDataElementSize(void) const;
bool SetOutputDataElementSize(const vector<size_t> &value);
// ----------------------

// ----------------------
// ������ ������� � �����������
// ----------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;
// ----------------------

// ----------------------
// ���������������� ������
// ----------------------
public:
// ���������� ������������ � ����� ���������� ������ �� �������
const UCItem& GetCItem(int c_index) const;

/// ���� ��������-���� �� ��������� �������
virtual void FindInputProperty(int index, UIProperty* &property) const;
virtual void FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const;

/// ���� ��������-����� �� ��������� �������
virtual void FindOutputProperty(int index, UIProperty* &property) const;
virtual void FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const;

// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index);
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
virtual void DisconnectFromItem(UEPtr<UItem> na, int i_index);
virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);
virtual void DisconnectFromIndex(const NameT &connector_property_name);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
virtual void DisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index);

// ��������� ��� ����� � ��������� ���� 'na'
virtual void DisconnectFromItem(UEPtr<UItem> na);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name);

/// ��������� ����� � ��������� ���� 'na', ������������ �� i_index � c_index
virtual void DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name);

// ��������� �������� ����� ��������� ������������ �����
//virtual bool AConnectToItem(UEPtr<UItem> na, int i_index, int c_index);

// ��������� �������� ����� ��������� ���������� �����
//virtual void ADisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index);

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
virtual void DisconnectFromIndex(int c_index);

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool CheckItem(UEPtr<UItem> item, int item_index, int conn_index);

// ���������, ��������� �� ����������� ��������� item � ����� ����������
bool CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name);

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UItem> &item, int item_index) const;
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UItem> &item, int item_index, int conn_index) const;
bool CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name) const;

/// ���� ������ ��������� ����
virtual NameT FindFreeInputName(void);

// ������������� ����� � ����������� 'c'
virtual bool Connect(UEPtr<UConnector> c, int i_index, int c_index=-1);
virtual bool Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(UEPtr<UConnector> c, int i_index, int c_index);

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
virtual void Disconnect(UEPtr<UConnector> c);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name);

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
virtual bool Disconnect(const UId &id);

// ���������� ������� ����� ���������� ��� ��������� ������.
virtual int GetNumAConnectors(int index) const;
virtual int GetNumAConnectors(const NameT &item_property_name) const;

// ����������  ��������� �� ������ �����������.
virtual UEPtr<UConnector> GetAConnectorByIndex(int output, int index) const;
virtual UEPtr<UConnector> GetAConnectorByIndex(const NameT &item_property_name, int index) const;
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

// ����� �������� ����� ��� ������ ��������
virtual bool Reset(void);
// --------------------------

// ----------------------
// ��������������� ������
// ----------------------
private:
// ��������� ������� ������ �� ������ ������������� �� ����� ����������
//void UpdateInputData(int index);

// ��������� ��������� �� ������� ������ � �������
void UpdatePointers(void);

// ��������� ����������� � ������������ ������ �������� ������
void CalcMinMaxInputDataSize(void);
// ----------------------
};


}


#endif

