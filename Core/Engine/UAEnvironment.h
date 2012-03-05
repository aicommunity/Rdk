/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAENVIRONMENT_H
#define UAENVIRONMENT_H

#include "UAStorage.h"
#include "UALibrary.h"

namespace RDK {

class UClassLibraryList
{
public:
protected: // Data
// Classes list
PUALibrary* Libraries;

// Number of libraries into storage
int Size;

// real number of allocated elements
int RealSize;

public: // Methods
// --------------------------
// ������������ � �����������
// --------------------------
UClassLibraryList(void);
UClassLibraryList(const UClassLibraryList &copy);
virtual ~UClassLibraryList(void);
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

// ������� �� ������� ���������� �� �������
// ���������� ��������� �� ��������� ����������
PUALibrary Erase(int index);

// ��������� ����� ������� � ����� ���������
void PushBack(PUALibrary classelement);

// ���������� ��������� �� ������ �������
PUALibrary* GetLibraries(void) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UClassLibraryList& operator = (const UClassLibraryList &copy);

// �������� �������
PUALibrary operator [] (int i);
// --------------------------
};

class UAEnvironment: virtual public UModule
{
protected: // ���������
// ������ ������������� �������� ������ ���������
// 0 - ��������� ������������ �����
int PredefinedStructure;

protected: // ���������
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool StoragePresent;

// ������� �������� �������������
bool Initialized;

// ������� ������� �������������� ���������
bool Structured;

// ������������� ���������� ������, ������� ����� �������������
ULongId ModelCalculationComponent;

protected: // ������
// ���������
UAStorage* Storage;

// ����������� ������
UEPtr<UAContainer> Model;

// ������ ��������� ���������
UClassLibraryList ClassLibraryList;

// ������ ���� ����������� �������
//vector<string> CompletedClassNames;

// ������ ���� �� ����������� �������
//vector<string> IncompletedClassNames;

protected: // �����������

protected: // ������ ������������ ��������� ������������


protected: // ���������� �������� �������
// ������� ��������� ������
UEPtr<UAComponent> CurrentComponent;

public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UAEnvironment(void);
virtual ~UAEnvironment(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ������ ������������� �������� ������ ���������
int GetPredefinedStructure(void) const;
bool SetPredefinedStructure(int value);

// ������������� ���������� ������, ������� ����� �������������
const ULongId& GetModelCalculationComponent(void) const;
bool SetModelCalculationComponent(const ULongId& value);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool IsStoragePresent(void) const;

// ���������� ��������� �������������
virtual bool IsInit(void) const;

// ������� ������� �������������� ���������
virtual bool IsStructured(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
virtual UAStorage* GetStorage(void);

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
virtual bool SetStorage(UAStorage *storage);

// ���������� ��������� �� ������
UEPtr<UAContainer> GetModel(void);

// ������� ����� ������ �� ��������� �� id ������
virtual bool CreateModel(const UId& classid);

// ���������� ������� ������
virtual bool DestroyModel(void);

// ���������� ���������� �� �������
UALibrary* GetClassLibrary(int index);

// ���������� ����� ���������
int GetNumClassLibraries(void) const;

// ��������������� ��������� ����� ������� ������ � ���������
virtual bool AddClass(UAContainer *newclass);

// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
virtual bool AddClassLibrary(UALibrary *library);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelClassLibrary(int index);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
virtual bool BuildStorage(void);
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
// ���������� ��������� �� ������
//NModel* const operator -> (void);

// ���������� ��������� �� ��������� ������ �� �������� �����
// �������� ��������� ���������� �������
// ���� ��������� �� ������, �� ���������� 0 � ���������� ������� ���������
//UAComponent* operator () (const string &longname);

// ���������� ��������� �� ��������� ������ �� �������� ��������������
// �������� ��������� ���������� �������
// ���� ��������� �� ������, �� ���������� 0 � ���������� ������� ���������
//UAComponent* operator () (const LongUId &longid);

// ���������� ��������� �� ������� ��������� ������
UEPtr<UAComponent> operator () (void);
// --------------------------

// --------------------------
// ������ ������������ ��������� ������������
// --------------------------
// --------------------------


// --------------------------
// ������ ����������
// --------------------------
public:
// ������������� �����
virtual bool Init(void);

// ��������������� �����
virtual bool UnInit(void);

// ��������� �������������� �������� ������ ���������
virtual bool CreateStructure(void);

// ���������� ������� ������ ���������
virtual bool DestroyStructure(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ������������� �����
virtual bool AInit(void);

// ��������������� �����
virtual bool AUnInit(void);

// ��������� �������������� �������� ������ ���������
virtual bool ACreateStructure(void);

// ���������� ������� ������ ���������
virtual bool ADestroyStructure(void);

protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------

// --------------------------
// ��������������� ������ ������������� �����
// --------------------------
protected:
// --------------------------
};

}
#endif
