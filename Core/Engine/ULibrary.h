/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UALibraryH
#define UALibraryH

//#include "ULibrary.h"
#include "UStorage.h"

namespace RDK {

class ULibrary//: public ULibrary
{
protected: // ������ ������ ��������� ���������
/// ������ ���������� ����������� ���������
//static std::list<ULibrary*> LibraryList;

protected: // ���������
// ��� ����������
string Name;

// ������ ����������
string Version;

/// ��� ����������
/// 0 - ���������� ���������� (������� ������ � �����)
/// 1 - ������� ���������� (��������� �� ������� dll)
/// 2 - ����������, ��������� �� ����� ����������
int Type;

/// ����������� ���������� �� ������ ���������
/// ���� <��� ����������, ������ ����������>
std::vector<pair<string, string> > Dependencies;

protected: // ������ ��������
/// ����� ������� ����������
vector<string> ClassesList;

// �������� ����� ���� ������� ����������� ��������
vector<string> Complete;

// �������� ����� ���� �� ����������� ��������
vector<string> Incomplete;

private: // ���������� ������
// ��������� ��� ��������
UStorage *Storage;
// ������ �������� �������, ����������� �����������
//map<string, UComponent*> ClassSamples;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
ULibrary(const string &name, const string &version, int type=0);
virtual ~ULibrary(void);
// --------------------------

// --------------------------
// ������ ���������� ���������� ������������ ������������
// --------------------------
/*
/// ���������� ��������� ���������� ����������� ���������
static const std::list<ULibrary*>& GetLibraryList(void);

/// ������� ��������� ���������
static void ClearLibraryList(void);

/// ��������� ������� ���������� �� �� �����
static bool CheckLibrary(const std::string &name);

/// ��������� ���������� � ������, ���� ���������� � ����� ������ ��� ��� � ������
static bool AddUniqueLibrary(ULibrary* const lib);

/// ������� ���������� �� ������.
/// ���� ������ ���������� ����, �� ���������� ���
static void RemoveLibrary(ULibrary* const lib);
*/
// --------------------------

// --------------------------
// ������ ������� � ���������� ����������
// --------------------------
// ���������� ��� ����������
const string& GetName(void) const;

// ���������� ������ ����������
const string& GetVersion(void) const;

/// ��� ����������
/// 0 - ���������� ���������� (������� ������ � �����)
/// 1 - ������� ���������� (��������� �� ������� dll)
/// 2 - ����������, ��������� �� ����� ����������
int GetType(void) const;

/// ����������� ���������� �� ������ ���������
const std::vector<pair<string, string> > GetDependencies(void) const;
// --------------------------

// --------------------------
// ������ ������� � ������ ��������
// --------------------------
/// ���������� true ���� ��������� ������������� ����� � ����� ������
bool IsClassNamePresent(const std::string &class_name) const;

/// ����� ������� ����������
const vector<string>& GetClassesList(void) const;

// �������� ����� ���� ������� ����������� ��������
const vector<string>& GetComplete(void) const;

// �������� ����� ���� �� ����������� ��������
const vector<string>& GetIncomplete(void) const;
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ��������� �������� ��������� ������� �������� �������.
// ���� ����� � �������� ������ ��� ����������, �� �� ������������.
// ���������� ����� ������� ����������� �������.
virtual int Upload(UStorage *storage);
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
/// ��������� ����������� ���������� �� ������ ���������
/// � ���������� ������ ����������� ���������
/// ���������� true ���� ��� ����������� ���������� ��� ���������
bool CheckDependencies(UStorage *storage, std::vector<pair<string, string> > &dependencies) const;

// ��������� � ��������� ��������� �����
virtual bool UploadClass(const UId &classid, UEPtr<UComponent> cont);
virtual bool UploadClass(const string &name, UEPtr<UComponent> cont);

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage)=0;
// --------------------------
};

class URuntimeLibrary: public ULibrary
{
protected: // ������ ������ ��������� ���������

protected: // ���������

protected: // ������
/// �������� ��������� ����������
USerStorageXML ClassesStructure;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
URuntimeLibrary(const string &name, const string &version);
virtual ~URuntimeLibrary(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// �������� ��������� ����������
const USerStorageXML& GetClassesStructure(void) const;
bool SetClassesStructure(const USerStorageXML& xml);
bool SetClassesStructure(const std::string &buffer);

/// ��������� ��������� ������� � ������������ � ����������
bool UpdateClassesStructure(void);
// --------------------------

// --------------------------
/// ������� ��������� �� �������� xml
UEPtr<UContainer> CreateClassSample(USerStorage &xml);

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------

};


typedef ULibrary* PUALibrary;

}

#endif
