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
#include <unordered_map>
#include "UEPtr.h"
#include "UContainer.h"
#include "../Serialize/USerStorageXML.h"
#include "UContainerDescription.h"

#include "UComponentFactory.h"


namespace RDK {

/* *********************************************************************** */
class ULibrary;
class URuntimeLibrary;

typedef UEPtr<UComponentAbstractFactory> UClassStorageElement;
typedef std::unordered_map<UId, UClassStorageElement> UClassesStorage;
typedef std::unordered_map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::unordered_map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::unordered_map<std::string, UEPtr<UContainerDescription> > UClassesDescription;
typedef std::unordered_map<std::string, UEPtr<UContainerDescription> >::iterator UClassesDescriptionIterator;
typedef std::unordered_map<std::string, UEPtr<UContainerDescription> >::const_iterator UClassesDescriptionCIterator;

typedef std::vector<ULibrary*> UClassLibraryList;
/* *********************************************************************** */
// ������� ������ ������������ �������� ������������� ������
class RDK_LIB_TYPE UInstancesStorageElement
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

typedef std::unordered_map<UId, UInstancesStorage> UObjectsStorage;
typedef std::unordered_map<UId, UInstancesStorage>::iterator UObjectsStorageIterator;
typedef std::unordered_map<UId, UInstancesStorage>::const_iterator UObjectsStorageCIterator;

class UMockUNet;
// ��������� �� �������, ������� ������� ��� ��������� ��������
// �������� �������� USerStorageXML ��� UMockUNet
typedef bool (*funcCrPropMock)(USerStorageXML*, UMockUNet*);

/* *********************************************************************** */


class RDK_LIB_TYPE UStorage
{
//friend class UContainer;
protected: // ��������� ��������
// ������� ������������ ���� � Id �������� �������
std::unordered_map<std::string,UId> ClassesLookupTable;

// ��������� �������� �������
UClassesStorage ClassesStorage;

/// ��������� ������ ��� �����������
mutable UEPtr<ULoggerEnv> Logger;

protected: // �������� �������
// XML �������� ���� ������� ���������
UClassesDescription ClassesDescription;

/// �������� ����� ������� ���������
std::unordered_map<std::string, UPropertyDescription> CommonDescriptions;

protected: // �������� ���������
// ������ ��������� ���������
UClassLibraryList CollectionList;


/// ������� ������������ ����� ������� ������� � ���������
/// ����� ��� <��� ������, ��� ��� ����������>
//std::map<std::string, std::string> ClassLibraryLookupTable;

// ������ ���� ����������� �������
vector<string> CompletedClassNames;

// ������ ���� �� ����������� �������
vector<string> IncompletedClassNames;

protected: // �������� ��������
// ������ ��������
UObjectsStorage ObjectsStorage;

// ��������� �������������� Id �������� �������
UId LastClassId;

// ������ ������ ���������:
// 1 -  ������� ������. ������ ����������� ���������, ����� ������������. ��� ���������� ������� (�� ��������)
// 2 -  ����������� ������. ������ �����������, ���������-��������, ����� ������������.
//      ��� ����������, ������� ������ ��������� �������� - �������.
//      ��������� (������� ���� � �����������-��������, �� ��� ������ � ���������) - ��������.
// 3 -  ������ ������ ��������. ������ ���������-��������, ����� ������������. ��� ���������� �������� (�� �������)
int BuildMode;

// ������ �������-���������� Property ��� MockUNet
std::list<funcCrPropMock> FunctionsCrPropMock;

// ���� � ������ ��������� (� ������ ���������� ����� ��� ����� MockLibs RTlibs)
std::string LibrariesPath;

// ���� � ����� � ���������� ������� (� ������ ���������� ����� ����: ����������\������ (en, ru ���)\�������������.xml)
std::string ClDesc;

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
virtual UId AddClass(UEPtr<UComponentAbstractFactory> factory, const UId &classid=ForbiddenId);
virtual UId AddClass(UEPtr<UComponentAbstractFactory> factory, const std::string &classname, const UId &classid=ForbiddenId);

// ������� ������� ������ ������� �� ���������
// ���� 'force' == true �� ������������� ������� �� ���������
// ��� ������� ����� ������
virtual void DelClass(const UId &classid, bool force=false);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;
virtual bool CheckClass(const string &classname) const;

// ���������� ������� ������
virtual UEPtr<UComponentAbstractFactory> GetComponentFactory(const UId &classid) const;
virtual UEPtr<UComponentAbstractFactory> GetComponentFactory(const std::string &class_name) const;

// ���������� ����� �������
int GetNumClasses(void) const;

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassIdList(std::vector<UId> &buffer) const;

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
virtual void GetClassNameList(std::vector<std::string> &buffer) const;

// ������� ��� �� ������������ ������� ������� �� ���������
/// ���� force == true �� ������� ���� ���� ��������� �������� �� ������
/// � ��������� ��������������� ����������
virtual void FreeClassesStorage(bool force=false);

/// ������� ��� ������� ������� �� ���������
/// ���� force == true �� ������� ���� ���� ��������� �������� �� ������
/// � ��������� ��������������� ����������
virtual void ClearClassesStorage(bool force=false);
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
virtual UEPtr<UComponent> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);
virtual UEPtr<UComponent> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const UId &classid, const UEPtr<UComponent> &prototype=0);

