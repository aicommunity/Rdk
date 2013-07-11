/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAContainerCPP
#define UAContainerCPP

#include <algorithm>
#include <string.h>
#include <cstdio>
#include "UContainer.h"
#include "UStorage.h"
#include "UConnector.h"
#include "UItem.h"
#include "UNet.h"
#include "../Serialize/Serialize.h"
#include "../Math/MUBinarySerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "UStorage.h"
#include "UContainerDescription.h"

namespace RDK {

NameT ForbiddenName="";


// --------------------------
// ������������ � �����������
// --------------------------
UPVariable::UPVariable(void)
{
 Id=ForbiddenId;
}

UPVariable::UPVariable(UId id, UEPtr<UIPointer> prop)
 : Id(id), Pointer(prop)
{

}

UPVariable::~UPVariable(void)
{

}
// --------------------------


/* *************************************************************************** */
// Class UContainer
/* *************************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UContainer::UContainer(void)
 : Id(0), Coord(0), PComponents(0), NumComponents(0), LastId(0)
{
 AddLookupProperty("Id",ptParameter | pgSystem,new UVProperty<UId,UContainer>(this,&UContainer::SetId,&UContainer::GetId));
 AddLookupProperty("Name",ptParameter | pgSystem,new UVProperty<NameT,UContainer>(this,&UContainer::SetName,&UContainer::GetName));
 AddLookupProperty("TimeStep",ptParameter | pgSystem,new UVProperty<UTime,UContainer>(this,&UContainer::SetTimeStep,&UContainer::GetTimeStep));
 AddLookupProperty("Activity",ptParameter | pgPublic,new UVProperty<bool,UContainer>(this,&UContainer::SetActivity,&UContainer::GetActivity));
 AddLookupProperty("Coord",ptParameter | pgPublic,new UVProperty<RDK::MVector<double,3>,UContainer>(this,&UContainer::SetCoord,&UContainer::GetCoord));
 InitFlag=false;
}

UContainer::~UContainer(void)
{
 DelAllComponents();
 UnLinkAllControllers();

 BreakOwner();

 if(Storage)
  Storage->PopObject(UEPtr<UContainer>(this));
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
UEPtr<UContainer> UContainer::GetOwner(void) const
{
 return dynamic_pointer_cast<UContainer>(Owner);
}

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UContainer> UContainer::GetMainOwner(void) const
{
 return dynamic_pointer_cast<UContainer>(MainOwner);
}

// ���������� ��������� ��������� ����� �������
UEPtr<UStorage> const UContainer::GetStorage(void) const
{
 return Storage;
}

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool UContainer::CheckOwner(UEPtr<UContainer> owner) const
{
 if(Owner == 0 && Owner != owner)
  return false;

 if(Owner == owner)
  return true;

 return GetOwner()->CheckOwner(owner);
}

// ���������� ������ Id �������
// (������� Id ���� ����������).
ULongId& UContainer::GetFullId(ULongId &buffer) const
{
 if(Owner == 0)
  {
   buffer.Add(Id);
   return buffer;
  }

 GetOwner()->GetFullId(buffer);
 buffer.Add(Id);

 return buffer;
}

// ����������  '�������' Id �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������.
ULongId& UContainer::GetLongId(UEPtr<UContainer> mainowner, ULongId &buffer) const
{
 if(Owner == 0 && Owner != mainowner)
  {
   buffer.Resize(0);
   return buffer;
  }

 if(Owner == mainowner)
  {
   buffer.Resize(0);
   buffer.Add(Id);
   return buffer;
  }

 if(GetOwner()->GetLongId(mainowner,buffer).GetSize() == 0)
  return buffer;

 buffer.Add(Id);

 return buffer;
}

// ������������� ������� ������������ ������, ������������� ������� ���
std::string& UContainer::GetLongId(UEPtr<UContainer> mainowner, std::string &buffer) const
{
 return GetLongName(mainowner,buffer);
}

// ���������� true ���� ������������ ������������� ������� ���������, � ��������� ������ ���������� false
bool UContainer::CheckLongId(const ULongId &id) const
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  return false;

 return true;
}

// ������������� ������� ������������ ������, ��������������� ������� ���
bool UContainer::CheckLongId(const std::string &id) const
{
 if(id.size() == 0)
  return false;

 return true;
}

// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ���������� � ������������ ����
const RDK::MVector<double,3>& UContainer::GetCoord(void) const
{
 return Coord;
}

bool UContainer::SetCoord(const RDK::MVector<double,3> &value)
{
 if(Coord != value)
  Coord=value;

 return true;
}


// �����, ����������� �� ��������� �������
// (��� ����� ������� ������� �������� ��������) (��)
long long UContainer::GetStepDuration(void) const
{
 long long res=0;
 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetFullStepDuration();

 return StepDuration-res;
}

// �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
long long UContainer::GetFullStepDuration(void) const
{
 return StepDuration;
}

// �����, ��������� ����� ����� ���������� ���������� �����
long long UContainer::GetInterstepsInterval(void) const
{
 return InterstepsInterval;
}

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double UContainer::GetInstantPerformance(void) const
{
 return ((GetFullStepDuration()*TimeStep)/1000.0);
}

// ������� ��������� �������
void UContainer::BreakOwner(void)
{
 UEPtr<UContainer> owner=GetOwner();
 if(owner)
  owner->DelComponent(this,false);
}

// ������������� ��������� �� �������� ��������� ���� ��������
// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
void UContainer::SetMainOwner(UEPtr<UComponent> mainowner)
{
 UComponent::SetMainOwner(mainowner);
}

void UContainer::SetMainOwner(UEPtr<UComponent> mainowner, int levels)
{
 if(MainOwner == mainowner && !levels)
  return;

 MainOwner=mainowner;

 if(!levels)
  return;

 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++, comps++)
 {
  // ������������� �������� ��������� ������ ��� �������� �����������
  // � ������� �� ��� �� �����
  if((*comps)->GetMainOwner() == 0)
  {
   if(levels<0)
    (*comps)->SetMainOwner(MainOwner,levels);
   else
    (*comps)->SetMainOwner(MainOwner,levels-1);
  }
 }
}

// ��������� ������������ Id 'id' �� ������������ � ������ �������, �������.
bool UContainer::CheckId(const UId &id)
{
 return (id>LastId)?true:false;
}

// ��������� ������������ ��� 'name' �� ������������ � ������
// ������� �������.
bool UContainer::CheckName(const NameT &name)
{
// if(Name == name)
//  return true;

// if(!GetOwner())
//  return true;

 if(CompsLookupTable.find(name) == CompsLookupTable.end())
  return true;

 return false;
}

// ���������� ���������� Id.
UId UContainer::GenerateId(void)
{
 return LastId+1;
}

#ifdef __BORLANDC__
#pragma warning (disable : 4996)
#endif
// ���������� ��� ���������� � ����������� ����� �������
NameT& UContainer::GenerateName(const NameT &prefix, NameT &namebuffer)
{
 int k=2;
 //char buffer[20];

 while(true)
  {
   std::stringstream stream;
   stream<<k;

//   std::sprintf(buffer,"%i",k);
   //stream.str();
   if(prefix.size()>0)
    namebuffer=prefix+stream.str();
   else
    namebuffer=NameT("n")+stream.str();
   if(CheckName(namebuffer))
    break;

   k++;

   if(k == 0) // ��������!! ������ ���� ���������� - ������� ���������
    return namebuffer;
  }

 return namebuffer;
}
#ifdef __BORLANDC__
#pragma warning (default : 4996)
#endif

// ������������� ��� �������.
const NameT& UContainer::GetName(void) const
{
 return Name;
}

bool UContainer::SetName(const NameT &name)
{
 if(Name == name)
  return true;

 if(name.empty())
  return false;

  if(GetOwner() != 0)
  {
   if(!GetOwner()->CheckName(name))
    throw EComponentNameAlreadyExist(name);

   GetOwner()->ModifyLookupComponent(Name, name);
  }
 Name=name;
 return true;
}

// ���������� ������ ��� �������
// (������� ����� ���� ����������).
NameT& UContainer::GetFullName(NameT &buffer) const
{
 if(!GetOwner())
  {
   buffer+=Name;
   return buffer;
  }

 GetOwner()->GetFullName(buffer);
 buffer+='.';
 buffer+=Name;

 return buffer;
}

// ����������  '�������' ����� �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������.
NameT& UContainer::GetLongName(const UEPtr<UContainer> mainowner, NameT &buffer) const
{
 if(!GetOwner() && GetOwner() != mainowner)
  {
   buffer.clear();
   return buffer;
  }

 if(GetOwner() == mainowner)
  {
   buffer=Name;
   return buffer;
  }

 if(GetOwner()->GetLongName(mainowner,buffer) == ForbiddenName)
  {
   buffer.clear();
   return buffer;
  }

 buffer+='.';
 buffer+=Name;

 return buffer;
}
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
// ���������� ��� ��������� ���������� �� ��� Id
const NameT& UContainer::GetComponentName(const UId &id) const
{
 for(std::map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
                                 J=CompsLookupTable.end(); I!=J; ++I)
 {
  if(I->second == id)
   return I->first;
 }
 throw EComponentIdNotExist(id);
}

// ���������� Id ��������� ���������� �� ��� �����
const UId& UContainer::GetComponentId(const NameT &name) const
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
  throw EComponentNameNotExist(name);

 return I->second;
}

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& UContainer::GetPointerName(const UId &id) const
{
 for(PointerMapCIteratorT I=PointerLookupTable.begin(),
                                 J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
   return I->first;
 }
 throw EPointerIdNotExist(id);
}

// ���������� Id ���������� ��������� �� ��� �����
const UId& UContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  throw EPointerNameNotExist(name);

 return I->second.Id;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���� ��������������
const UTime& UContainer::GetTimeStep(void) const
{
 return TimeStep;
}

bool UContainer::SetTimeStep(const UTime &timestep)
{
 if(timestep <= 0)
  return false;

 TimeStep=timestep;

 if(Owner)
  OwnerTimeStep=GetOwner()->TimeStep;
 else
  OwnerTimeStep=timestep;

 // ��������� �� ���� ����������� �������
 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->OwnerTimeStep=timestep;

 return true;
}

// ������������� �������� ���� �������������� ���������� � ���� ��� �������� �����������
bool UContainer::SetGlobalTimeStep(UTime timestep)
{
 if(!SetTimeStep(timestep))
  return false;

 // ��������� �� ���� ����������� �������
 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(!(*comps)->SetGlobalTimeStep(timestep))
   return false;

 return true;
}



// ������������� ���� ���������� �������
const bool& UContainer::GetActivity(void) const
{
 return Activity;
}

bool UContainer::SetActivity(const bool &activity)
{
 if(Activity == activity)
  return true;

 Activity=true;
 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->SetActivity(activity);

// if(activity)
//  return Reset(); // !!! ��������. �������� ��� �� �����!

 Activity=activity;
 StepDuration=0;
 InterstepsInterval=0;

 if(!Activity)
  return Reset();

 return true;
}

// Id �������
const UId& UContainer::GetId(void) const
{
 return Id;
}

bool UContainer::SetId(const UId &id)
{
 if(Id == id)
  return true;

 if(id == ForbiddenId)
  return true;// ��������!! ��� ���! throwEForbiddenId(id);

 if(id < 0)
  throw EInvalidId(id);


 if(Owner != 0)
  {
   if(!GetOwner()->CheckId(id))
    throw EComponentIdAlreadyExist(id);

   GetOwner()->SetLookupComponent(Name, id);
  }
 Id=id;
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ����� ����� ������� � ����������� ���� ���������
// � �������� ����������.
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
UEPtr<UContainer> UContainer::Alloc(UEPtr<UStorage> stor, bool copystate)
{
 UEPtr<UContainer> copy;
 UEPtr<UStorage> storage=(stor!=0)?stor:GetStorage();

 if(storage)
 {
  copy=dynamic_pointer_cast<UContainer>(storage->TakeObject(Class,this));
 }
 else
 {
  copy=New();
  Copy(copy,stor,copystate);
 }

 return copy;
}

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
bool UContainer::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 CopyProperties(target, ptParameter);
 target->Build();

 if(copystate)
  CopyProperties(target, ptState);

 CopyComponents(target,stor);
 return true;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UContainer::Free(void)
{
 while(NumComponents)
  PComponents[0]->Free();

 if(Storage)
 {
  BreakOwner();
  GetStorage()->ReturnObject(this);
 }
 else
  delete this;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ���������� ����� �������� ���������
int UContainer::GetNumComponents(void) const
{
 return NumComponents;
}


// ���������� ������ ����� �������� ���������
// (������� ��� ���������� �������� ���������)
int UContainer::GetNumAllComponents(void) const
{
 int res=NumComponents;

 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  res+=(*comps)->GetNumAllComponents();

 return res;
}


// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool UContainer::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
UEPtr<UContainer> UContainer::GetComponent(const UId &id) const
{
 if(id == ForbiddenId)
  throw EComponentIdNotExist(id);

 UEPtr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(id == (*comps)->Id)
   return *comps;

 throw EComponentIdNotExist(id);
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
UEPtr<UContainer> UContainer::GetComponent(const NameT &name) const
{
 return GetComponent(GetComponentId(name));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'.
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
UEPtr<UContainer> UContainer::GetComponentL(const ULongId &id) const
{
 UEPtr<UContainer> comp;

 if(id.GetSize() == 0)
  return 0;

 comp=GetComponent(id[0]);
 for(int i=1;i<id.GetSize();i++)
  {
   if(!comp)
    return 0;
   comp=comp->GetComponent(id[i]);
  }
 return comp;
}


// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� ����� 'name'
UEPtr<UContainer> UContainer::GetComponentL(const NameT &name) const
{
 UEPtr<UContainer> comp;
 NameT::size_type pi,pj;

 pi=name.find_first_of('.');
 if(pi == NameT::npos)
  return GetComponent(name);

 comp=GetComponent(name.substr(0,pi));
 while(pi != name.size())
  {
   if(!comp)
    return 0;
   pj=pi+1;
   pi=name.find_first_of('.',pj);
   if(pi == NameT::npos)
    pi=name.size();
   comp=comp->GetComponent(name.substr(pj,pi-pj));
  }
 return comp;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ����������� ������ � ������ ���������
// ����� ���������� 0, ���� ������ ������� �� ������� �������
UEPtr<UContainer> UContainer::GetComponentByIndex(int index) const
{
 return Components[index];
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
void UContainer::BeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 ABeforeAddComponent(comp,pointer);
}

void UContainer::AfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 AAfterAddComponent(comp,pointer);
}

UId UContainer::AddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  throw EAddComponentAlreadyHaveOwner(comp->Id);

 if(!CheckComponentType(comp))
  throw EAddComponentHaveInvalidType(comp->Id);

 BeforeAddComponent(comp,pointer);

 NameT namebuffer;

 if(!CheckName(comp->Name))
  comp->SetName(GenerateName(comp->Name,namebuffer));
 UId id=GenerateId();

 bool res=true;
 int i=0;
 for(i=0;i<NumComponents;i++)
  if(PComponents[i]->Id == id)
  {
   res=false;
   break;
  }

 if(!res)
  throw EComponentIdAlreadyExist(id);

 comp->SetId(id);
 comp->SetOwner(this);

 // ��������� ��������� � ������� ������������ ���������
 SetLookupComponent(comp->Name, comp->Id);

 // ���������� � ���� ���������
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 if(MainOwner)
  comp->SetMainOwner(MainOwner);

 try{
  AAddComponent(comp,pointer);
  comp->SharesInit();
  AfterAddComponent(comp,pointer);
 }
 catch(UException &exception)
 {
  // �����
  // ������� ��������� �� ������� ������������ ���������
  DelLookupComponent(comp->Name);

  // �������� �� ���� ���������
  DelComponentTable(comp);

  comp->Owner=0;
  throw;
 }

 return comp->Id;
}

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'id' - ������ ���� �������� Id
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void UContainer::DelComponent(const UId &id, bool canfree)
{
 UEPtr<UContainer> comp=GetComponent(id);

 DelComponent(comp, canfree);
}

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'name' - ������ ���� �������� ������
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void UContainer::DelComponent(const NameT &name, bool canfree)
{
 DelComponent(GetComponentId(name),canfree);
}

// ������������� ������� ��� �������� ����������
void UContainer::DelAllComponents(void)
{
 while(NumComponents)
  DelComponent(PComponents[NumComponents-1],true);
}

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void UContainer::GetComponentsList(std::vector<UId> &buffer) const
{
 UEPtr<UContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Id);
}

void UContainer::GetComponentsList(vector<NameT> &buffer) const
{
 UEPtr<UContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Name);
}

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������.
void UContainer::CopyComponents(UEPtr<UContainer> comp, UEPtr<UStorage> stor) const
{
 UEPtr<UContainer> bufcomp;

 // ������� ������ ���������� �� 'comp'
 comp->DelAllComponents();

 UEPtr<UContainer> * pcomponents=0;
 PointerMapCIteratorT I;
 PointerMapIteratorT J;
 if(NumComponents>0)
  pcomponents=&PComponents[0];

 for(int i=0;i<NumComponents;i++,pcomponents++)
  {
   bufcomp=(*pcomponents)->Alloc(stor);
   UEPtr<UIPointer> pointer=0;
   I=FindLookupPointer(*pcomponents);
   if(I != PointerLookupTable.end())
   {
    J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second.Pointer;
   }

   comp->AddComponent(bufcomp,pointer);
   bufcomp->SetId((*pcomponents)->Id);
   comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
  }
 /*
 // ������� ������ ���������� �� 'comp'
 for(int i=0;i<comp->GetNumComponents();i++)
 {
  bufcomp=comp->GetComponentByIndex(i);
  if(!GetComponent(bufcomp->Id))
   if(!comp->DelComponent(bufcomp,true))
    return false;
 }

 for(int i=0;i<NumComponents;i++)
  {
   bufcomp=comp->GetComponent(PComponents[i]->Id());
   if(bufcomp)
	if(!comp->DelComponent(bufcomp->Id()))
     return false;

   bufcomp=PComponents[i]->Alloc(PComponents[i]->Name(),stor);
   UIPointer *pointer=0;
   std::map<UId,UIPointer*>::const_iterator I=FindLookupPointer(PComponents[i]);
   if(I != PointerLookupTable.end())
   {
    std::map<UId,UIPointer*>::iterator J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second;
   }

   if(comp->AddComponent(bufcomp,pointer) == ForbiddenId)
    return false;

   bufcomp->Id=PComponents[i]->Id;
   comp->SetLookupComponent(bufcomp->Name(), bufcomp->Id());
  }
 return true;    */
}


// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
bool UContainer::ChangeComponentPosition(int index, int step)
{
 if(index<0 || index >= NumComponents)
  return false;

 if(step == 0)
  return true;

 int result=index+step;
 if(result<0)
  result=0;
 if(result>=NumComponents)
  result=NumComponents-1;

 UEPtr<UContainer> comp=PComponents[index];
 if(result>index)
 {
  for(size_t i=index;i<=result;i++)
   PComponents[i]=PComponents[i+1];
  PComponents[result]=comp;
 }
 else
 {
  for(size_t i=result;i<index;i++)
   PComponents[i+1]=PComponents[i];
  PComponents[result]=comp;
 }

// Components.insert(Components.begin()+result+1,Components[index]);
// Components.erase(Components.begin()+index);
// PComponents=&Components[0];
 return true;
}

bool UContainer::ChangeComponentPosition(const NameT &name, int step)
{
 for(size_t i=0;i<NumComponents;i++)
  if(PComponents[i]->GetName() == name)
   return ChangeComponentPosition(i,step);

 return false;
}
// --------------------------


// ----------------------
// ������ ���������� ����������������� ������������
// ----------------------
// ���������� ������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
ULongIdVector& UContainer::GetConnectorsList(ULongIdVector &buffer,
							int sublevel, UEPtr<UContainer> ownerlevel)
{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UContainer> cont=GetComponentByIndex(i);
  UEPtr<UConnector> temp;
  temp=dynamic_pointer_cast<UConnector>(cont);
  if(temp)
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetConnectorsList(buffer,(sublevel<0)?-1:sublevel-1,ownerlevel);
 }

 return buffer;
}

// ���������� ������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
ULongIdVector& UContainer::GetItemsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UContainer> ownerlevel)

{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UContainer> cont=GetComponentByIndex(i);
  UEPtr<UItem> temp;
  temp=dynamic_pointer_cast<UItem>(cont);
  if(temp)
//  if(dynamic_cast<UItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetItemsList(buffer,(sublevel<0)?-1:sublevel-1,ownerlevel);
 }

 return buffer;
}

// ���������� ������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
ULongIdVector& UContainer::GetNetsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UContainer> ownerlevel)
{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UContainer> cont=GetComponentByIndex(i);
  UEPtr<UNet> temp;
  temp=dynamic_pointer_cast<UNet>(cont);
  if(temp)
//  if(dynamic_cast<UNet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetNetsList(buffer,(sublevel<0)?-1:sublevel-1,ownerlevel);
 }

 return buffer;
}
// ----------------------


// --------------------------
// ������ ���������� ���������� �����������
// --------------------------
// ������������� �������� ��������� 'id' � �������� ��������� ������ ��������� ����������
// 'pointerid'
bool UContainer::SetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UContainer> cont=GetComponent(id);

 PointerMapIteratorT K=PointerLookupTable.end();
 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   if(J->second.Id == pointerid)
    return true;
  }

  if(J->second.Id == pointerid)
   K=J;

  ++J;
 }

 if(K != PointerLookupTable.end() && K->second.Pointer)
 {
  K->second.Pointer->Set(cont);
  return true;
 }

 return false;
}

