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
protected: // ��������� ��������
// ������� ������������ ���� � Id �������� �������
map<string,UId> ClassesLookupTable;

protected: // �������� ��������
// ������ ��������
UObjectsStorage ObjectsStorage;

// ��������� �������������� Id �������� �������
UId LastClassId;

protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAContainerStorage(void);
virtual ~UAContainerStorage(void);
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
// ��������� ������� ������ ������� � ���������
// ���������� id ������
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid=ForbiddenId);

protected:
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);
public:
// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
virtual void DelClass(const UId &classid);

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassIdList(std::vector<UId> &buffer) const;
//virtual void GetClassIdList(UIdVector &buffer) const;

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassNameList(vector<string> &buffer) const;

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
// ������ ���������� ��������� �������
// --------------------------
public:
// ��������� ����� �������� ���� ������� � xml
virtual bool SaveCommonClassesDescription(Serialize::USerStorageXML &xml);

// ��������� ����� �������� ���� ������� �� xml
virtual bool LoadCommonClassesDescription(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// ������� ������ ���������� ��������� �������
// --------------------------
// ��������� �������� XML �������� ��� ��������� ������
// ����� � ��������� ������ ������������
// ������� ��� ��������� ���� ��������
//virtual bool GenerateClassDescription(UClassesStorageIterator classI,
//										Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
protected:
// ��������� ����� � ������ 'name' � ������� ������������
virtual UId AddLookupClass(const string &name);

// ������� ����� � ������ 'name' �� ������� �����������
virtual void DelLookupClass(const string &name);
// --------------------------

// --------------------------
// ����������
// --------------------------
public:
// ������� ������ � ������� �� �����, �������������� � ���������
class EClassNameDontExist;

// ������������ ��� ������
class EInvalidClassName;

// ������� ������ � ������� �� ��������������, �������������� � ���������
class EObjectIdDontExist;

// ������� ���������� ����������� �������� � �������, ������� �������� ������������ � ���������
class EObjectStorageNotEmpty;

// ����� � �������� ������ ��� ����������
class EClassNameAlreadyExist;
// --------------------------
};

// ������� ������ � ������� �� �����, �������������� � ���������
class UAContainerStorage::EClassNameDontExist: public EError
{
public: // ������
// ��������� �������������
std::string Name;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EClassNameDontExist(const std::string &name);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������������ ��� ������
class UAContainerStorage::EInvalidClassName: public EError
{
public: // ������
// ��������� �������������
std::string Name;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EInvalidClassName(const std::string &name);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������� ������ � ������� �� ��������������, �������������� � ���������
class UAContainerStorage::EObjectIdDontExist: public EError
{
public: // ������
// ��������� �������������
UId Id;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EObjectIdDontExist(UId id);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������� ���������� ����������� �������� � �������, ������� �������� ������������ � ���������
class UAContainerStorage::EObjectStorageNotEmpty: public UAStorage::EClassIdDontExist
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
EObjectStorageNotEmpty(UId id) : UAStorage::EClassIdDontExist(id) {};
// --------------------------
};

// ����� � �������� ������ ��� ����������
class UAContainerStorage::EClassNameAlreadyExist: public EError
{
public: // ������
// ��������� ���
std::string Name;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EClassNameAlreadyExist(const std::string &name);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};



}

#endif

