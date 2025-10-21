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
#include "UEnvironment.h"
#include "../../Deploy/Include/rdk_exceptions.h"
#include "UEnvException.h"

namespace RDK {

UIPointer::~UIPointer()
{

}


// --------------------------
// ������������ � �����������
// --------------------------
UPVariable::UPVariable(void)
{
 Id=ForbiddenId;
}

UPVariable::UPVariable(UId id, std::shared_ptr<UIPointer> prop)
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
  : Name("Name", this, &UContainer::SetName)
  , Id("Id", this, &UContainer::SetId)
  , Activity("Activity", this, &UContainer::SetActivity)
  , TimeStep("TimeStep", this, &UContainer::SetTimeStep)
  , MaxCalculationDuration("MaxCalculationDuration", this, &UContainer::SetMaxCalculationDuration)
  , CalculationDurationThreshold("CalculationDurationThreshold", this, &UContainer::SetCalculationDurationThreshold)
  , Coord("Coord", this, &UContainer::SetCoord)
  , StepDuration("StepDuration", this)
  , DebugSysEventsMask("DebugSysEventsMask", this, &UContainer::SetDebugSysEventsMask)
  , PComponents(0), NumComponents(0), LastId(0)

{
 Id = 0;
 Activity = false;

 InitFlag=false;

 CalculationDurationThreshold= -1;
 MaxCalculationDuration = -1;
 MovingFlag=false;
}

UContainer::~UContainer(void)
{
 if(GetStaticFlag() && Owner.lock())
  GetOwner()->DelStaticComponent(this);
 DelAllComponentsRaw();
 DelAllStaticComponents();


 UnLinkAllControllers();

 BreakOwner();

 auto storage = Storage.lock();
 if(storage && !GetStaticFlag())
  storage->PopObject(UEPtr<UContainer>(this));
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
std::shared_ptr<UContainer> UContainer::GetOwner(void) const
{
 return std::dynamic_pointer_cast<UContainer>(Owner.lock());
}

// ���������� ��������� �� �������� ��������� ���� ��������
std::shared_ptr<UContainer> UContainer::GetMainOwner(void) const
{
 return std::dynamic_pointer_cast<UContainer>(MainOwner.lock());
}

// ���������� ��������� ��������� ����� �������
std::shared_ptr<UStorage> UContainer::GetStorage(void) const
{
 return Storage.lock();
}

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool UContainer::CheckOwner(UEPtr<UContainer> owner) const
{
 if(Owner.lock() == nullptr && Owner.lock().get() != owner.Get())
  return false;

 if(Owner.lock().get() == owner.Get())
  return true;

 return GetOwner()->CheckOwner(owner);
}

// ���������� ������ Id �������
// (������� Id ���� ����������).
ULongId& UContainer::GetFullId(ULongId &buffer) const
{
 if(!Owner.lock())
  {
   buffer.Add(Id);
   return buffer;
  }

 GetOwner()->GetFullId(buffer);
 buffer.Add(Id);

 return buffer;
}

ULongId UContainer::GetFullId(void) const
{
 ULongId id;

 return GetFullId(id);
}


// ����������  '�������' Id �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������.
ULongId& UContainer::GetLongId(std::shared_ptr<UContainer> mainowner, ULongId &buffer) const
{
 if(Owner.lock() == nullptr && Owner.lock() != mainowner)
  {
   buffer.Resize(0);
   return buffer;
  }

 if(Owner.lock() == mainowner)
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

ULongId UContainer::GetLongId(std::shared_ptr<UContainer> mainowner) const
{
 ULongId id;

 return GetLongId(mainowner,id);
}


// ������������� ������� ������������ ������, ������������� ������� ���
std::string& UContainer::GetLongId(std::shared_ptr<UContainer> mainowner, std::string &buffer) const
{
 return GetLongName(UEPtr<UContainer>(mainowner.get()),buffer);
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

// ���������� ������ ���������� ����� �������
bool UContainer::SetEnvironment(std::shared_ptr<UEnvironment> environment)
{
 if(!UComponent::SetEnvironment(environment))
  return false;

 bool res=true;

 for(int i=0;i<NumComponents;i++)
  res&=PComponents[i]->SetEnvironment(environment);

 return res;
}

// ��������� �� ������
bool UContainer::SetLogger(std::shared_ptr<ULoggerEnv> logger)
{
 if(!UComponent::SetLogger(logger))
  return false;

 bool res=true;

 for(int i=0;i<NumComponents;i++)
  res&=PComponents[i]->SetLogger(logger);

 return res;
}

// ����� ����������� ���������� �����
void UContainer::ProcessException(UException &exception)
{
 auto logger = Logger.lock();
 if(logger)
  logger->ProcessException(exception);
 else
  throw exception;
}


// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
void UContainer::LogMessage(int msg_level, const std::string &line, int error_event_number)
{
 auto logger = Logger.lock();
 if(logger)
 {
  logger->LogMessageEx(msg_level, GetFullName(), line, error_event_number);
 }
}

void UContainer::LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 auto logger = Logger.lock();
 if(logger)
 {
  logger->LogMessageEx(msg_level, GetFullName(), method_name, line, error_event_number);
 }
}

void UContainer::LogMessageEx(int msg_level, const std::string &line, int error_event_number)
{
 auto logger = Logger.lock();
 if(logger)
 {
  std::string full_name;
  GetFullName(full_name);
  logger->LogMessageEx(msg_level, full_name, full_name+std::string(" - ")+line, error_event_number);
 }
}

void UContainer::LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 auto logger = Logger.lock();
 if(logger)
 {
  std::string full_name;
  GetFullName(full_name);
  logger->LogMessageEx(msg_level, full_name, method_name, full_name+std::string(" - ")+line, error_event_number);
 }
}

void UContainer::LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number)
{
 std::string str_line = sntoa(line);
 LogMessageEx(msg_level, method_name, str_line, error_event_number);
}

void UContainer::LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier)
{
 auto logger = Logger.lock();
 if(logger && logger->GetDebugMode() && (logger->GetDebugSysEventsMask() & (debug_sys_msg_type & DebugSysEventsMask)))
 {
  std::string prefix;
  switch(debug_sys_msg_type)
  {
  case RDK_SYS_DEBUG_CALC:
   prefix="Calculate: ";
  break;

  case RDK_SYS_DEBUG_RESET:
   prefix="Reset: ";
  break;
  }

  std::string suffix;
  switch(modifier)
  {
  case RDK_SYS_MESSAGE_ENTER:
   suffix="Enter";
  break;

  case RDK_SYS_MESSAGE_EXIT_OK:
   suffix="Exit: OK";
  break;

  case RDK_SYS_MESSAGE_EXIT_ININIT_FAIL:
   suffix="Exit: InInit == false";
  break;

  case RDK_SYS_MESSAGE_NEW_CALC_ITERATION:
   suffix="<======== NEW ITERATION ========>";
  break;
  }

  LogMessageEx(RDK_EX_DEBUG, prefix+suffix);
 }
}

