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
#include "UProperty.h"
#include "ULocalProperty.h"
#include "UShare.h"
#include "UTime.h"
#include "UController.h"

namespace RDK {

class UContainer;

// ����� �������� ��������� ����������
class RDK_LIB_TYPE UIPointer
{
protected: // ������

public:
virtual UEPtr<UContainer> const Get(void) const=0;

virtual void Set(UEPtr<UContainer> source)=0;

virtual void Del(UEPtr<UContainer> source)=0;

// ���������, ���������� �� ����� ��������� � ���� ������
// ���������� 0 ���� ��, � <0 ���� ���
virtual int Find(UEPtr<const UContainer> cont) const=0;

// -----------------
// ���������
// -----------------
UIPointer& operator = (UEPtr<UContainer> source)
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
 UEPtr<UIPointer> Pointer;

// --------------------------
// ������������ � �����������
// --------------------------
UPVariable(void);
UPVariable(UId id, UEPtr<UIPointer> prop);
virtual ~UPVariable(void);
// --------------------------
};


class UInstancesStorageElement;

typedef long int IndexT;
//class UContainer;
typedef UContainer* PUAContainer;

// ������ ���������� �� ����������
typedef std::vector<UEPtr<UContainer> > UAContainerVector;

// ������ ����������� ���������
typedef std::map<UEPtr<UContainer>, NameT> UAStaticContainerMap;

class RDK_LIB_TYPE UContainer: public UComponent
{
public: // ���� ������
typedef std::map<NameT,UPVariable> PointerMapT;
typedef std::map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

friend class UStorage;
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
std::vector<UEPtr<UController> > Controllers;

protected: // �������� ��������

protected: // ������������� ��������
// ��� �������
NameT Name;

// Id �������
UId Id;

// ���� ���������� �������
// true - ������ ������� ����� �����������
// false - ������ ������� ����� ��������������
bool Activity;

// ��� ����� � ����� �������
// �������� ��� = 1./TimeStep
UTime TimeStep;

/// ����������� ���������� ����� ������� ���������� ������ � ��������� ������������
/// � �������������.
/// ���� ����� ������� ���������, �� ������ ����������� �������� ���������
/// �� �����������
/// ���� �������� ��������� <0, �� ��� �����������
long long MaxCalculationDuration;

/// ����� ������� ���������� ������ � ��������� ������������
/// � �������������, �� ���������� �������� �������� ��������������� ��������� � ���.
/// ���� �������� ��������� <0, �� ��� �����������
long long CalculationDurationThreshold;

public: // ���������� ��������
// ���������� ���������� � ������������ ����
RDK::MVector<double,3> Coord;

// �����, ����������� �� ��������� �������
// (��� ����� ������� ������� �������� ��������) (��)
unsigned long long StepDuration;

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long InterstepsInterval;

/// ����� ��������������� �������� ������ ��������� ���������� ����������
unsigned int DebugSysEventsMask;

protected: // ��������� ����������
// ���� 'TimeStep' > 'Owner->TimeStep' �� 'CalcCounter' ��������
// ��������� �������� ��������� ��������.
// � ��������� ������ 'CalcCounter' �� �����������
UTime CalcCounter;

// ������ �������� ���� ����� �������������
// ������� ��� �������� ���� ����� ����� �������
// ���� ������������� ������� �� ����������
UTime OwnerTimeStep;

// ��������� �� 0-� ������� ������� ���������
UEPtr<UContainer>* PComponents;

// ���������� ��������� � ������� ���������
int NumComponents;

// ��������� �� ���� ������ � ���������
UEPtr<UInstancesStorageElement> ObjectIterator;

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

protected:
/// ������ �������, ��������� � ��������� ���
std::vector<std::string> PropertiesForDetailedLog;

// --------------------------
// ������������ � �����������
// --------------------------
public:
UContainer(void);
virtual ~UContainer(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
UEPtr<UContainer> GetOwner(void) const;

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UContainer> GetMainOwner(void) const;

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const GetStorage(void) const;

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool CheckOwner(UEPtr<UContainer> owner) const;

// ���������� ������ Id �������
// (������� Id ���� ����������)
ULongId& GetFullId(ULongId &buffer) const;
ULongId GetFullId(void) const;

// ����������  '�������' Id �������
// (�������� ��� ��������� 'mainowner')
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������
ULongId& GetLongId(UEPtr<UContainer> mainowner, ULongId &buffer) const;
ULongId GetLongId(UEPtr<UContainer> mainowner) const;
// ������������� ������� ������������ ������, ������������� ������� ���
std::string& GetLongId(UEPtr<UContainer> mainowner, std::string &buffer) const;

// ���������� true ���� ������������ ������������� ������� ���������, � ��������� ������ ���������� false
bool CheckLongId(const ULongId &id) const;
// ������������� ������� ������������ ������, ��������������� ������� ���
bool CheckLongId(const std::string &id) const;

// ���������� ������ ���������� ����� �������
virtual bool SetEnvironment(UEPtr<UEnvironment> environment);

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
void SetMainOwner(UEPtr<UComponent> mainowner);
void SetMainOwner(UEPtr<UComponent> mainowner, int levels);

// ��������� ������������ Id 'id' �� ������������ � ������ �������, �������.
bool CheckId(const UId &id);

// ���������� ���������� Id
virtual UId GenerateId(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
public:
// ���������� ���������� � ������������ ����
const RDK::MVector<double,3>& GetCoord(void) const;
bool SetCoord(const RDK::MVector<double,3> &value);

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
bool SetTimeStep(const UTime &timestep);

// ������������� �������� ���� �������������� ���������� � ���� ��� �������� �����������
bool SetGlobalTimeStep(UTime timestep);

// ������������� ���� ���������� �������
const bool& GetActivity(void) const;
virtual bool SetActivity(const bool &activity);

// Id �������
const UId& GetId(void) const;
bool SetId(const UId &id);

// ��������� ������������ ��� 'name' �� ������������ � ������ ������� �������
bool CheckName(const NameT &name);

// ���������� ��� ���������� � ����������� ����� �������
virtual NameT& GenerateName(const NameT &prefix, NameT &namebuffer);

// ������������� ��� �������
const NameT& GetName(void) const;
bool SetName(const NameT &name);


// ���������� ������ ��� �������
// (������� ����� ���� ����������)
NameT& GetFullName(NameT &buffer) const;
NameT GetFullName(void) const;

// ����������  '�������' ��� �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������
NameT& GetLongName(const UEPtr<UContainer> &mainowner, NameT &buffer) const;
NameT GetLongName(const UEPtr<UContainer> &mainowner) const;

/// ����������� ���������� ����� ������� ���������� ������ � ��������� ������������
/// � �������������.
/// ���� ����� ������� ���������, �� ������ ����������� �������� ���������
/// �� �����������
/// ���� �������� ��������� <0, �� ��� �����������
const long long& GetMaxCalculationDuration(void) const;
bool SetMaxCalculationDuration(const long long &value);

/// ����� ������� ���������� ������ � ��������� ������������
/// � �������������, �� ���������� �������� �������� ��������������� ��������� � ���.
/// ���� �������� ��������� <0, �� ��� �����������
const long long& GetCalculationDurationThreshold(void) const;
bool SetCalculationDurationThreshold(const long long& value);

/// ����� ��������������� �������� ������ ��������� ���������� ����������
const unsigned int& GetDebugSysEventsMask(void) const;
bool SetDebugSysEventsMask(const unsigned int &value);
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
const vector<UEPtr<UContainer> >& GetComponentsByClassName(const NameT &name, vector<UEPtr<UContainer> > &buffer, bool find_all=false);

// ������������ ����� ���� ��������� �� ��������� ����� ������
// � ���������� ������ ������� ���� ��������� ���� ������ ������
// find_all
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
const vector<NameT>& GetComponentsNameByClassName(const NameT &name, vector<NameT> &buffer, bool find_all=false);
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
virtual UEPtr<UContainer> Alloc(UEPtr<UStorage> stor=0, bool copystate=false);

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);

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
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
// ���� nothrow == true �� ���������� 0 � �� ������ ����������
virtual UEPtr<UContainer> GetComponent(const UId &id, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponent(const UId &id, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponent(id,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
virtual UEPtr<UContainer> GetComponent(const NameT &name, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponent(const NameT &name, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponent(name,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
UEPtr<UContainer> GetComponentL(const ULongId &id, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponentL(const ULongId &id, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponentL(id,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� ����� 'name'
virtual UEPtr<UContainer> GetComponentL(const NameT &name, bool nothrow=false) const;

template<class T>
UEPtr<T> GetComponentL(const NameT &name, bool nothrow=false) const
{
 return dynamic_pointer_cast<T>(GetComponentL(name,nothrow));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ����������� ������ � ������ ���������
// ����� ���������� 0, ���� ������ ������� �� ������� �������
UEPtr<UContainer> GetComponentByIndex(int index) const;

template<class T>
UEPtr<T> GetComponentByIndex(int index) const
{
 return dynamic_pointer_cast<T>(GetComponentByIndex(index));
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
virtual void BeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual UId AddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'id' - ������ ���� �������� Id
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
virtual void DelComponent(const UId &id, bool canfree=true);

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'name' - ������ ���� �������� ������
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void DelComponent(const NameT &name, bool canfree=true);

// ������������� ������� ��� �������� ����������
void DelAllComponents(void);

/// ��������� ��������� ��� ����������� ���������� ������� ��� ��� ������ 'classname'
/// � ��� 'name'
virtual void AddStaticComponent(const NameT &classname, const NameT &name, UEPtr<UContainer> comp);

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
virtual bool MoveComponent(UEPtr<UContainer> comp, UEPtr<UContainer> target);

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void GetComponentsList(vector<UId> &buffer) const;
void GetComponentsList(vector<NameT> &buffer) const;

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������
// ���� ��������� stor != 0 �� ������������ ���
virtual void CopyComponents(UEPtr<UContainer> comp, UEPtr<UStorage> stor=0) const;

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
						  int sublevel=-1, UEPtr<UContainer> ownerlevel=0);

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
							int sublevel=-1, UEPtr<UContainer> ownerlevel=0);

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
							int sublevel=-1, UEPtr<UContainer> ownerlevel=0);
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
void AddController(UEPtr<UController> controller, bool forchilds=false);

// ������� ���������� �� ������
void DelController(UEPtr<UController> controller, bool forchilds=false);

// ������� ��� �����������
void DelAllControllers(bool forchilds=false);

// ���������� ���������� ���� ������������
void UnLinkAllControllers(bool forchilds=false);

// ���������, ���������� �� ���������� � ������
bool CheckController(UEPtr<UController> controller) const;

// ���������� ����� ������������
size_t GetNumControllers(void) const;

// ���������� ���������� �� �������
UEPtr<UController> GetController(int index);
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
public:
// ��������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer);

protected:
// ������� ��������� � ID 'id' �� ������� �����������
void DelLookupPointer(const NameT &name);

// ������������ ����� � ������� ���������, ���������������� ��������� ���������
PointerMapCIteratorT FindLookupPointer(UEPtr<UContainer> source) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void AddComponentTable(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ������� ��������� 'comp' �� ������� ���������
void DelComponentTable(UEPtr<UContainer> comp);
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
UId UpdateStaticComponent(const NameT &classname, UEPtr<UContainer> comp);

// ������� ��������� comp
// ����� ������������, ��� ��������� ����������� �������
virtual void BeforeDelComponent(UEPtr<UContainer> comp, bool canfree=true);
virtual void AfterDelComponent(UEPtr<UContainer> comp, bool canfree=true);
void DelComponent(UEPtr<UContainer> comp, bool canfree);

// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
// ����� ���� ������� ��������� �� ��������� ����������
virtual void ABeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual void AAfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual void ABeforeDelComponent(UEPtr<UContainer> comp, bool canfree);
virtual void AAfterDelComponent(UEPtr<UContainer> comp, bool canfree);
virtual bool ADelComponent(UEPtr<UContainer> comp);
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
EIContainer(const UContainer *cont);
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
EComponentIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ���������� ��� ����������
struct EComponentIdAlreadyExist: public EIdAlreadyExist
{
EComponentIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ���������� �� �������
struct EComponentNameNotExist: public ENameNotExist
{
EComponentNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ���������� ��� ����������
struct EComponentNameAlreadyExist: public ENameAlreadyExist
{
EComponentNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};


// Id ��������� �� ������
struct EPointerIdNotExist: public EIdNotExist
{
EPointerIdNotExist(UId id) : EIdNotExist(id) {};
};

// Id ��������� ��� ����������
struct EPointerIdAlreadyExist: public EIdAlreadyExist
{
EPointerIdAlreadyExist(UId id) : EIdAlreadyExist(id) {};
};

// ��� ��������� �� �������
struct EPointerNameNotExist: public ENameNotExist
{
EPointerNameNotExist(const std::string &name) : ENameNotExist(name) {};
};

// ��� ��������� ��� ����������
struct EPointerNameAlreadyExist: public ENameAlreadyExist
{
EPointerNameAlreadyExist(const std::string &name) : ENameAlreadyExist(name) {};
};

// ������ ����������/�������� ���������
// Id ��������� ��� ����������
struct EAddComponentAlreadyHaveOwner: public EIdError
{
EAddComponentAlreadyHaveOwner(UId id) : EIdError(id) {};
};

// ������������ ��� ��������� ����������
struct EAddComponentHaveInvalidType: public EIdError
{
EAddComponentHaveInvalidType(UId id) : EIdError(id) {};
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

}

#include "UPointer.h"
#include "UShare.h"

#endif

