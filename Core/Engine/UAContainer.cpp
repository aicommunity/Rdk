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
#include "UAContainer.h"
#include "UAContainerStorage.h"
#include "UAConnector.h"
#include "UAItem.h"
#include "UANet.h"
#include "../Serialize/Serialize.h"
#include "../Math/MUBinarySerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "UAContainerStorage.h"

namespace RDK {

NameT ForbiddenName="";

/* *********************************************************************** */
/* *********************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UAContainerVector::UAContainerVector(void)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;
}

UAContainerVector::UAContainerVector(const UAContainerVector &copy)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 *this=copy;
}

UAContainerVector::~UAContainerVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int UAContainerVector::GetSize(void) const
{
 return Size;
}

// �������� ������ ������� � ����������� ������� ������
void UAContainerVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUAContainer *newbuffer=new PUAContainer[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ������� ������
void UAContainerVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// ���������� ��������� �� ������ ������
PUAContainer* UAContainerVector::GetBuffer(void)
{
 return Buffer;
}

// ��������� ������� � ����� �������
// ���������� ������ ��������
int UAContainerVector::Add(PUAContainer value)
{
 Resize(Size+1);
 Buffer[Size-1]=value;
 return Size-1;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UAContainerVector& UAContainerVector::operator = (const UAContainerVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// �������� �������
PUAContainer& UAContainerVector::operator [] (int index)
{
 return Buffer[index];
}

UAContainer& UAContainerVector::operator () (int index)
{
 return *Buffer[index];
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */


/* *************************************************************************** */
// Class UAContainer
/* *************************************************************************** */
// --------------------------
// ���������� ��������
// --------------------------
// ������� ����� ������
ULongTime UAContainer::Time=0;
double UAContainer::DoubleTime=0;
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
   cstorage->PopObject(this);
  }
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool UAContainer::CheckOwner(const UAContainer *owner) const
{
 if(Owner == 0 && Owner != owner)
  return false;

 if(Owner == owner)
  return true;

 return static_cast<UAContainer*>(Owner)->CheckOwner(owner);
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

 static_cast<UAContainer*>(Owner)->GetFullId(buffer);
 buffer.Add(Id);

 return buffer;
}

// ����������  '�������' Id �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������.
ULongId& UAContainer::GetLongId(const UAContainer *mainowner, ULongId &buffer) const
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

 if(static_cast<UAContainer*>(Owner)->GetLongId(mainowner,buffer).GetSize() == 0)
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
  return static_cast<UAContainer*>(Owner)->DelComponent(this,false);

 return true;
}

// ������������� ��������� �� �������� ��������� ���� ��������
// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
bool UAContainer::SetMainOwner(RDK::UAComponent* const mainowner)
{
 return RDK::UAComponent::SetMainOwner(mainowner);
}

bool UAContainer::SetMainOwner(RDK::UAComponent* const mainowner, int levels)
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
    if(!PComponents[i]->SetMainOwner(static_cast<UAContainer*>(MainOwner),levels))
     return false;
   }
   else
    if(!PComponents[i]->SetMainOwner(static_cast<UAContainer*>(MainOwner),levels-1))
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
 unsigned long int k=2;
 char buffer[20];

 while(true)
  {
   sprintf(buffer,"%i",k);
   if(prefix.size()>0)
    namebuffer=prefix+buffer;
   else
    namebuffer=NameT("n")+buffer;
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
   if(!dynamic_cast<UAContainer*>(GetOwner())->CheckName(name))
    return false;

   if(!dynamic_cast<UAContainer*>(GetOwner())->ModifyLookupComponent(Name, name))
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

 dynamic_cast<const UAContainer*>(GetOwner())->GetFullName(buffer);
 buffer+='.';
 buffer+=Name;

 return buffer;
}

