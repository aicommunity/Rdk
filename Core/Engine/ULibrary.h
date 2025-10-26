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

#include <glog/logging.h>

//#include "ULibrary.h"
#include "UStorage.h"
#include "../../Deploy/Include/rdk_version.h"

#ifndef RDK_LIB_CVS_REVISION
#define RDK_LIB_CVS_REVISION 0
#endif

namespace RDK {

class UMockLibrary;

class RDK_LIB_TYPE ULibrary//: public ULibrary
{
protected: // ������ ������ ��������� ���������
/// ������ ���������� ����������� ���������
//static std::list<ULibrary*> LibraryList;

protected: // ���������
/// ��� ����������
string Name;

/// ������ ����������
string Version;

/// ������� �������� ������
int Revision;

/// ������ ����, ������������� ��� ������ ���������
std::shared_ptr<UVersion> CoreVersion;

/// ��� ����������
/// 0 - ���������� ���������� (������� ������ � �����)
/// 1 - ������� ���������� (��������� �� ������� dll)
/// 2 - ����������, ��������� �� ����� ����������
/// 3 - ����������-�������� (��� ����������-��������)
int Type;

/// ������� ����

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

// ��������� ��� ��������
UStorage *Storage;

private: // ���������� ������
// ��������� ��� ��������
//UStorage *Storage;
// ������ �������� �������, ����������� �����������
//map<string, UComponent*> ClassSamples;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
ULibrary(const string &name, const string &version, int type=0, int revision=RDK_LIB_CVS_REVISION); // Deprecated
ULibrary(const string &name, const string &version, const RDK::UVersion &core_version, int type=0, int revision=RDK_LIB_CVS_REVISION);
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
/// ���������� ��� ����������
const string& GetName(void) const;

/// ���������� ������ ����������
const string& GetVersion(void) const;

/// ���������� ������� ������� �������� ������
int GetRevision(void) const;

/// ���������� ������ ����, ��������������� ��� ������ ����������
const std::shared_ptr<UVersion> GetCoreVersion(void) const;

/// ��� ����������
/// 0 - ���������� ���������� (������� ������ � �����)
/// 1 - ������� ���������� (��������� �� ������� dll)
/// 2 - ����������, ��������� �� ����� ����������
/// 3 - ����������-�������� (��� ����������-��������)
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

// ������� ��� ������ Complete � Incomplete
void ClearIncompleteAndComplete(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ������� ������� ���� T � ������ ������ � ��������� class_name � ������
/// ���������� �� ��������� component_name
/// ���������� ��������� �� ��������� �������
template<class T>
T* UploadClass(const std::string &class_name, const std::string &component_name, bool memory_measure=false);

template<class T>
void UploadClass(const std::string &class_name);

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

/// ��������� � ��������� ��������� �����
virtual bool UploadClass(const UId &classid, std::shared_ptr<UComponent> cont);
virtual bool UploadClass(const string &name, std::shared_ptr<UComponent> cont);
virtual bool UploadClass(const std::string &class_name, const std::string &component_name, UContainer* (*funcPointer)(void));

/// �������� ��������� ������ �� ������ ������� �����������
/// ����� ����������� � ������������� (Incomplete)
virtual void RemoveClassFromCompletedList(const string &name);

/// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
/// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage)=0;

// --------------------------
// �������������� ������
// --------------------------
/// ��������� ����������-�������� ����� XML ���������� ����������� �����������
void FillMockLibrary(UMockLibrary* lib);

// --------------------------
};

class RDK_LIB_TYPE URuntimeLibrary: public ULibrary
{
protected: // ������ ������ ��������� ���������

protected: // ���������

protected: // ������
/// ���� ����������
std::string LibPath;

/// �������� �������� ���������� XML
USerStorageXML CurrentComponentStruct;

/// �������� ��������� ���������� � ������� ����� (xml)
vector<string> ClassesStructures;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
URuntimeLibrary(const string &name, const string &version, const string& path);
virtual ~URuntimeLibrary(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------

/// ��������� ���� ����������
const std::string& GetLibPath() const;

/// ��������� ����� ��������� (��������, ����������)
bool AddNewClass(const std::string &new_class_name, const std::string &new_comp_name, UContainer *newclass);

/// ������� ����� �� ��������� � Storage
bool DelClass(const std::string &class_name);

/// ��������� �������� ��������� �� ������ � ������ �����
bool LoadCompDescriptions(void);

/// ������� ���������� ���������� ������ �� ����� �������
bool DeleteOwnDirectory(void);
// --------------------------

// --------------------------
/// ������� ��������� �� �������� xml
std::shared_ptr<UContainer> CreateClassSample(UStorage *storage, USerStorageXML &xml);

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------

};

class RDK_LIB_TYPE UMockLibrary: public ULibrary
{
protected: // ������ ������ ��������� ���������

protected: // ���������

protected: // ������
/// ���� ����������
std::string LibPath;

/// �������� �������� ���������� XML
USerStorageXML CurrentComponentStruct;

/// �������� ��������� ���������� � ������� ����� (xml)
vector<string> ClassesStructures;
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UMockLibrary(const string &name, const string &version, const string& path);
virtual ~UMockLibrary(void);


// ��������� �������� ���������� � ClassesStructures
bool AddNewCompDescription(USerStorageXML& descript);

// ���������� � ����
bool SaveLibraryToFile();

// �������� �������� ������� �� xml ����� � ClassesStructures
void LoadFromXML(USerStorageXML& xml);
// --------------------------
/// ������� ��������� �� �������� xml
std::shared_ptr<UContainer> CreateClassSample(USerStorageXML &xml, UStorage *storage);

// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------

};


/// ������� ������� ���� T � ������ ������ � ��������� class_name � ������
/// ���������� �� ��������� component_name
/// ���������� ��������� �� ��������� �������
template<class T>
T* ULibrary::UploadClass(const std::string &class_name, const std::string &component_name, bool memory_measure)
{
 unsigned long long total_used_memory_before(0);
 unsigned long long largest_free_block_before(0);
 if(memory_measure)
  ReadUsedMemoryInfo(total_used_memory_before, largest_free_block_before);

 T* cont=new T;
 cont->SetName(component_name);
 UploadClass(class_name, cont);
 cont->Default();
 unsigned long long total_used_memory_after(0);
 unsigned long long largest_free_block_after(0);
 if(memory_measure && ReadUsedMemoryInfo(total_used_memory_after, largest_free_block_after))
  LOG(INFO) << class_name << " eats " << (total_used_memory_after-total_used_memory_before) << " bytes of RAM. Largest RAM block decreased to " << (largest_free_block_before-largest_free_block_after) << " bytes";
 return cont;
}
			   /*
template<class T>
void ULibrary::UploadClass(const std::string &class_name)
{
 UploadClass(class_name, T::NewStatic
}                */


typedef ULibrary* PUALibrary;

}


#endif
