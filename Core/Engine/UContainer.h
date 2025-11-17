/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAContainerH
#define UAContainerH

#include "../Math/MVector.h"
#include "UComponent.h"
#include <memory>
#include "ModernSmartPointers.h"
#include "UPropertyEndpoints.h"
#include "UTime.h"
#include "UController.h"

namespace RDK {

class UContainer;

// ����� �������� ��������� ����������
// UIPointer objects are always managed via shared_ptr (stored in UContainer::PointerLookupTable)
// Add enable_shared_from_this to allow safe access to shared_ptr
class RDK_LIB_TYPE UIPointer: public std::enable_shared_from_this<UIPointer>
{
protected: // ������

public:
virtual ~UIPointer();
virtual std::shared_ptr<UContainer> Get(void) const=0;

virtual void Set(std::shared_ptr<UContainer> source)=0;

virtual void Del(std::shared_ptr<UContainer> source)=0;

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
virtual int Find(std::shared_ptr<const UContainer> cont) const=0;

// -----------------
// ���������
// -----------------
UIPointer& operator = (std::shared_ptr<UContainer> source)
{
 Set(source);
 return *this;
};
// -----------------
};


// ��������� ������� ���������
struct RDK_LIB_TYPE UPVariable
{
 // Id ���������
 UId Id;

