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

const UTime DefaultTimeStep=(UTime)2000;

NameT ForbiddenName="";

/* *************************************************************************** */
// Class UAContainer
/* *************************************************************************** */
// --------------------------
// ���������� ��������
// --------------------------
// ������� ����� ������
ULongTime UAContainer::Time=0;
double UAContainer::DoubleTime=0;

// �������� �����
ULongTime UAContainer::RealTime=0;
double UAContainer::DoubleRealTime=0;

// ���������� ��� � �������� �������
ULongTime UAContainer::RealTimeStep=1;
double UAContainer::DoubleRealTimeStep=1.0e-6;
// --------------------------

/* ������ ���������� */
// --------------------------
// ������������ � �����������
// --------------------------
UAContainer::IException::IException(void)
{

}

UAContainer::IException::IException(const UAContainer *cont)
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


UAContainer::IException::IException(const IException &copy)
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

UAContainer::IException::~IException(void)
{

}
// --------------------------


// --------------------------
// ������ ���������� ����������� ����������
// --------------------------
// ���������� ������� ����� ������
const ULongTime& UAContainer::GetTime(void)
{
 return Time;
}

const double& UAContainer::GetDoubleTime(void)
{
 return DoubleTime;
}

// ������������� ������� ����� ������
bool UAContainer::SetTime(ULongTime value)
{
 if(Time == value)
  return true;

 Time=value;
 DoubleTime=Time/1000000.0;
 return true;
}

// ���������� �������� �����
const ULongTime& UAContainer::GetRealTime(void)
{
 return RealTime;
}

const double& UAContainer::GetDoubleRealTime(void)
{
 return DoubleRealTime;
}

// ������������� �������� �����
bool UAContainer::SetRealTime(ULongTime value)
{
 RealTimeStep=value-RealTime;
 if(RealTimeStep == 0)
  RealTimeStep=1;

 DoubleRealTimeStep=RealTimeStep/1.0e6;

 RealTime=value;
 DoubleRealTime=RealTime/1000000.0;
 return true;
}

// ����������� �������� ����� �� �������� ��������
bool UAContainer::IncreaseRealTime(ULongTime value)
{
 return SetRealTime(RealTime+value);
}

// ���������� ���������� ��� � �������� �������
const ULongTime& UAContainer::GetRealTimeStep(void)
{
 return RealTimeStep;
}

