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
protected:
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index);
virtual bool ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index);

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
virtual void DisconnectFromIndex(int c_index);
virtual void DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index);
//virtual void DisconnectFromIndex(const NameT &connector_property_name, int index=0);
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

