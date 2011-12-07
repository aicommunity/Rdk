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

#include <vector>
#include <map>
#include "UAComponent.h"
#include "UEnvSupport.h"
#include "UController.h"
#include "../Math/MVector.h"

namespace RDK {

class UAContainerStorage;
class UInstancesStorageElement;

typedef long int IndexT;
typedef int UTime;
typedef long long ULongTime;

//class UAContainer;
typedef UAContainer* PUAContainer;

// ������ ���������� �� ����������
typedef std::vector<UEPtr<UAContainer> > UAContainerVector;

extern const UTime DefaultTimeStep;


class UAContainer: public UAComponent
{
public: // ���� ������
typedef std::map<NameT,UVariable> VariableMapT;
typedef std::map<NameT,UVariable>::iterator VariableMapIteratorT;
typedef std::map<NameT,UVariable>::const_iterator VariableMapCIteratorT;

typedef std::map<NameT,UPVariable> PointerMapT;
typedef std::map<NameT,UPVariable>::iterator PointerMapIteratorT;
typedef std::map<NameT,UPVariable>::const_iterator PointerMapCIteratorT;

friend class UAContainerStorage;
friend class UController;

public: // ������ �������� ����������
 /* ������� ����� �������� ���������� */
 class IException
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
 IException(void);
 IException(const UAContainer *cont);
 IException(const IException &copy);
 virtual ~IException(void);
 // --------------------------
 };
/* **************************** */


private: // ��������� ��������
// ������� ������������ ���� � Id ��������� �������
std::map<NameT,UId> CompsLookupTable;

// ������� ������������ ���� � Id ���������� �������
VariableMapT PropertiesLookupTable;

// ������� ������������ ���� � Id ������ ��������� �������
VariableMapT StateLookupTable;

// ������� ��������� ���������� �� �������� ����������
PointerMapT PointerLookupTable;

// ������� ������������ <���������� � ������� Components, ��������� ���������>
//std::map<UAContainer*,UAContainer**> ComponentPointers;

private: // ���������� ��������
// ������� ����� ������ � �������������
static ULongTime Time;

// ������� ����� ������ � ��������
static double DoubleTime;

// �������� ����� � �������������
static ULongTime RealTime;

// �������� ����� � ��������
static double DoubleRealTime;

// ���������� ��� � �������� ������� � �������������
static ULongTime RealTimeStep;

// ���������� ��� � �������� ������� � ��������
static double DoubleRealTimeStep;

private: // ��������� ��������

// ������� ���������
UAContainerVector Components;

// ������� ������������ ����������
std::vector<UController*> Controllers;

// ������� ������������ <���������� � ������� Components, ��������� ���������>
//std::map<UAContainer*,UAContainer**> ComponentPointers;

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

public: // ���������� ��������
// ���������� ���������� � ������������ ����
RDK::MVector<double> Coord;


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
UEPtr<UAContainer>* PComponents;

// ���������� ��������� � ������� ���������
int NumComponents;

// ��������� �� ���� ������ � ���������
UInstancesStorageElement* ObjectIterator;

// ��������� �������������� Id ���������
UId LastId;

public: // �������� ������
// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
static const ULongTime& GetTime(void);
static const double& GetDoubleTime(void);

// ������������� ������� ����� ������
static bool SetTime(ULongTime value);

// ���������� �������� �����
static const ULongTime& GetRealTime(void);
static const double& GetDoubleRealTime(void);

// ������������� �������� �����
static bool SetRealTime(ULongTime value);

// ���������� ���������� ��� � �������� �������
static const ULongTime& GetRealTimeStep(void);
static const double& GetDoubleRealTimeStep(void);
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
UAContainer(void);
virtual ~UAContainer(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
UEPtr<UAContainer> GetOwner(void) const;

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UAContainer> GetMainOwner(void) const;

// ���������� ��������� ��������� ����� �������
UAContainerStorage* const GetStorage(void) const;

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool CheckOwner(UEPtr<UAContainer> owner) const;

// ���������� ������ Id �������
// (������� Id ���� ����������)
ULongId& GetFullId(ULongId &buffer=ULongIdemp) const;

// ����������  '�������' Id �������
// (�������� ��� ��������� 'mainowner')
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������
ULongId& GetLongId(UEPtr<UAContainer> mainowner, ULongId &buffer=ULongIdemp) const;
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ������� ��������� �������
bool BreakOwner(void);

// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
bool SetMainOwner(UEPtr<UAComponent> mainowner);
bool SetMainOwner(UEPtr<UAComponent> mainowner, int levels);

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
RDK::MVector<double> GetCoord(void) const;
bool SetCoord(RDK::MVector<double> value);

// ������������� �������� ���� ��������������
UTime GetTimeStep(void) const;
bool SetTimeStep(UTime timestep);

// ������������� ���� ���������� �������
bool GetActivity(void) const;
bool SetActivity(bool activity);

UId GetClass(void) const;
bool SetClass(UId value);

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

// ����������  '�������' ��� �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������
NameT& GetLongName(const UEPtr<UAContainer> mainowner, NameT &buffer) const;
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
public:
// ���������� ��� ��������� ���������� �� ��� Id
const NameT& GetComponentName(const UId &id) const;

// ���������� Id ��������� ���������� �� ��� �����
const UId& GetComponentId(const NameT &name) const;

// ���������� ��� ��������� �� ��� Id
const NameT& GetPropertyName(const UId &id) const;

// ���������� Id ��������� �� ��� �����
const UId& GetPropertyId(const NameT &name) const;

// ���������� ������ ��� ��������� ��� �������� NMSDK, � �������� '*'
NameT GetPropertyLongName(const NameT &name) const;
NameT GetPropertyLongName(const UId &id) const;

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& GetStateName(const UId &id) const;

// ���������� Id ���������� ��������� �� ��� �����
const UId& GetStateId(const NameT &name) const;

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
NameT GetStateLongName(const NameT &name) const;
NameT GetStateLongName(const UId &id) const;

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& GetPointerName(const UId &id) const;

// ���������� Id ���������� ��������� �� ��� �����
const UId& GetPointerId(const NameT &name) const;
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UAContainer* New(void)=0;

// ������� ����� ����� ������� � ����������� ���� ���������
// � �������� ����������.
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual UEPtr<UAContainer> Alloc(UAContainerStorage *stor=0, bool copystate=false);

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(UEPtr<UAContainer> target, UAContainerStorage *stor=0, bool copystate=false) const;

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ���������� ����� �������� ���������
int GetNumComponents(void) const;

// ���������� ������ ����� �������� ���������
// (������� ��� ���������� �������� ���������)
int GetNumAllComponents(void) const;

// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
virtual UEPtr<UAContainer> GetComponent(const UId &id) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
virtual UEPtr<UAContainer> GetComponent(const NameT &name) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
UEPtr<UAContainer> GetComponentL(const ULongId &id) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� ����� 'name'
virtual UEPtr<UAContainer> GetComponentL(const NameT &name) const;

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ����������� ������ � ������ ���������
// ����� ���������� 0, ���� ������ ������� �� ������� �������
UEPtr<UAContainer> GetComponentByIndex(int index) const;

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
virtual UId AddComponent(UEPtr<UAContainer> comp, UIPointer* pointer=0);

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'id' - ������ ���� �������� Id
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
virtual bool DelComponent(const UId &id, bool canfree=true);

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'name' - ������ ���� �������� ������
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
bool DelComponent(const NameT &name, bool canfree);

// ������������� ������� ��� �������� ����������
void DelAllComponents(void);

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void GetComponentsList(UId *buffer) const;

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������
// ���� ��������� stor != 0 �� ������������ ���
virtual bool CopyComponents(UEPtr<UAContainer> comp, UAContainerStorage* stor=0) const;
// --------------------------


// --------------------------
// ������ ������� � ����������
// --------------------------
// ���������� �������� ��������� �� Id 'id'
UVariableData* GetProperty(const UId &id, UVariableData *values) const;

// ���������� �������� ��������� �� ����� 'name'
UVariableData* GetProperty(const NameT &name, UVariableData *values) const;

// ������������� �������� ��������� �� Id 'id'
bool SetProperty(const UId &id, UVariableData *values);

// ������������� �������� ��������� �� ����� 'name'
bool SetProperty(const NameT &name, UVariableData *values);

// ���������� ������ Id ����������, ������������ ���������������
// � ���� �������
const UAContainer::VariableMapT& GetPropertiesList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindPropertyName(const UIProperty *prop) const;

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
virtual bool CopyProperties(UEPtr<UAContainer> comp) const;
// --------------------------

// ----------------------
// ������ ���������� ����������������� ������������
// ----------------------
public:
// ���������� ������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetConnectorsList(ULongIdVector &buffer,
						  int sublevel=-1, UEPtr<UAContainer> ownerlevel=0);

// ���������� ������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetItemsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UAContainer> ownerlevel=0);

// ���������� ������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
virtual ULongIdVector& GetNetsList(ULongIdVector &buffer,
							int sublevel=-1, UEPtr<UAContainer> ownerlevel=0);
// ----------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// ���������� �������� ���������� ��������� �� Id 'id'
virtual UVariableData* GetState(const UId &id, UVariableData *values) const;

// ���������� �������� ���������� ��������� �� ����� 'name'
UVariableData* GetState(const NameT &name, UVariableData *values) const;

// ������������� �������� ���������� ��������� �� Id 'id'
virtual bool SetState(const UId &id, UVariableData *values);

// ������������� �������� ���������� ��������� �� ����� 'name'
bool SetState(const NameT &name, UVariableData *values);

// ���������� ������ ���� � Id ���������� ���������, ������������ ���������������
// � ���� �������
const UAContainer::VariableMapT& GetStateList(void) const;

// ���� ��� �������� �� ��������� �� ����
const NameT& FindStateName(const UIProperty *prop) const;

// �������� ��� ���������� ��������� ����� ������� � ������ 'comp', ���� ��������.
virtual bool CopyState(UEPtr<UAContainer> comp) const;
// --------------------------

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
virtual bool SharesInit(void);

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
virtual bool SharesUnInit(void);
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool Default(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool Build(void);

// ����� �������� ����� ��� ������ ��������
virtual bool Reset(void);

// ��������� ������ ����� �������
virtual bool Calculate(void);

// ��������� ��������� MainOwner ����� ������� ����� �������
virtual bool UpdateMainOwner(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������������� ����������
// �������� ������������ ����� �� ���������� ������
// --------------------------
protected:
// ��������� ����� ����������
bool AddController(UController *controller, bool forchilds=false);

// ������� ���������� �� ������
bool DelController(UController *controller, bool forchilds=false);

// ������� ��� �����������
bool DelAllControllers(bool forchilds=false);

// ���������� ���������� ���� ������������
bool UnLinkAllControllers(bool forchilds=false);

// ���������, ���������� �� ���������� � ������
bool CheckController(UController *controller) const;

// ���������� ����� ������������
bool GetNumControllers(void) const;

// ���������� ���������� �� �������
UController* GetController(int index);
// --------------------------

protected:
// --------------------------
// ������� ������ ���������� ����������
// --------------------------
public:
// ��������� ���������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupState(const NameT &name,UIProperty *property, bool delenable=true);

protected:
// ������� ���������� ��������� � ������ 'name' �� ������� �����������
bool DelLookupState(const NameT &name);

// ������� ��� ������� ������������
void ClearLookupStateTable(void);

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
NameT GetStateLongName(const UIProperty &property) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
public:
// ��������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupPointer(const NameT &name, UIPointer *pointer);

protected:
// ������� ��������� � ID 'id' �� ������� �����������
bool DelLookupPointer(const NameT &name);

// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT GetPointerLongName(const UIPointer &pointer) const;

// ������������ ����� � ������� ���������, ���������������� ��������� ���������
PointerMapCIteratorT FindLookupPointer(UEPtr<UAContainer> source) const;
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void AddComponentTable(UEPtr<UAContainer> comp, UIPointer* pointer=0);

// ������� ��������� 'comp' �� ������� ���������
void DelComponentTable(UEPtr<UAContainer> comp);
// --------------------------

// --------------------------
// ������� ������ ���������� ������ (shared) �����������
// --------------------------
protected:
// ����� ������������� ����� ����������. ���������� ������������� ��� ����������
// ������� ���������
virtual bool ASharesInit(void);

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
virtual bool ASharesUnInit(void);
// --------------------------


protected:
// --------------------------
// ������� ������ ���������� �������� ����������� ���������
// --------------------------
// ��������� ������� ������������ ��������� ������� 'oldname'
// ��� ���������� �� 'newname'
bool ModifyLookupComponent(const NameT &oldname, const NameT newname);

// ��������� ������� ������������ ��������� ������������ Id 'id'
// ��� ���������� � ������ 'name'
void SetLookupComponent(const NameT &name, const UId &id);

// ��������� ������� ������������ ��������� ������ ������
// ���������� � ������ 'name'
void DelLookupComponent(const NameT &name);
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
public:
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId AddLookupProperty(const NameT &name, UIProperty *property, bool delenable=true);

protected:
// ������� �������� � ������ 'name' �� ������� �����������
// ����������
bool DelLookupProperty(const NameT &name);

// ������� ��� ������� ������������
void ClearLookupPropertyTable(void);

// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT GetPropertyLongName(const UIProperty &property) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ������� ��������� comp
// ����� ������������, ��� ��������� ����������� �������
bool DelComponent(UEPtr<UAContainer> comp, bool canfree);

// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
// ����� ���� ������� ��������� �� ��������� ����������
virtual bool AAddComponent(UEPtr<UAContainer> comp, UIPointer* pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UAContainer> comp);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ��������� ��������� MainOwner ����� ������� ����� �������
virtual bool AUpdateMainOwner(void);
// --------------------------
};



}

#include "UProperty.h"
#include "UPointer.h"   
#include "UShare.h"
#include "ULocalProperty.h"

#endif
