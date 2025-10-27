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
#include <memory>
#include "UContainer.h"
#include "../Serialize/USerStorageXML.h"
#include "UContainerDescription.h"

#include "UComponentFactory.h"


namespace RDK {

/* *********************************************************************** */
class ULibrary;
class URuntimeLibrary;

typedef std::shared_ptr<UComponentAbstractFactory> UClassStorageElement;
typedef std::map<UId, UClassStorageElement> UClassesStorage;
typedef std::map<UId, UClassStorageElement>::iterator UClassesStorageIterator;
typedef std::map<UId, UClassStorageElement>::const_iterator UClassesStorageCIterator;

typedef std::map<std::string, std::shared_ptr<UContainerDescription> > UClassesDescription;
typedef std::map<std::string, std::shared_ptr<UContainerDescription> >::iterator UClassesDescriptionIterator;
typedef std::map<std::string, std::shared_ptr<UContainerDescription> >::const_iterator UClassesDescriptionCIterator;

typedef std::vector<ULibrary*> UClassLibraryList;
/* *********************************************************************** */
// ������� ������ ������������ �������� ������������� ������
class RDK_LIB_TYPE UInstancesStorageElement
{
public: // ������
// ��������� �� ������
std::shared_ptr<UContainer> Object;

// ������� ���� �������� �� ������
bool UseFlag;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement(void);
UInstancesStorageElement(const UInstancesStorageElement &copy);
UInstancesStorageElement(const std::shared_ptr<UContainer> &object, bool useflag);
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

class UMockUNet;
// ��������� �� �������, ������� ������� ��� ��������� ��������
// �������� �������� USerStorageXML ��� UMockUNet
typedef bool (*funcCrPropMock)(USerStorageXML*, UMockUNet*);

/* *********************************************************************** */


class RDK_LIB_TYPE UStorage: public std::enable_shared_from_this<UStorage>
{
//friend class UContainer;
protected: // ��������� ��������
// ������� ������������ ���� � Id �������� �������
std::map<std::string,UId> ClassesLookupTable;

// ��������� �������� �������
UClassesStorage ClassesStorage;

/// ��������� ������ ��� �����������


protected: // �������� �������
// XML �������� ���� ������� ���������
UClassesDescription ClassesDescription;

/// �������� ����� ������� ���������
std::map<std::string, UPropertyDescription> CommonDescriptions;

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
virtual UId AddClass(std::shared_ptr<UComponentAbstractFactory> factory, const UId &classid=ForbiddenId);
virtual UId AddClass(std::shared_ptr<UComponentAbstractFactory> factory, const std::string &classname, const UId &classid=ForbiddenId);

// ������� ������� ������ ������� �� ���������
// ���� 'force' == true �� ������������� ������� �� ���������
// ��� ������� ����� ������
virtual void DelClass(const UId &classid, bool force=false);

// ��������� ������� ������� ������ ������� � ���������
virtual bool CheckClass(const UId &classid) const;
virtual bool CheckClass(const string &classname) const;

// ���������� ������� ������
virtual std::shared_ptr<UComponentAbstractFactory> GetComponentFactory(const UId &classid) const;
virtual std::shared_ptr<UComponentAbstractFactory> GetComponentFactory(const std::string &class_name) const;

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
virtual std::shared_ptr<UContainer> TakeObject(const UId &classid, const std::shared_ptr<UContainer> &prototype=0);
virtual std::shared_ptr<UContainer> TakeObject(const string &classname, const std::shared_ptr<UContainer> &prototype=0);

template<class T>
std::shared_ptr<T> TakeObject(const UId &classid, const std::shared_ptr<UContainer> &prototype=0);

template<class T>
std::shared_ptr<T> TakeObject(const string &classname, const std::shared_ptr<UContainer> &prototype=0);

// ���������� Id ������, ���������� ������� 'object'
virtual UId FindClass(std::shared_ptr<UContainer> object) const;

// ��������� ���������� �� ������ 'object' � ���������
virtual bool CheckObject(std::shared_ptr<UContainer> object) const;

// ���� �������, ��������������� �������� �������� ���������
virtual UVirtualMethodFactory* FindVirualMethodFactory(std::shared_ptr<UContainer> object);

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
virtual void DefaultObject(std::shared_ptr<UContainer> object);
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
const std::shared_ptr<UContainerDescription> GetClassDescription(const std::string &classname, bool nothrow=false) const;

// ������������� XML �������� ������
// ����� � ��������� ������ ������������
void SetClassDescription(const std::string &classname, const std::shared_ptr<UContainerDescription>& description);

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

// ���������� ���������� �� �������
std::shared_ptr<ULibrary> GetCollection(int index);

// ���������� ����� ���������
int GetNumCollections(void) const;

// ���������� ���������� �� �����
std::shared_ptr<ULibrary> GetCollection(const string &name);

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
virtual std::shared_ptr<ULibrary> FindCollection(const std::string &class_name);
virtual std::shared_ptr<ULibrary> FindCollection(const UId &classid);

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
virtual void PushObject(const UId &classid, std::shared_ptr<UContainer> object);

public:
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
virtual UId PopObject(std::shared_ptr<UContainer> object);

// --------------------------
// Helper методы для безопасного получения shared_ptr на this
// --------------------------
std::shared_ptr<UStorage> get_shared_from_this() {
    return shared_from_this();
}

std::weak_ptr<UStorage> get_weak_from_this() {
    return weak_from_this();
}

protected:
// ���������� ������ � ������ ���������
virtual void MoveObject(std::shared_ptr<UContainer> object, std::shared_ptr<UStorage> newstorage);

public:
// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
virtual void ReturnObject(std::shared_ptr<UContainer> object);

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
std::shared_ptr<T> UStorage::TakeObject(const UId &classid, const std::shared_ptr<UContainer> &prototype)
{
 std::shared_ptr<T> p;
 std::shared_ptr<UContainer> got_class=TakeObject(classid,prototype);
 p=dynamic_pointer_cast<T>(got_class);
 if(!p)
 {
  ReturnObject(got_class);
  throw EInvalidClassType(FindClassName(classid), typeid(T).name());
 }
 return p;
}

template<class T>
std::shared_ptr<T> UStorage::TakeObject(const string &classname, const std::shared_ptr<UContainer> &prototype)
{
 std::shared_ptr<T> p;
 std::shared_ptr<UContainer> got_class=TakeObject(classname,prototype);
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

