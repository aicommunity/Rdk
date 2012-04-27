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
#include "UAContainer.h"
#include "UAContainerStorage.h"
#include "UAConnector.h"
#include "UAItem.h"
#include "UANet.h"
#include "../Serialize/Serialize.h"
#include "../Math/MUBinarySerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "UAContainerStorage.h"
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
// Class UAContainer
/* *************************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UAContainer::UAContainer(void)
 : PComponents(0), NumComponents(0), Id(0), LastId(0)
{
 AddLookupProperty("Id",new UVProperty<UId,UAContainer>(this,&UAContainer::SetId,&UAContainer::GetId));
 AddLookupProperty("Name",new UVProperty<NameT,UAContainer>(this,&UAContainer::SetName,&UAContainer::GetName));
 AddLookupProperty("TimeStep",new UVProperty<UTime,UAContainer>(this,&UAContainer::SetTimeStep,&UAContainer::GetTimeStep));
 AddLookupProperty("Activity",new UVProperty<bool,UAContainer>(this,&UAContainer::SetActivity,&UAContainer::GetActivity));
 AddLookupProperty("Coord",new UVProperty<RDK::MVector<double,3>,UAContainer>(this,&UAContainer::SetCoord,&UAContainer::GetCoord));
}

UAContainer::~UAContainer(void)
{
 DelAllComponents();
 UnLinkAllControllers();

 BreakOwner();

 UEPtr<UAContainerStorage> cstorage=dynamic_pointer_cast<UAContainerStorage>(Storage);
 if(cstorage)
  {
   cstorage->PopObject(UEPtr<UAContainer>(this));
  }
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
UEPtr<UAContainer> UAContainer::GetOwner(void) const
{
 return dynamic_pointer_cast<UAContainer>(Owner);
}

// ���������� ��������� �� �������� ��������� ���� ��������
UEPtr<UAContainer> UAContainer::GetMainOwner(void) const
{
 return dynamic_pointer_cast<UAContainer>(MainOwner);
}

// ���������� ��������� ��������� ����� �������
UEPtr<UAContainerStorage> const UAContainer::GetStorage(void) const
{
 return dynamic_pointer_cast<UAContainerStorage>(Storage);
}

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool UAContainer::CheckOwner(UEPtr<UAContainer> owner) const
{
 if(Owner == 0 && Owner != owner)
  return false;

 if(Owner == owner)
  return true;

 return GetOwner()->CheckOwner(owner);
}

// ���������� ������ Id �������
// (������� Id ���� ����������).
ULongId& UAContainer::GetFullId(ULongId &buffer) const
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
ULongId& UAContainer::GetLongId(UEPtr<UAContainer> mainowner, ULongId &buffer) const
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
std::string& UAContainer::GetLongId(UEPtr<UAContainer> mainowner, std::string &buffer) const
{
 return GetLongName(mainowner,buffer);
}

// ���������� true ���� ������������ ������������� ������� ���������, � ��������� ������ ���������� false
bool UAContainer::CheckLongId(const ULongId &id) const
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  return false;

 return true;
}

// ������������� ������� ������������ ������, ��������������� ������� ���
bool UAContainer::CheckLongId(const std::string &id) const
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
RDK::MVector<double,3> UAContainer::GetCoord(void) const
{
 return Coord;
}

bool UAContainer::SetCoord(RDK::MVector<double,3> value)
{
 if(Coord != value)
  Coord=value;

 return true;
}


// �����, ����������� �� ��������� �������
// (��� ����� ������� ������� �������� ��������) (��)
long long UAContainer::GetStepDuration(void) const
{
 long long res=0;
 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetFullStepDuration();

 return StepDuration-res;
}

// �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
long long UAContainer::GetFullStepDuration(void) const
{
 return StepDuration;
}

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
double UAContainer::GetInstantPerformance(void) const
{
 return ((GetFullStepDuration()*TimeStep)/1000.0);
}

// ������� ��������� �������
void UAContainer::BreakOwner(void)
{
 UEPtr<UAContainer> owner=GetOwner();
 if(owner)
  owner->DelComponent(this,false);
}

// ������������� ��������� �� �������� ��������� ���� ��������
// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
void UAContainer::SetMainOwner(UEPtr<UAComponent> mainowner)
{
 UAComponent::SetMainOwner(mainowner);
}

void UAContainer::SetMainOwner(UEPtr<UAComponent> mainowner, int levels)
{
 if(MainOwner == mainowner && !levels)
  return;

 MainOwner=mainowner;

 if(!levels)
  return;

 UEPtr<UAContainer>* comps=PComponents;
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
bool UAContainer::CheckId(const UId &id)
{
 return (id>LastId)?true:false;
}

// ��������� ������������ ��� 'name' �� ������������ � ������
// ������� �������.
bool UAContainer::CheckName(const NameT &name)
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
UId UAContainer::GenerateId(void)
{
 return LastId+1;
}

#pragma warning (disable : 4996)
// ���������� ��� ���������� � ����������� ����� �������
NameT& UAContainer::GenerateName(const NameT &prefix, NameT &namebuffer)
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
#pragma warning (default : 4996)

// ������������� ��� �������.
const NameT& UAContainer::GetName(void) const
{
 return Name;
}

bool UAContainer::SetName(const NameT &name)
{
 if(Name == name)
  return true;

 if(name.empty())
  return false;

  if(GetOwner() != 0)
  {
   if(!GetOwner()->CheckName(name))
	throw new EComponentNameAlreadyExist(name);

   GetOwner()->ModifyLookupComponent(Name, name);
  }
 Name=name;
 return true;
}

// ���������� ������ ��� �������
// (������� ����� ���� ����������).
NameT& UAContainer::GetFullName(NameT &buffer) const
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
NameT& UAContainer::GetLongName(const UEPtr<UAContainer> mainowner, NameT &buffer) const
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
const NameT& UAContainer::GetComponentName(const UId &id) const
{
 for(std::map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
								 J=CompsLookupTable.end(); I!=J; ++I)
 {
  if(I->second == id)
   return I->first;
 }
 throw new EComponentIdNotExist(id);
}

// ���������� Id ��������� ���������� �� ��� �����
const UId& UAContainer::GetComponentId(const NameT &name) const
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
  throw new EComponentNameNotExist(name);

 return I->second;
}

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& UAContainer::GetPointerName(const UId &id) const
{
 for(PointerMapCIteratorT I=PointerLookupTable.begin(),
								 J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
   return I->first;
 }
 throw new EPointerIdNotExist(id);
}

// ���������� Id ���������� ��������� �� ��� �����
const UId& UAContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  throw new EPointerNameNotExist(name);

 return I->second.Id;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���� ��������������
UTime UAContainer::GetTimeStep(void) const
{
 return TimeStep;
}

bool UAContainer::SetTimeStep(UTime timestep)
{
 if(timestep <= 0)
  return false;

 TimeStep=timestep;

 if(Owner)
  OwnerTimeStep=GetOwner()->TimeStep;
 else
  OwnerTimeStep=timestep;

 // ��������� �� ���� ����������� �������
 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->OwnerTimeStep=timestep;

 return true;
}

// ������������� �������� ���� �������������� ���������� � ���� ��� �������� �����������
bool UAContainer::SetGlobalTimeStep(UTime timestep)
{
 if(!SetTimeStep(timestep))
  return false;

 // ��������� �� ���� ����������� �������
 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(!(*comps)->SetGlobalTimeStep(timestep))
   return false;

 return true;
}



// ������������� ���� ���������� �������
bool UAContainer::GetActivity(void) const
{
 return Activity;
}

bool UAContainer::SetActivity(bool activity)
{
 if(Activity == activity)
  return true;

 Activity=true;
 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->Activity=activity;

 if(activity)
  return Reset(); // !!! ��������. �������� ��� �� �����!

 Activity=activity;

 return true;
}

// Id �������
const UId& UAContainer::GetId(void) const
{
 return Id;
}

bool UAContainer::SetId(const UId &id)
{
 if(Id == id)
  return true;

 if(id == ForbiddenId)
  return true;// ��������!! ��� ���! throw new EForbiddenId(id);

 if(id < 0)
  throw new EInvalidId(id);


 if(Owner != 0)
  {
   if(!GetOwner()->CheckId(id))
	throw new EComponentIdAlreadyExist(id);

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
UEPtr<UAContainer> UAContainer::Alloc(UEPtr<UAContainerStorage> stor, bool copystate)
{
 UEPtr<UAContainer> copy;
 UEPtr<UAContainerStorage> storage=(stor!=0)?stor:GetStorage();

 if(storage)
 {
  copy=dynamic_pointer_cast<UAContainer>(storage->TakeObject(Class,this));
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
bool UAContainer::Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor, bool copystate) const
{
 CopyProperties(target);
 target->Build();

 if(copystate)
  CopyState(target);

 CopyComponents(target,stor);
 return true;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UAContainer::Free(void)
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
int UAContainer::GetNumComponents(void) const
{
 return NumComponents;
}


// ���������� ������ ����� �������� ���������
// (������� ��� ���������� �������� ���������)
int UAContainer::GetNumAllComponents(void) const
{
 int res=NumComponents;

 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  res+=(*comps)->GetNumAllComponents();

 return res;
}


// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool UAContainer::CheckComponentType(UEPtr<UAContainer> comp) const
{
 return false;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
UEPtr<UAContainer> UAContainer::GetComponent(const UId &id) const
{
 if(id == ForbiddenId)
  throw new EComponentIdNotExist(id);

 UEPtr<UAContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(id == (*comps)->Id)
   return *comps;

 throw new EComponentIdNotExist(id);
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
UEPtr<UAContainer> UAContainer::GetComponent(const NameT &name) const
{
 return GetComponent(GetComponentId(name));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'.
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
UEPtr<UAContainer> UAContainer::GetComponentL(const ULongId &id) const
{
 UEPtr<UAContainer> comp;

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
UEPtr<UAContainer> UAContainer::GetComponentL(const NameT &name) const
{
 UEPtr<UAContainer> comp;
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
UEPtr<UAContainer> UAContainer::GetComponentByIndex(int index) const
{
 return Components[index];
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
void UAContainer::BeforeAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 ABeforeAddComponent(comp,pointer);
}

void UAContainer::AfterAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 AAfterAddComponent(comp,pointer);
}

UId UAContainer::AddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  throw new EAddComponentAlreadyHaveOwner(comp->Id);

 if(!CheckComponentType(comp))
  throw new EAddComponentHaveInvalidType(comp->Id);

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
 catch(Exception *exception)
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
void UAContainer::DelComponent(const UId &id, bool canfree)
{
 UEPtr<UAContainer> comp=GetComponent(id);

 DelComponent(comp, canfree);
}

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'name' - ������ ���� �������� ������
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void UAContainer::DelComponent(const NameT &name, bool canfree)
{
 DelComponent(GetComponentId(name),canfree);
}

// ������������� ������� ��� �������� ����������
void UAContainer::DelAllComponents(void)
{
 while(NumComponents)
  DelComponent(PComponents[NumComponents-1],true);
}

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void UAContainer::GetComponentsList(std::vector<UId> &buffer) const
{
 UEPtr<UAContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Id);
}

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������.
void UAContainer::CopyComponents(UEPtr<UAContainer> comp, UEPtr<UAContainerStorage> stor) const
{
 UEPtr<UAContainer> bufcomp;

 // ������� ������ ���������� �� 'comp'
 comp->DelAllComponents();

 UEPtr<UAContainer> * pcomponents=0;
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
// --------------------------


// ----------------------
// ������ ���������� ����������������� ������������
// ----------------------
// ���������� ������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
ULongIdVector& UAContainer::GetConnectorsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UAContainer> ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAContainer> cont=GetComponentByIndex(i);
  UEPtr<UAConnector> temp;
  temp=dynamic_pointer_cast<UAConnector>(cont);
  if(temp)
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UAContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetConnectorsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}

// ���������� ������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
ULongIdVector& UAContainer::GetItemsList(ULongIdVector &buffer,
							int sublevel, UEPtr<UAContainer> ownerlevel)

{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAContainer> cont=GetComponentByIndex(i);
  UEPtr<UAItem> temp;
  temp=dynamic_pointer_cast<UAItem>(cont);
  if(temp)
//  if(dynamic_cast<UAItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UAContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetItemsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}

// ���������� ������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
ULongIdVector& UAContainer::GetNetsList(ULongIdVector &buffer,
                            int sublevel, UEPtr<UAContainer> ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UEPtr<UAContainer> cont=GetComponentByIndex(i);
  UEPtr<UANet> temp;
  temp=dynamic_pointer_cast<UANet>(cont);
  if(temp)
//  if(dynamic_cast<UANet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:UEPtr<UAContainer>(this),id);
   buffer.Add(id);
  }

  if(sublevel != 0)
   cont->GetNetsList(buffer,(sublevel<0)?sublevel:sublevel-1,ownerlevel);
 }

 return buffer;
}
// ----------------------


// --------------------------
// ������ ���������� ���������� �����������
// --------------------------
// ������������� �������� ��������� 'id' � �������� ��������� ������ ��������� ����������
// 'pointerid'
bool UAContainer::SetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UAContainer> cont=GetComponent(id);

 PointerMapIteratorT I=PointerLookupTable.end();
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
bool UAContainer::SetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UAContainer> cont=GetComponent(name);

 PointerMapIteratorT I=PointerLookupTable.end();;
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
bool UAContainer::ResetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UAContainer> cont=GetComponent(id);

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
bool UAContainer::ResetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UAContainer> cont=GetComponent(name);

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
bool UAContainer::ResetComponentAll(const UId &id)
{
 UEPtr<UAContainer> cont=GetComponent(id);

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
bool UAContainer::ResetComponentAll(const NameT &name)
{
 UEPtr<UAContainer> cont=GetComponent(name);

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
void UAContainer::DelAllComponentsAs(const NameT &pointername, bool canfree)
{
 PointerMapIteratorT J=PointerLookupTable.find(pointername);

 if(J != PointerLookupTable.end())
 {
  UEPtr<UAContainer> cont=J->second.Pointer->Get();

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
void UAContainer::SharesInit(void)
{
 ASharesInit();
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
void UAContainer::SharesUnInit(void)
{
 ASharesUnInit();
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UAContainer::Default(void)
{
 Ready=false;
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Default();

 // ���� ���������� �������� � ���������, �� ����� ��������� ����������
 // �� ���������
 UEPtr<UAContainer> original;
 if(Storage)
  original=dynamic_pointer_cast<UAContainer>(GetStorage()->GetClass(Class));

 SetTimeStep(2000);

 if(original && original != this)
 {
  NameT name=Name;
  bool activity=Activity;
  original->CopyProperties(this);
  SetName(name);
  SetActivity(activity);
 }

 return ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UAContainer::Build(void)
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
bool UAContainer::Reset(void)
{
 Build();

 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Reset();

 AReset();

 CalcCounter=0;
 SkipComponentCalculation=false;
 ComponentReCalculation=false;
 return true;
}

// ��������� ������ ����� �������
bool UAContainer::Calculate(void)
{
 if(!Activity)
  return true;

 Build();

 long long tempstepduration=GetCurrentStartupTime();
 UEPtr<UAContainer> *comps=PComponents;
 int i=0;
 while((i<NumComponents) & !SkipComponentCalculation)
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
 return true;
}

// ��������� ��������� MainOwner ����� ������� ����� �������
void UAContainer::UpdateMainOwner(void)
{
 if(!MainOwner)
  return;

 return AUpdateMainOwner();
}

// ������ ���������� �������� ����������� � ��������� ������ ������� ��������
// ��������� �� ���� ���� �����
void UAContainer::ForceSkipComponentCalculation(void)
{
 SkipComponentCalculation=true;
}

// ������ ���������� �������� ����������� � ������� ���������� ������� ��������
// ��������� �� ���� ���� ����� �������
void UAContainer::ForceComponentReCalculation(void)
{
 ComponentReCalculation=true;
}
// --------------------------


// --------------------------
// ������� ������ ���������� �������� ����������� ���������
// --------------------------
// ��������� ������� ������������ ��������� ������� 'oldname'
// ��� ���������� �� 'newname'
void UAContainer::ModifyLookupComponent(const NameT &oldname,
										const NameT newname)
{
 UId id;

 std::map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  throw new EComponentNameNotExist(oldname);

 id=I->second;
 CompsLookupTable.erase(I);
 CompsLookupTable[newname]=id;
}

// ��������� ������� ������������ ��������� ������������ Id 'id'
// ��� ���������� � ������ 'name'
void UAContainer::SetLookupComponent(const NameT &name, const UId &id)
{
 CompsLookupTable[name]=id;
 if(LastId<id)
  LastId=id;
}

// ��������� ������� ������������ ��������� ������ ������
// ���������� � ������ 'name'
void UAContainer::DelLookupComponent(const NameT &name)
{
 std::map<NameT,UId>::iterator I=CompsLookupTable.find(name);

 if(I == CompsLookupTable.end())
  throw new EComponentNameNotExist(name);
 CompsLookupTable.erase(name);
}
// --------------------------

// --------------------------
// ������ ���������� ������������� ����������
// �������� ������������ ����� �� ���������� ������
// --------------------------
// ��������� ����� ����������
void UAContainer::AddController(UEPtr<UController> controller, bool forchilds)
{
 if(CheckController(controller))
  return;

 Controllers.push_back(controller);
 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->AddController(controller,forchilds);
 }
}

// ������� ���������� �� ������
void UAContainer::DelController(UEPtr<UController> controller, bool forchilds)
{
 vector<UEPtr<UController> >::iterator I=find(Controllers.begin(),Controllers.end(),controller);

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelController(controller,forchilds);
 }
}

// ������� ��� �����������
void UAContainer::DelAllControllers(bool forchilds)
{
 Controllers.clear();
 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelAllControllers(forchilds);
 }
}

// ���������� ���������� ���� ������������
void UAContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(this);

 if(forchilds)
 {
  UEPtr<UAContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->UnLinkAllControllers(forchilds);
 }
}


// ���������, ���������� �� ���������� � ������
bool UAContainer::CheckController(UEPtr<UController> controller) const
{
 if(find(Controllers.begin(),Controllers.end(),controller) != Controllers.end())
  return true;
 return false;
}

// ���������� ����� ������������
size_t UAContainer::GetNumControllers(void) const
{
 return Controllers.size();
}

// ���������� ���������� �� �������
UEPtr<UController> UAContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
// ��������� ��������� � ������� �����������
// ������ ���������� � ������������� �������
UId UAContainer::AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer)
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
void UAContainer::DelLookupPointer(const NameT &name)
{
 PointerMapIteratorT I=PointerLookupTable.find(name);

 if(I == PointerLookupTable.end())
  throw new EPointerNameNotExist(name);

 delete I->second.Pointer;
 PointerLookupTable.erase(I);
}
/*
// ���������� ������ ��� ��������� ��� �������� NMSDK, � �������� '*'
NameT UAContainer::GetPointerLongName(const UIPointer &pointer) const
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
UAContainer::PointerMapCIteratorT UAContainer::FindLookupPointer(UEPtr<UAContainer> source) const
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
void UAContainer::ASharesInit(void)
{
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
void UAContainer::ASharesUnInit(void)
{
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void UAContainer::AddComponentTable(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
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

void UAContainer::DelComponentTable(UEPtr<UAContainer> comp)
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

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UEPtr<UAContainer>));
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
void UAContainer::BeforeDelComponent(UEPtr<UAContainer> comp, bool canfree)
{
 ABeforeDelComponent(comp,canfree);
}

void UAContainer::AfterDelComponent(UEPtr<UAContainer> comp, bool canfree)
{
 AAfterDelComponent(comp,canfree);
}

void UAContainer::DelComponent(UEPtr<UAContainer> comp, bool canfree)
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
void UAContainer::ABeforeAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{

}

void UAContainer::AAfterAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{

}

bool UAContainer::AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
void UAContainer::ABeforeDelComponent(UEPtr<UAContainer> comp, bool canfree)
{

}

void UAContainer::AAfterDelComponent(UEPtr<UAContainer> comp, bool canfree)
{

}

bool UAContainer::ADelComponent(UEPtr<UAContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ��������� ��������� MainOwner ����� ������� ����� �������
void UAContainer::AUpdateMainOwner(void)
{
}
// --------------------------

/* *************************************************************************** */
/* ������ ���������� */
// class EIContainer
// --------------------------
// ������������ � �����������
// --------------------------
UAContainer::EIContainer::EIContainer(void)
{

}

