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
#include "UAComponent.h"
#include "../Serialize/USerStorageXML.h"
#include "UComponentDescription.h"

namespace RDK {

typedef UEPtr<UAComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::map<std::string, UEPtr<UComponentDescription> > UClassesDescription;
typedef std::map<std::string, UEPtr<UComponentDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<std::string, UEPtr<UComponentDescription> >::const_iterator UClassesDescriptionCIterator;

class UAStorage
{
protected: // ��������� ��������
// ������� ������������ ���� � Id �������� �������
std::map<std::string,UId> ClassesLookupTable;

// ��������� �������� �������
UClassesStorage ClassesStorage;

protected: // �������� �������
// XML �������� ���� ������� ���������
UClassesDescription ClassesDescription;

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
// ���������� Id ������ �� ��� �����
const UId& GetClassId(const std::string &name) const;

// ���������� ��� ������ �� ��� Id
const NameT GetClassName(const UId &id) const;
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
// ���� classid == ForbiddenId, �� id ����������� �������������
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const std::string &classname, const UId &classid=ForbiddenId);

// ������� ������� ������ ������� �� ���������
virtual void DelClass(const UId &classid);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;

// ���������� ������� ������
virtual UEPtr<UAComponent> GetClass(const UId &classid) const;

// ���������� ����� �������
int GetNumClasses(void) const;

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassNameList(std::vector<std::string> &buffer) const;



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
virtual UEPtr<UAComponent> TakeObject(const UId &classid, const UEPtr<UAComponent> prototype=0);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(UEPtr<UAComponent> object) const;
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
// --------------------------
protected:
// ���������� ������ � ���������
// � ������� ���������� ������ ������� ������ � ���������� true
virtual void ReturnObject(UEPtr<UAComponent> object);
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
class IException: public Exception {};

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
// --------------------------
};
	 /*
// ������� ������ � ������� �� �������������� classid ������������� � ���������
class UAStorage::EClassIdNotExist: public EError
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
class UAStorage::EClassNameNotExist: public EError
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
class UAStorage::EInvalidClassName: public EError
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
class UAStorage::EClassNameAlreadyExist: public EError
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