const double& UAContainer::GetDoubleRealTimeStep(void)
{
 return DoubleRealTimeStep;
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
UAContainer::UAContainer(void)
{
 // ��������� �� 0-� ������� ������� ���������
 PComponents=0;

 // ���������� ��������� � ������� ���������
 NumComponents=0;

 LastId=0;

 ObjectIterator=0;

 AddLookupProperty("Id",new UVProperty<UId,UAContainer>(this,&UAContainer::SetId,&UAContainer::GetId));
 AddLookupProperty("Name",new UVProperty<NameT,UAContainer>(this,&UAContainer::SetName,&UAContainer::GetName));
 AddLookupProperty("TimeStep",new UVProperty<UTime,UAContainer>(this,&UAContainer::SetTimeStep,&UAContainer::GetTimeStep));
 AddLookupProperty("Activity",new UVProperty<bool,UAContainer>(this,&UAContainer::SetActivity,&UAContainer::GetActivity));
 AddLookupProperty("Coord",new UVProperty<RDK::MVector<double>,UAContainer>(this,&UAContainer::SetCoord,&UAContainer::GetCoord));
}

UAContainer::~UAContainer(void)
{
 ClearLookupPropertyTable();
 ClearLookupStateTable();
 DelAllComponents();
 UnLinkAllControllers();

 if(!BreakOwner())
  return;// ��������! ������������ ������, ��������� ����������

 UAContainerStorage *cstorage=dynamic_cast<UAContainerStorage*>(Storage);
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
UAContainerStorage* const UAContainer::GetStorage(void) const
{
 return dynamic_cast<UAContainerStorage*>(Storage);
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
                 /*
// ���������� ��������� �������������� Id ���������
UId UAContainer::GetLastId(void) const
{
 return LastId;
}                  */
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ���������� � ������������ ����
RDK::MVector<double> UAContainer::GetCoord(void) const
{
 return Coord;
}

bool UAContainer::SetCoord(RDK::MVector<double> value)
{
 if(Coord == value)
  return true;

 Coord=value;
 return true;
}

// ������� ��������� �������
bool UAContainer::BreakOwner(void)
{
 if(Owner)
  return GetOwner()->DelComponent(this,false);

 return true;
}

// ������������� ��������� �� �������� ��������� ���� ��������
// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
bool UAContainer::SetMainOwner(UEPtr<UAComponent> mainowner)
{
 return UAComponent::SetMainOwner(mainowner);
}

bool UAContainer::SetMainOwner(UEPtr<UAComponent> mainowner, int levels)
{
 if(MainOwner == mainowner && !levels)
  return true;

 MainOwner=mainowner;

 if(!levels)
  return true;

 for(int i=0;i<NumComponents;i++)
 {
  // ������������� �������� ��������� ������ ��� �������� �����������
  // � ������� �� ��� �� �����
  if(PComponents[i]->GetMainOwner() == 0)
  {
   if(levels<0)
   {
    if(!PComponents[i]->SetMainOwner(MainOwner,levels))
     return false;
   }
   else
    if(!PComponents[i]->SetMainOwner(MainOwner,levels-1))
     return false;
  }
 }

 return true;
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
 if(Name == name)
  return true;

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
	return false;

   if(!GetOwner()->ModifyLookupComponent(Name, name))
    return false;
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
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
                                 J=CompsLookupTable.end();
 while(I != J)
  {
   if(I->second == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// ���������� Id ��������� ���������� �� ��� �����
const UId& UAContainer::GetComponentId(const NameT &name) const
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
  return ForbiddenId;
 else return I->second;
}

// ���������� ��� ��������� �� ��� Id
const NameT& UAContainer::GetPropertyName(const UId &id) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                         J=PropertiesLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// ���������� Id ��������� �� ��� �����
const UId& UAContainer::GetPropertyId(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
}

// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT UAContainer::GetPropertyLongName(const NameT &name) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.find(name);
 if(I == PropertiesLookupTable.end())
  return NameT();
 else return GetPropertyLongName(*(I->second.Property));
}

NameT UAContainer::GetPropertyLongName(const UId &id) const
{
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                 J=PropertiesLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return GetPropertyLongName(*(I->second.Property));
   ++I;
  }
 return NameT();
}


// ���������� ��� ���������� ��������� �� ��� Id
const NameT& UAContainer::GetStateName(const UId &id) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                                 J=StateLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// ���������� Id ���������� ��������� �� ��� �����
const UId& UAContainer::GetStateId(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
}

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
NameT UAContainer::GetStateLongName(const NameT &name) const
{
 VariableMapCIteratorT I=StateLookupTable.find(name);
 if(I == StateLookupTable.end())
  return NameT();
 else return GetStateLongName(*(I->second.Property));
}

NameT UAContainer::GetStateLongName(const UId &id) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                                 J=StateLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return GetStateLongName(*(I->second.Property));
   ++I;
  }
 return NameT();
}


// ���������� ��� ���������� ��������� �� ��� Id
const NameT& UAContainer::GetPointerName(const UId &id) const
{
 PointerMapCIteratorT I=PointerLookupTable.begin(),
                                 J=PointerLookupTable.end();
 while(I != J)
  {
   if(I->second.Id == id)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// ���������� Id ���������� ��������� �� ��� �����
const UId& UAContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  return ForbiddenId;
 else return I->second.Id;
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
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->OwnerTimeStep=timestep;

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
 for(int i=0;i<NumComponents;i++)
  PComponents[i]->Activity=activity;

 if(activity)
  return Reset();

 Activity=activity;

 return true;
}

UId UAContainer::GetClass(void) const
{
 return RDK::UAComponent::GetClass();
}

bool UAContainer::SetClass(UId value)
{
 return RDK::UAComponent::SetClass(value);
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

 if(Owner != 0)
  {
   if(!GetOwner()->CheckId(id))
    return false;

   GetOwner()->SetLookupComponent(Name, id);
  }
 Id=id;

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
UContainerDescription* UAContainer::NewDescription(void)
{
 UContainerDescription* result=new UContainerDescription;

 result->SetClassId(sntoa(Class));

 VariableMapIteratorT I=PropertiesLookupTable.begin();
 UParameterDescription dummydescr;
 while(I != PropertiesLookupTable.end())
 {
  result->SetParameter(I->first,dummydescr);
  ++I;
 }

 return result;
}

// ������� ����� ����� ������� � ����������� ���� ���������
// � �������� ����������.
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
UEPtr<UAContainer> UAContainer::Alloc(UAContainerStorage *stor, bool copystate)
{
 UEPtr<UAContainer> copy;
 UAContainerStorage *storage=(stor!=0)?stor:GetStorage();

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
bool UAContainer::Copy(UEPtr<UAContainer> target, UAContainerStorage *stor, bool copystate) const
{
 bool res=CopyProperties(target);
 if(!res)
  return false;

 if(!target->Build())
  return false;

 if(copystate)
 {
  res=CopyState(target);
  if(!res)
   return false;
 }

 res=CopyComponents(target,stor);
 if(!res)
  return false;

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
  if(!BreakOwner())
   return; // ��������, ����� ������ ���� ����������!!
//   throw; // ��������, ����� ������ ���� ����������!!
//  Storage->ReturnObject(this);
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

 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetNumAllComponents();

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
  return 0;

 for(int i=0;i<NumComponents;i++)
  if(id == PComponents[i]->Id)
   return PComponents[i];

 return 0;
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
 if(index >= NumComponents)
  return 0;

 return PComponents[index];
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
UId UAContainer::AddComponent(UEPtr<UAContainer> comp, UIPointer* pointer)
{
// NameT namebuffer;

 if(!comp)
  return ForbiddenId;

 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  return ForbiddenId;

 if(!CheckComponentType(comp.operator->()))
  return ForbiddenId;

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
  return ForbiddenId;

 comp->SetId(id);

 comp->SetOwner(this);


 // ��������� ��������� � ������� ������������ ���������
 SetLookupComponent(comp->Name, comp->Id);

 // ���������� � ���� ���������
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 if(MainOwner)
  comp->SetMainOwner(MainOwner);

 if(!AAddComponent(comp,pointer) || !comp->SharesInit())
  {
   // �����
   // ������� ��������� �� ������� ������������ ���������
   DelLookupComponent(comp->Name);

   // �������� �� ���� ���������
   DelComponentTable(comp);

   comp->Owner=0;

   return ForbiddenId;
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
bool UAContainer::DelComponent(const UId &id, bool canfree)
{
 UEPtr<UAContainer> comp=GetComponent(id);

 if(!comp)
  return false;

 return DelComponent(comp, canfree);
}

// ������������� ������� ��� �������� ����������
void UAContainer::DelAllComponents(void)
{
 while(NumComponents)
  DelComponent(PComponents[NumComponents-1],true);
/*
 for(int i=0;i<NumComponents;i++)
 {
  PComponents[i]->Owner=0;
  PComponents[i]->Free();
 }

 Components.Resize(0);
 PComponents=0;
 NumComponents=0;
// CompsLookupTable.clear();
*/
}

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ����� ������� 'name' - ������ ���� �������� ������
// ���������� �������
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
bool UAContainer::DelComponent(const NameT &name, bool canfree)
{
 return DelComponent(GetComponentId(name),canfree);
}

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void UAContainer::GetComponentsList(UId *buffer) const
{
 UEPtr<UAContainer> *pcomps=PComponents;
 for(int i=0;i<NumComponents;i++,buffer++,pcomps++)
  *buffer=(*pcomps)->Id;
}

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������.
bool UAContainer::CopyComponents(UEPtr<UAContainer> comp, UAContainerStorage* stor) const
{
 UEPtr<UAContainer> bufcomp;

 if(!comp)
  return false;

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
   UIPointer *pointer=0;
   I=FindLookupPointer(*pcomponents);
   if(I != PointerLookupTable.end())
   {
    J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second.Pointer;
   }

   if(comp->AddComponent(bufcomp,pointer) == ForbiddenId)
    return false;

   bufcomp->SetId((*pcomponents)->Id);
   comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
  }
 return true;
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

// --------------------------
// ������ ������� � ����������
// --------------------------
// ���������� �������� ��������� �� Id 'id'
UVariableData* UAContainer::GetProperty(const UId &id, UVariableData *values) const
{
 VariableMapCIteratorT I,J;

 I=PropertiesLookupTable.begin();
 J=PropertiesLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }

  ++I;
 }

 return values;
}

// ���������� �������� ��������� �� ����� 'name'
UVariableData* UAContainer::GetProperty(const NameT &name, UVariableData *values) const
{
 return GetProperty(GetPropertyId(name),values);
}

// ������������� �������� ��������� �� Id 'id'
bool UAContainer::SetProperty(const UId &id, UVariableData *values)
{
 VariableMapCIteratorT I,J;

 I=PropertiesLookupTable.begin();
 J=PropertiesLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   return I->second.Property->Load(values);
  }

  ++I;
 }

 return false;
}

// ������������� �������� ��������� �� ����� 'name'
bool UAContainer::SetProperty(const NameT &name, UVariableData *values)
{
 return SetProperty(GetPropertyId(name),values);
}

const UAContainer::VariableMapT& UAContainer::GetPropertiesList(void) const
{
 return PropertiesLookupTable;
}

// �������� ��� ��������� ����� ������� � ������ 'comp', ���� ��������.
bool UAContainer::CopyProperties(UEPtr<UAContainer> comp) const
{
 bool key=true;

 if(!comp)
  return false;

// UEPtr<UAContainer> ccomp=static_cast<UEPtr<UAContainer>>(comp);

 Serialize::USerStorageBinary databuffer;
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   databuffer.clear();
   key &= comp->SetProperty(I->second.Id,GetProperty(I->second.Id,&databuffer));
   ++I;
  }
 return key;
}

// ���� ��� �������� �� ��������� �� ����
const NameT& UAContainer::FindPropertyName(const UIProperty *prop) const
{
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   if(I->second.Property == prop)
    return I->first;
   ++I;
  }
 return ForbiddenName;
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
// ������ ������� � ���������� ���������
// --------------------------
// ���������� �������� ���������� ��������� �� Id 'id'
UVariableData* UAContainer::GetState(const UId &id, UVariableData *values) const
{
 VariableMapCIteratorT I,J;

 I=StateLookupTable.begin();
 J=StateLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   I->second.Property->Save(values);
   return values;
  }

  ++I;
 }

 return values;
}