// ������������� �������� ��������� 'name' � �������� ��������� ������ ��������� ����������
// 'pointername'
bool UContainer::SetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UContainer> cont=GetComponent(name);

 PointerMapIteratorT K=PointerLookupTable.end();;
 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   if(J->first == pointername)
    return true;
  }

  if(J->first == pointername)
   K=J;

  ++J;
 }

 if(K != PointerLookupTable.end() && K->second.Pointer)
 {
  K->second.Pointer->Set(cont);
  return true;
 }

 return false;
}

// ���������� ��������� ���������� ���������� 'id' � ��������� ������ ��������� ����������
// 'pointerid'
bool UContainer::ResetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UContainer> cont=GetComponent(id);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Id == pointerid && J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
   return true;
  }

  ++J;
 }

 return true;
}

// ���������� ��������� ���������� ���������� 'name' � ��������� ������ ��������� ����������
// 'pointername'
bool UContainer::ResetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UContainer> cont=GetComponent(name);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->first == pointername && J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
   return true;
  }

  ++J;
 }

 return true;
}

// ���������� ��������� ��������� ���������� 'id' �� ���� ������� ��������� ����������
bool UContainer::ResetComponentAll(const UId &id)
{
 UEPtr<UContainer> cont=GetComponent(id);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
  }

  ++J;
 }

 return true;
}

