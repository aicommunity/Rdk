/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UACONTAINER_STORAGE_H
#define UACONTAINER_STORAGE_H

//#include <map>
//#include <string>
//#include <vector>
#include "UAStorage.h"
#include "UAContainer.h"

namespace RDK {

/* *********************************************************************** */
/* *********************************************************************** */
// ������� ������ ������������ �������� ������������� ������
class UInstancesStorageElement
{
friend class UInstancesStorage;
public: // ��������� ������
// ��������� �� ���������� ������� ������
UInstancesStorageElement *Prev;

// ��������� �� ��������� ������� ������
UInstancesStorageElement *Next;

public: // ������ ����������
// ��������� �� ������
UAContainer* Object;

// ������� ���� �������� �� ������
bool UseFlag;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
virtual ~UInstancesStorageElement(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UInstancesStorageElement& operator = (const UInstancesStorageElement &copy);

// ��������� ���������
bool operator < (const UInstancesStorageElement &value);
bool operator > (const UInstancesStorageElement &value);
bool operator <= (const UInstancesStorageElement &value);
bool operator >= (const UInstancesStorageElement &value);
bool operator == (const UInstancesStorageElement &value);
bool operator != (const UInstancesStorageElement &value);
// --------------------------
};

typedef UInstancesStorageElement* PUInstancesStorageElement;


// ������ ������������ �������� ������������� ������ � ������ �������������
// <��������� �������, ���� true ���� ����� ��� false ���� ��������>
class UInstancesStorage
{
protected: // ��������� ������
// ��������� �� ������ ������� ������
UInstancesStorageElement* First;

// ��������� �� ��������� ������� ������
UInstancesStorageElement* Last;

// ������ ������
int Size;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorage(void);
UInstancesStorage(const UInstancesStorage& copy);
virtual ~UInstancesStorage(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������� ������
void Clear(void);

// ���������� ������ ������
int GetSize(void) const;

// ��������� ������� � ������ � ���������� ��������� �� ���������� �������
// �������� ������������� ����������� �� object
// ����� �������� ������ �� ������������ � ��������� ������������ ������ ����
// ����� ������ ��� ����������
UInstancesStorageElement* Add(UInstancesStorageElement* value);
UInstancesStorageElement* Add(UAContainer* object, bool useflag);

// ������� ������� �� ������
void Del(UAContainer* object);

// ������� ������� �� ������ ������ � ��� ������, ���� value �������� ��������� ������
void Del(UInstancesStorageElement* value);

// ���� �������� ������ � ���������� ��������� �� ������� ������ � ���� ��������
UInstancesStorageElement* Find(const UAContainer* object);
const UInstancesStorageElement* Find(const UAContainer* object) const;

// ���� "���������" � ���������� ��������� �� ������� ������ � ���� ��������
UInstancesStorageElement* FindFree(void);

// ��������� �� ������ ������� ������
UInstancesStorageElement* GetFirst(void);
const UInstancesStorageElement* GetFirst(void) const;

// ��������� �� ��������� ������� ������
UInstancesStorageElement* GetLast(void);
const UInstancesStorageElement* GetLast(void) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UInstancesStorage& operator = (const UInstancesStorage &copy);
// --------------------------

};
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// ������� ��������� �������� �������
struct UObjectStorageElement
{
UId Id;
UInstancesStorage Objects;

// --------------------------
// ������������ � �����������
// --------------------------
UObjectStorageElement(void);
UObjectStorageElement(const UObjectStorageElement &copy);
virtual ~UObjectStorageElement(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UObjectStorageElement& operator = (const UObjectStorageElement &copy);
// --------------------------
};

// ��������� �������� �������� �� ������ �� Id �������
class UObjectsStorage
{
protected: // Data
// Objects list
UObjectStorageElement* Objects;

// Number of classes into storage
int Size;

// real number of allocated elements
int RealSize;

public: // Methods
// --------------------------
// ������������ � �����������
// --------------------------
UObjectsStorage(void);
UObjectsStorage(const UObjectsStorage &copy);
virtual ~UObjectsStorage(void);
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

// ���� ������ �� Id
UInstancesStorage* Find(const UId &id);
const UInstancesStorage* Find(const UId &id) const;

// ���� ����� �� Id � ������� ��� �� �������
void Erase(const UId &id);

// ��������� ����� ������� � ����� ���������
UObjectStorageElement* PushBack(const UObjectStorageElement &classelement);
UObjectStorageElement* PushBack(const UId &id, UInstancesStorage *instance);

// ������� ����� ������� � ��������� � �������� id � ���������� ��������� �� ����
// ���� ����� ������� ��� ���������� �� ���������� ��������� �� ���� �������
UInstancesStorage* Create(const UId &id);

// ���������� ��������� �� ������ �������
UObjectStorageElement* GetObjects(void) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UObjectsStorage& operator = (const UObjectsStorage &copy);

// �������� �������
UObjectStorageElement& operator [] (int i);
const UObjectStorageElement& operator [] (int i) const;
// --------------------------
};
/* *********************************************************************** */
/* *********************************************************************** */


/* *********************************************************************** */
/* *********************************************************************** */
class UAContainerStorage: public UAStorage
{
protected: // ��������� ��������
// ������� ������������ ���� � Id �������� �������
map<string,UId> ClassesLookupTable;

protected: // �������� ��������
// ������ ��������� ������� ��������
//ClassesStorageT ClassesStorage;

// ������ ��������
UObjectsStorage ObjectsStorage;

// ��������� �������������� Id �������� �������
UId LastClassId;

protected: // ��������� ����������
// ������� ���������� �� ��������� ������� �������
//UFreeObjectsTable FreeObjectsTable;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAContainerStorage(void);
virtual ~UAContainerStorage(void);
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
// ���������� Id ������ �� ��� �����
const UId& GetClassId(const string &name) const;

// ���������� ��� ������ �� ��� Id
const NameT GetClassName(const UId &id) const;
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ��������� � �������������� ����������� id
// ���������� id ������
//virtual UId AddClass(UAComponent *classtemplate);

// ��������� ������� ������ ������� � ���������
// ���������� id ������
virtual UId AddClass(UAComponent *classtemplate, const string &classname, const UId &classid=ForbiddenId);

// ���������� id ������
//virtual UId AddClass(const string &classname, UAComponent *classtemplate);

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
virtual bool DelClass(const UId &classid);

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassIdList(UId* buffer, int max_num_classes) const;
virtual void GetClassIdList(UIdVector &buffer) const;

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassNameList(vector<string> &buffer) const;

// ������� ��� �� ������������ ������� ������� �� ���������
virtual void FreeClassesStorage(void);

// ������� ��� ������� ������� �� ���������
// ���������� false ���� � ��������� ������������ �������
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
virtual UAContainer* TakeObject(const UId &classid, const UAComponent *prototype=0);
virtual UAComponent* TakeObject(const string &classname, const UAComponent *prototype=0);

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
virtual bool ReturnObject(UAComponent *object);

// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
virtual bool PushObject(const UId &classid, UAContainer *object);

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UAContainer *object);

// ���������� ������ � ������ ���������
virtual bool MoveObject(UAContainer *object, UAContainerStorage *newstorage);

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(const UAContainer *object) const;

// ��������� ��������� ����� �������� � ���������
virtual int CalcNumObjects(void) const;
virtual int CalcNumObjects(const UId &classid) const;
virtual size_t CalcNumObjects(const string &classname) const;

// �������� ��� ��������� ������� �� ���������
virtual void FreeObjectsStorage(void);

// ������� ��� ������� �� ���������
virtual void ClearObjectsStorage(void);
// --------------------------

// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
protected:
// ��������� ����� � ������ 'name' � ������� ������������
virtual UId AddLookupClass(const string &name);

// ������� ����� � ������ 'name' �� ������� �����������
virtual bool DelLookupClass(const string &name);
// --------------------------
};

}

#endif