template<class T>
UEPtr<T> TakeObject(const string &classname, const UEPtr<UComponent> &prototype=0);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(UEPtr<UComponent> object) const;

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(UEPtr<UContainer> object) const;

// ���� �������, ��������������� �������� �������� ���������
virtual UVirtualMethodFactory* FindVirualMethodFactory(UEPtr<UContainer> object);

// ��������� ��������� ����� �������� � ���������
virtual int CalcNumObjects(void) const;
virtual int CalcNumObjects(const UId &classid) const;
virtual size_t CalcNumObjects(const string &classname) const;

// ������� ��� ��������� ������� �� ���������
/// ���� force == true �� ������� ���� ���� ������� ������������
virtual void FreeObjectsStorage(bool force=false);

// ������� ��� ��������� ������� ��������� ������ �� ���������
virtual void FreeObjectsStorageByClass(const UId &classid);

// ������� ��� ������� �� ���������
/// ���� force == true �� ������� ���� ���� ������� ������������
virtual void ClearObjectsStorage(bool force=false);

// ������� ��� ������� ��������� ������ �� ���������
virtual void ClearObjectsStorageByClass(const UId &classid);

/// ������������� ��������� ��� ��������� ���������� � ��������� �� ���������
virtual void DefaultObject(UEPtr<UContainer> object);
// --------------------------

// --------------------------
// ������ ���������� ��������� �������
// --------------------------
// ��������� ���� � ����� � ���������� �������
void SetClDescPath(const std::string& value);

// ��������� ���� � ����� � ���������� �������
const std::string GetClDescPath() const;

// ��������� ���� � ����� � ��������� ����������� ������
// ����� ������ ����������� �����
const std::string GetCreateClDescPath(const std::string& class_name);

// ���������� XML �������� ������
const UEPtr<UContainerDescription> GetClassDescription(const std::string &classname, bool nothrow=false) const;

// ������������� XML �������� ������
// ����� � ��������� ������ ������������
void SetClassDescription(const std::string &classname, const UEPtr<UContainerDescription>& description);

// �������� �������� ������� �� xml-��������
virtual void LoadClassesDescription();

// ��������� �������� ������ � ���� �� ����
virtual void SaveClassDescriptionToFile(const std::string &classname);

// ��������� �������� ������ � xml
virtual void SaveClassDescription(const std::string &classname,
										USerStorageXML &xml);

// ��������� �������� ������ �� xml
virtual void LoadClassDescription(const std::string &classname,
										USerStorageXML &xml);

// ��������� �������� ���� ������� � xml
virtual void SaveClassesDescription(USerStorageXML &xml);