// ���������� ��������� ��������� ���������� 'name' �� ���� ������� ��������� ����������
bool UContainer::ResetComponentAll(const NameT &name)
{
 UEPtr<UContainer> cont=GetComponent(name);

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont)>=0)
  {
   J->second.Pointer->Del(cont);
  }

  ++J;
 }

 return true;
}

// ������� ��� ���������� ����������� � ��������� ������ ��������� ����������
void UContainer::DelAllComponentsAs(const NameT &pointername, bool canfree)
{
 PointerMapIteratorT J=PointerLookupTable.find(pointername);

 if(J != PointerLookupTable.end())
 {
  UEPtr<UContainer> cont=J->second.Pointer->Get();

  while(cont)
  {
   DelComponent(cont,canfree);
   cont=J->second.Pointer->Get();
  }
 }
}
// --------------------------


// --------------------------
// ������ ���������� ������ (shared) �����������
// --------------------------
// ����� ������������� ����� ����������. ���������� ������������� ��� ����������
// ������� ���������
void UContainer::SharesInit(void)
{
 ShareMapIteratorT I=ShareLookupTable.begin();
 ShareMapIteratorT J=ShareLookupTable.end();
 for(;I != J;++I)
  I->second->Init(MainOwner);
 ASharesInit();
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
void UContainer::SharesUnInit(void)
{
 ASharesUnInit();
 ShareMapIteratorT I=ShareLookupTable.begin();
 ShareMapIteratorT J=ShareLookupTable.end();
 for(;I != J;++I)
  I->second->UnInit();
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UContainer::Default(void)
{
 Ready=false;
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Default();

 // ���� ���������� �������� � ���������, �� ����� ��������� ����������
 // �� ���������
 UEPtr<UContainer> original;
 if(Storage)
  original=dynamic_pointer_cast<UContainer>(GetStorage()->GetClass(Class));

 SetTimeStep(2000);

 if(original && original != this)
 {
  NameT name=Name;
  bool activity=Activity;
  original->CopyProperties(this,ptParameter);
  SetName(name);
  SetActivity(activity);
 }

 return ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UContainer::Build(void)
{
 if(Ready)
  return true;

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Build();

 ABuild();
 Ready=true;
 Reset();

 return true;
}

// ����� �������� �����.
bool UContainer::Reset(void)
{
 Build();

// Init(); // ��������

 if(!IsInit())
  return true; // TODO //false;

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Reset();

 AReset();

 CalcCounter=0;
 SkipComponentCalculation=false;
 ComponentReCalculation=false;
 LastCalcTime=-1;
 InterstepsInterval=0;
 StepDuration=0;
 return true;
}

// ��������� ������ ����� �������
bool UContainer::Calculate(void)
{
 if(!Activity)
  return true;
RDK_SYS_TRY {
 try
 {
  Init(); // ��������

  if(!IsInit())
   return false;

  Build();

  int i=0;
  long long tempstepduration=GetCurrentStartupTime();
  InterstepsInterval=(LastCalcTime>=0)?CalcDiffTime(tempstepduration,LastCalcTime):0;
  LastCalcTime=tempstepduration;

  UEPtr<UContainer> *comps=PComponents;
  while((i<NumComponents) && !SkipComponentCalculation)
  {
   (*comps)->Calculate();
   if(ComponentReCalculation)
   {
	ComponentReCalculation=false;
	i=0; comps=PComponents;
   }
   else
   {
	++i,++comps;
   }
  }

  SkipComponentCalculation=false;
  ComponentReCalculation=false;

  if(!Owner)
  {
   ACalculate();
  }
  else
  if(TimeStep == OwnerTimeStep)
  {
   ACalculate();
  }
  else
  if(TimeStep < OwnerTimeStep)
  {
   --CalcCounter;
   if(CalcCounter <= 0)
	{
	 CalcCounter=OwnerTimeStep/TimeStep;
	 ACalculate();
	}
  }
  else
  if(TimeStep > OwnerTimeStep)
  {
   for(UTime i=TimeStep/OwnerTimeStep;i>=0;--i)
	ACalculate();
  }

  UpdateMainOwner();
  InterstepsInterval-=StepDuration;
  StepDuration=CalcDiffTime(GetCurrentStartupTime(),tempstepduration);
  // ������������ �����������
  int numcontrollers=Controllers.size();

  if(numcontrollers)
  {
   UEPtr<UController>* controllers=&Controllers[0];
   for(int i=0;i<numcontrollers;i++,controllers++)
   {
	(*controllers)->Update();
   }
  }
 }
 catch(UException &exception)
 {
  throw;
 }
} RDK_SYS_CATCH
{
 throw EComponentSystemException(this,0,GET_SYSTEM_EXCEPTION_DATA);
}

 return true;
}


// ��������� ��������� ������������� ����� �������
void UContainer::Init(void)
{
 if(IsInit())
  return;

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Init();

 AInit();
 InitFlag=true;
 Reset();
}

// ��������� ��������������� ����� �������
void UContainer::UnInit(void)
{
 if(!IsInit())
  return;

 AUnInit();

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->UnInit();

 InitFlag=false;
}
// ��������� ��������� MainOwner ����� ������� ����� �������
void UContainer::UpdateMainOwner(void)
{
 if(!MainOwner)
  return;

 return AUpdateMainOwner();
}

// ������ ���������� �������� ����������� � ��������� ������ ������� ��������
// ��������� �� ���� ���� �����
void UContainer::ForceSkipComponentCalculation(void)
{
 SkipComponentCalculation=true;
}

// ������ ���������� �������� ����������� � ������� ���������� ������� ��������
// ��������� �� ���� ���� ����� �������
void UContainer::ForceComponentReCalculation(void)
{
 ComponentReCalculation=true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
void UContainer::AInit(void)
{
}

void UContainer::AUnInit(void)
{
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ����������� ���������
// --------------------------
// ��������� ������� ������������ ��������� ������� 'oldname'
// ��� ���������� �� 'newname'
void UContainer::ModifyLookupComponent(const NameT &oldname,
                                        const NameT newname)
{
 UId id;

 std::map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  throw EComponentNameNotExist(oldname);

 id=I->second;
 CompsLookupTable.erase(I);
 CompsLookupTable[newname]=id;
}

// ��������� ������� ������������ ��������� ������������ Id 'id'
// ��� ���������� � ������ 'name'
void UContainer::SetLookupComponent(const NameT &name, const UId &id)
{
 CompsLookupTable[name]=id;
 if(LastId<id)
  LastId=id;
}

// ��������� ������� ������������ ��������� ������ ������
// ���������� � ������ 'name'
void UContainer::DelLookupComponent(const NameT &name)
{
 std::map<NameT,UId>::iterator I=CompsLookupTable.find(name);

 if(I == CompsLookupTable.end())
  throw EComponentNameNotExist(name);
 CompsLookupTable.erase(name);
}
// --------------------------

// --------------------------
// ������ ���������� ������������� ����������
// �������� ������������ ����� �� ���������� ������
// --------------------------
// ��������� ����� ����������
void UContainer::AddController(UEPtr<UController> controller, bool forchilds)
{
 if(CheckController(controller))
  return;

 Controllers.push_back(controller);
 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->AddController(controller,forchilds);
 }
}

// ������� ���������� �� ������
void UContainer::DelController(UEPtr<UController> controller, bool forchilds)
{
 vector<UEPtr<UController> >::iterator I=find(Controllers.begin(),Controllers.end(),controller);

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelController(controller,forchilds);
 }
}

// ������� ��� �����������
void UContainer::DelAllControllers(bool forchilds)
{
 Controllers.clear();
 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelAllControllers(forchilds);
 }
}

// ���������� ���������� ���� ������������
void UContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(this);

 if(forchilds)
 {
  UEPtr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->UnLinkAllControllers(forchilds);
 }
}


// ���������, ���������� �� ���������� � ������
bool UContainer::CheckController(UEPtr<UController> controller) const
{
 if(find(Controllers.begin(),Controllers.end(),controller) != Controllers.end())
  return true;
 return false;
}

// ���������� ����� ������������
size_t UContainer::GetNumControllers(void) const
{
 return Controllers.size();
}

// ���������� ���������� �� �������
UEPtr<UController> UContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
// ��������� ��������� � ������� �����������
// ������ ���������� � ������������� �������
UId UContainer::AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer)
{
 UPVariable P(1,pointer);

 if(PointerLookupTable.find(name) != PointerLookupTable.end())
  throw EPointerNameAlreadyExist(name);

 for(PointerMapIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(P.Id <= I->second.Id)
   P.Id=I->second.Id+1;
 }

 PointerLookupTable.insert(make_pair(name,P));
 return P.Id;
}