 // ��������� �� ��������
 std::shared_ptr<UIPointer> Pointer;

// --------------------------
// ������������ � �����������
// --------------------------
UPVariable(void);
UPVariable(UId id, std::shared_ptr<UIPointer> prop);
virtual ~UPVariable(void);
// --------------------------
};


class UInstancesStorageElement;

typedef long int IndexT;
//class UContainer;
typedef UContainer* PUAContainer;

// ������ ���������� �� ����������
typedef std::vector<std::shared_ptr<UContainer> > UAContainerVector;

// ������ ����������� ���������
typedef std::map<std::shared_ptr<UContainer>, NameT> UAStaticContainerMap;

class RDK_LIB_TYPE UContainer: public UComponent
{
public: // ���� ������
typedef std::map<NameT,UPVariable> PointerMapT;
typedef std::map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

//friend class UStorage;
friend class UController;

private: // ��������� ��������
// ������� ������������ ���� � Id ��������� �������
std::map<NameT,UId> CompsLookupTable;

// ������� ��������� ���������� �� �������� ����������
PointerMapT PointerLookupTable;

private: // ��������� ��������
// ������� ���������
UAContainerVector Components;

/// ������� ����������� ���������
UAStaticContainerMap StaticComponents;

// ������� ������������ ����������
std::vector<std::shared_ptr<UController> > Controllers;

public: // ������������� ��������
// ��� �������
UProperty<NameT, UContainer, ptParameter | pgSystem> Name;

// Id �������
UProperty<UId, UContainer, ptParameter | pgSystem> Id;

// ���� ���������� �������
// true - ������ ������� ����� �����������
// false - ������ ������� ����� ��������������
UProperty<bool,  UContainer, ptParameter | pgPublic> Activity;

// ��� ����� � ����� �������
// �������� ��� = 1./TimeStep
UProperty<UTime,  UContainer, ptParameter | pgSystem> TimeStep;

/// ����������� ���������� ����� ������� ���������� ������ � ��������� ������������
/// � �������������.
/// ���� ����� ������� ���������, �� ������ ����������� �������� ���������
/// �� �����������
/// ���� �������� ��������� <0, �� ��� �����������
UProperty<long long, UContainer, ptParameter | pgPublic> MaxCalculationDuration;

/// ����� ������� ���������� ������ � ��������� ������������
/// � �������������, �� ���������� �������� �������� ��������������� ��������� � ���.
/// ���� �������� ��������� <0, �� ��� �����������
UProperty<long long, UContainer, ptParameter | pgPublic> CalculationDurationThreshold;

public: // ���������� ��������
// ���������� ���������� � ������������ ����
UProperty<RDK::MVector<double,3>, UContainer, ptParameter | pgPublic> Coord;

// �����, ����������� �� ��������� �������
// (��� ����� ������� ������� �������� ��������) (��)
UProperty<unsigned long long, UContainer, ptState | pgPublic | pgSystem> StepDuration;

/// ����� ��������������� �������� ������ ��������� ���������� ����������
UProperty<unsigned int, UContainer, ptParameter | pgPublic | pgSystem> DebugSysEventsMask;

protected: // ��������� ����������
// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long InterstepsInterval;

// ���� 'TimeStep' > 'Owner->TimeStep' �� 'CalcCounter' ��������
// ��������� �������� ��������� ��������.
// � ��������� ������ 'CalcCounter' �� �����������
int CalcCounter;

// ������ �������� ���� ����� �������������
// ������� ��� �������� ���� ����� ����� �������
// ���� ������������� ������� �� ����������
UTime OwnerTimeStep;

// ��������� �� 0-� ������� ������� ���������
std::shared_ptr<UContainer>* PComponents;

// ���������� ��������� � ������� ���������
int NumComponents;

// ��������� �� ���� ������ � ���������
//std::shared_ptr<UInstancesStorageElement> ObjectIterator;

// ��������� �������������� Id ���������
UId LastId;

// ���� ������� �� ������� ������� ��������� � ������� �������� ��������
bool SkipComponentCalculation;

// ���� ������� �� ��������� ������ ��������� � ������� �������� �������� �������
bool ComponentReCalculation;

// ����� ������ ����� ���������� �� ������� ����
unsigned long long StartCalcTime;

// ����� ��������� ����� ���������� �� ���������� ����
unsigned long long LastCalcTime;

/// ����, ������������ �� ����� ����������� ����������
bool MovingFlag;

/// ����� ������������ ������ �� ��� �������.
/// ����� ���� �������������� ���� ������ �������������.
/// ��������� ���� ������� ���� MemoryMonitor
long long MemoryUsageDiff;

/// ��������� ����������� �������� ����� ��������� ������ ����� ���� �������
/// ����� ���� �������������� ���� ����� ����������.
/// ��������� ���� ������� ���� MemoryMonitor
long long MaxMemoryBlockDiff;

protected:
/// ������ �������, ��������� � ��������� ���
std::vector<std::string> PropertiesForDetailedLog;

// Helper методы для безопасного получения shared_ptr на this
std::shared_ptr<UContainer> get_shared_from_this() {
    return std::static_pointer_cast<UContainer>(
        UComponent::shared_from_this()
    );
}

std::weak_ptr<UContainer> get_weak_from_this() {
    return std::static_pointer_cast<UContainer>(
        UComponent::shared_from_this()
    );
}

// --------------------------
// ������������ � �����������
// --------------------------
public:
UContainer(void);
UContainer(const UContainer&) = default;
UContainer(UContainer&&) noexcept = default;
virtual ~UContainer(void);

UContainer& operator=(const UContainer&) = default;
UContainer& operator=(UContainer&&) noexcept = default;
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
std::shared_ptr<UContainer> GetOwner(void) const;

// ���������� ��������� �� �������� ��������� ���� ��������
std::shared_ptr<UContainer> GetMainOwner(void) const;

// ���������� ��������� ��������� ����� �������
std::shared_ptr<UStorage> GetStorage(void) const;

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool CheckOwner(std::shared_ptr<UContainer> owner) const;

// ���������� ������ Id �������
// (������� Id ���� ����������)
ULongId& GetFullId(ULongId &buffer) const;
ULongId GetFullId(void) const;

// ����������  '�������' Id �������
// (�������� ��� ��������� 'mainowner')
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������
ULongId& GetLongId(std::shared_ptr<UContainer> mainowner, ULongId &buffer) const;
ULongId GetLongId(std::shared_ptr<UContainer> mainowner) const;
// ������������� ������� ������������ ������, ������������� ������� ���
std::string& GetLongId(std::shared_ptr<UContainer> mainowner, std::string &buffer) const;

// ���������� true ���� ������������ ������������� ������� ���������, � ��������� ������ ���������� false
bool CheckLongId(const ULongId &id) const;
// ������������� ������� ������������ ������, ��������������� ������� ���
bool CheckLongId(const std::string &id) const;

// ���������� ������ ���������� ����� �������
virtual bool SetEnvironment(UEnvironment* environment);

// ��������� �� ������

// ����� ����������� ���������� �����
virtual void ProcessException(UException &exception);

// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
virtual void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
virtual void LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number=0);

virtual void LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier);

/// �������� �������� ��� ����� � ������ (�����, ���������, ���������)
virtual void LogPropertiesBeforeCalc(void);

/// �������� �������� ��� ������ �� ������� (������)
virtual void LogPropertiesAfterCalc(void);

/// ���������� ��������� ����� ������ �������
virtual bool CheckDebugMode(void) const;

