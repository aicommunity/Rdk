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
#include "UAItem.h"
#include "UItemData.h"

namespace RDK {

class UIDataInfo
{
public: // ������
// ------------------------
// ������������ � �����������
// ------------------------
UIDataInfo(void);
virtual ~UIDataInfo(void);
// ------------------------

// ------------------------
// ������ ���������� ����������� �����
// ------------------------
virtual const type_info& GetType(void) const=0;
virtual bool Compare(const USharedPtr<UIDataInfo> &dt) const=0;
virtual UIDataInfo* New(void)=0;
// ------------------------
};

template<typename T>
class UDataInfo: public UIDataInfo
{
public: // ������
// ------------------------
// ������������ � �����������
// ------------------------
UDataInfo(void) {};
virtual ~UDataInfo(void) {};
// ------------------------

// ------------------------
// ������ ���������� ����������� �����
// ------------------------
virtual const type_info& GetType(void) const
{
 return typeid(T);
};

virtual bool Compare(const USharedPtr<UIDataInfo> &dt) const
{
 return GetType() == dt->GetType();
};

virtual UIDataInfo* New(void)
{
 return new UDataInfo<T>;
};
// ------------------------
};

class UADItem: public UAItem
{
friend class UItemData;
private: // ��������� �������� ������
// ������ �������� ������
vector<UItemData> OutputData;

private: // ��������� ������� ������
// ������ ���������� �� ������� ������
vector<UEPtr<const UItemData> > InputData;

// ������ �������� �������� ������
//vector<size_t> InputDataSize;

protected: // �������� ������ � �������
// �������� �������� ������
vector<USharedPtr<UIDataInfo> > OutputDataInfo;

// �������� ������� ������
vector<USharedPtr<UIDataInfo> > InputDataInfo;

// ����� �������
vector<NameT> OutputNames;

// ����� ������
vector<NameT> InputNames;

protected: // ������������� ��������
// ������ �������� ��������
//vector<size_t> OutputDataSize;

// ������ ���������� ������� ������� ������� � ������
//vector<size_t> OutputDataElementSize;

protected: // ���������� �������� ������� � ������� ������. Read only!
// ��������� �� ������ ������� ������� ���������� �� ������� ������
//const UItemData** PInputData;

// ��������� �� ������ ������� ������� �������� �������� ������
//size_t* PInputDataSize;

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
const UEPtr<const UItemData>& GetInputData(const UEPtr<UAItem> &citem) const;

// ���������� ��������� �� ������ ������ InputData �� �������
// �� ��������� ������ �� ������������
const UEPtr<const UItemData>& GetInputData(size_t index) const;

// ���������� ������ ������� ������ InputData �� �������
// �� ��������� ������ �� ������������
size_t GetInputDataSize(size_t index) const;

// ���������� ��������� ������ ���� �������� ������
size_t GetFullInputDataSize(void) const;
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
// �������� �������� ������
const USharedPtr<UIDataInfo>& GetOutputDataInfo(int index) const;
bool SetOutputDataInfo(int index, const USharedPtr<UIDataInfo> value);

// �������� ������� ������
const USharedPtr<UIDataInfo>& GetInputDataInfo(int index) const;
bool SetInputDataInfo(int index, const USharedPtr<UIDataInfo> value);

// ����� �������
const NameT& GetOutputName(int index) const;
bool SetOutputName(int index, const NameT& name);

// ����� ������
const NameT& GetInputName(int index) const;
bool SetInputName(int index, const NameT& name);

// �������� �������� ������� � �������� ������ � item
virtual bool CopyDataInfo(UEPtr<UADItem> item) const;

// �������� ����� ������ � ������� � item
virtual bool CopyIONames(UEPtr<UADItem> item) const;
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
virtual bool Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor=0, bool copystate=false) const;
// ----------------------

// ----------------------
// ���������������� ������
// ----------------------
protected:
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ���������� false ���� na ��� ��������� � ����� �����.
// ��� �������� ����������� c_index �������� �������� ������ ������������� �����
virtual bool ConnectToItem(UEPtr<UAItem> na, int i_index, int &c_index);

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
virtual void DisconnectFromIndex(int c_index);
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

