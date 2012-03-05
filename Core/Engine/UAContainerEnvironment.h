/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UACONTAINERENVIRONMENT_H
#define UACONTAINERENVIRONMENT_H

#include "UAEnvironment.h"
#include "UAContainerStorage.h"
//#include "ULibrary.h"

namespace RDK {

class UAContainerEnvironment: public UAEnvironment
{
protected: // ���������

protected: // ������
// ������ ���� ����������� �������
vector<string> CompletedClassNames;

// ������ ���� �� ����������� �������
vector<string> IncompletedClassNames;

protected: // �����������
// ������ ����������� ������������
// <����������, ����, ����������� ������� ���������� �����>
//map<NController*, bool> Controllers;

protected: // ������ ������������ ��������� ������������
// ������������� ������
//map<LongIdT, NGraphics*> Graphics;

protected: // ����� ���������

protected: // ���������� �������� �������


public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UAContainerEnvironment(void);
virtual ~UAContainerEnvironment(void);
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
virtual UAContainerStorage* GetStorage(void);

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
//virtual bool SetStorage(UContainerStorage *storage);

// ���������� ��������� �� ������
//UAComponent* GetModel(void);

// ������� ����� ������ �� ��������� �� id ������
virtual bool CreateModel(const UId& classid);

// ������� ����� ������ �� ��������� �� ����� ������
virtual bool CreateModel(const NameT& classname);

// ���������� ������� ������
//virtual bool DestroyModel(void);

// ���������� ���������� �� �������
//ULibrary* GetClassLibrary(int index);

// ���������� ���������� �� �����
UALibrary* GetClassLibrary(const string &name);

// ���������� ��� ���������� �� �������
const string& GetClassLibraryName(int index);

// ���������� ������ ���������� �� �������
const string& GetClassLibraryVersion(int index);

// ���������� ����� ���������
//size_t GetNumClassLibraries(void) const;

// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool AddClassLibrary(UALibrary *library);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool DelClassLibrary(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool DelClassLibrary(const string &name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool BuildStorage(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������� � ������ �����
// --------------------------

// --------------------------
// ��������� ������� � ������ �����
// --------------------------
// ���������� ��������� �� ������� ��������� ������
UEPtr<UAContainer> GetCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������
// ���� ��� ��� id �� ������, ��� Forbidden, �� �������������
// ��������� �� ������
virtual void SelectCurrentComponent(const NameT &name);
virtual void SelectCurrentComponent(const ULongId &id);

// ������������� ��������� �� ������� ��������� ������ �� ���� ������
virtual void ResetCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������ �� ������������ ���������
// (������� �� ������� �����). ���� ��� ��������� �� ������, �� �� ������ ������
virtual void UpCurrentComponent(void);

// ������������� ��������� �� ������� ��������� ������ �� �������� ��������� ��
// ����� ������ (������� �� ������� ����).
virtual void DownCurrentComponent(const NameT &name);
virtual void DownCurrentComponent(const ULongId &id);
// --------------------------

// --------------------------

// --------------------------
// ������ ������������ ��������� ������������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ������ ���������
// --------------------------
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// --------------------------

// --------------------------
// ��������������� ������ ������������� �����
// --------------------------
protected:
// ��������� ������������ ���������� � ����������� ��������� � ����������
// ��������� �� ����.
// � ������� ������ �� ������ ������ � ���������� 0.
virtual UAContainerStorage* LoadStorageLibrary(const string &filename);

// ��������� ������������ ���������� � ����������� ���������.
// � ������� ������ �� ������ ������ � ���������� true.
virtual bool UnLoadStorageLibrary(void);

// ��������� ������������ ���������� � ������� �������� �������
// � ���� ���������� ��������� � ���������� ��������� �� ����.
// � ������� ������ �� ������ ������ � ���������� 0.
virtual UAContainerStorage* LoadClassLibrary(const string &libname, const string &filename);

// ��������� ������������ ���������� � ������� �������� �������.
// � ������� ������ �� ������ ������ � ���������� true.
virtual bool UnLoadClassLibrary(const string &libname);
// --------------------------
};

}
#endif