UAContainer::EIContainer::EIContainer(const UAContainer *cont)
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


UAContainer::EIContainer::EIContainer(const EIContainer &copy)
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

UAContainer::EIContainer::~EIContainer(void)
{

}
// --------------------------


// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAContainer::EIContainer::CreateLogMessage(void) const
{
 string result;//=UException::CreateLogMessage();

// EIContainer *iexception=dynamic_cast<EIContainer*>(exception);

// if(iexception)
 {
  // �������� ��� ���������� � ������� ������������� ����������
  result+=" Name=";
  result+=Name;

  // �������� ������������� ���������� � ������� ������������� ����������
//  result+=" Id=";
//  result+=iexception->Id;

  // ������ ��� ��������� ���������� � ������� ������������� ����������
  result+=" OwnerName=";
  result+=OwnerName;

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
 }


 return result;
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
UAContainer::EComponentCalculate::EComponentCalculate(void)
{

}

UAContainer::EComponentCalculate::EComponentCalculate(const UAContainer *cont, const UAContainer *subcont)
 : EError(), EIContainer(cont)
{
 if(!subcont)
  return;

 // �������� ��� ���������� � ������� ������������� ����������
 SubName=subcont->GetName();

 // �������� ������������� ���������� � ������� ������������� ����������
 SubId=subcont->GetId();
}


UAContainer::EComponentCalculate::EComponentCalculate(const EComponentCalculate &copy)
 : EError(copy), EIContainer(copy)
{
 // �������� ��� ���������� � ������� ������������� ����������
 SubName=copy.SubName;

 // �������� ������������� ���������� � ������� ������������� ����������
 SubId=copy.SubId;
}

UAContainer::EComponentCalculate::~EComponentCalculate(void)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAContainer::EComponentCalculate::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+EIContainer::CreateLogMessage();
}
// --------------------------




}

#endif