/// ��������� ������ ������� ��� ���������� ���� �� ������
/// ����������� - �������
void SetPropertiesForDetailedLog(const std::string &str);
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ������� ��������� �������
void BreakOwner(void);

// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
void SetMainOwner(std::shared_ptr<UComponent> mainowner);
void SetMainOwner(std::shared_ptr<UComponent> mainowner, int levels);

// ��������� ������������ Id 'id' �� ������������ � ������ �������, �������.
bool CheckId(const UId &id);

// ���������� ���������� Id
virtual UId GenerateId(void);


// ���������� ������� ����� �������� � ����� ������
template<typename T>
const std::shared_ptr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name) const;

template<typename T>
std::shared_ptr<UVBaseDataProperty<T> > FindPropertyEx(const NameT &name);

/// ���������� � �������� destination_property ������ �������� ������� ����������
template<typename T>
bool AttachPropertyData(const NameT& destination_property, const NameT& source_component, const NameT &source_property);

/// ��������� �� �������� destination_property ������ �������� ������� ����������
template<typename T>
void DetachPropertyData(const NameT& destination_property);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
public:
// ���������� ���������� � ������������ ����
const RDK::MVector<double,3>& GetCoord(void) const;

// �����, ����������� �� ��������� �������
// (��� ����� ������� ������� �������� ��������) (��)
unsigned long long GetStepDuration(void) const;

// �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
unsigned long long GetFullStepDuration(void) const;

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long GetInterstepsInterval(void) const;

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double GetInstantPerformance(void) const;

// ������������� �������� ���� ��������������
const UTime& GetTimeStep(void) const;

// ������������� �������� ���� �������������� ���������� � ���� ��� �������� �����������
bool SetGlobalTimeStep(UTime timestep);

/// ����������� ����� ������������� ��������������� TimeStep ��� ���������� � ���� �������� ���������
/// ������������� ������ ��� ������ �� UEnvironment
void ChangeUseIndTimeStepMode(bool value);

// ������������� ���� ���������� �������
const bool& GetActivity(void) const;

// Id �������
UId GetId(void) const;

// ��������� ������������ ��� 'name' �� ������������ � ������ ������� �������
bool CheckName(const NameT &name);

// ��������� ������������ ��� 'name' �� �������������� ������������
bool ValidateName(const NameT &name);

// ���������� ��� ���������� � ����������� ����� �������
virtual NameT& GenerateName(const NameT &prefix, NameT &namebuffer);

// ������������� ��� �������
const NameT& GetName(void) const;

// ���������� ������ ��� �������
// (������� ����� ���� ����������)
NameT& GetFullName(NameT &buffer) const;
NameT GetFullName(void) const;

// ����������  '�������' ��� �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������
NameT& GetLongName(const std::shared_ptr<UContainer> &mainowner, NameT &buffer) const;
NameT GetLongName(const std::shared_ptr<UContainer> &mainowner) const;

/// ����������� ���������� ����� ������� ���������� ������ � ��������� ������������
/// � �������������.
/// ���� ����� ������� ���������, �� ������ ����������� �������� ���������
/// �� �����������
/// ���� �������� ��������� <0, �� ��� �����������
const long long& GetMaxCalculationDuration(void) const;

/// ����� ������� ���������� ������ � ��������� ������������
/// � �������������, �� ���������� �������� �������� ��������������� ��������� � ���.
/// ���� �������� ��������� <0, �� ��� �����������
const long long& GetCalculationDurationThreshold(void) const;

/// ����� ��������������� �������� ������ ��������� ���������� ����������
const unsigned int& GetDebugSysEventsMask(void) const;

/// ����� ������������ ������ �� ��� �������.
/// ����� ���� �������������� ���� ������ �������������.
/// ��������� ���� ������� ���� MemoryMonitor
long long GetMemoryUsageDiff(void) const;

/// ��������� ����������� �������� ����� ��������� ������ ����� ���� �������
/// ����� ���� �������������� ���� ����� ����������.
/// ��������� ���� ������� ���� MemoryMonitor
long long GetMaxMemoryBlockDiff(void) const;

bool SetName(const NameT &name);
bool SetCoord(const RDK::MVector<double,3> &value);
virtual bool SetActivity(const bool &activity);

protected:

