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

namespace RDK {

typedef UEPtr<UAComponent> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

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
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
virtual UId AddClass(UEPtr<UAComponent> classtemplate, const UId &classid=ForbiddenId);

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
virtual bool DelClass(const UId &classid);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;

// ���������� ������� ������
virtual UEPtr<UAComponent> GetClass(const UId &classid) const;

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
virtual UEPtr<UAComponent> TakeObject(const UId &classid, const UAComponent *prototype=0);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(UEPtr<UAComponent> object) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� ��������
// --------------------------
protected:
// ���������� ������ � ���������
// � ������� ���������� ������ ������� ������ � ���������� true
virtual bool ReturnObject(UEPtr<UAComponent> object);
// --------------------------
};

}


#endif