/// �������� �������� ��� ����� � ������ (�����, ���������, ���������)
void UContainer::LogPropertiesBeforeCalc(void)
{
 auto logger = Logger.lock();
 if(logger && logger->GetDebugMode() && (logger->GetDebugSysEventsMask() & (RDK_SYS_DEBUG_PROPERTIES & DebugSysEventsMask)))
 {
  std::string log_message;

  if(PropertiesForDetailedLog.empty())
  {
   VariableMapCIteratorT I=PropertiesLookupTable.begin(),J=PropertiesLookupTable.end();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyType() & ptInput)
	 if(PreparePropertyLogString(I->second, ptInput, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }

   I=PropertiesLookupTable.begin();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyGroup() != pgPublic)
	 continue;

	if(I->second.GetPropertyType() & ptParameter)
	 if(PreparePropertyLogString(I->second, ptParameter, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }
  }
  else
  {
   for(size_t i=0;i<PropertiesForDetailedLog.size();i++)
   {
	VariableMapCIteratorT I=PropertiesLookupTable.find(PropertiesForDetailedLog[i]);
	if(PreparePropertyLogString(I->second, ptInput | ptParameter, log_message))
 	 LogMessageEx(RDK_EX_DEBUG, log_message);
   }
  }
 }
}

/// �������� �������� ��� ������ �� ������� (������)
void UContainer::LogPropertiesAfterCalc(void)
{
 auto logger = Logger.lock();
 if(logger && logger->GetDebugMode() && (logger->GetDebugSysEventsMask() & (RDK_SYS_DEBUG_PROPERTIES & DebugSysEventsMask)))
 {
  std::string log_message;
  if(PropertiesForDetailedLog.empty())
  {
   VariableMapCIteratorT I=PropertiesLookupTable.begin(),J=PropertiesLookupTable.end();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyType() & ptOutput)
	 if(PreparePropertyLogString(I->second, ptOutput, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }

   I=PropertiesLookupTable.begin();
   for(; I != J; ++I)
   {
	if(I->second.GetPropertyGroup() != pgPublic)
	 continue;

	if(I->second.GetPropertyType() & ptState && !(I->second.GetPropertyType() & ptOutput)  && !(I->second.GetPropertyType() & ptInput))
	 if(PreparePropertyLogString(I->second, ptState, log_message))
	  LogMessageEx(RDK_EX_DEBUG, log_message);
   }

  }
  else
  {
   for(size_t i=0;i<PropertiesForDetailedLog.size();i++)
   {
	VariableMapCIteratorT I=PropertiesLookupTable.find(PropertiesForDetailedLog[i]);
	if(PreparePropertyLogString(I->second, ptOutput | ptState, log_message))
	 LogMessageEx(RDK_EX_DEBUG, log_message);
   }
  }
 }
}

/// ���������� ��������� ����� ������ �������
bool UContainer::CheckDebugMode(void) const
{
 auto logger = Logger.lock();
 if(logger)
 {
  return logger->GetDebugMode();
 }
 return false;
}


/// ��������� ������ ������� ��� ���������� ���� �� ������
/// ����������� - �������
void UContainer::SetPropertiesForDetailedLog(const std::string &str)
{
 PropertiesForDetailedLog.clear();
 RDK::separatestring(str, PropertiesForDetailedLog, ',');
}
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ���������� � ������������ ����
const RDK::MVector<double,3>& UContainer::GetCoord(void) const
{
 return Coord.v;
}

bool UContainer::SetCoord(const RDK::MVector<double,3> &value)
{
 if(Coord.v == value)
  return true;

 Coord.v =value;

 return true;
}


// �����, ����������� �� ��������� �������
// (��� ����� ������� ������� �������� ��������) (��)
unsigned long long UContainer::GetStepDuration(void) const
{
 unsigned long long res=0;
 for(int i=0;i<NumComponents;i++)
  res+=PComponents[i]->GetFullStepDuration();

 return StepDuration-res;
}

// �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
unsigned long long UContainer::GetFullStepDuration(void) const
{
 return StepDuration;
}

// �����, ��������� ����� ����� ���������� ���������� �����
unsigned long long UContainer::GetInterstepsInterval(void) const
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
 UEPtr<UContainer> owner(GetOwner().get());
 if(owner)
  owner->DelComponent(this,false);
}

// ������������� ��������� �� �������� ��������� ���� ��������
// ��������� ��������������� �� ����� ������� �������� ���������
// 'levels'. ���� levels < 0 �� ��������������� ����������� �� ���� �������
void UContainer::SetMainOwner(std::shared_ptr<UComponent> mainowner)
{
 UComponent::SetMainOwner(mainowner);
}

void UContainer::SetMainOwner(std::shared_ptr<UComponent> mainowner, int levels)
{
 if(MainOwner.lock() == mainowner && !levels)
  return;

 MainOwner=mainowner;

 if(!levels)
  return;

 std::shared_ptr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++, comps++)
 {
  // ������������� �������� ��������� ������ ��� �������� �����������
  // � ������� �� ��� �� �����
  if((*comps)->GetMainOwner() == nullptr)
  {
   if(levels<0)
    (*comps)->SetMainOwner(MainOwner.lock(),levels);
   else
    (*comps)->SetMainOwner(MainOwner.lock(),levels-1);
  }
 }
}

// ��������� ������������ Id 'id' �� ������������ � ������ �������, �������.
bool UContainer::CheckId(const UId &id)
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.begin();
 for(;I != CompsLookupTable.end(); I++)
  if(I->second == id)
   return false;
 return true;
}

bool UContainer::CheckComponent(const NameT &name)
{
  return !CheckName(name);
}

bool UContainer::CheckComponentL(const NameT &name)
{
 if(GetComponentL(name,true) != 0)
  return true;
 return false;
}

// ��������� ������������ ��� 'name' �� ������������ � ������
// ������� �������.
bool UContainer::CheckName(const NameT &name)
{
 if(CompsLookupTable.find(name) == CompsLookupTable.end())
  return true;

 return false;
}

// ��������� ������������ ��� 'name' �� �������������� ������������
bool UContainer::ValidateName(const NameT &name)
{
 if(name.empty())
  return false;
 if(name.find_first_of(" ") != std::string::npos)
  return false;
 std::locale loc;
 if(!std::isalpha(*name.begin(),loc))
  return false;
 for (std::string::const_iterator it=name.begin(); it!=name.end(); ++it)
  {
	if (!std::isalnum(*it,loc) && *it != '_' && *it != '-')
	 return false;
  }
 return true;
}