// ��������� �������� ���� ������� �� xml
virtual void LoadClassesDescription(USerStorageXML &xml);

// ��������� ����� �������� ���� ������� � xml
virtual bool SaveCommonClassesDescription(USerStorageXML &xml);

// ��������� ����� �������� ���� ������� �� xml
virtual bool LoadCommonClassesDescription(USerStorageXML &xml);
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
// ��������� �� ������
UEPtr<ULoggerEnv> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

// ���������� ���������� �� �������
UEPtr<ULibrary> GetCollection(int index);

// ���������� ����� ���������
int GetNumCollections(void) const;

// ���������� ���������� �� �����
UEPtr<ULibrary> GetCollection(const string &name);

// ���������� ��� ���������� �� �������
const string& GetCollectionName(int index);

// ���������� ������ ���������� �� �������
const string& GetCollectionVersion(int index);

// ������� ������ Complete � Incomplete �� ���� �����������
// ����� ����� �������
void ClearAllLibsClassesNameArrays(void);

// ������������ ������ ���� ��������� ����������� ����, ����������� �������
// ����� 'buffer' ����� ������ �� ���������� ��������
void GetLibsNameListByType(std::string &buffer, int type) const;

// ��������������� ��������� ����� ������� ������ � ���������
//virtual bool AddClass(UContainer *newclass);

// ��������� ���� � ������ ���������
void SetLibrariesPath(const std::string& value);

// ��������� ���� � ������ ���������
const std::string GetLibrariesPath() const;

//������ � RT-������������
/// ������������� ������������ ������������ ���������
/// ���������� � Engine ���� ���. ��������� ���������� � CollectionList (������ ����������� ���)
void InitRTlibs(void);

/// ��������� runtime-���������� �� � ����� (��� �������� �����������)
virtual bool LoadRuntimeCollection(const std::string &lib_name);

/// ��������� ����� ������� ������ � ��������� (����� ��������� � ����)
/// ���� ����� � ����� ������ ���������� �������� ���������� ��� force_replace = true
virtual bool AddClassToCollection(const std::string &new_class_name, const std::string &new_comp_name, bool force_replace, UContainer *newclass, const std::string &lib_name);

/// ������� ������� ������ �� RT ���������
virtual bool DelClassFromCollection(const std::string &class_name, const std::string &lib_name);

/// ������� ����� ���������� � �������� ������
virtual bool CreateRuntimeCollection(const std::string &lib_name);

/// ������� runtime-���������� ������ � ������
bool DeleteRuntimeCollection(const std::string &lib_name);

// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
// ���� force_build == true �� ���������� ������������ ������������� ����������
// � ���������
virtual bool AddCollection(ULibrary *library, bool force_build=false);

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelCollection(int index);

// ��������� ��� ��������� � ����� MockLibs ���������� � CollectionList
// � ���������� ������� (������� �������� � ���������� �����)
bool InitMockLibs(void);

// �������� ���� ���������-�������� (������� �� ����������)
// �� ����������� ���������
bool CreateMockLibs(void);

// ��������� ��� ����������-�������� � ����� Rtv-VideoAnalytics/Bin/MockLibs � ���� XML ������
// ����� ��������� ������� ��������� � ���� ���������� �����
bool SaveMockLibs(void);

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool DelCollection(const string &name);

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
virtual bool DelAllCollections(void);

// ��������� ������������ ������ ������
void SetBuildMode(int mode);

// ��������� �������� ������ ������
int GetBuildMode();

// ��������� ��������� ������� ���������
virtual bool BuildStorage(void);

// ��������� ��������� ������� ��������� ������������ ����
// ��� ����������:
// 0 - ���������� ���������� (������� ������ � �����)
// 1 - ������� ���������� (��������� �� ������� dll)
// 2 - ����������, ��������� �� ����� ����������
// 3 - ����������-�������� (��� ����������-��������)
virtual bool BuildStorage(int lib_type);

/// ������� ��� ������� �������, ��� ������� ��� ���������
/// � ����� ��� ��������� �������
virtual void DelAbandonedClasses(void);

