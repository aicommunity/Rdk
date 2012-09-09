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
UEPtr<UAContainer> Object;

// ������� ���� �������� �� ������
bool UseFlag;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
UInstancesStorageElement(const UEPtr<UAContainer> &object, bool useflag);
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
friend class UAContainer;
protected: // �������� ��������
// ������ ��������
UObjectsStorage ObjectsStorage;

protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAContainerStorage(void);
virtual ~UAContainerStorage(void);
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
//protected:
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid=ForbiddenId);
//public:
// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
virtual void DelClass(const UId &classid);

// ������� ��� �� ������������ ������� ������� �� ���������
virtual void FreeClassesStorage(void);

// ������� ��� ������� ������� �� ���������
// ���������� false ���� � ��������� ������������ �������
virtual void ClearClassesStorage(void);
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
virtual UEPtr<UAComponent> TakeObject(const UId &classid, const UEPtr<UAComponent> prototype=0);
virtual UEPtr<UAComponent> TakeObject(const string &classname, const UEPtr<UAComponent> prototype=0);

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(UEPtr<UAContainer> object) const;

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
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
protected:
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
virtual void PushObject(const UId &classid, UEPtr<UAContainer> object);

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UEPtr<UAContainer> object);

// ���������� ������ � ������ ���������
virtual void MoveObject(UEPtr<UAContainer> object, UEPtr<UAContainerStorage> newstorage);

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
virtual void ReturnObject(UEPtr<UAComponent> object);

// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator);
// --------------------------

// --------------------------
// ����������
// --------------------------
public: // ������ �������� ����������
class IException: public UException {};

public:
// ������� ������ � ������� �� ��������������, �������������� � ���������
struct EObjectIdNotExist: public EIdNotExist
{
EObjectIdNotExist(UId id) : EIdNotExist(id) {};
};

// ������� ���������� ����������� �������� � �������, ������� �������� ������������ � ���������
struct EObjectStorageNotEmpty: public EIdError
{
EObjectStorageNotEmpty(UId id) : EIdError(id) {};
};
// --------------------------
};
     /*
// ������� ������ � ������� �� ��������������, �������������� � ���������
class UAContainerStorage::EObjectIdNotExist: public EError
{
public: // ������
// ��������� �������������
UId Id;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EObjectIdNotExist(UId id);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};         */
				  /*
// ������� ���������� ����������� �������� � �������, ������� �������� ������������ � ���������
class UAContainerStorage::EObjectStorageNotEmpty: public UAStorage::EClassIdNotExist
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
EObjectStorageNotEmpty(UId id) : UAStorage::EClassIdNotExist(id) {};
// --------------------------
};
              */

}

#endif