// ���������� ���������� Id.
UId UContainer::GenerateId(void)
{
 return ++LastId;
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
 return Name.v;
}

bool UContainer::SetName(const NameT &name)
{
 if(Name.v == name)
  return true;

 if(name.empty())
  return false;

 if(!ValidateName(name))
  RDK_THROW(EComponentNameInvalid(name));

  if(GetOwner() != 0)
  {
   if(!GetOwner()->CheckName(name))
	RDK_THROW(EComponentNameAlreadyExist(name));

   GetOwner()->ModifyLookupComponent(Name, name);
  }
 Name.v=name;
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

NameT UContainer::GetFullName(void) const
{
 std::string buf;
 return GetFullName(buf);
}

// ����������  '�������' ����� �������
// (�������� ��� ��������� 'mainowner').
// ����� ���������� ������ ������, ���� 'mainowner' - �� ��������
// ���������� ������� �� �� ����� ������ ��������.
NameT& UContainer::GetLongName(const UEPtr<UContainer> &mainowner, NameT &buffer) const
{
 auto owner = GetOwner();
 if(!owner && owner != std::shared_ptr<UContainer>(mainowner.Get()))
  {
   buffer.clear();
   return buffer;
  }

 if(owner == std::shared_ptr<UContainer>(mainowner.Get()))
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

NameT UContainer::GetLongName(const UEPtr<UContainer> &mainowner) const
{
 std::string buf;
 return GetLongName(mainowner,buf);
}
// --------------------------

// --------------------------
// ������ ������� � �������� �����������
// --------------------------
// ���������� ��� ��������� ���������� �� ��� Id
const NameT& UContainer::GetComponentName(const UId &id) const
{
 std::map<NameT,UId>::const_iterator I,J;
 for(I=CompsLookupTable.begin(), J=CompsLookupTable.end(); I!=J; ++I)
 {
  if(I->second == id)
   break;
 }

 if(I ==J)
  RDK_THROW(EComponentIdNotExist(id));

 return I->first;
}

// ���������� Id ��������� ���������� �� ��� �����
const UId& UContainer::GetComponentId(const NameT &name, bool nothrow) const
{
 std::map<NameT,UId>::const_iterator I=CompsLookupTable.find(name);
 if(I == CompsLookupTable.end())
 {
  if(nothrow)
   return ForbiddenId;
  RDK_THROW(EComponentNameNotExist(name));
 }

 return I->second;
}

// ���������� ��� ���������� ��������� �� ��� Id
const NameT& UContainer::GetPointerName(const UId &id) const
{
 PointerMapCIteratorT I,J;
 for(I=PointerLookupTable.begin(), J=PointerLookupTable.end(); I!=J; ++I)
 {
  if(I->second.Id == id)
   break;
 }

 if(I == J)
  RDK_THROW(EPointerIdNotExist(id));

 return I->first;
}

// ���������� Id ���������� ��������� �� ��� �����
const UId& UContainer::GetPointerId(const NameT &name) const
{
 PointerMapCIteratorT I=PointerLookupTable.find(name);
 if(I == PointerLookupTable.end())
  RDK_THROW(EPointerNameNotExist(name));

 return I->second.Id;
}

// ������������ ����� ���� ��������� �� ��������� ����� ������
// � ���������� ������ ������� ���� ��������� ���� ������ ������
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
const vector<UEPtr<UContainer> >& UContainer::GetComponentsByClassName(const NameT &name, vector<UEPtr<UContainer> > &buffer, bool find_all)
{
 int numComp=GetNumComponents();
 UEPtr<UContainer> comp;

 switch(find_all)
 {
  case false:
   for(int i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	if( comp->GetCompClassName() == name )
	{
	 buffer.push_back(comp);
	}
   }
   break;

  case true:
   for(int i=0; i<numComp; i++)
   {
	comp=GetComponentByIndex(i);
	comp->GetComponentsByClassName(name, buffer, true);
	if( comp->GetCompClassName() == name )
	{
	 buffer.push_back(comp);
	}
   }
   break;
 }

 return buffer;
}

// ������������ ����� ���� ��������� �� ��������� ����� ������
// � ���������� ������ ������� ���� ��������� ���� ������ ������
// false - ������ � ������� ����������
// true -  ������ � ������� ���������� � ������
const vector<NameT>& UContainer::GetComponentsNameByClassName(const NameT &name, vector<NameT> &buffer, bool find_all)
{
 vector<UEPtr<UContainer> > components;
 string compName;
 GetComponentsByClassName(name, components, find_all);
 size_t numComp=components.size();

 for(size_t i=0; i<numComp; i++)
 {
  compName=components[i]->GetLongName(this, compName);
  buffer.push_back(compName);
 }

 return buffer;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���� ��������������
const UTime& UContainer::GetTimeStep(void) const
{
 return TimeStep.v;
}

bool UContainer::SetTimeStep(const UTime &timestep)
{
 if(timestep <= 0)
  return false;

 TimeStep.v=timestep;

 auto owner = Owner.lock();
 if(owner)
  OwnerTimeStep=std::dynamic_pointer_cast<UContainer>(owner)->TimeStep;
 else
  OwnerTimeStep=timestep;

 // ��������� �� ���� ����������� �������
 std::shared_ptr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->OwnerTimeStep=timestep;

 return true;
}

/// ����������� ����� ������������� ��������������� TimeStep ��� ���������� � ���� �������� ���������
/// ������������� ������ ��� ������ �� UEnvironment
void UContainer::ChangeUseIndTimeStepMode(bool value)
{
 if(value)
  ChangeLookupPropertyType("TimeStep",ptPubParameter | pgSystem);
 else
  ChangeLookupPropertyType("TimeStep",ptParameter | pgSystem);

 // ��������� �� ���� ����������� �������
 std::shared_ptr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->ChangeUseIndTimeStepMode(value);
}

// ������������� �������� ���� �������������� ���������� � ���� ��� �������� �����������
bool UContainer::SetGlobalTimeStep(UTime timestep)
{
 if(!SetTimeStep(timestep))
  return false;

 // ��������� �� ���� ����������� �������
 std::shared_ptr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(!(*comps)->SetGlobalTimeStep(timestep))
   return false;

 return true;
}



// ������������� ���� ���������� �������
const bool& UContainer::GetActivity(void) const
{
 return Activity.v;
}

bool UContainer::SetActivity(const bool &activity)
{
// if(Activity.v == activity)
//  return true;

 Activity.v=true;
 std::shared_ptr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  (*comps)->Activity = activity;

// if(activity)
//  return Reset(); // !!! ��������. �������� ��� �� �����!

 Activity.v=activity;
 StepDuration=0;
 InterstepsInterval=0;

 if(!Activity)
  return Reset();

 return true;
}

// Id �������
UId UContainer::GetId(void) const
{
 return Id.v;
}

bool UContainer::SetId(const UId &id)
{
 if(id == ForbiddenId)
  return true;// ��������!! ��� ���! throwEForbiddenId(id);

 if(id < 0)
  RDK_THROW(EInvalidId(id));


 if(Owner.lock())
  {
   if(!GetOwner()->CheckId(id))
    RDK_THROW(EComponentIdAlreadyExist(id));

   GetOwner()->SetLookupComponent(Name, id);
  }
 Id.v=id;
 return true;
}

/// ����������� ���������� ����� ������� ���������� ������ � ��������� ������������
/// � �������������.
/// ���� ����� ������� ���������, �� ������ ����������� �������� ���������
/// �� �����������
/// ���� �������� ��������� <0, �� ��� �����������
const long long& UContainer::GetMaxCalculationDuration(void) const
{
 return MaxCalculationDuration.v;
}

bool UContainer::SetMaxCalculationDuration(const long long &value)
{
 MaxCalculationDuration.v=value;
 return true;
}

/// ����� ������� ���������� ������ � ��������� ������������
/// � �������������, �� ���������� �������� �������� ��������������� ��������� � ���.
/// ���� �������� ��������� <0, �� ��� �����������
const long long& UContainer::GetCalculationDurationThreshold(void) const
{
 return CalculationDurationThreshold.v;
}

bool UContainer::SetCalculationDurationThreshold(const long long& value)
{
 CalculationDurationThreshold.v=value;
 return true;
}


/// ����� ��������������� �������� ������ ��������� ���������� ����������
const unsigned int& UContainer::GetDebugSysEventsMask(void) const
{
 return DebugSysEventsMask.v;
}

bool UContainer::SetDebugSysEventsMask(const unsigned int &value)
{
 DebugSysEventsMask.v=value;
 return true;
}

/// ����� ������������ ������ �� ��� �������.
/// ����� ���� �������������� ���� ������ �������������.
/// ��������� ���� ������� ���� MemoryMonitor
long long UContainer::GetMemoryUsageDiff(void) const
{
 return MemoryUsageDiff;
}

/// ��������� ����������� �������� ����� ��������� ������ ����� ���� �������
/// ����� ���� �������������� ���� ����� ����������.
/// ��������� ���� ������� ���� MemoryMonitor
long long UContainer::GetMaxMemoryBlockDiff(void) const
{
 return MaxMemoryBlockDiff;
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
 UEPtr<UStorage> storage=(stor!=0)?stor:UEPtr<UStorage>(GetStorage().get());

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
 CopyProperties(std::shared_ptr<UComponent>(target.Get()), ptParameter);
 target->Build();

 if(copystate)
  CopyProperties(std::shared_ptr<UComponent>(target.Get()), ptState);

 CopyComponents(target,stor);
 return true;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UContainer::Free(void)
{
 while(NumComponents)
  PComponents[0]->Free();

 if(Storage.lock())
 {
  BreakOwner();
  if(!StaticFlag)
   GetStorage()->ReturnObject(this);
 }
 else
  UComponent::Free();
}

	  /*
// ��������� �� ���� ������ � ���������
UEPtr<UInstancesStorageElement> UContainer::GetObjectIterator(void)
{
 return ObjectIterator;
}

void UContainer::SetObjectIterator(UEPtr<UInstancesStorageElement> value)
{
 if(ObjectIterator == value)
  return;
 ObjectIterator=value;
}       */

/// ������������ ���������� ���������� ������ ����������, �������������� ��� �����������
void UContainer::AUpdateInternalData(void)
{
 std::map<std::shared_ptr<UContainer>, NameT>::iterator I=StaticComponents.begin();
 for(;I!=StaticComponents.end();++I)
 {
  UpdateStaticComponent(I->second,UEPtr<UContainer>(I->first.get()));
 }
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

 std::shared_ptr<UContainer>* comps=PComponents;
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
UEPtr<UContainer> UContainer::GetComponent(const UId &id, bool nothrow) const
{
 if(id == ForbiddenId)
 {
  if(nothrow)
   return 0;
  RDK_THROW(EComponentIdNotExist(id));
 }

 std::shared_ptr<UContainer>* comps=PComponents;
 for(int i=0;i<NumComponents;i++,comps++)
  if(id == (*comps)->Id)
   return UEPtr<UContainer>(comps->get());

 if(!nothrow)
  RDK_THROW(EComponentIdNotExist(id));
 return 0;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
UEPtr<UContainer> UContainer::GetComponent(const NameT &name, bool nothrow) const
{
 return GetComponent(GetComponentId(name,nothrow),nothrow);
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'.
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
UEPtr<UContainer> UContainer::GetComponentL(const ULongId &id, bool nothrow) const
{
 UEPtr<UContainer> comp;

 if(id.GetSize() == 0)
  return 0;

 comp=GetComponent(id[0],nothrow);
 for(int i=1;i<id.GetSize();i++)
  {
   if(!comp)
	return 0;
   comp=comp->GetComponent(id[i],nothrow);
  }
 return comp;
}


// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� ����� 'name'
UEPtr<UContainer> UContainer::GetComponentL(const NameT &name, bool nothrow) const
{
 UEPtr<UContainer> comp;
 NameT::size_type pi,pj;

 pi=name.find_first_of('.');
 if(pi == NameT::npos)
  return GetComponent(name,nothrow);

 comp=GetComponent(name.substr(0,pi),nothrow);
 while(pi != name.size())
  {
   if(!comp)
	return 0;
   pj=pi+1;
   pi=name.find_first_of('.',pj);
   if(pi == NameT::npos)
	pi=name.size();
   comp=comp->GetComponent(name.substr(pj,pi-pj),nothrow);
  }
 return comp;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ����������� ������ � ������ ���������
// ����� ���������� 0, ���� ������ ������� �� ������� �������
UEPtr<UContainer> UContainer::GetComponentByIndex(int index) const
{
 return UEPtr<UContainer>(Components[index].get());
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
 if(comp->GetOwner().get() == this)
  return comp->Id;

 if(comp->GetOwner())
  RDK_THROW(EAddComponentAlreadyHaveOwner(comp->Id));

 if(!CheckComponentType(comp))
  RDK_THROW(EAddComponentHaveInvalidType(comp->Id));

 BeforeAddComponent(comp,pointer);

 NameT namebuffer;

 if(!CheckName(comp->Name))
  comp->Name = GenerateName(comp->Name,namebuffer);
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
  RDK_THROW(EComponentIdAlreadyExist(id));

 comp->SetLogger(Logger.lock());
 comp->Id = id;
 comp->SetOwner(std::shared_ptr<UComponent>(this));

 // ��������� ��������� � ������� ������������ ���������
 SetLookupComponent(comp->Name, comp->Id);

 // ���������� � ���� ���������
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 auto main_owner = MainOwner.lock();
 if(main_owner)
  comp->SetMainOwner(main_owner);

 comp->SetEnvironment(Environment.lock());

 const UEPtr<UIProperty> prop_ts=FindProperty("TimeStep");
 unsigned int time_step_prop_type=prop_ts->GetType();
 if((time_step_prop_type & ptPubParameter) == ptPubParameter)
  comp->ChangeUseIndTimeStepMode(true);
 else
  comp->ChangeUseIndTimeStepMode(false);

 try{
  AAddComponent(comp,pointer);
  comp->SharesInit();
  AfterAddComponent(comp,pointer);
 }
 catch(UException &)
 {
  // �����
  BeforeDelComponent(comp);
  comp->SharesUnInit();
  // ������� ��������� �� ������� ������������ ���������
  DelLookupComponent(comp->Name);

  // �������� �� ���� ���������
  DelComponentTable(comp);

  comp->Owner.reset();
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
/*void UContainer::DelComponent(const UId &id, bool canfree)
{
 UEPtr<UContainer> comp=GetComponent(id);

 DelComponent(comp, canfree);
} */

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void UContainer::DelComponent(const NameT &name, bool canfree)
{
 UEPtr<UContainer> comp=GetComponentL(name,true);
 if(comp && comp->GetOwner())
  comp->GetOwner()->DelComponent(comp,canfree);
}

// ������������� ������� ��� �������� ����������
void UContainer::DelAllComponents(void)
{
 DelAllComponentsRaw();
 UpdateInternalData();
}

void UContainer::DelAllComponentsRaw(void)
{
 while(NumComponents)
  DelComponent(UEPtr<UContainer>(PComponents[NumComponents-1].get()),true);
}

/// ��������� ��������� ��� ����������� ���������� ������� ��� ��� ������ 'classname'
/// � ��� 'name'
void UContainer::AddStaticComponent(const NameT &classname, const NameT &name, UEPtr<UContainer> comp)
{
 comp->SetStaticFlag(true);
 comp->Name = name;
 StaticComponents[std::shared_ptr<UContainer>(comp.Get())]=classname;

 const UEPtr<UIProperty> prop_ts=FindProperty("TimeStep");
 unsigned int time_step_prop_type=prop_ts->GetType();
 if((time_step_prop_type & ptPubParameter) == ptPubParameter)
  comp->ChangeUseIndTimeStepMode(true);
 else
  comp->ChangeUseIndTimeStepMode(false);
}

/// ������� ��������� ��� ����������� ����������
void UContainer::DelStaticComponent(UEPtr<UContainer> comp)
{
 std::map<std::shared_ptr<UContainer>, NameT>::iterator I=StaticComponents.find(std::shared_ptr<UContainer>(comp.Get()));
 if(I != StaticComponents.end())
  StaticComponents.erase(I);
}

/// ������� ��������� ��� ����������� ����������
void UContainer::DelAllStaticComponents(void)
{
 StaticComponents.clear();
}

/// ���������� ��������� �� static ���������
/// � ������� 'classname' � ������ 'name'
UEPtr<UContainer> UContainer::FindStaticComponent(const NameT &classname, const NameT &name) const
{
 std::map<std::shared_ptr<UContainer>, NameT>::const_iterator I=StaticComponents.begin();
 for(;I!=StaticComponents.end();++I)
 {
  if(I->second == classname && I->first->GetName() == name)
   return UEPtr<UContainer>(I->first.get());
 }

 return 0;
}

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
bool UContainer::MoveComponent(UEPtr<UContainer> comp, UEPtr<UContainer> target)
{
 if(!comp || !target)
  return false;

 if(comp->GetOwner().get() != this)
  return false;

 if(target->GetStorage() != GetStorage())
  return false;

 if(!target->CheckComponentType(comp))
  return false;

 comp->MovingFlag=true;
 try
 {
  DelComponent(comp,false);
  if(target->AddComponent(comp) == ForbiddenId)
   return false;
 }
 catch(...)
 {
  comp->MovingFlag=false;
  throw;
 }

 return true;
}

// ���������� ������ ���� � Id ���������, ������������ ���������������
// � ���� �������
// ������ ������ ���� ��������
void UContainer::GetComponentsList(std::vector<UId> &buffer) const
{
 std::shared_ptr<UContainer> *pcomps=PComponents;
 buffer.resize(0);
 buffer.reserve(NumComponents);
 for(int i=0;i<NumComponents;i++,pcomps++)
  buffer.push_back((*pcomps)->Id);
}

void UContainer::GetComponentsList(vector<NameT> &buffer) const
{
 std::shared_ptr<UContainer> *pcomps=PComponents;
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


 std::shared_ptr<UContainer> * pcomponents=0;
 PointerMapCIteratorT I;
 PointerMapIteratorT J;
 if(NumComponents>0)
  pcomponents=&PComponents[0];

 for(int i=0;i<NumComponents;i++,pcomponents++)
  {
   if((*pcomponents)->GetStaticFlag())
    continue;
   bufcomp=(*pcomponents)->Alloc(stor);
   UEPtr<UIPointer> pointer=0;
   I=FindLookupPointer(UEPtr<UContainer>(pcomponents->get()));
   if(I != PointerLookupTable.end())
   {
    J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=UEPtr<UIPointer>(J->second.Pointer.get());
   }

   comp->AddComponent(bufcomp,pointer);
   bufcomp->Id = (*pcomponents)->Id.v;
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

 UEPtr<UContainer> comp=UEPtr<UContainer>(PComponents[index].get());
 if(result>index)
 {
  for(int i=index;i<=result;i++)
   PComponents[i]=PComponents[i+1];
  PComponents[result]=std::shared_ptr<UContainer>(comp.Get());
 }
 else
 {
  for(int i=index;i>result;i--)
   PComponents[i]=PComponents[i-1];
  PComponents[result]=std::shared_ptr<UContainer>(comp.Get());
 }

 return true;
}

bool UContainer::ChangeComponentPosition(const NameT &name, int step)
{
 for(int i=0;i<NumComponents;i++)
  if(PComponents[i]->GetName() == name)
   return ChangeComponentPosition(i,step);

 return false;
}

// ������������� ��������� � ������� �������� index ��� ������ 'name' ��
// �������� �������
// ����������� ��� ��������� ������� ������� ���������
bool UContainer::SetComponentPosition(int index, int new_position)
{
 int step=new_position-index;
 return ChangeComponentPosition(index, step);
}

bool UContainer::SetComponentPosition(const NameT &name, int new_position)
{
 for(int i=0;i<NumComponents;i++)
  if(PComponents[i]->GetName() == name)
   return SetComponentPosition(i,new_position);

 return false;
}

/// ����, ������������ �� ����� ����������� ����������
bool UContainer::IsMoving(void) const
{
 return MovingFlag;
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
  this->GetLongId((ownerlevel)?std::shared_ptr<UContainer>(ownerlevel.Get()):std::shared_ptr<UContainer>(this),id);
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
   cont->GetLongId((ownerlevel)?std::shared_ptr<UContainer>(ownerlevel.Get()):std::shared_ptr<UContainer>(this),id);
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
  this->GetLongId((ownerlevel)?std::shared_ptr<UContainer>(ownerlevel.Get()):std::shared_ptr<UContainer>(this),id);
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
   cont->GetLongId((ownerlevel)?std::shared_ptr<UContainer>(ownerlevel.Get()):std::shared_ptr<UContainer>(this),id);
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
  this->GetLongId((ownerlevel)?std::shared_ptr<UContainer>(ownerlevel.Get()):std::shared_ptr<UContainer>(this),id);
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
   cont->GetLongId((ownerlevel)?std::shared_ptr<UContainer>(ownerlevel.Get()):std::shared_ptr<UContainer>(this),id);
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
  if(J->second.Pointer && J->second.Pointer->Find(std::shared_ptr<const UContainer>(cont.Get()))>=0)
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
  K->second.Pointer->Set(std::shared_ptr<UContainer>(cont.Get()));
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
  if(J->second.Pointer && J->second.Pointer->Find(std::shared_ptr<const UContainer>(cont.Get()))>=0)
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
  K->second.Pointer->Set(std::shared_ptr<UContainer>(cont.Get()));
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
  if(J->second.Id == pointerid && J->second.Pointer && J->second.Pointer->Find(std::shared_ptr<const UContainer>(cont.Get()))>=0)
  {
   J->second.Pointer->Del(std::shared_ptr<UContainer>(cont.Get()));
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
  if(J->first == pointername && J->second.Pointer && J->second.Pointer->Find(std::shared_ptr<const UContainer>(cont.Get()))>=0)
  {
   J->second.Pointer->Del(std::shared_ptr<UContainer>(cont.Get()));
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
  if(J->second.Pointer && J->second.Pointer->Find(std::shared_ptr<const UContainer>(cont.Get()))>=0)
  {
   J->second.Pointer->Del(std::shared_ptr<UContainer>(cont.Get()));
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
  if(J->second.Pointer && J->second.Pointer->Find(std::shared_ptr<const UContainer>(cont.Get()))>=0)
  {
   J->second.Pointer->Del(std::shared_ptr<UContainer>(cont.Get()));
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
  UEPtr<UContainer> cont=UEPtr<UContainer>(J->second.Pointer->Get().get());

  while(cont)
  {
   DelComponent(cont,canfree);
   cont=UEPtr<UContainer>(J->second.Pointer->Get().get());
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
 if(!ShareLookupTable.empty())
 {
  ShareMapIteratorT I=ShareLookupTable.begin();
  ShareMapIteratorT J=ShareLookupTable.end();
  for(;I != J;++I)
   I->second->Init(MainOwner.lock());
 }
 ASharesInit();
}

// ����� ��������������� ����� ����������. ���������� ������������� ��� ��������
// ������� �� ���������
void UContainer::SharesUnInit(void)
{
 ASharesUnInit();
 if(!ShareLookupTable.empty())
 {
  ShareMapIteratorT I=ShareLookupTable.begin();
  ShareMapIteratorT J=ShareLookupTable.end();
  for(;I != J;++I)
   I->second->UnInit();
 }
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UContainer::Default(void)
{
 RDK_SYS_TRY
 {
  try
  {
   BeforeDefault();
   Ready=false;
   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Default();

   // ���� ���������� �������� � ���������, �� ����� ��������� ����������
   // �� ���������
   /*UEPtr<UContainer> original; // ������ ���� - ���������� ���� �� ����������� ��������
   if(Storage)
 original=dynamic_pointer_cast<UContainer>(GetStorage()->GetClass(Class));*/

   SetTimeStep(2000);
   SetMaxCalculationDuration(-1);
   SetCalculationDurationThreshold(-1);
   SetDebugSysEventsMask(0xFFFFFFFF);


   /*if(original && original != this)
   {
	NameT name=Name;
	bool activity=Activity;
	original->CopyProperties(this,ptParameter);
	SetName(name);
	SetActivity(activity);
			}*/

   if(!ADefault())
	return false;
   AfterDefault();
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

/// ����� ������ ���������� �� �������� �� ���������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
bool UContainer::DefaultAll(UContainer* cont, bool subcomps)
{
 if(!cont)
  return false;

 RDK_SYS_TRY
 {
  try
  {
   if(!cont->Default())
	return false;

   bool res=true;
   if(subcomps)
   {
	for(int i=0;i<cont->GetNumComponents();i++)
	 res &= DefaultAll(cont->GetComponentByIndex(i),subcomps);
   }
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
  }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UContainer::Build(void)
{
 if(Ready)
  return true;

 auto storage = Storage.lock();
 if(!storage) // TODO: ����� � ������ ���� ��������� �� ������.
  return true;

 RDK_SYS_TRY
 {
  try
  {
   BeforeBuild();

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Build();

   ABuild();
   Ready=true;
// Reset();

   AfterBuild();
   UpdateComputationOrder();
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
  }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

// ����� �������� �����.
bool UContainer::Reset(void)
{
 RDK_SYS_TRY
 {
  try
  {
   LogDebugSysMessage(RDK_SYS_DEBUG_RESET, RDK_SYS_MESSAGE_ENTER);
   Build();
   SharesInit(); // TODO: �������. ��� ���� � ��������� ����������� (�� ���� �����)
   // ���� �������� �� ������������������. �������� ������� � ����������� �������� ����������� � ���

   // Init(); // ��������
   BeforeReset();

   if(!IsInit())
   {
	LogDebugSysMessage(RDK_SYS_DEBUG_RESET, RDK_SYS_MESSAGE_EXIT_ININIT_FAIL);
	return true; // TODO //false;
   }

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Reset();

   AReset();


   MemoryUsageDiff=0;
   MaxMemoryBlockDiff=0;

   CalcCounter=0;
   SkipComponentCalculation=false;
   ComponentReCalculation=false;
   LastCalcTime=0;
   InterstepsInterval=0;
   StepDuration=0;
   AfterReset();
   LogDebugSysMessage(RDK_SYS_DEBUG_RESET, RDK_SYS_MESSAGE_EXIT_OK);
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
 //  RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return true;
}

// ��������� ������ ����� �������
bool UContainer::Calculate(void)
{
 if(!Activity)
  return true;
 int i=0;
 RDK_SYS_TRY
 {
  try
  {
   Init(); // ��������

   auto owner = Owner.lock();
   if(!owner)
   {
	LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_NEW_CALC_ITERATION);
   }

   LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_ENTER);
   if(!IsInit())
   {
	LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_EXIT_ININIT_FAIL);
	return false;
   }

   unsigned long long tempstepduration=StartCalcTime=GetCurrentStartupTime();
   InterstepsInterval=(LastCalcTime>0)?CalcDiffTime(tempstepduration,LastCalcTime):0;
   LastCalcTime=tempstepduration;

   Build();

//   unsigned long long total_used_memory_before(0);
//   unsigned long long largest_free_block_before(0);

   BeforeCalculate();

   std::shared_ptr<UContainer> *comps=PComponents;
   while((i<NumComponents) && !SkipComponentCalculation)
   {
	if((*comps)->GetStaticFlag())
	{
	 ++i,++comps;
	 continue;
	}
	(*comps)->Calculate();
	if(ComponentReCalculation)
	{
	 ComponentReCalculation=false;
	 std::string temp;
	 LogMessage(RDK_EX_DEBUG, string("Components recaltulation after ")+(*comps)->GetFullName(temp));
	 i=0; comps=PComponents;
	}
	else
	{
	 unsigned long long calc_duration=CalcDiffTime(GetCurrentStartupTime(),StartCalcTime);
	 if((MaxCalculationDuration >= 0) && (calc_duration > ULongTime(MaxCalculationDuration)))
	 {
	  ForceSkipComponentCalculation();
	  std::string temp;
      LogMessage(RDK_EX_WARNING, string("CalcTime[")+sntoa(calc_duration)+std::string("]>MaxCalculationDuration[")+sntoa(MaxCalculationDuration.v)+("] after ")+(*comps)->GetFullName(temp));
     }
	 ++i,++comps;
	}
   }

   SkipComponentCalculation=false;
   ComponentReCalculation=false;

   LogPropertiesBeforeCalc();

   unsigned long long acalc_start_time=GetCurrentStartupTime();
   auto owner2 = Owner.lock();
   if(!owner2)
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
	for(int i=int(TimeStep/OwnerTimeStep);i>=0;--i)
	 ACalculate();
   }
   unsigned long long calc_duration=CalcDiffTime(GetCurrentStartupTime(),acalc_start_time);

   LogPropertiesAfterCalc();

   UpdateMainOwner();
   InterstepsInterval-=StepDuration;


   if((MaxCalculationDuration >= 0) && (calc_duration > ULongTime(MaxCalculationDuration)))
   {
	auto owner3 = Owner.lock(); if(owner3)
	{
	 GetOwner()->ForceSkipComponentCalculation();
	}
    LogMessage(RDK_EX_WARNING, string("ACalculate CalcTime[")+sntoa(calc_duration)+std::string("]>MaxCalculationDuration[")+sntoa(MaxCalculationDuration.v)+"]");
   }

   StepDuration=CalcDiffTime(GetCurrentStartupTime(),tempstepduration);

   if((CalculationDurationThreshold >= 0) && (StepDuration > ULongTime(CalculationDurationThreshold)))
   {
    LogMessageEx(RDK_EX_WARNING, string("Performance warning: StepDuration>")+RDK::sntoa(CalculationDurationThreshold.v)+" ms");
   }

   // ������������ �����������
   size_t numcontrollers=Controllers.size();

   if(numcontrollers>0)
   {
	std::shared_ptr<UController>* controllers=&Controllers[0];
    for(size_t i=0;i<numcontrollers;i++,controllers++)
	{
	 (*controllers)->Update();
	}
   }
   AfterCalculate();
   LogDebugSysMessage(RDK_SYS_DEBUG_CALC, RDK_SYS_MESSAGE_EXIT_OK);
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return true;
}


// ��������� ��������� ������������� ����� �������
void UContainer::Init(void)
{
 if(!Activity)
  return;

 RDK_SYS_TRY
 {
  try
  {
   if(IsInit())
	return;

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->Init();

   AInit();
   InitFlag=true;
   Reset();
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
}

// ��������� ��������������� ����� �������
void UContainer::UnInit(void)
{
 if(!IsInit())
  return;

 RDK_SYS_TRY
 {
  try
  {
   AUnInit();

   for(int i=0;i<NumComponents;i++)
	PComponents[i]->UnInit();
  }
  catch(UException &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   throw;
  }
  catch(std::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   auto logger = Logger.lock();
   if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+exception.what());
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  auto logger = Logger.lock();
  if(logger) logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw system exception: ")+GET_SYSTEM_EXCEPTION_DATA);
//  RDK_THROW(UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 InitFlag=false;
}
// ��������� ��������� MainOwner ����� ������� ����� �������
void UContainer::UpdateMainOwner(void)
{
 auto main_owner2 = MainOwner.lock(); if(!main_owner2)
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

/// ��������� ������� ������������ ������� ����� ����������
/// � ���� ��� ��������� MaxCalculationDuration � MaxCalculationDuration>=0
/// �� ��������� ������ ��������� ������� �������� ���������
bool UContainer::CheckDurationAndSkipComponentCalculation(void)
{
 if((MaxCalculationDuration >= 0) && (CalcDiffTime(GetCurrentStartupTime(),StartCalcTime) > ULongTime(MaxCalculationDuration)))
 {
  GetOwner()->ForceSkipComponentCalculation();
  return true;
 }
 return false;
}

// ������������� ���������� � ��������� ������� �������
void UContainer::UpdateComputationOrder(void)
{

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
                                        const NameT &newname)
{
 UId id;

 std::map<NameT,UId>::iterator I=CompsLookupTable.find(oldname);
 if(I == CompsLookupTable.end())
  RDK_THROW(EComponentNameNotExist(oldname));

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
  RDK_THROW(EComponentNameNotExist(name));
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

 Controllers.push_back(std::shared_ptr<UController>(controller.Get()));
 if(forchilds)
 {
  std::shared_ptr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->AddController(controller,forchilds);
 }
}

// ������� ���������� �� ������
void UContainer::DelController(UEPtr<UController> controller, bool forchilds)
{
 auto I=find(Controllers.begin(),Controllers.end(),std::shared_ptr<UController>(controller.Get()));

 if(I != Controllers.end())
  Controllers.erase(I);

 if(forchilds)
 {
  std::shared_ptr<UContainer>* comps=PComponents;
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
  std::shared_ptr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->DelAllControllers(forchilds);
 }
}

// ���������� ���������� ���� ������������
void UContainer::UnLinkAllControllers(bool forchilds)
{
 while(Controllers.begin() != Controllers.end())
  Controllers.front()->UnLink(forchilds);

 if(forchilds)
 {
  std::shared_ptr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->UnLinkAllControllers(forchilds);
 }
}


// ���������, ���������� �� ���������� � ������
bool UContainer::CheckController(UEPtr<UController> controller) const
{
 if(find(Controllers.begin(),Controllers.end(),std::shared_ptr<UController>(controller.Get())) != Controllers.end())
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
 return UEPtr<UController>(Controllers[index].get());
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
// ��������� ��������� � ������� �����������
// ������ ���������� � ������������� �������
UId UContainer::AddLookupPointer(const NameT &name, UEPtr<UIPointer> pointer)
{
 UPVariable P(1,std::shared_ptr<UIPointer>(pointer.Get()));

 if(PointerLookupTable.find(name) != PointerLookupTable.end())
  RDK_THROW(EPointerNameAlreadyExist(name));

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
  RDK_THROW(EPointerNameNotExist(name));

 // delete I->second.Pointer; // shared_ptr will auto-delete
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
   if(I->second.Pointer && I->second.Pointer->Find(std::shared_ptr<const UContainer>(source.Get()))>=0)
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
 Components.push_back(std::shared_ptr<UContainer>(comp.Get()));
 PComponents=&Components[0];
 NumComponents=int(Components.size());

 if(pointer)
  pointer->Set(std::shared_ptr<UContainer>(comp.Get()));
 else
 {
  PointerMapCIteratorT I=FindLookupPointer(comp);
  if(I != PointerLookupTable.end())
  {
   I->second.Pointer->Del(std::shared_ptr<UContainer>(comp.Get()));
  }
 }
}

void UContainer::DelComponentTable(UEPtr<UContainer> comp)
{
 int i;

 if(NumComponents)
 {
  if(PComponents[NumComponents-1]==std::shared_ptr<UContainer>(comp.Get()))
   Components.resize(NumComponents-1);
  else
  {
   for(i=0;i<NumComponents;i++)
    if(PComponents[i] == std::shared_ptr<UContainer>(comp.Get()))
     break;

   if(i>=NumComponents)
    return;

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(UEPtr<UContainer>));
   Components.resize(NumComponents-1);
  }
 }

 NumComponents=int(Components.size());
 if(NumComponents>0)
  PComponents=&Components[0];
 else
  PComponents=0;

 PointerMapCIteratorT I=FindLookupPointer(comp);
 if(I != PointerLookupTable.end())
 {
  I->second.Pointer->Del(std::shared_ptr<UContainer>(comp.Get()));
 }
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
/// ���������� ����������� �������� �� ���������� ������������ ����������
UId UContainer::UpdateStaticComponent(const NameT &classname, UEPtr<UContainer> comp)
{
 comp->SetLogger(GetLogger());
 comp->SetStorage(GetStorage());
 comp->SetEnvironment(GetEnvironment());
 if(GetStorage())
 {
  comp->SetClass(GetStorage()->FindClassId(classname));

  for(int i=0;i<NumComponents;i++)
   if(PComponents[i] == std::shared_ptr<UContainer>(comp.Get()))
	return PComponents[i]->GetId();
  return AddComponent(comp);
 }
 return ForbiddenId;
}

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

 comp->SetEnvironment(0);

 //if(comp->GetMainOwner() == MainOwner)
 comp->SetMainOwner(0);

 // �������� �� ���� ���������
 // ������� ��������� �� ������� ������������ ���������
 DelLookupComponent(comp->Name);

 // �������� �� ���� ���������
 DelComponentTable(comp);

 comp->Owner.reset();

 AfterDelComponent(comp,canfree);

 if(canfree)
  comp->Free();

 if(!NumComponents)
  LastId=0;
 comp->SetLogger(0);
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
 string result;

 if(OwnerName.size()>0)
 {
  // ������ ��� ���������� � ������� ������������� ����������
  result+=" Component=";
  result+=OwnerName;
  result+=".";
  result+=Name;
 }
 else
 if(Name.size()>0)
 {
  // �������� ��� ���������� � ������� ������������� ����������
  result+=" Component=";
  result+=Name;
 }
/*
 if(MainOwnerName != OwnerName && MainOwnerName.size()>0)
 {
  // ������ ��� �������� ��������� ���������� � ������� ������������� ����������
  result+=" MainOwnerName=";
  result+=MainOwnerName;
 }
  */
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

UContainer::EComponentSystemException::~EComponentSystemException(void) throw()
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


/// ������� �������������� ������ ��� �����������
bool PreparePropertyLogString(const UVariable& variable, unsigned int expected_type, std::string &result)
{
 USerStorageXML xml;
 std::string str_type;
 unsigned int type=variable.GetPropertyType();

 if(!(type & expected_type))
  return false;

 if(type & ptInput)
  str_type="Input ";
 else
 if(type & ptOutput)
  str_type="Output ";
 else
 if(type & ptParameter)
  str_type="Parameter ";
 else
 if(type & ptState)
  str_type="State ";

 if((type & ptInput) && variable.Property->GetName().find("DataInput") != string::npos)
  return false;

 if((type & ptOutput) && variable.Property->GetName().find("DataOutput") != string::npos)
  return false;

 std::string line=str_type+variable.Property->GetName();
 result=line;

 if(type & ptInput && !dynamic_cast<UIPropertyInput*>(variable.Property)->IsConnected())
 {
  result=line+"[<Disconnected>]";
 }
 else
 {
  if(type & ptInput)
  {
   //line+=std::string("[")+variable.Property->GetItemFullName()+std::string(":")+variable.Property->GetItemOutputName()+"]";

  }
  line+=" = ";

  try
  {
   variable.Property->Save(std::shared_ptr<USerStorage>(&xml),true);
   std::string str_data=xml.GetNodeText();
   if(str_data.empty())
   {
	xml.Save(str_data);
	line+="\n";
   }
   result=line+str_data;
  }
  catch(UIProperty::EPropertyZeroPtr &)
  {
   result=line+"[<Disconnected>]";
  }
 }
 return true;
}



}

#endif