// ������� ��������� � ID 'id' �� ������� �����������
void UContainer::DelLookupPointer(const NameT &name)
{
 PointerMapIteratorT I=PointerLookupTable.find(name);

 if(I == PointerLookupTable.end())
  throw EPointerNameNotExist(name);

 delete I->second.Pointer;
 PointerLookupTable.erase(I);
}
/*
// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT UContainer::GetPointerLongName(const UIPointer &pointer) const
{

 NameT name=pointer.GetOwnerName();

 int i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=7;

 int j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
 return "";
}                        */

// ������������ ����� � ������� ���������, ���������������� ��������� ���������
UContainer::PointerMapCIteratorT UContainer::FindLookupPointer(UEPtr<UContainer> source) const
{
 for(PointerMapCIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end(); I!=J; ++I)
 {
   if(I->second.Pointer && I->second.Pointer->Find(source)>=0)
    return I;
 }

 return PointerLookupTable.end();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������ (shared) �����������
// --------------------------
// ����� ������������� ����� ����������. ���������� ������������� ��� ����������
// ������� ���������
void UContainer::ASharesInit(void)
{
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
void UContainer::ASharesUnInit(void)
{
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void UContainer::AddComponentTable(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 Components.push_back(comp);
 PComponents=&Components[0];
 NumComponents=Components.size();

 if(pointer)
  pointer->Set(comp);
 else
 {
  PointerMapCIteratorT I=FindLookupPointer(comp);
  if(I != PointerLookupTable.end())
  {
   I->second.Pointer->Del(comp);
  }
 }
}

void UContainer::DelComponentTable(UEPtr<UContainer> comp)
{
 int i;

 if(NumComponents)
 {
  if(PComponents[NumComponents-1]==comp)
   Components.resize(NumComponents-1);
  else
  {
   for(i=0;i<NumComponents;i++)
    if(PComponents[i] == comp)
     break;

   if(i>=NumComponents)
    return;

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UEPtr<UContainer>));
   Components.resize(NumComponents-1);
  }
 }

 NumComponents=Components.size();
 if(NumComponents>0)
  PComponents=&Components[0];
 else
  PComponents=0;

 PointerMapCIteratorT I=FindLookupPointer(comp);
 if(I != PointerLookupTable.end())
 {
  I->second.Pointer->Del(comp);
 }
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ������� ��������� comp
// ����� ������������, ��� ��������� ����������� �������
void UContainer::BeforeDelComponent(UEPtr<UContainer> comp, bool canfree)
{
 ABeforeDelComponent(comp,canfree);
}

void UContainer::AfterDelComponent(UEPtr<UContainer> comp, bool canfree)
{
 AAfterDelComponent(comp,canfree);
}

void UContainer::DelComponent(UEPtr<UContainer> comp, bool canfree)
{
 BeforeDelComponent(comp,canfree);
 SharesUnInit();
 ADelComponent(comp);

 if(comp->GetMainOwner() == MainOwner)
  comp->SetMainOwner(0);

 // �������� �� ���� ���������
 // ������� ��������� �� ������� ������������ ���������
 DelLookupComponent(comp->Name);

 // �������� �� ���� ���������
 DelComponentTable(comp);

 comp->Owner=0;

 AfterDelComponent(comp,canfree);

 if(canfree)
  comp->Free();

 if(!NumComponents)
  LastId=0;
}


// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
void UContainer::ABeforeAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

}

void UContainer::AAfterAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

}

