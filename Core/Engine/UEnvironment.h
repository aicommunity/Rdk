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

#include "UStorage.h"
#include "ULibrary.h"

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

class UEnvironment: virtual public UModule
{
protected: // ���������
// ������ ������������� �������� ������ ���������
// 0 - ��������� ������������ �����
int PredefinedStructure;

// ��� �������� �������� �������� ������
std::string CurrentDataDir;

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
UStorage* Storage;

// ����������� ������
UEPtr<UContainer> Model;

// ������ ��������� ���������
UClassLibraryList ClassLibraryList;

// ������ ���� ����������� �������
vector<string> CompletedClassNames;

// ������ ���� �� ����������� �������
vector<string> IncompletedClassNames;

/// ����� �����
UTimeControl Time;

// ������������ ��������� �������, ������� ������ �������� "�������" � ������
// ������� ��������� ������� (��)
double MaxModelDuration;

protected: // �����������

protected: // ������ ������������ ��������� ������������


protected: // ���������� �������� �������
// ������� ��������� ������
UEPtr<UComponent> CurrentComponent;

protected: // ��������� ����������
long long StartupTime;

long long /*StartProcTime,*/CurrentTime,LastDuration/*,LastSentTime*/, ProcEndTime;

public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment(void);
virtual ~UEnvironment(void);
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

// ��� �������� �������� �������� ������
const std::string& GetCurrentDataDir(void);
void SetCurrentDataDir(const std::string& dir);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool IsStoragePresent(void) const;

// ������� ������� �������������� ���������
virtual bool IsStructured(void) const;
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
virtual UStorage* GetStorage(void);

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
virtual bool SetStorage(UStorage *storage);

// ���������� ��������� �� ������
UEPtr<UContainer> GetModel(void);

// ������� ����� ������ �� ��������� �� ����� ������
virtual bool CreateModel(const NameT& classname);

// ������� ����� ������ �� ��������� �� id ������
virtual bool CreateModel(const UId& classid);

// ���������� ������� ������
virtual bool DestroyModel(void);

// ���������� ���������� �� �������
ULibrary* GetClassLibrary(int index);

// ���������� ����� ���������
int GetNumClassLibraries(void) const;

// ���������� ���������� �� �����
ULibrary* GetClassLibrary(const string &name);

// ���������� ��� ���������� �� �������
const string& GetClassLibraryName(int index);

// ���������� ������ ���������� �� �������
const string& GetClassLibraryVersion(int index);

// ��������������� ��������� ����� ������� ������ � ���������
virtual bool AddClass(UContainer *newclass);

// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
virtual bool AddClassLibrary(ULibrary *library);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelClassLibrary(int index);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool DelClassLibrary(const string &name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelAllClassLibraries(void);

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
virtual bool BuildStorage(void);

/// ����� �����
const UTimeControl& GetTime(void) const;
UTimeControl& GetTime(void);
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
// ���������� ��������� �� ������� ��������� ������
UEPtr<UContainer> GetCurrentComponent(void);

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
// ������ ���������� ������
// --------------------------
public:
// ���������� ���������� ������� ������ �� ��������� ��������
void IncreaseModelTimeByStep(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
public:
// ������������� �����
virtual void Init(void);

// ��������������� �����
virtual void UnInit(void);

// ��������� �������������� �������� ������ ���������
virtual bool CreateStructure(void);

// ���������� ������� ������ ���������
virtual bool DestroyStructure(void);

// ������ ������ � �������� �������
virtual void RTCalculate(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ������������� �����
virtual void AInit(void);

// ��������������� �����
virtual void AUnInit(void);

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
};

}
#endif
