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
#include "UEPtr.h"
#include "UContainer.h"
#include "../Serialize/USerStorageXML.h"
#include "UComponentDescription.h"

namespace RDK {

/* *********************************************************************** */
typedef UEPtr<UComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::map<std::string, UEPtr<UComponentDescription> > UClassesDescription;
typedef std::map<std::string, UEPtr<UComponentDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<std::string, UEPtr<UComponentDescription> >::const_iterator UClassesDescriptionCIterator;

/* *********************************************************************** */
// ������� ������ ������������ �������� ������������� ������
class UInstancesStorageElement
{
public: // ������
// ��������� �� ������
UEPtr<UContainer> Object;

// ������� ���� �������� �� ������
bool UseFlag;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
UInstancesStorageElement(const UEPtr<UContainer> &object, bool useflag);
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

class UStorage
{
friend class UContainer;
protected: // ��������� ��������
// ������� ������������ ���� � Id �������� �������
std::map<std::string,UId> ClassesLookupTable;

// ��������� �������� �������
UClassesStorage ClassesStorage;

protected: // �������� �������
// XML �������� ���� ������� ���������
UClassesDescription ClassesDescription;

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
UStorage(void);
virtual ~UStorage(void);
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
// ���������� Id ������ �� ��� �����
const UId& FindClassId(const std::string &name) const;

// ���������� ��� ������ �� ��� Id
const NameT FindClassName(const UId &id) const;
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
// ���� classid == ForbiddenId, �� id ����������� �������������
virtual UId AddClass(UEPtr<UComponent> classtemplate, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UComponent> classtemplate, const std::string &classname, const UId &classid=ForbiddenId);

// ������� ������� ������ ������� �� ���������
virtual void DelClass(const UId &classid);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;

// ���������� ������� ������
virtual UEPtr<UComponent> GetClass(const UId &classid) const;

// ���������� ����� �������
int GetNumClasses(void) const;

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassNameList(std::vector<std::string> &buffer) const;

// ������� ��� �� ������������ ������� ������� �� ���������
virtual void FreeClassesStorage(void);

// ������� ��� ������� ������� �� ���������
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
virtual UEPtr<UComponent> TakeObject(const UId &classid, const UEPtr<UComponent> prototype=0);
virtual UEPtr<UComponent> TakeObject(const string &classname, const UEPtr<UComponent> prototype=0);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(UEPtr<UComponent> object) const;

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(UEPtr<UContainer> object) const;

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
// ������ ���������� ��������� �������
// --------------------------
// ���������� XML �������� ������
const UEPtr<UComponentDescription> GetClassDescription(const UId &classid) const;

// ������������� XML �������� ������
// ����� � ��������� ������ ������������
void SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description);

// ��������� �������� ������ � xml
virtual void SaveClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml);

// ��������� �������� ������ �� xml
virtual void LoadClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml);

// ��������� �������� ���� ������� � xml
virtual void SaveClassesDescription(Serialize::USerStorageXML &xml);

// ��������� �������� ���� ������� �� xml
virtual void LoadClassesDescription(Serialize::USerStorageXML &xml);

// ��������� ����� �������� ���� ������� � xml
virtual bool SaveCommonClassesDescription(Serialize::USerStorageXML &xml);

// ��������� ����� �������� ���� ������� �� xml
virtual bool LoadCommonClassesDescription(Serialize::USerStorageXML &xml);
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
protected:
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
virtual void PushObject(const UId &classid, UEPtr<UContainer> object);

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UEPtr<UContainer> object);

// ���������� ������ � ������ ���������
virtual void MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage);

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
virtual void ReturnObject(UEPtr<UComponent> object);

// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator);
// --------------------------


// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
protected:
// ��������� ����� � ������ 'name' � ������� ������������
virtual UId AddLookupClass(const std::string &name);

// ������� ����� � ������ 'name' �� ������� �����������
virtual void DelLookupClass(const std::string &name);
// --------------------------


// --------------------------
// ����������
// --------------------------
public:
class IException: public UException {};

// ������� ������ � ������� �� �������������� classid ������������� � ���������
struct EClassIdNotExist: public EIdNotExist
{
EClassIdNotExist(UId id) : EIdNotExist(id) {};
};

// ����� � �������� ������ ��� ����������
struct EClassIdAlreadyExist: public EIdAlreadyExist
{
EClassIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ������� ������ � ������� �� �����, �������������� � ���������
struct EClassNameNotExist: public ENameNotExist
{
EClassNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ����� � �������� ������ ��� ����������
struct EClassNameAlreadyExist: public ENameAlreadyExist
{
EClassNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ������������ ��� ������
struct EInvalidClassName: public ENameError
{
EInvalidClassName(const std::string &name) : ENameError(name) {};
};

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

// --------------------------
};
	 /*
// ������� ������ � ������� �� �������������� classid ������������� � ���������
class UStorage::EClassIdNotExist: public EError
{
public: // ������
// ��������� �������������
UId Id;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EClassIdNotExist(UId id);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������� ������ � ������� �� �����, �������������� � ���������
class UStorage::EClassNameNotExist: public EError
{
public: // ������
// ��������� �������������
std::string Name;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EClassNameNotExist(const std::string &name);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ������������ ��� ������
class UStorage::EInvalidClassName: public EError
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

// ����� � �������� ������ ��� ����������
class UStorage::EClassNameAlreadyExist: public EError
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

                  */
}


#endif