bool UContainer::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
void UContainer::ABeforeDelComponent(UEPtr<UContainer> comp, bool canfree)
{

}

void UContainer::AAfterDelComponent(UEPtr<UContainer> comp, bool canfree)
{

}

bool UContainer::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ��������� ��������� MainOwner ����� ������� ����� �������
void UContainer::AUpdateMainOwner(void)
{
}
// --------------------------

/* *************************************************************************** */
/* ������ ���������� */
// class EIContainer
// --------------------------
// ������������ � �����������
// --------------------------
UContainer::EIContainer::EIContainer(void)
{

}

UContainer::EIContainer::EIContainer(const UContainer *cont)
{
 if(!cont)
  return;

 // �������� ��� ���������� � ������� ������������� ����������
 Name=cont->GetName();

 // �������� ������������� ���������� � ������� ������������� ����������
 Id=cont->GetId();

 // ������ ��� ��������� ���������� � ������� ������������� ����������
 if(cont->GetOwner())
 {
  cont->GetOwner()->GetFullName(OwnerName);

  // ������ ������������� ��������� ���������� � ������� ������������� ����������
  OwnerId=cont->GetOwner()->GetFullId();
 }

 if(cont->GetMainOwner())
 {
  // ������ ��� �������� ��������� ���������� � ������� ������������� ����������
  cont->GetMainOwner()->GetFullName(MainOwnerName);

  // ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
  MainOwnerId=cont->GetMainOwner()->GetFullId();
 }
}