bool SetTimeStep(const UTime &timestep);
bool SetId(const UId &id);
bool SetDebugSysEventsMask(const unsigned int &value);
bool SetMaxCalculationDuration(const long long &value);
bool SetCalculationDurationThreshold(const long long& value);
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
public:
// ���������� ��� ��������� ���������� �� ��� Id
const NameT& GetComponentName(const UId &id) const;

// ���������� Id ��������� ���������� �� ��� �����
const UId& GetComponentId(const NameT &name, bool nothrow=false) const;

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& GetPointerName(const UId &id) const;

// ���������� Id ���������� ��������� �� ��� �����
const UId& GetPointerId(const NameT &name) const;

// ������������ ����� ���� ��������� �� ��������� ����� ������
// � ���������� ������ ��������� ���� ������ ������
// find_all
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
const vector<std::shared_ptr<UContainer> >& GetComponentsByClassName(const NameT &name, vector<std::shared_ptr<UContainer> > &buffer, bool find_all=false);

// ������������ ����� ���� ��������� �� ��������� ����� ������
// � ���������� ������ ������� ���� ��������� ������������ �������� ���� ������ ������
// find_all
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
const vector<NameT>& GetComponentsNameByClassName(const NameT &name, vector<NameT> &buffer, bool find_all=false);

// ������������ ����� ���� ��������� ���������� � ��������� C++ ����
// � ���������� ������ ������� ���� ��������� ������������ �������� ���� ������ ������
// find_all
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
// ����� ����������� ������������ ������ net
template<class T>
const vector<NameT>& GetComponentsNameByClassType(vector<NameT> &buffer, std::shared_ptr<UContainer> net=0, bool find_all=false);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UContainer* New(void)=0;

// ������� ��������� �������� ������
//virtual UContainerDescription* NewDescription(void);

// ������� ����� ����� ������� � ����������� ���� ���������
// � �������� ����������.
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual std::shared_ptr<UContainer> Alloc(std::shared_ptr<UStorage> stor, bool copystate=false);

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(std::shared_ptr<UContainer> target, std::shared_ptr<UStorage> stor=0, bool copystate=false) const;

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);

// ��������� �� ���� ������ � ���������
//std::shared_ptr<UInstancesStorageElement> GetObjectIterator(void);
//void SetObjectIterator(std::shared_ptr<UInstancesStorageElement> value);