// ���������� �������� ���������� ��������� �� ����� 'name'
UVariableData* UAContainer::GetState(const NameT &name, UVariableData *values) const
{
 return GetState(GetStateId(name),values);
}

// ������������� �������� ���������� ��������� �� Id 'id'
bool UAContainer::SetState(const UId &id, UVariableData *values)
{
 VariableMapCIteratorT I,J;

 I=StateLookupTable.begin();
 J=StateLookupTable.end();
 while(I != J)
 {
  if(I->second.Id == id)
  {
   return I->second.Property->Load(values);
  }

  ++I;
 }

 return false;
}

// ������������� �������� ���������� ��������� �� ����� 'name'
bool UAContainer::SetState(const NameT &name, UVariableData *values)
{
 return SetState(GetStateId(name),values);
}

// ���������� ������ ���� � Id ���������� ���������, ������������ ���������������
// � ���� �������
const UAContainer::VariableMapT& UAContainer::GetStateList(void) const
{
 return StateLookupTable;
}

// ���� ��� �������� �� ��������� �� ����
const NameT& UAContainer::FindStateName(const UIProperty *prop) const
{
  VariableMapCIteratorT I=StateLookupTable.begin(),
                        J=StateLookupTable.end();
  while(I != J)
  {
   if(I->second.Property == prop)
    return I->first;
   ++I;
  }
 return ForbiddenName;
}

