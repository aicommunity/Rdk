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

typedef std::map<UId, UEPtr<UComponentDescription> > UClassesDescription;
typedef std::map<UId, UEPtr<UComponentDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<UId, UEPtr<UComponentDescription> >::const_iterator UClassesDescriptionCIterator;

class UAStorage
{
protected: // ��������� ��������
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
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
// ���� classid == ForbiddenId, �� id ����������� �������������
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);

// ������� ������� ������ ������� �� ���������
virtual void DelClass(const UId &classid);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;

// ���������� ������� ������
virtual UEPtr<UAComponent> GetClass(const UId &classid) const;

// ���������� ����� �������
int GetNumClasses(void) const;

// ���������� ������ ��������������� ���� ������� ���������
// max_num_classes - ������������ ����� �������
virtual void GetClassIdList(std::vector<UId> &buffer) const;

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
// ����������
// --------------------------
public:
class IException: public Exception {};

// ������� ������ � ������� �� �������������� classid ������������� � ���������
class EClassIdDontExist;
// --------------------------
};

// ������� ������ � ������� �� �������������� classid ������������� � ���������
class UAStorage::EClassIdDontExist: public EError
{
public: // ������
// ��������� �������������
UId Id;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EClassIdDontExist(UId id);
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

