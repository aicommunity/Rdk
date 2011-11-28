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

#include <map>
#include <string>
#include <vector>
#include "UAStorage.h"
#include "UAContainer.h"

namespace RDK {



/* *********************************************************************** */
/* *********************************************************************** */
// ������� ������ ������������ �������� ������������� ������
class UInstancesStorageElement
{
public: // ������
// ��������� �� ������
UESharedPtr<UAContainer> Object;

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
typedef list<UInstancesStorageElement> UInstancesStorage;
typedef list<UInstancesStorageElement>::iterator UInstancesStorageIterator;

typedef pair<UId,UInstancesStorage> UObjectStorageElement;

typedef map<UId, UInstancesStorage> UObjectsStorage;
typedef map<UId, UInstancesStorage>::iterator UObjectsStorageIterator;
typedef map<UId, UInstancesStorage>::const_iterator UObjectsStorageCIterator;

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

protected:
virtual UId AddClass(UAComponent *classtemplate, const UId &classid=ForbiddenId);
public:

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
virtual UESharedPtr<UAComponent> TakeObject(const UId &classid, const UAComponent *prototype=0);
virtual UESharedPtr<UAComponent> TakeObject(const string &classname, const UAComponent *prototype=0);

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
virtual bool ReturnObject(UEPtr<UAComponent> object);

// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
virtual bool PushObject(const UId &classid, UESharedPtr<UAContainer> object);

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UEPtr<UAContainer> object);

// ���������� ������ � ������ ���������
virtual bool MoveObject(UESharedPtr<UAContainer> object, UAContainerStorage *newstorage);

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(UESharedPtr<UAContainer> object) const;

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