// �������� ��� ���������� ��������� ����� ������� � ������ 'comp', ���� ��������.
bool UAContainer::CopyState(UEPtr<UAContainer> comp) const
{
 VariableMapCIteratorT I=StateLookupTable.begin(),
                       J=StateLookupTable.end();
 bool key=true;

 if(!comp)
  return false;

 Serialize::USerStorageBinary serstorage;
 while(I != J)
  {
   if(!comp->SetState(I->second.Id,GetState(I->second.Id,&serstorage)))
    key=false;
   ++I;
  }
 return key;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� �����������
// --------------------------
// ������������� �������� ��������� 'id' � �������� ��������� ������ ��������� ����������
// 'pointerid'
bool UAContainer::SetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UAContainer> cont=GetComponent(id);

 if(!cont)
  return false;

 PointerMapIteratorT I=PointerLookupTable.end();;
 PointerMapIteratorT K=PointerLookupTable.end();;
 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont.operator->())>=0)
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
  K->second.Pointer->Set(cont.operator->());
  return true;
 }

 return false;
}

// ������������� �������� ��������� 'name' � �������� ��������� ������ ��������� ����������
// 'pointername'
bool UAContainer::SetComponentAs(const NameT &name,const NameT &pointername)
{
 UEPtr<UAContainer> cont=GetComponent(name);

 if(!cont)
  return false;

 PointerMapIteratorT I=PointerLookupTable.end();;
 PointerMapIteratorT K=PointerLookupTable.end();;
 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont.operator->())>=0)
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
  K->second.Pointer->Set(cont.operator->());
  return true;
 }

 return false;
}