UContainer::EIContainer::EIContainer(const EIContainer &copy)
{
 // �������� ��� ���������� � ������� ������������� ����������
 Name=copy.Name;

 // �������� ������������� ���������� � ������� ������������� ����������
 Id=copy.Id;

 // ������ ��� ��������� ���������� � ������� ������������� ����������
 OwnerName=copy.OwnerName;

 // ������ ������������� ��������� ���������� � ������� ������������� ����������
 OwnerId=copy.OwnerId;

 // ������ ��� �������� ��������� ���������� � ������� ������������� ����������
 MainOwnerName=copy.MainOwnerName;

 // ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
 MainOwnerId=copy.MainOwnerId;
}

UContainer::EIContainer::~EIContainer(void)
{

}
// --------------------------


// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UContainer::EIContainer::CreateLogMessage(void) const
{
 string result;//=UException::CreateLogMessage();

// EIContainer *iexception=dynamic_cast<EIContainer*>(exception);

// if(iexception)
 if(Name.size()>0)
 {
  // �������� ��� ���������� � ������� ������������� ����������
  result+=" Name=";
  result+=Name;
 }
  // �������� ������������� ���������� � ������� ������������� ����������
//  result+=" Id=";
//  result+=iexception->Id;

 if(OwnerName.size()>0)
 {
  // ������ ��� ��������� ���������� � ������� ������������� ����������
  result+=" OwnerName=";
  result+=OwnerName;
 }

  // ������ ������������� ��������� ���������� � ������� ������������� ����������
//  result+=" OwnerId=";
//  result+=iexception->OwnerId;

 if(MainOwnerName != OwnerName && MainOwnerName.size()>0)
 {
  // ������ ��� �������� ��������� ���������� � ������� ������������� ����������
  result+=" MainOwnerName=";
  result+=MainOwnerName;
 }

  // ������ ������������� �������� ��������� ���������� � ������� ������������� ����������
//  result+=" MainOwnerId=";
//  result+=iexception->MainOwnerId;


 return result;
}
// --------------------------

// ������������ ����� ��� ��������� ������ ����� ���������
//class EICalculateContainer: public EIContainer
// --------------------------
// ������������ � �����������
// --------------------------
UContainer::EICalculateContainer::EICalculateContainer(void)
{

}

UContainer::EICalculateContainer::EICalculateContainer(const UContainer *cont, const UContainer *subcont)
: EIContainer(cont)
{
 if(!subcont)
  return;

 // �������� ��� ���������� � ������� ������������� ����������
 SubName=subcont->GetName();

 // �������� ������������� ���������� � ������� ������������� ����������
 SubId=subcont->GetId();
}

UContainer::EICalculateContainer::EICalculateContainer(const EICalculateContainer &copy)
 :EIContainer(copy)
{
 SubName=copy.Name;
 SubId=copy.SubId;
}

UContainer::EICalculateContainer::~EICalculateContainer(void)
{

}
 // --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UContainer::EICalculateContainer::CreateLogMessage(void) const
{
 string result=UContainer::EIContainer::CreateLogMessage();
 if(SubName.size()>0)
 {
  result+=" SubName=";
  result+=SubName;
 }
 return result;
}
// --------------------------


// --------------------------
// ������������ � �����������
// --------------------------
UContainer::EComponentSystemException::EComponentSystemException(void)
{

}

UContainer::EComponentSystemException::EComponentSystemException(const UContainer *cont, const UContainer *subcont, const std::string &info)
 : ESystemException(info), EICalculateContainer(cont,subcont)
{
}


UContainer::EComponentSystemException::EComponentSystemException(const EComponentSystemException &copy)
 : ESystemException(copy), EICalculateContainer(copy)
{
}

UContainer::EComponentSystemException::~EComponentSystemException(void)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UContainer::EComponentSystemException::CreateLogMessage(void) const
{
 return ESystemException::CreateLogMessage()+EICalculateContainer::CreateLogMessage();
}
// --------------------------




}

#endif


