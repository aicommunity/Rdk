/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UASTORAGE_H
#define UASTORAGE_H

#include <map>
#include "../Utilities/UPtr.h"
#include "UAComponent.h"

namespace RDK {

typedef USharedPtr<UAComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;
/*
// ������� ��������� �������� �������
class UClassStorageElement: public USharedPtr<UAComponent>
{
public:
// Id ������
UId Id;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UClassStorageElement(void);
UClassStorageElement(UAComponent *comp, UId id);
// --------------------------
};

// ��������� �������� �������
class UClassesStorage
{
protected: // Data
// Classes list
UClassStorageElement* Classes;

// Number of classes into storage
int Size;

// real number of allocated elements
int RealSize;

public: // Methods
// --------------------------
// ������������ � �����������
// --------------------------
UClassesStorage(void);
UClassesStorage(const UClassesStorage &copy);
virtual ~UClassesStorage(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void Clear(void);

// �������� ������ ���������
// ����� ��������� ������ ������
void Resize(int newsize);

// ���������� ������ �������
int GetSize(void) const;

// ���� ����� �� Id
USharedPtr<UAComponent> Find(const UId &id) const;

// ���� ����� �� Id � ������� ��� �� �������
// ���������� ��������� �� ��������� �����
USharedPtr<UAComponent> Erase(const UId &id);

// ��������� ����� ������� � ����� ���������
void PushBack(const UClassStorageElement &classelement);
void PushBack(const UId &id, UAComponent *component);

// ���������� ��������� �� ������ �������
UClassStorageElement* GetClasses(void) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UClassesStorage& operator = (const UClassesStorage &copy);

// �������� �������
UClassStorageElement& operator [] (int i);
// --------------------------
};    */

class UAStorage
{
protected: // ��������� ��������
UClassesStorage ClassesStorage;

protected: // �������� ��������
// ��������� �������������� Id �������� �������
UId LastClassId;

protected: // ��������� ����������


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAStorage(void);
virtual ~UAStorage(void);
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �������������� Id �������
UId GetLastClassId(void) const;
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
virtual UId AddClass(UAComponent *classtemplate, const UId &classid=ForbiddenId);

// ��������� ������� ������ ������� � ��������� � �������������� ����������� id
// ���������� id ������
//virtual UId AddClass(UAComponent *classtemplate);

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
virtual bool DelClass(const UId &classid);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;

// ���������� ������� ������
virtual USharedPtr<UAComponent> GetClass(const UId &classid) const;

// ���������� ����� �������
int GetNumClasses(void) const;

// ���������� ������ ��������������� ���� ������� ���������
// max_num_classes - ������������ ����� �������
virtual void GetClassIdList(UId* buffer, int max_num_classes) const;

// ������� ��� ������� ������� �� ���������
virtual bool ClearClassesStorage(void);
// --------------------------

// --------------------------
// ������ ���������� ���������� ��������
// --------------------------
// ��������� ������ �� ���������
// ���������� ��������� �� ��������� ������ �� ����� ������
// ��������� ������ ���������� ��� ������� � ���������
// ���� 'Activity' ������� ������������ � true
// ���� ���������� ������� �� ���������� �� ��������� � �����������
// � ���������
virtual UAComponent* TakeObject(const UId &classid, const UAComponent *prototype=0);

// ���������� ������ � ���������
// � ������� ���������� ������ ������� ������ � ���������� true
virtual bool ReturnObject(UAComponent *object);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(const UAComponent *object) const;
// --------------------------

// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
// --------------------------
};

}


#endif