// ���������� ��������� ���������� ���������� 'id' � ��������� ������ ��������� ����������
// 'pointerid'
bool UAContainer::ResetComponentAs(const UId &id, const UId &pointerid)
{
 UEPtr<UAContainer> cont=GetComponent(id);

 if(!cont)
  return false;

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Id == pointerid && J->second.Pointer && J->second.Pointer->Find(cont.operator->())>=0)
  {
   J->second.Pointer->Del(cont.operator->());
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

 if(!cont)
  return false;

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->first == pointername && J->second.Pointer && J->second.Pointer->Find(cont.operator->())>=0)
  {
   J->second.Pointer->Del(cont.operator->());
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

 if(!cont)
  return false;

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont.operator->())>=0)
  {
   J->second.Pointer->Del(cont.operator->());
  }

  ++J;
 }

 return true;
}

// ���������� ��������� ��������� ���������� 'name' �� ���� ������� ��������� ����������
bool UAContainer::ResetComponentAll(const NameT &name)
{
 UEPtr<UAContainer> cont=GetComponent(name);

 if(!cont)
  return false;

 PointerMapIteratorT J=PointerLookupTable.begin();

 while(J != PointerLookupTable.end())
 {
  if(J->second.Pointer && J->second.Pointer->Find(cont.operator->())>=0)
  {
   J->second.Pointer->Del(cont.operator->());
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
bool UAContainer::SharesInit(void)
{
 return ASharesInit();
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
bool UAContainer::SharesUnInit(void)
{
 return ASharesUnInit();
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
  if(!PComponents[i]->Default())
   return false;

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

 return ADefault();//Build();
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
  if(!PComponents[i]->Build())
   return false;

 if(ABuild())
 {
  Ready=true;
  return Reset();
 }

 Ready=false;
 return false;
}

// ����� �������� �����.
bool UAContainer::Reset(void)
{
 if(!Ready)
  return Build();

 bool key=true;
 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Reset())
  {
   key=false;
   break;
  }

 if(!key)
  return false;

 if(!AReset())
  return false;

 CalcCounter=0;
 return true;
}

// ��������� ������ ����� �������
bool UAContainer::Calculate(void)
{
 if(!Activity)
  return true;

 if(!Ready && !Build())
  return false;

// bool key=true;
 UEPtr<UAContainer> *comps=PComponents;
 for(int i=0;i<NumComponents;++i)
  if(!(*comps++)->Calculate())
   return false;
//  {
//   key=false;
//   break;
//  }
// if(!key)
//  return false;

 if(!Owner)
 {
  if(!ACalculate())
   return false;
 }

 if(TimeStep == OwnerTimeStep)
 {
  if(!ACalculate())
   return false;
 }
 else
 if(TimeStep < OwnerTimeStep)
  {
   --CalcCounter;
   if(CalcCounter <= 0)
    {
	 CalcCounter=OwnerTimeStep/TimeStep;
	 if(!ACalculate())
	  return false;
    }
  }
 else
 if(TimeStep > OwnerTimeStep)
  {
   for(UTime i=TimeStep/OwnerTimeStep;i>=0;--i)
    if(!ACalculate())
     return false;
  }

 if(!UpdateMainOwner())
  return false;

 // ������������ �����������
 int numcontrollers=Controllers.size();

 if(numcontrollers)
 {
  UController** controllers=&Controllers[0];
  for(int i=0;i<numcontrollers;i++,controllers++)
  {
   (*controllers)->Update();
  }
 }
 return true;
}

// ��������� ��������� MainOwner ����� ������� ����� �������
bool UAContainer::UpdateMainOwner(void)
{
 if(!MainOwner)
  return true;

 return AUpdateMainOwner();
}
// --------------------------


// --------------------------
// ������� ������ ���������� �������� ����������� ���������
// --------------------------
// ��������� ������� ������������ ��������� ������� 'oldname'
// ��� ���������� �� 'newname'
bool UAContainer::ModifyLookupComponent(const NameT &oldname,
										const NameT newname)
{
 UId id;

 std::map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  return false;

 id=I->second;
 CompsLookupTable.erase(I);
 CompsLookupTable[newname]=id;

 return true;
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
  return;

 CompsLookupTable.erase(name);
}
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
// ��������� �������� � ������ 'name' � ������� �����������
// ���������� � ��������� ��� ���������� ������
// ������ ���������� � ������������� �������
UId UAContainer::AddLookupProperty(const NameT &name, UIProperty *property, bool delenable)
{
 if(!property)
  return ForbiddenId;

 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(PropertiesLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 PropertiesLookupTable.insert(make_pair(name,P));
// PropertiesLookupTable[name]=P;

 return P.Id;
}

// ������� �������� � ������ 'name' �� ������� �����������
// ����������
bool UAContainer::DelLookupProperty(const NameT &name)
{
 VariableMapIteratorT I=PropertiesLookupTable.find(name);

 if(I == PropertiesLookupTable.end())
  return false;

 if(I->second.DelEnable)
  delete I->second.Property;
 PropertiesLookupTable.erase(I);
 return true;
}

// ������� ��� ������� ������������
void UAContainer::ClearLookupPropertyTable(void)
{
  VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
  while(I != J)
  {
   if(I->second.Property && I->second.DelEnable)
	delete I->second.Property;

   ++I;
  }
 PropertiesLookupTable.clear();
}

// ���������� ������ ��� ��������� ��� �������� RDK, � �������� '*'
NameT UAContainer::GetPropertyLongName(const UIProperty &property) const
{
 NameT name=property.GetOwnerName();

 size_t i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=5;

 size_t j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
}
// --------------------------


// --------------------------
// ������ ���������� ������������� ����������
// �������� ������������ ����� �� ���������� ������
// --------------------------
// ��������� ����� ����������
bool UAContainer::AddController(UController *controller, bool forchilds)
{
 if(!controller)
  return false;

 if(CheckController(controller))
  return true;

 Controllers.push_back(controller);
 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->AddController(controller,forchilds);
  }
 return true;
}

// ������� ���������� �� ������
bool UAContainer::DelController(UController *controller, bool forchilds)
{
 vector<UController*>::iterator I=find(Controllers.begin(),Controllers.end(),controller);

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->DelController(controller,forchilds);
  }

 return true;
}

// ������� ��� �����������
bool UAContainer::DelAllControllers(bool forchilds)
{
 Controllers.clear();
 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->DelAllControllers(forchilds);
  }
 return true;
}

// ���������� ���������� ���� ������������
bool UAContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(this);

 if(forchilds)
  for(int i=0;i<NumComponents;i++)
  {
   PComponents[i]->UnLinkAllControllers(forchilds);
  }

 return true;
}