// ����������  '�������' ����� �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������.
NameT& UAContainer::GetLongName(const UAContainer *mainowner, NameT &buffer) const
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

 if(dynamic_cast<UAContainer*>(GetOwner())->GetLongName(mainowner,buffer) == ForbiddenName)
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
 map<NameT,UId>::const_iterator I=CompsLookupTable.begin(),
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
 map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
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
  OwnerTimeStep=static_cast<UAContainer*>(Owner)->TimeStep;
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
   if(!static_cast<UAContainer*>(Owner)->CheckId(id))
    return false;

   static_cast<UAContainer*>(Owner)->SetLookupComponent(Name, id);
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
UAContainer* UAContainer::Alloc(UAContainerStorage *stor, bool copystate)
{
 UAContainer* copy;
 UAContainerStorage *storage=(stor!=0)?stor:static_cast<UAContainerStorage*>(Storage);

 if(storage)
 {
  copy=static_cast<UAContainer*>(storage->TakeObject(Class,this));
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
bool UAContainer::Copy(UAContainer *target, UAContainerStorage *stor, bool copystate) const
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
 if(Storage)
 {
  if(!BreakOwner())
   return; // ��������, ����� ������ ���� ����������!!
//   throw; // ��������, ����� ������ ���� ����������!!
  Storage->ReturnObject(this);
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
bool UAContainer::CheckComponentType(const UAContainer* comp) const
{
 return false;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
UAContainer* UAContainer::GetComponent(const UId &id) const
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
UAContainer* UAContainer::GetComponent(const NameT &name) const
{
 return GetComponent(GetComponentId(name));
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'.
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
UAContainer* UAContainer::GetComponentL(const ULongId &id) const
{
 UAContainer* comp;

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
UAContainer* UAContainer::GetComponentL(const NameT &name) const
{
 UAContainer* comp;
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
UAContainer* UAContainer::GetComponentByIndex(int index) const
{
 if(index >= NumComponents)
  return 0;

 return PComponents[index];
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
UId UAContainer::AddComponent(UAContainer* comp, UIPointer* pointer)
{
// NameT namebuffer;

 if(!comp)
  return ForbiddenId;

 if(comp->GetOwner() == this)
  return comp->Id;

 if(comp->GetOwner())
  return ForbiddenId;

 if(!CheckComponentType(comp))
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

 comp->SetId(id);

 comp->SetOwner(this);


 // ��������� ��������� � ������� ������������ ���������
 SetLookupComponent(comp->Name, comp->Id);

 // ���������� � ���� ���������
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 if(MainOwner)
  comp->SetMainOwner(MainOwner);

 if(!AAddComponent(comp,pointer))
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
 UAContainer *comp=GetComponent(id);

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
 UAContainer **pcomps=PComponents;
 for(int i=0;i<NumComponents;i++,buffer++,pcomps++)
  *buffer=(*pcomps)->Id;
}

// �������� ��� ���������� ����� ������� � ������ 'comp', ���� ��������.
bool UAContainer::CopyComponents(UAContainer* comp, UAContainerStorage* stor) const
{
 UAContainer* bufcomp;

 if(!comp)
  return false;

 // ������� ������ ���������� �� 'comp'
 comp->DelAllComponents();

 UAContainer ** pcomponents=0;
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
   map<UId,UIPointer*>::const_iterator I=FindLookupPointer(PComponents[i]);
   if(I != PointerLookupTable.end())
   {
    map<UId,UIPointer*>::iterator J=comp->PointerLookupTable.find(I->first);
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
bool UAContainer::CopyProperties(UAContainer* comp) const
{
 bool key=true;

 if(!comp)
  return false;

 UAContainer* ccomp=static_cast<UAContainer*>(comp);

 Serialize::USerStorageBinary databuffer;
  VariableMapCIteratorT I=PropertiesLookupTable.begin(),
                                        J=PropertiesLookupTable.end();
  while(I != J)
  {
   databuffer.clear();
   key &= ccomp->SetProperty(I->second.Id,GetProperty(I->second.Id,&databuffer));
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
                            int sublevel, UAContainer *ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UAContainer *cont=GetComponentByIndex(i);
  if(dynamic_cast<UAConnector*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:this,id);
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
                            int sublevel, UAContainer *ownerlevel)

{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UAContainer *cont=GetComponentByIndex(i);
  if(dynamic_cast<UAItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:this,id);
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
                            int sublevel, UAContainer *ownerlevel)
{
 ULongId id;

 for(int i=0;i<NumComponents;i++)
 {
  UAContainer *cont=GetComponentByIndex(i);
  if(dynamic_cast<UANet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:this,id);
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
bool UAContainer::CopyState(UAContainer* comp) const
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
 UAContainer* cont=GetComponent(id);

 if(!cont)
  return false;

 PointerMapIteratorT I=PointerLookupTable.end();;
 PointerMapIteratorT K=PointerLookupTable.end();;
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
 UAContainer* cont=GetComponent(name);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(id);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(name);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(id);

 if(!cont)
  return false;

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
 UAContainer* cont=GetComponent(name);

 if(!cont)
  return false;

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
  UAContainer *cont=J->second.Pointer->Get();

  while(cont)
  {
   DelComponent(cont,canfree);
   cont=J->second.Pointer->Get();
  }
 }
}
// --------------------------

// --------------------------
// ������ ���������� ������� ����������
// --------------------------
/*
// ��������� ������ � ��������� ������������
bool UAContainer::Save(UVariableData *values)
{
 NameT name=Name;

 comp->SetName(name);

 values->

 map<NameT,NVariable>::iterator I,J;
 I=ParamsLookupTable.begin();
 J=ParamsLookupTable.end();

 NameT str,str2,paramname;
 while(I != J)
 {
  paramname=GetParameterLongName(*(I->second.Property));
//  paramname+="::";
//  paramname+=I->first;
  stringstream stream(ios::out);
  I->second.Property->Save(stream);
  (*comp)()[paramname][I->first]=stream.str();
  ++I;
 }

 buffer.AddSorted(comp);

 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Save(*comp))
   return false;

 return true;
}

// ��������� ������ �� ���������� ������������
// ��� �������� ��������� ������������
// ��������� ����� ������� ��� storage, ���� �� �� 0
// ���� ������� ��������� �� ������, �� �� ������ ������
// ���� ���� paramsonly == true �� ������ ��������� ������ ����������
// ��� ���������� � ���� ��� ��� ������������ �������� ���������
bool UAContainer::Load(UVariableData *values, UAContainerStorage *storage, bool paramsonly)
{
}
            */
/* // ��������� ������ � ������
bool UAContainer::Save(RDK::UClassRegistry &buffer)
{
 RDK::UClassRegistry *comp=new RDK::UClassRegistry;

 NameT name=Name;

 if(Storage)
 {
  UId classid=Storage->FindClass(this);
  if(classid != ForbiddenId)
  {
   name+=": ";
   name+=static_cast<UAContainerStorage*>(Storage)->GetClassName(classid);
  }

 }

 comp->SetName(name);

 map<NameT,NVariable>::iterator I,J;
 I=ParamsLookupTable.begin();
 J=ParamsLookupTable.end();

 NameT str,str2,paramname;
 while(I != J)
 {
  paramname=GetParameterLongName(*(I->second.Property));
//  paramname+="::";
//  paramname+=I->first;
  stringstream stream(ios::out);
  I->second.Property->Save(stream);
  (*comp)()[paramname][I->first]=stream.str();
  ++I;
 }

 buffer.AddSorted(comp);

 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->Save(*comp))
   return false;

 return true;
}

// ��������� ������ �� ������
// ��� �������� ��������� ������������
// ��������� ����� ������� ��� storage, ���� �� �� 0
// ���� ������� ��������� �� ������, �� �� ������ ������
bool UAContainer::Load(RDK::UClassRegistry &buffer, UAContainerStorage *storage, bool paramsonly)
{
 // ������� ������ ������ ������ ���������

 if(!Storage && !storage && paramsonly == false)
  return false;

 UAContainerStorage* stor=(storage != 0)?storage:static_cast<UAContainerStorage*>(Storage);

 UAContainer *comp=0;

 // ��������������� ���
 NameT name=buffer.GetName();
 int name_i=name.find_last_of(":");
 if(name_i == NameT::npos)
  Name=name;
 else
  Name=name.substr(0,name_i);

 RDK::UClassRegDataIterator I,J;
 RDK::URegDataIterator rI,rJ;
 map<NameT,NVariable>::iterator K;

 // ��������������� ���������
 I=buffer().begin();
 J=buffer().end();
 while(I != J)
 {
  rI=I->second.begin();
  rJ=I->second.end();
  while(rI != rJ)
  {
   K=ParamsLookupTable.find(rI->first);
   if(K != ParamsLookupTable.end())
   {
    stringstream stream(buffer()[I->first][rI->first],ios::in);
    K->second.Property->Load(stream);
   }
   ++rI;
  }

  ++I;
 }

 // ��������������� ��������� �������� ��������� ���� ����������
 vector<NameT> treebuf;
 vector<NameT> treeclassbuf;
 vector<NameT>::iterator tI,tJ,tK;
 buffer.GetSubTrees(treebuf);
 treeclassbuf.resize(treebuf.size());
 for(int i=0;i<treebuf.size();i++)
 {
  name_i=treebuf[i].find_last_of(":");
  if(name_i != NameT::npos)
  {
   treeclassbuf[i]=treebuf[i].substr(name_i+2);
   treebuf[i]=treebuf[i].substr(0,name_i);
  }
 }

 if(!paramsonly)
 {
  vector<UId> deletelist;

  for(int i=0;i<NumComponents;i++)
  {
   tI=find(treebuf.begin(),treebuf.end(),PComponents[i]->Name);
   if(tI == treebuf.end())
    deletelist.push_back(PComponents[i]->Id);
   else
   {
    tJ=treeclassbuf.begin();
    for(tK=treebuf.begin();tK!=tI;++tK,++tJ);
    PComponents[i]->Load(buffer[*tI+string(": ")+*tJ],storage,paramsonly);
    treebuf.erase(tI); treeclassbuf.erase(tJ);
   }
  }

  // ������� ���������� ������� ����� ��� � ������
  for(int i=0;i<deletelist.size();i++)
   DelComponent(deletelist[i]);

  // ��������� ����� ����������
  for(int i=0;i<treebuf.size();i++)
  {
   if(treeclassbuf[i] != "")
   {
    comp=stor->TakeObject(treeclassbuf[i]);
    if(comp)
    {
     if(!AddComponent(comp))
      delete comp;
     else
      comp->Load(buffer[treebuf[i]+string(": ")+treeclassbuf[i]],storage,paramsonly);
    }
   }
  }
 }
 else
 {
  // �������� ��� ���������
  for(int i=0;i<treebuf.size();i++)
  {
   RDK::UClassRegistry &temp=buffer[treebuf[i]+string(": ")+treeclassbuf[i]];
   name=temp.GetName();
   name_i=name.find_last_of(":");
   if(name_i != NameT::npos)
    name=name.substr(0,name_i);
   comp=GetComponent(name);
   if(comp)
    comp->Load(temp,storage,paramsonly);
  }
 }

 return true;
}

// ��������� ��������� ������� � ������
bool UAContainer::SaveState(RDK::UClassRegistry &buffer)
{
 RDK::UClassRegistry *comp=new RDK::UClassRegistry;

 NameT name=Name;

 if(Storage)
 {
//  UId classid=Storage->FindClass(this);
//  if(classid != ForbiddenId)
  if(Class != ForbiddenId)
  {
   name+=": ";
   name+=static_cast<UAContainerStorage*>(Storage)->GetClassName(Class);//Storage->GetClassName(classid);
  }

 }

 comp->SetName(name);

 map<NameT,NVariable>::iterator I,J;
 I=StateLookupTable.begin();
 J=StateLookupTable.end();

 NameT str,str2,statename;
 while(I != J)
 {
  statename=GetStateLongName(*(I->second.Property));
//  paramname+="::";
//  paramname+=I->first;
  stringstream stream(ios::out);
  I->second.Property->Save(stream);
  (*comp)()[statename][I->first]=stream.str();
//  (*comp)()[statename][I->first].assign(1500,'a');
  ++I;
 }

 buffer.AddSorted(comp);

 for(int i=0;i<NumComponents;i++)
  if(!PComponents[i]->SaveState(*comp))
   return false;

 return true;
}

// ��������� ��������� ������� �� ������
// ���� �����-�� �� �������� ��������� �� �������, �� �� ��������� �� �����������������
// ������ ����� ���������� true.
// ���� ��� �����-�� �������� ��������� ������ ��������� �����������, ��
// ����� ���������� false
bool UAContainer::LoadState(RDK::UClassRegistry &buffer)
{
 UAContainer *comp=0;

 // ��������������� ���
 NameT name=buffer.GetName();
 int name_i=name.find_last_of(":");
 if(name_i != NameT::npos)
  name=name.substr(0,name_i);

 if(Name != name)
  return false;

 RDK::UClassRegDataIterator I,J;
 RDK::URegDataIterator rI,rJ;
 map<NameT,NVariable>::iterator K;

 I=buffer().begin();
 J=buffer().end();
 while(I != J)
 {
  rI=I->second.begin();
  rJ=I->second.end();
  while(rI != rJ)
  {
   K=StateLookupTable.find(rI->first);
   if(K != StateLookupTable.end())
   {
    stringstream stream(buffer()[I->first][rI->first],ios::in);
    K->second.Property->Load(stream);
   }
   ++rI;
  }

  ++I;
 }

 // ��������������� ��������� �������� ��������� ���� ����������
 vector<NameT> treebuf;
 vector<NameT>::iterator tI;
 buffer.GetSubTrees(treebuf);

 for(int i=0;i<treebuf.size();i++)
 {
  comp=GetComponent(buffer[treebuf[i]].GetName());
  if(comp)
   comp->LoadState(buffer[treebuf[i]]);
 }

 return true;
}              */
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
 UAContainer *original=0;
 if(Storage)
  original=static_cast<UAContainer*>(Storage->GetClass(Class));

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
 UAContainer **comps=PComponents;
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
  return ACalculate();

 if(TimeStep == OwnerTimeStep)
  return ACalculate();
 else
 if(TimeStep < OwnerTimeStep)
  {
   --CalcCounter;
   if(CalcCounter <= 0)
    {
	 CalcCounter=OwnerTimeStep/TimeStep;
     return ACalculate();
    }
  }
 else
 if(TimeStep > OwnerTimeStep)
  {
   for(UTime i=TimeStep/OwnerTimeStep;i>=0;--i)
    if(!ACalculate())
     return false;
  }
// if(!ACalculate())
//  return false;

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

 map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
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
 map<NameT,UId>::iterator I=CompsLookupTable.find(name);

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
bool UAContainer::GetNumControllers(void) const
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
UAContainer::PointerMapCIteratorT UAContainer::FindLookupPointer(const UAContainer *source) const
{
 PointerMapCIteratorT I=PointerLookupTable.begin(),
                      J=PointerLookupTable.end();

 while(I != J)
  {
   if(I->second.Pointer && I->second.Pointer->Find(source)>=0)
    break;
   ++I;
  }

 return I;
}
// --------------------------


// --------------------------
// ������� ������ ���������� �������� ���������
// --------------------------
// ��������� ��������� 'comp' � ������� ���������
void UAContainer::AddComponentTable(UAContainer *comp, UIPointer* pointer)
{
 Components.Add(comp);
 PComponents=&Components[0];
 NumComponents=Components.GetSize();

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

// ������� ��������� 'comp' �� ������� ���������
void UAContainer::DelComponentTable(UAContainer *comp)
{
 int i;

 if(PComponents[NumComponents-1]==comp)
  Components.Resize(NumComponents-1);
 else
  {
   for(i=0;i<NumComponents;i++)
    if(PComponents[i] == comp)
     break;

   if(i>=NumComponents)
    return;

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UAContainer*));
   Components.Resize(NumComponents-1);
  }

 NumComponents=Components.GetSize();
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
bool UAContainer::DelComponent(UAContainer* comp, bool canfree)
{
 if(!ADelComponent(comp))
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
bool UAContainer::AAddComponent(UAContainer* comp, UIPointer* pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool UAContainer::ADelComponent(UAContainer* comp)
{
 return true;
}
// --------------------------

/* *************************************************************************** */


}

#endif