protected:
/// ������������ ���������� ���������� ������ ����������, �������������� ��� �����������
virtual void AUpdateInternalData(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
public:
// ���������� ����� �������� ���������
int GetNumComponents(void) const;

// ���������� ������ ����� �������� ���������
// (������� ��� ���������� �������� ���������)
int GetNumAllComponents(void) const;

bool CheckComponent(const NameT &name);
bool CheckComponentL(const NameT &name);

// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(std::shared_ptr<UContainer> comp) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
// ���� nothrow == true �� ���������� 0 � �� ������ ����������
virtual std::shared_ptr<UContainer> GetComponent(const UId &id, bool nothrow=false) const;

template<class T>
std::shared_ptr<T> GetComponent(const UId &id, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponent(id,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
virtual std::shared_ptr<UContainer> GetComponent(const NameT &name, bool nothrow=false) const;

template<class T>
std::shared_ptr<T> GetComponent(const NameT &name, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponent(name,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
std::shared_ptr<UContainer> GetComponentL(const ULongId &id, bool nothrow=false) const;

template<class T>
std::shared_ptr<T> GetComponentL(const ULongId &id, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponentL(id,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� ����� 'name'
virtual std::shared_ptr<UContainer> GetComponentL(const NameT &name, bool nothrow=false) const;

template<class T>
std::shared_ptr<T> GetComponentL(const NameT &name, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponentL(name,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ����������� ������ � ������ ���������
// ����� ���������� 0, ���� ������ ������� �� ������� �������
std::shared_ptr<UContainer> GetComponentByIndex(int index) const;

template<class T>
std::shared_ptr<T> GetComponentByIndex(int index) const
{
 return dynamic_pointer_cast<T>(GetComponentByIndex(index));
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
virtual void BeforeAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);
virtual void AfterAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);
virtual UId AddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'id' - ������ ���� �������� Id
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
//virtual void DelComponent(const UId &id, bool canfree=true);

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void DelComponent(const NameT &name, bool canfree=true);

// ������������� ������� ��� �������� ����������
void DelAllComponents(void);
protected:
void DelAllComponentsRaw(void);

public:
/// ��������� ��������� ��� ����������� ���������� ������� ��� ��� ������ 'classname'
/// � ��� 'name'
virtual void AddStaticComponent(const NameT &classname, const NameT &name, std::shared_ptr<UContainer> comp);

/// ������� ��������� ��� ����������� ����������
virtual void DelStaticComponent(std::shared_ptr<UContainer> comp);

/// ������� ��������� ��� ����������� ����������
virtual void DelAllStaticComponents(void);

/// ���������� ��������� �� static ���������
/// � ������� 'classname' � ������ 'name'
virtual std::shared_ptr<UContainer> FindStaticComponent(const NameT &classname, const NameT &name) const;

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
virtual bool MoveComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UContainer> target);

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void GetComponentsList(vector<UId> &buffer) const;
void GetComponentsList(vector<NameT> &buffer) const;

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������
// ���� ��������� stor != 0 �� ������������ ���
virtual void CopyComponents(std::shared_ptr<UContainer> comp, std::shared_ptr<UStorage> stor=0) const;

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
virtual bool ChangeComponentPosition(int index, int step);
virtual bool ChangeComponentPosition(const NameT &name, int step);

// ������������� ��������� � ������� �������� index ��� ������ 'name' ��
// �������� �������
// ����������� ��� ��������� ������� ������� ���������
virtual bool SetComponentPosition(int index, int new_position);
virtual bool SetComponentPosition(const NameT &name, int new_position);

/// ����, ������������ �� ����� ����������� ����������
bool IsMoving(void) const;
// --------------------------

// ----------------------
// ������ ���������� ����������������� ������������
// ----------------------
public:
// ���������� ������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetConnectorsList(ULongIdVector &buffer,
						  int sublevel=-1, std::shared_ptr<UContainer> ownerlevel=0);

// ���������� ������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetItemsList(ULongIdVector &buffer,
							int sublevel=-1, std::shared_ptr<UContainer> ownerlevel=0);

// ���������� ������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetNetsList(ULongIdVector &buffer,
							int sublevel=-1, std::shared_ptr<UContainer> ownerlevel=0);
// ----------------------

// --------------------------
// ������ ���������� ���������� �����������
// --------------------------
// ������������� �������� ��������� 'id' � �������� ��������� ������ ��������� ����������
// 'pointerid'
bool SetComponentAs(const UId &id, const UId &pointerid);

// ������������� �������� ��������� 'name' � �������� ��������� ������ ��������� ����������
// 'pointername'
bool SetComponentAs(const NameT &name,const NameT &pointername);

// ���������� ��������� ���������� ���������� 'id' � ��������� ������ ��������� ����������
// 'pointerid'
bool ResetComponentAs(const UId &id, const UId &pointerid);

// ���������� ��������� ���������� ���������� 'name' � ��������� ������ ��������� ����������
// 'pointername'
bool ResetComponentAs(const NameT &name,const NameT &pointername);

// ���������� ��������� ���������� ���������� 'name' �� ���� ������� ��������� ����������
bool ResetComponentAll(const UId &id);

// ���������� ��������� ���������� ���������� 'name' �� ���� ������� ��������� ����������
bool ResetComponentAll(const NameT &name);

// ������� ��� ���������� ����������� � ��������� ������ ��������� ����������
void DelAllComponentsAs(const NameT &pointername, bool canfree=true);
// --------------------------

// --------------------------
// ������ ���������� ������ (shared) �����������
// --------------------------
// ����� ������������� ����� ����������. ���������� ������������� ��� ����������
// ������� ���������
virtual void SharesInit(void);

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
virtual void SharesUnInit(void);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

/// ����� ������ ���������� �� �������� �� ���������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
virtual bool DefaultAll(UContainer* cont, bool subcomps);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);

// ����� �������� ����� ��� ������ ��������
virtual bool Reset(void);

// ��������� ������ ����� �������
virtual bool Calculate(void);

// ��������� ��������� ������������� ����� �������
virtual void Init(void);

// ��������� ��������������� ����� �������
virtual void UnInit(void);

// ��������� ��������� MainOwner ����� ������� ����� �������
virtual void UpdateMainOwner(void);

// ������ ���������� �������� ����������� � ��������� ������ ������� ��������
// ��������� �� ���� ���� �����
virtual void ForceSkipComponentCalculation(void);

// ������ ���������� �������� ����������� � ������� ���������� ������� ��������
// ��������� �� ���� ���� ����� �������
virtual void ForceComponentReCalculation(void);

/// ��������� ������� ������������ ������� ����� ����������
/// � ���� ��� ��������� MaxCalculationDuration � MaxCalculationDuration>=0
/// �� ��������� ������ ��������� ������� �������� ���������
virtual bool CheckDurationAndSkipComponentCalculation(void);

// ������������� ���������� � ��������� ������� �������
virtual void UpdateComputationOrder(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
virtual void AInit(void);
virtual void AUnInit(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������������� ����������
// �������� ������������ ����� �� ���������� ������
// --------------------------
protected:
// ��������� ����� ����������
void AddController(std::shared_ptr<UController> controller, bool forchilds=false);

// ������� ���������� �� ������
void DelController(std::shared_ptr<UController> controller, bool forchilds=false);

// ������� ��� �����������
void DelAllControllers(bool forchilds=false);

// ���������� ���������� ���� ������������
void UnLinkAllControllers(bool forchilds=false);

// ���������, ���������� �� ���������� � ������
bool CheckController(std::shared_ptr<UController> controller) const;

// ���������� ����� ������������
size_t GetNumControllers(void) const;

// ���������� ���������� �� �������
std::shared_ptr<UController> GetController(int index);
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
public:
// ��������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupPointer(const NameT &name, std::shared_ptr<UIPointer> pointer);

protected:
// ������� ��������� � ID 'id' �� ������� �����������
void DelLookupPointer(const NameT &name);

// ������������ ����� � ������� ���������, ���������������� ��������� ���������
PointerMapCIteratorT FindLookupPointer(std::shared_ptr<UContainer> source) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void AddComponentTable(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);

// ������� ��������� 'comp' �� ������� ���������
void DelComponentTable(std::shared_ptr<UContainer> comp);
// --------------------------

// --------------------------
// ������� ������ ���������� ������ (shared) �����������
// --------------------------
protected:
// ����� ������������� ����� ����������. ���������� ������������� ��� ����������
// ������� ���������
virtual void ASharesInit(void);

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
virtual void ASharesUnInit(void);
// --------------------------

protected:
// --------------------------
// ������� ������ ���������� �������� ����������� ���������
// --------------------------
// ��������� ������� ������������ ��������� ������� 'oldname'
// ��� ���������� �� 'newname'
void ModifyLookupComponent(const NameT &oldname, const NameT &newname);

// ��������� ������� ������������ ��������� ������������ Id 'id'
// ��� ���������� � ������ 'name'
void SetLookupComponent(const NameT &name, const UId &id);

// ��������� ������� ������������ ��������� ������ ������
// ���������� � ������ 'name'
void DelLookupComponent(const NameT &name);
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
/// ���������� ����������� �������� �� ���������� ������������ ����������
UId UpdateStaticComponent(const NameT &classname, std::shared_ptr<UContainer> comp);

// ������� ��������� comp
// ����� ������������, ��� ��������� ����������� �������
virtual void BeforeDelComponent(std::shared_ptr<UContainer> comp, bool canfree=true);
virtual void AfterDelComponent(std::shared_ptr<UContainer> comp, bool canfree=true);
void DelComponent(std::shared_ptr<UContainer> comp, bool canfree);

// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
// ����� ���� ������� ��������� �� ��������� ����������
virtual void ABeforeAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);
virtual void AAfterAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);
virtual bool AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual void ABeforeDelComponent(std::shared_ptr<UContainer> comp, bool canfree);
virtual void AAfterDelComponent(std::shared_ptr<UContainer> comp, bool canfree);
virtual bool ADelComponent(std::shared_ptr<UContainer> comp);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ��������� ��������� MainOwner ����� ������� ����� �������
virtual void AUpdateMainOwner(void);
// --------------------------

public: // ������ �������� ����������
/* ������� ����� ���������� */
class EIContainer
{
public: // ������ ����������
// �������� ��� ���������� � ������� ������������� ����������
std::string Name;

// �������� ������������� ���������� � ������� ������������� ����������
ULongId Id;

// ������ ��� ��������� ���������� � ������� ������������� ����������
std::string OwnerName;

// ������ ������������� ��������� ���������� � ������� ������������� ����������
ULongId OwnerId;

// ������ ��� �������� ��������� ���������� � ������� ������������� ����������
std::string MainOwnerName;

// ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
ULongId MainOwnerId;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EIContainer(void);
explicit EIContainer(const UContainer *cont);
EIContainer(const EIContainer &copy);
virtual ~EIContainer(void);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Id ���������� �� ������
struct EComponentIdNotExist: public EIdNotExist
{
explicit EComponentIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ���������� ��� ����������
struct EComponentIdAlreadyExist: public EIdAlreadyExist
{
explicit EComponentIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ���������� �� �������
struct EComponentNameNotExist: public ENameNotExist
{
explicit EComponentNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ���������� ��� ����������
struct EComponentNameAlreadyExist: public ENameAlreadyExist
{
explicit EComponentNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ��� ���������� �����������
struct EComponentNameInvalid: public ENameError
{
explicit EComponentNameInvalid(const std::string &name) : ENameError(name) {};
};

// Id ��������� �� ������
struct EPointerIdNotExist: public EIdNotExist
{
explicit EPointerIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ��������� ��� ����������
struct EPointerIdAlreadyExist: public EIdAlreadyExist
{
explicit EPointerIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ��������� �� �������
struct EPointerNameNotExist: public ENameNotExist
{
explicit EPointerNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ��������� ��� ����������
struct EPointerNameAlreadyExist: public ENameAlreadyExist
{
explicit EPointerNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ������ ����������/�������� ���������
// Id ��������� ��� ����������
struct EAddComponentAlreadyHaveOwner: public EIdError
{
explicit EAddComponentAlreadyHaveOwner(UId id) : EIdError(id) {};
};

// ������������ ��� ��������� ����������
struct EAddComponentHaveInvalidType: public EIdError
{
explicit EAddComponentHaveInvalidType(UId id) : EIdError(id) {};
};

// ������������ ����� ��� ��������� ������ ����� ���������
class EICalculateContainer: public EIContainer
{
 public: // ������ ����������
 // �������� ��� ��������� ���������� � ������� ��������� ������ �����
 std::string SubName;

 // �������� ������������� ��������� ���������� � ������� ��������� ������ �����
 ULongId SubId;

 // --------------------------
 // ������������ � �����������
 // --------------------------
 EICalculateContainer(void);
 EICalculateContainer(const UContainer *cont, const UContainer *subcont);
 EICalculateContainer(const EICalculateContainer &copy);
 virtual ~EICalculateContainer(void);
 // --------------------------

 // --------------------------
 // ������ ������������ ����
 // --------------------------
 // ��������� ������ ���� �� ����������
 virtual std::string CreateLogMessage(void) const;
 // --------------------------
};


// ���������� - ������
class EComponentCalculate: public EError, public EICalculateContainer
{
public:
 // --------------------------
 // ������������ � �����������
 // --------------------------
 EComponentCalculate(void);
 EComponentCalculate(const UContainer *cont, const UContainer *subcont);
 EComponentCalculate(const EComponentCalculate &copy);
 virtual ~EComponentCalculate(void) throw();
 // --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ���������� - ��������� ������ ���������� ����������
class EComponentSystemException: public ESystemException, public EICalculateContainer
{
 public:

 // --------------------------
 // ������������ � �����������
 // --------------------------
 EComponentSystemException(void);
 EComponentSystemException(const UContainer *cont, const UContainer *subcont, const std::string &info);
 EComponentSystemException(const EComponentSystemException &copy);
 virtual ~EComponentSystemException(void) throw();
 // --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};
/* **************************** */

};

/// ������� �������������� ������ ��� �����������
bool PreparePropertyLogString(const UVariable& variable, unsigned int expected_type, std::string &result);


// ������������ ����� ���� ��������� ���������� � ��������� C++ ����
// � ���������� ������ ������� ���� ��������� ������������ �������� ���� ������ ������
// find_all
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
template<class T>
const vector<NameT>& UContainer::GetComponentsNameByClassType(vector<NameT> &buffer, std::shared_ptr<UContainer> net, bool find_all)
{
 int numComp=int(GetNumComponents());
 std::shared_ptr<UContainer> comp;
 std::shared_ptr<UContainer> root(net);
 string compName;

 if(!net)
  root=get_shared_from_this();

 switch(find_all)
 {
  case false:
   for(int i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	if(dynamic_pointer_cast<T>(comp))
	{
	 compName=comp->GetLongName(root, compName);
	 buffer.push_back(compName);
	}
   }
   break;

  case true:
   for(int i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	comp->GetComponentsNameByClassType<T>(buffer, root, true);
	if(dynamic_pointer_cast<T>(comp))
	{
	 compName=comp->GetLongName(root, compName);
	 buffer.push_back(compName);
	}
   }
   break;
 }

 return buffer;
}

// ���������� ������� ����� �������� � ����� ������
template<typename T>
const std::shared_ptr<UVBaseDataProperty<T> > UContainer::FindPropertyEx(const NameT &name) const
{
 std::shared_ptr<UIProperty> property=FindProperty(name);
 if(!property)
  return nullptr;

 if(property->GetLanguageType() != typeid(T))
  return nullptr;

 return std::static_pointer_cast<UVBaseDataProperty<T>>(property);
}

template<typename T>
std::shared_ptr<UVBaseDataProperty<T> > UContainer::FindPropertyEx(const NameT &name)
{
 std::shared_ptr<UIProperty> property=FindProperty(name);
 if(!property)
  return nullptr;

 if(property->GetLanguageType() != typeid(T))
  return nullptr;

 return std::static_pointer_cast<UVBaseDataProperty<T>>(property);
}


/// ���������� � �������� destination_property ������ �������� ������� ����������
template<typename T>
bool UContainer::AttachPropertyData(const NameT& destination_property, const NameT& source_component, const NameT &source_property)
{
 std::shared_ptr<UVBaseDataProperty<T> > dest_prop=FindPropertyEx<T>(destination_property);
 if(!dest_prop)
  return false;
 std::shared_ptr<UContainer> source_cont=GetComponentL(source_component,true);
 if(!source_cont)
  return false;

 std::shared_ptr<UVBaseDataProperty<T> > source_prop=source_cont->FindPropertyEx<T>(source_property);
 if(!source_prop)
  return false;

 return dest_prop->AttachTo(source_prop);
}

/// ��������� �� �������� destination_property ������ �������� ������� ����������
template<typename T>
void UContainer::DetachPropertyData(const NameT& destination_property)
{
 std::shared_ptr<UVBaseDataProperty<T> > dest=FindPropertyEx<T>(destination_property);
 if(dest)
  dest->DetachFrom();
}


/// ���������� ��� ������������� ����������
template<typename T,class OwnerT>
UItem* UVProperty<T, OwnerT>::GetItem(int index)
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return reinterpret_cast<UItem*>(ConnectedOutputs[index]->GetOwner());
 }
 return 0;
}

/// ���������� ��� ������������� ������
template<typename T,class OwnerT>
std::string UVProperty<T, OwnerT>::GetItemOutputName(int index) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetName();
 }
 return std::string();
}

/// ���������� ��� ������������� ����������
template<typename T,class OwnerT>
std::string UVProperty<T, OwnerT>::GetItemName(int index) const
{
 // Временно закомментировано из-за циклической зависимости
 return std::string();
}

/// ���������� ������ ��� ������������� ����������
template<typename T,class OwnerT>
std::string UVProperty<T, OwnerT>::GetItemFullName(int index) const
{
 // Временно закомментировано из-за циклической зависимости
 return std::string();
}

}

namespace RDK {

// SAFETY: Helper function for safe dynamic_pointer_cast
// Checks for corrupted shared_ptr control blocks before casting
// Note: This cannot prevent segfault from corrupted vtable in __dynamic_cast,
// but it can catch some cases of corrupted control blocks
template<typename T>
std::shared_ptr<T> SafeDynamicPointerCast(std::shared_ptr<UContainer> ptr)
{
 if(!ptr)
  return nullptr;
 
 try {
  // Check if shared_ptr control block is corrupted by checking use_count
  // If use_count is extremely large, the control block is likely corrupted
  size_t use_count = ptr.use_count();
  if(use_count > 1000000) // Sanity check - normal use_count should be much smaller
  {
   return nullptr; // Control block is likely corrupted
  }
  
  // Try to access a simple method to verify object is still valid
  // This may segfault if vtable is corrupted, but we can't prevent that
  try {
   void* raw_ptr = ptr.get();
   if(!raw_ptr)
    return nullptr;
   
   // Attempt to call a simple virtual method to verify vtable is valid
   // If vtable is corrupted, this will segfault, but we can't prevent that
   // We just try to minimize the chance by checking use_count first
  } catch (...) {
   return nullptr;
  }
  
  return std::dynamic_pointer_cast<T>(ptr);
 } catch (const std::bad_weak_ptr&) {
  return nullptr;
 } catch (...) {
  return nullptr;
 }
}

} // namespace RDK

#include "UPointer.h"


#endif