// ���������, ���������� �� ���������� � ������
bool UAContainer::CheckController(UController *controller) const
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
UController* UAContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// ������� ������ ���������� ����������
// --------------------------
// ��������� ���������� ��������� � ������ 'name' � ������� �����������
// ���������� � ��������� �� ���������� ������
// ������ ���������� � ������������� �������
UId UAContainer::AddLookupState(const NameT &name,UIProperty *property, bool delenable)
{
 if(!property)
  return ForbiddenId;

 VariableMapIteratorT I=StateLookupTable.begin(),
					  J=StateLookupTable.end();
 UVariable P(1,property);
 P.DelEnable=delenable;

 if(StateLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 StateLookupTable.insert(make_pair(name,P));

 return P.Id;
}

// ������� ���������� ��������� � ������ 'name' �� ������� �����������
bool UAContainer::DelLookupState(const NameT &name)
{
 VariableMapIteratorT I=StateLookupTable.find(name);

 if(I == StateLookupTable.end())
  return false;

 if(I->second.DelEnable)
  delete I->second.Property;
 StateLookupTable.erase(I);
 return true;
}

// ������� ��� ������� ������������
void UAContainer::ClearLookupStateTable(void)
{
  VariableMapIteratorT I=StateLookupTable.begin(),
                      J=StateLookupTable.end();
  while(I != J)
  {
   if(I->second.Property && I->second.DelEnable)
    delete I->second.Property;

   ++I;
  }
 StateLookupTable.clear();
}

// ���������� ������ ��� ���������� ��������� ��� �������� NMSDK, � �������� '*'
NameT UAContainer::GetStateLongName(const UIProperty &property) const
{
 NameT name=property.GetOwnerName();

 size_t i=name.find("RDK::");
 if(i == NameT::npos)
  i=0;
 else
  i=5;

 size_t j=name.find_last_of("*");
 if(j == NameT::npos)
  j=name.size()-i;
 else
  j-=i+1;

 return name.substr(i,j);
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
// ��������� ��������� � ������� �����������
// ������ ���������� � ������������� �������
UId UAContainer::AddLookupPointer(const NameT &name, UIPointer *pointer)
{
 if(!pointer)
  return ForbiddenId;

 PointerMapIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end();
 UPVariable P(1,pointer);

 if(PointerLookupTable.find(name) != J)
  return ForbiddenId;

 while(I != J)
  {
   if(P.Id <= I->second.Id)
    P.Id=I->second.Id+1;
   ++I;
  }

 PointerLookupTable.insert(make_pair(name,P));

 return P.Id;
}

// ������� ��������� � ID 'id' �� ������� �����������
bool UAContainer::DelLookupPointer(const NameT &name)
{
 PointerMapIteratorT I=PointerLookupTable.find(name);

 if(I == PointerLookupTable.end())
  return false;

 delete I->second.Pointer;
 PointerLookupTable.erase(I);
 return true;
}

// ���������� ������ ��� ��������� ��� �������� NMSDK, � �������� '*'
NameT UAContainer::GetPointerLongName(const UIPointer &pointer) const
{
/*
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

 return name.substr(i,j);*/
 return "";
}

// ������������ ����� � ������� ���������, ���������������� ��������� ���������
UAContainer::PointerMapCIteratorT UAContainer::FindLookupPointer(UEPtr<UAContainer> source) const
{
 PointerMapCIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end();

 while(I != J)
  {
   if(I->second.Pointer && I->second.Pointer->Find(source.operator->())>=0)
    break;
   ++I;
  }

 return I;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������ (shared) �����������
// --------------------------
// ����� ������������� ����� ����������. ���������� ������������� ��� ����������
// ������� ���������
bool UAContainer::ASharesInit(void)
{
 return true;
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
bool UAContainer::ASharesUnInit(void)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void UAContainer::AddComponentTable(UEPtr<UAContainer> comp, UIPointer* pointer)
{
 Components.push_back(comp);
 PComponents=&Components[0];
 NumComponents=Components.size();

 if(pointer)
  pointer->Set(comp.operator->());
 else
 {
  PointerMapCIteratorT I=FindLookupPointer(comp.operator->());
  if(I != PointerLookupTable.end())
  {
   I->second.Pointer->Del(comp.operator->());
  }
 }
}

// ������� ��������� 'comp' �� ������� ���������
void UAContainer::DelComponentTable(UEPtr<UAContainer> comp)
{
 int i;

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

 NumComponents=Components.size();
 if(NumComponents>0)
  PComponents=&Components[0];
 else
  PComponents=0;

 PointerMapCIteratorT I=FindLookupPointer(comp.operator->());
 if(I != PointerLookupTable.end())
 {
  I->second.Pointer->Del(comp.operator->());
 }
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ������� ��������� comp
// ����� ������������, ��� ��������� ����������� �������
bool UAContainer::DelComponent(UEPtr<UAContainer> comp, bool canfree)
{
 if(!SharesUnInit() || !ADelComponent(comp))
  return false;

 if(comp->GetMainOwner() == MainOwner)
  comp->SetMainOwner(0);

 // �������� �� ���� ���������
 // ������� ��������� �� ������� ������������ ���������
 DelLookupComponent(comp->Name);

 // �������� �� ���� ���������
 DelComponentTable(comp);

 comp->Owner=0;

 if(canfree)
  comp->Free();

 if(!NumComponents)
  LastId=0;
 return true;
}


// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool UAContainer::AAddComponent(UEPtr<UAContainer> comp, UIPointer* pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UAContainer::ADelComponent(UEPtr<UAContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ��������� ��������� MainOwner ����� ������� ����� �������
bool UAContainer::AUpdateMainOwner(void)
{
 return true;
}
// --------------------------

/* *************************************************************************** */


}

#endif