/// ���������� ��������� �� ���������� ������ �� ����� ������
virtual UEPtr<ULibrary> FindCollection(const std::string &class_name);
virtual UEPtr<ULibrary> FindCollection(const UId &classid);

/// ��������� ������ ������������ ������ ���������� �� ���������
/// ����� �� ������� ���������� ������ ���������, � ������ ��������� ���
virtual void FindComponentDependencies(const std::string &class_name, std::vector<std::pair<std::string,std::string> > &dependencies);
// --------------------------

// --------------------------
// ������ ��� ������ � ������������-���������� (UMockUnet)
// --------------------------
// ���������� �������-��������� ������� ��� UMockUnet � ������ � Storage
bool AddCrPropMockFunc(funcCrPropMock func_ptr);

// ��������� ������� �������-���������� ������� ��� UMockUnet
const std::list<funcCrPropMock> &GetFunctionsCrPropMock() const;

// --------------------------
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
protected:
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
virtual void PushObject(const UId &classid, UEPtr<UContainer> object);

public:
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(UEPtr<UContainer> object);

protected:
// ���������� ������ � ������ ���������
virtual void MoveObject(UEPtr<UContainer> object, UEPtr<UStorage> newstorage);

public:
// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
virtual void ReturnObject(UEPtr<UComponent> object);

protected:
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
explicit EClassIdNotExist(UId id) : EIdNotExist(id) {};
};

// ����� � �������� ������ ��� ����������
struct EClassIdAlreadyExist: public EIdAlreadyExist
{
explicit EClassIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ������� ������ � ������� �� �����, �������������� � ���������
struct EClassNameNotExist: public ENameNotExist
{
explicit EClassNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ����� � �������� ������ ��� ����������
struct EClassNameAlreadyExist: public ENameAlreadyExist
{
explicit EClassNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ������������ ��� ������
struct EInvalidClassName: public ENameError
{
explicit EInvalidClassName(const std::string &name) : ENameError(name) {};
};

// ������� ������ � ������� �� ��������������, �������������� � ���������
struct EObjectIdNotExist: public EIdNotExist
{
explicit EObjectIdNotExist(UId id) : EIdNotExist(id) {};
};

// ������� ���������� ����������� �������� � �������, ������� �������� ������������ � ���������
struct EObjectStorageNotEmpty: public EIdError
{
explicit EObjectStorageNotEmpty(UId id) : EIdError(id) {};
};

struct EInvalidClassType: public UException
{
 std::string ClassName;
 std::string ExpectedTypeName;

explicit EInvalidClassType(const std::string &expected_type_name, const std::string &class_name) :
 ClassName(class_name), ExpectedTypeName(expected_type_name) {};
virtual ~EInvalidClassType(void) throw() {};

// ��������� ������ ���� �� ����������
std::string CreateLogMessage(void) const
{
 return UException::CreateLogMessage()+std::string(" ClassName=")+ClassName+
  std::string(" ExpectedTypeName=")+ExpectedTypeName;
};
};
// --------------------------

// --------------------------
};

template<class T>
UEPtr<T> UStorage::TakeObject(const UId &classid, const UEPtr<UComponent> &prototype)
{
 UEPtr<T> p;
 UEPtr<UComponent> got_class=TakeObject(classid,prototype);
 p=dynamic_pointer_cast<T>(got_class);
 if(!p)
 {
  ReturnObject(got_class);
  throw EInvalidClassType(FindClassName(classid), typeid(T).name());
 }
 return p;
}

template<class T>
UEPtr<T> UStorage::TakeObject(const string &classname, const UEPtr<UComponent> &prototype)
{
 UEPtr<T> p;
 UEPtr<UComponent> got_class=TakeObject(classname,prototype);
 p=dynamic_pointer_cast<T>(got_class);
 if(!p)
 {
  ReturnObject(got_class);
  throw EInvalidClassType(classname, typeid(T).name());
 }
 return p;
}

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

