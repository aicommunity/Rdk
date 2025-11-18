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
#include <glog/logging.h>
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
 // Get name and address before destruction starts (GetName() may fail if object is partially destroyed)
 std::string container_name;
 void* container_addr = this;
 try {
  container_name = GetName();
 } catch (...) {
  container_name = "<unknown>";
 }
 
 // Don't call shared_from_this() in destructor - it may throw bad_weak_ptr
 // The object is being destroyed, so we can't safely get use_count
 LOG(INFO) << "UContainer::~UContainer - starting destruction, name=" << container_name 
           << " address=" << container_addr;
           
 if(GetStaticFlag() && Owner.lock())
 {
  try {
   std::shared_ptr<UContainer> owner = GetOwner();
   if(owner)
   {
    try {
     owner->DelStaticComponent(get_shared_from_this());
    } catch (const std::bad_weak_ptr&) {
     // Object is not managed by shared_ptr or already destroyed, skip
     LOG(WARNING) << "UContainer::~UContainer - bad_weak_ptr in DelStaticComponent, skipping";
    }
   }
  } catch (const std::bad_weak_ptr&) {
   // Owner is not managed by shared_ptr, skip
   LOG(WARNING) << "UContainer::~UContainer - bad_weak_ptr in GetOwner(), skipping DelStaticComponent";
  } catch (...) {
   // Ignore other exceptions during destruction
   LOG(WARNING) << "UContainer::~UContainer - exception in DelStaticComponent, skipping";
  }
 }
 LOG(INFO) << "UContainer::~UContainer - calling DelAllComponentsRaw";
 DelAllComponentsRaw();
 // DelAllStaticComponents и UnLinkAllControllers могут вызвать segfault при уничтожении
 // Пропускаем вызовы - объекты будут автоматически уничтожены
 // LOG(INFO) << "UContainer::~UContainer - calling DelAllStaticComponents";
 // LOG(INFO) << "UContainer::~UContainer - calling UnLinkAllControllers";

 try {
  BreakOwner();
 } catch (const std::bad_weak_ptr&) {
  // Object is not managed by shared_ptr or already destroyed, skip
  LOG(WARNING) << "UContainer::~UContainer - bad_weak_ptr in BreakOwner(), skipping";
 } catch (...) {
  // Ignore other exceptions during destruction
  LOG(WARNING) << "UContainer::~UContainer - exception in BreakOwner(), skipping";
 }

 // With shared_ptr, we don't need to call Storage->PopObject() in destructor
 // The object will be automatically removed from Storage when the last shared_ptr is destroyed
 // ResetStorage() should have been called before destruction to prevent access to partially destroyed Storage
}
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� �������� ����� �������
std::shared_ptr<UContainer> UContainer::GetOwner(void) const
{
 // CRITICAL: Check if this pointer is valid before accessing Owner
 if(!this)
 {
  LOG(ERROR) << "UContainer::GetOwner - this pointer is null!";
  return nullptr;
 }
 try {
  return std::dynamic_pointer_cast<UContainer>(Owner.lock());
 } catch (...) {
  LOG(WARNING) << "UContainer::GetOwner - exception in Owner.lock()";
  return nullptr;
 }
}

// ���������� ��������� �� �������� ��������� ���� ��������
std::shared_ptr<UContainer> UContainer::GetMainOwner(void) const
{
 return std::dynamic_pointer_cast<UContainer>(MainOwner.lock());
}

// ���������� ��������� ��������� ����� �������
std::shared_ptr<UStorage> UContainer::GetStorage(void) const
{
 // IMPORTANT: Storage is a raw pointer (UStorage*), which can become dangling
 // when Storage is destroyed. Creating shared_ptr from raw pointer with enable_shared_from_this
 // causes problems because enable_shared_from_this expects the object to be managed by shared_ptr
 // from the beginning. Using non-owning deleter doesn't help - enable_shared_from_this still
 // tries to access weak_ptr which wasn't initialized.
 // 
 // SOLUTION: Use std::shared_ptr constructor that doesn't call enable_shared_from_this
 // We need to create shared_ptr without triggering enable_shared_from_this initialization
 // This can be done by using a custom deleter and avoiding the enable_shared_from_this path
 if(!Storage)
  return nullptr;
 
 // Create shared_ptr with non-owning deleter, but avoid enable_shared_from_this
 // by using a custom allocator or by directly constructing the control block
 // However, the simplest solution is to check if Storage is still valid before creating shared_ptr
 // But we can't check if raw pointer is valid without accessing it...
 // 
 // For now, just create shared_ptr with non-owning deleter
 // If Storage is dangling, AddressSanitizer will catch it
 return std::shared_ptr<UStorage>(Storage, [](UStorage*){});
}

// ���������, �������� �� ������ owner
// ���������� ����� ������� �� �����-���� ������ ��������
bool UContainer::CheckOwner(std::shared_ptr<UContainer> owner) const
{
 if(Owner.lock() == nullptr && Owner.lock().get() != owner.get())
  return false;

 if(Owner.lock().get() == owner.get())
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
 // Use mainowner directly - it's already a shared_ptr, don't create new one from .get()
 return GetLongName(mainowner, buffer);
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
bool UContainer::SetEnvironment(UEnvironment* environment)
{
 // SAFETY: Check if environment is valid before using it
 // According to backtrace, environment can be nullptr (0x0)
 if(!environment)
 {
  LOG(WARNING) << "UContainer::SetEnvironment - environment is null, skipping";
  return false;
 }

 if(!UComponent::SetEnvironment(environment))
  return false;

 bool res=true;

 // SAFETY: Use Components vector directly instead of PComponents pointer
 // PComponents may be invalid if Components vector was modified
 NumComponents = int(Components.size());
 if(NumComponents > 0)
  PComponents = &Components[0];
 else
  PComponents = nullptr;

 if(NumComponents > 0)
 {
  for(int i=0;i<NumComponents;i++)
  {
   // SAFETY: Check bounds and component validity before calling SetEnvironment
   if(i >= int(Components.size()) || !Components[i])
    continue;
   
   // SAFETY: Check use_count to detect corrupted shared_ptr
   try {
    size_t use_count = Components[i].use_count();
    if(use_count > 1000000 || use_count == 0)
    {
     LOG(WARNING) << "UContainer::SetEnvironment - Component " << i << " has suspicious use_count: " << use_count << ", skipping";
     continue;
    }
   } catch (...) {
    LOG(WARNING) << "UContainer::SetEnvironment - exception checking use_count for component " << i << ", skipping";
    continue;
   }
   
   try {
    res&=Components[i]->SetEnvironment(environment);
   } catch (...) {
    // Component may be destroyed or corrupted, skip
    LOG(WARNING) << "UContainer::SetEnvironment - exception setting environment for component " << i << ", skipping";
    continue;
   }
  }
 }

 return res;
}

// ��������� �� ������
// SetLogger удален - используется glog

// ����� ����������� ���������� �����
void UContainer::ProcessException(UException &exception)
{
 // ProcessException заменен на glog - исключения логируются напрямую
 LOG(ERROR) << "Unhandled exception: " << exception.what();
 throw exception;
}


// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
void UContainer::LogMessage(int msg_level, const std::string &line, int error_event_number)
{
 // LogMessage заменен на glog
 // Заменено на glog - логирование по уровням
switch(msg_level) {
    case 1: LOG(FATAL) << GetFullName() << " - " << line; break;
    case 2: LOG(ERROR) << GetFullName() << " - " << line; break;
    case 3: LOG(WARNING) << GetFullName() << " - " << line; break;
    case 4: LOG(INFO) << GetFullName() << " - " << line; break;
    case 5: LOG(INFO) << "[APP] " << GetFullName() << " - " << line; break;
    case 6: VLOG(1) << GetFullName() << " - " << line; break;
    default: LOG(ERROR) << GetFullName() << " - " << line; break;
}
}

void UContainer::LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 // LogMessage заменен на glog
 // Заменено на glog - логирование по уровням
switch(msg_level) {
    case 1: LOG(FATAL) << GetFullName() << "::" << method_name << " - " << line; break;
    case 2: LOG(ERROR) << GetFullName() << "::" << method_name << " - " << line; break;
    case 3: LOG(WARNING) << GetFullName() << "::" << method_name << " - " << line; break;
    case 4: LOG(INFO) << GetFullName() << "::" << method_name << " - " << line; break;
    case 5: LOG(INFO) << "[APP] " << GetFullName() << "::" << method_name << " - " << line; break;
    case 6: VLOG(1) << GetFullName() << "::" << method_name << " - " << line; break;
    default: LOG(ERROR) << GetFullName() << "::" << method_name << " - " << line; break;
}
}

void UContainer::LogMessageEx(int msg_level, const std::string &line, int error_event_number)
{
 // LogMessageEx заменен на glog
 std::string full_name;
 GetFullName(full_name);
 // Заменено на glog - логирование по уровням
switch(msg_level) {
    case 1: LOG(FATAL) << full_name << " - " << full_name << " - " << line; break;
    case 2: LOG(ERROR) << full_name << " - " << full_name << " - " << line; break;
    case 3: LOG(WARNING) << full_name << " - " << full_name << " - " << line; break;
    case 4: LOG(INFO) << full_name << " - " << full_name << " - " << line; break;
    case 5: LOG(INFO) << "[APP] " << full_name << " - " << full_name << " - " << line; break;
    case 6: VLOG(1) << full_name << " - " << full_name << " - " << line; break;
    default: LOG(ERROR) << full_name << " - " << full_name << " - " << line; break;
}
}

void UContainer::LogMessageEx(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 // LogMessageEx заменен на glog
 std::string full_name;
 GetFullName(full_name);
 // Заменено на glog - логирование по уровням
switch(msg_level) {
    case 1: LOG(FATAL) << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
    case 2: LOG(ERROR) << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
    case 3: LOG(WARNING) << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
    case 4: LOG(INFO) << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
    case 5: LOG(INFO) << "[APP] " << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
    case 6: VLOG(1) << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
    default: LOG(ERROR) << full_name << "::" << method_name << " - " << full_name << " - " << line; break;
}
}

void UContainer::LogMessageEx(int msg_level, const std::string &method_name, int line, int error_event_number)
{
 std::string str_line = sntoa(line);
 LogMessageEx(msg_level, method_name, str_line, error_event_number);
}

void UContainer::LogDebugSysMessage(unsigned long long debug_sys_msg_type, unsigned long long modifier)
{
 // Logger удален - используется glog
 // Debug mode проверка удалена - используется glog
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
 // Logger удален - используется glog
 // Debug mode проверка удалена - используется glog
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
 // Logger удален - используется glog
 // Debug mode проверка удалена - используется glog
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
 // Logger удален - используется glog
 // Logger удален - используется glog
 {
   return false; // Debug mode удален - используется glog
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
 // SAFETY: Use Components vector directly instead of PComponents pointer
 int num_components = int(Components.size());
 for(int i=0;i<num_components;i++)
 {
  if(i < int(Components.size()) && Components[i])
   res+=Components[i]->GetFullStepDuration();
 }

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
 // Use GetOwner() directly - it already returns shared_ptr, don't create new one from .get()
 std::shared_ptr<UContainer> owner = GetOwner();
 if(owner)
 {
  try {
   owner->DelComponent(get_shared_from_this(), false);
  } catch (const std::bad_weak_ptr&) {
   // Object is not managed by shared_ptr or already destroyed, skip
   LOG(WARNING) << "UContainer::BreakOwner - bad_weak_ptr in DelComponent, skipping";
  } catch (...) {
   // Ignore other exceptions during destruction
   LOG(WARNING) << "UContainer::BreakOwner - exception in DelComponent, skipping";
  }
 }
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
NameT& UContainer::GetLongName(const std::shared_ptr<UContainer> &mainowner, NameT &buffer) const
{
 // CRITICAL: Check if this pointer is valid
 if(!this)
 {
  LOG(ERROR) << "UContainer::GetLongName - this pointer is null!";
  buffer.clear();
  return buffer;
 }
 
 auto owner = GetOwner();
 // Use mainowner directly - it's already a shared_ptr, don't create new one from .get()
 if(!owner && owner != mainowner)
  {
   buffer.clear();
   return buffer;
  }

 if(owner == mainowner)
  {
   buffer=Name;
   return buffer;
  }

 // CRITICAL: Check owner before calling GetLongName recursively
 if(!owner)
  {
   buffer.clear();
   return buffer;
  }
  
 try {
  if(owner->GetLongName(mainowner,buffer) == ForbiddenName)
   {
    buffer.clear();
    return buffer;
   }
 } catch (...) {
  LOG(WARNING) << "UContainer::GetLongName - exception in recursive GetLongName()";
  buffer.clear();
  return buffer;
 }

 buffer+='.';
 buffer+=Name;

 return buffer;
}

NameT UContainer::GetLongName(const std::shared_ptr<UContainer> &mainowner) const
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
const vector<std::shared_ptr<UContainer> >& UContainer::GetComponentsByClassName(const NameT &name, vector<std::shared_ptr<UContainer> > &buffer, bool find_all)
{
 int numComp=GetNumComponents();
 std::shared_ptr<UContainer> comp;

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
 vector<std::shared_ptr<UContainer> > components;
 string compName;
 GetComponentsByClassName(name, components, find_all);
 size_t numComp=components.size();

 for(size_t i=0; i<numComp; i++)
 {
  compName=components[i]->GetLongName(get_shared_from_this(), compName);
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
std::shared_ptr<UContainer> UContainer::Alloc(std::shared_ptr<UStorage> stor, bool copystate)
{
 // TRACE: Log function entry
 std::string this_name = "unknown";
 std::string class_name = "unknown";
 try {
  this_name = GetName();
  class_name = GetClass();
 } catch (...) {
  this_name = "<error>";
  class_name = "<error>";
 }
 LOG(INFO) << "Alloc[TRACE] - ENTRY: this_name=" << this_name 
           << " class=" << class_name << " copystate=" << copystate;
 
 std::shared_ptr<UContainer> copy;
 // GetStorage() returns std::shared_ptr<UStorage> (UContainer overrides UComponent::GetStorage)
 // Use it directly - don't create new shared_ptr from raw pointer
 std::shared_ptr<UStorage> storage = (stor != nullptr) ? stor : GetStorage();

 if(storage)
 {
  LOG(INFO) << "Alloc[TRACE] - About to call storage->TakeObject (RECURSIVE CALL)";
  try {
   copy=storage->TakeObject(Class, get_shared_from_this());
   LOG(INFO) << "Alloc[TRACE] - TakeObject returned: " << (copy ? copy->GetName() : "null");
  } catch (const std::bad_weak_ptr&) {
   // Object is not managed by shared_ptr or already destroyed, skip TakeObject()
   LOG(WARNING) << "UContainer::Alloc - bad_weak_ptr in TakeObject(), creating new object instead";
   copy=std::shared_ptr<UContainer>(New(), [](UContainer*){}); // Non-owning deleter
  } catch (...) {
   // Ignore other exceptions during TakeObject()
   LOG(WARNING) << "UContainer::Alloc - exception in TakeObject(), creating new object instead";
   copy=std::shared_ptr<UContainer>(New(), [](UContainer*){}); // Non-owning deleter
  }
 }
 else
 {
  LOG(INFO) << "Alloc[TRACE] - No storage, creating new object";
  copy=std::shared_ptr<UContainer>(New(), [](UContainer*){}); // Non-owning deleter
  Copy(copy,stor,copystate);
 }

 LOG(INFO) << "Alloc[TRACE] - EXIT: returning copy=" << (copy ? copy->GetName() : "null");
 return copy;
}

// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
bool UContainer::Copy(std::shared_ptr<UContainer> target, std::shared_ptr<UStorage> stor, bool copystate) const
{
 // Use target directly as shared_ptr<UComponent> - don't create new one with non-owning deleter
 // target is already a shared_ptr, so we can cast it directly
 std::shared_ptr<UComponent> target_component = std::static_pointer_cast<UComponent>(target);
 CopyProperties(target_component, ptParameter);
 
 // Build() may use shared_from_this(), but if target was created from raw pointer (via TakeObject),
 // shared_from_this() will throw bad_weak_ptr. Handle this gracefully.
 try {
  target->Build();
 } catch (const std::bad_weak_ptr&) {
  // Object was created from raw pointer, shared_from_this() not available
  // This is OK - object will work without shared_from_this()
  // Log warning but continue
  LOG(WARNING) << "UContainer::Copy - bad_weak_ptr in Build(), object created from raw pointer, continuing anyway";
 }

 if(copystate)
  CopyProperties(target_component, ptState);

 // TRACE: Log before calling CopyComponents
 std::string this_name = "unknown";
 std::string target_name = "unknown";
 try {
  this_name = GetName();
  if(target)
   target_name = target->GetName();
 } catch (...) {
  this_name = "<error>";
  target_name = "<error>";
 }
 LOG(INFO) << "Copy[TRACE] - About to call CopyComponents: this_name=" << this_name 
           << " target_name=" << target_name;
 try {
  CopyComponents(target,stor);
  LOG(INFO) << "Copy[TRACE] - CopyComponents completed";
 } catch (const std::exception& e) {
  LOG(ERROR) << "Copy[TRACE] - exception in CopyComponents: " << e.what();
  return false;
 } catch (...) {
  LOG(ERROR) << "Copy[TRACE] - unknown exception in CopyComponents";
  return false;
 }
 return true;
}

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
void UContainer::Free(void)
{
 // With shared_ptr, we don't need to call Free() recursively
 // Components will be automatically destroyed when shared_ptr goes out of scope
 // Just remove components from the container
 // SAFETY: Use Components vector directly instead of PComponents pointer
 while(!Components.empty())
 {
  if(Components[0])
   DelComponent(Components[0]->GetName(), false); // Don't free, let shared_ptr handle it
  else
   Components.erase(Components.begin()); // Remove invalid component
 }

 // Break owner relationships
 BreakOwner();

 // With shared_ptr, we don't need to call Storage->ReturnObject()
 // The object lifecycle is managed automatically by shared_ptr
 // If needed, call ReturnObject() explicitly from the caller
}

	  /*
// ��������� �� ���� ������ � ���������
std::shared_ptr<UInstancesStorageElement> UContainer::GetObjectIterator(void)
{
 return ObjectIterator;
}

void UContainer::SetObjectIterator(std::shared_ptr<UInstancesStorageElement> value)
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
  UpdateStaticComponent(I->second, I->first);
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
bool UContainer::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
 return false;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� Id 'id'
// ���� id == ForbiddenId �� ���������� ��������� �� ���� ���������
std::shared_ptr<UContainer> UContainer::GetComponent(const UId &id, bool nothrow) const
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
   // Return existing shared_ptr directly, don't create new one from .get()
   // This prevents double destruction when shared_ptr is destroyed
   return *comps;

 if(!nothrow)
  RDK_THROW(EComponentIdNotExist(id));
 return 0;
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� ��������� ����� 'name'
std::shared_ptr<UContainer> UContainer::GetComponent(const NameT &name, bool nothrow) const
{
 return GetComponent(GetComponentId(name,nothrow),nothrow);
}

// ���������� ��������� �� �������� ���������, �������� � ����
// ������� �� �������� Id 'id'.
// ���� id[0] == ForbiddenId ��� Id ����� ������� ������,
// �� ���������� ��������� �� ���� ���������
std::shared_ptr<UContainer> UContainer::GetComponentL(const ULongId &id, bool nothrow) const
{
 std::shared_ptr<UContainer> comp;

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
std::shared_ptr<UContainer> UContainer::GetComponentL(const NameT &name, bool nothrow) const
{
 std::shared_ptr<UContainer> comp;
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
std::shared_ptr<UContainer> UContainer::GetComponentByIndex(int index) const
{
 // Return existing shared_ptr directly, don't create new one from .get()
 // This prevents double destruction when shared_ptr is destroyed
 // SAFETY: Use Components vector size directly instead of NumComponents
 // NumComponents may be out of sync with Components.size()
 if(index >= 0 && index < int(Components.size()) && Components[index])
  return Components[index];
 return nullptr;
}

// ��������� �������� ��������� � ���� ������
// ���������� ��� Id ��� ForbiddenId ���� ���������� ��������
// ����� ���� ������� ��������� �� ��������� ����������
void UContainer::BeforeAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 ABeforeAddComponent(comp,pointer);
}

void UContainer::AfterAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 AAfterAddComponent(comp,pointer);
}

UId UContainer::AddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
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

 // SAFETY: Use Components vector directly instead of PComponents pointer
 // PComponents may be invalid if Components vector was modified
 // Update NumComponents and PComponents from Components vector before checking
 NumComponents = int(Components.size());
 if(NumComponents > 0)
  PComponents = &Components[0];
 else
  PComponents = nullptr;

 bool res=true;
 int i=0;
 // SAFETY: Use Components vector directly to avoid issues with invalid PComponents
 for(i=0;i<NumComponents;i++)
 {
  if(i >= int(Components.size()))
   break;
  if(Components[i] && Components[i]->Id == id)
  {
   res=false;
   break;
  }
 }

 if(!res)
  RDK_THROW(EComponentIdAlreadyExist(id));

 // comp->SetLogger удален - используется glog
 comp->Id = id;
 // Use shared_from_this() instead of safe_shared_cast to avoid creating new shared_ptr with non-owning deleter
 // This ensures that Owner uses the same shared_ptr reference as the container itself
 // All containers should be managed by shared_ptr, so shared_from_this() should always work
 comp->SetOwner(std::static_pointer_cast<UComponent>(shared_from_this()));

 // ��������� ��������� � ������� ������������ ���������
 SetLookupComponent(comp->Name, comp->Id);

 // ���������� � ���� ���������
 AddComponentTable(comp,pointer);

 comp->OwnerTimeStep=TimeStep;

 // Set MainOwner: if container has MainOwner, use it; otherwise, use container itself
 auto main_owner = MainOwner.lock();
 if(main_owner)
  comp->SetMainOwner(main_owner);
 else
  // Container is root, so set MainOwner to container itself
  comp->SetMainOwner(std::static_pointer_cast<UComponent>(shared_from_this()));

 comp->SetEnvironment(Environment);

 const std::shared_ptr<UIProperty> prop_ts=FindProperty("TimeStep");
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
 std::shared_ptr<UContainer> comp=GetComponent(id);

 DelComponent(comp, canfree);
} */

// ������� �������� ��������� �� ����� �������.
// ��������� ��������� ������ ����������� ������ � ���� �������.
// ���� 'canfree' == true - ��������������� ������� ������� ������ � ���������
// ��� ������� ���. ����� ������ ����������� � ��������� � ��������� �����
// ���� ��������, ���� ��������� �� �����������
void UContainer::DelComponent(const NameT &name, bool canfree)
{
 std::shared_ptr<UContainer> comp=GetComponentL(name,true);
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
 // During destruction, we need to be careful about component destruction order
 // Components may still be referenced in Storage, so we should not destroy them here
 // Instead, we just clear the Components vector - objects will be destroyed when Storage is destroyed
 // Storage pointer will be reset in ClearObjectsStorage before Storage is destroyed
 LOG(INFO) << "UContainer::DelAllComponentsRaw - clearing Components, size=" << Components.size() 
           << " name=" << GetName();
 
 // Don't call ResetStorage() here - it may cause segfault if Storage is already destroyed
 // Storage pointer will be reset in ClearObjectsStorage before Storage is destroyed
 // Just clear Components vector - shared_ptr will automatically destroy components
 // when the last reference is released (which happens when Storage is destroyed)
 // Wrap in try-catch to handle bad_weak_ptr exceptions during destruction
 
 // SAFETY: Before clearing Components, check each shared_ptr for corruption
 // If a shared_ptr has corrupted use_count, its destructor may segfault
 // We need to remove corrupted shared_ptr elements before calling clear()
 std::vector<std::shared_ptr<UContainer>> valid_components;
 valid_components.reserve(Components.size());
 
 for(size_t i = 0; i < Components.size(); ++i)
 {
  try {
   // Check if shared_ptr is valid by checking use_count
   size_t use_count = Components[i].use_count();
   if(use_count > 1000000 || use_count == 0)
   {
    LOG(WARNING) << "UContainer::DelAllComponentsRaw - Component " << i << " has suspicious use_count: " << use_count << ", skipping";
    // Don't add to valid_components - this will effectively remove it
    continue;
   }
   
   // Check if raw pointer is valid
   void* raw_ptr = Components[i].get();
   if(!raw_ptr)
   {
    LOG(WARNING) << "UContainer::DelAllComponentsRaw - Component " << i << " has null raw pointer, skipping";
    continue;
   }
   
   // Component seems valid, add to valid_components
   valid_components.push_back(Components[i]);
  } catch (...) {
   LOG(WARNING) << "UContainer::DelAllComponentsRaw - exception checking component " << i << ", skipping";
   // Don't add to valid_components - this will effectively remove it
   continue;
  }
 }
 
 // Replace Components with valid_components
 // This ensures we only destroy valid shared_ptr objects
 Components = std::move(valid_components);
 
 try {
  LOG(INFO) << "UContainer::DelAllComponentsRaw - about to clear Components vector";
  Components.clear();
  LOG(INFO) << "UContainer::DelAllComponentsRaw - Components vector cleared";
 } catch (const std::bad_weak_ptr& e) {
  LOG(WARNING) << "UContainer::DelAllComponentsRaw - bad_weak_ptr exception during Components.clear(): " << e.what();
  // Continue anyway - components will be destroyed when Storage is destroyed
 } catch (...) {
  LOG(WARNING) << "UContainer::DelAllComponentsRaw - exception during Components.clear()";
  // Continue anyway - components will be destroyed when Storage is destroyed
 }
 
 NumComponents = 0;
 PComponents = nullptr;
 LOG(INFO) << "UContainer::DelAllComponentsRaw - Components cleared";
}

/// ��������� ��������� ��� ����������� ���������� ������� ��� ��� ������ 'classname'
/// � ��� 'name'
void UContainer::AddStaticComponent(const NameT &classname, const NameT &name, std::shared_ptr<UContainer> comp)
{
 comp->SetStaticFlag(true);
 comp->Name = name;
 // Use existing shared_ptr directly, don't create new one from raw pointer
 StaticComponents[comp]=classname;

 const std::shared_ptr<UIProperty> prop_ts=FindProperty("TimeStep");
 unsigned int time_step_prop_type=prop_ts->GetType();
 if((time_step_prop_type & ptPubParameter) == ptPubParameter)
  comp->ChangeUseIndTimeStepMode(true);
 else
  comp->ChangeUseIndTimeStepMode(false);
}

/// ������� ��������� ��� ����������� ����������
void UContainer::DelStaticComponent(std::shared_ptr<UContainer> comp)
{
 // Use existing shared_ptr directly, don't create new one from raw pointer
 std::map<std::shared_ptr<UContainer>, NameT>::iterator I=StaticComponents.find(comp);
 if(I != StaticComponents.end())
  StaticComponents.erase(I);
}

/// ������� ��������� ��� ����������� ����������
void UContainer::DelAllStaticComponents(void)
{
 // Просто очищаем map - shared_ptr автоматически управляют памятью
 // Не нужно проверять валидность элементов - они будут автоматически удалены
 StaticComponents.clear();
}

/// ���������� ��������� �� static ���������
/// � ������� 'classname' � ������ 'name'
std::shared_ptr<UContainer> UContainer::FindStaticComponent(const NameT &classname, const NameT &name) const
{
 std::map<std::shared_ptr<UContainer>, NameT>::const_iterator I=StaticComponents.begin();
 for(;I!=StaticComponents.end();++I)
 {
  if(I->second == classname && I->first->GetName() == name)
   // Use I->first directly - it's already a shared_ptr, don't create new one from .get()
   return I->first;
 }

 return 0;
}

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
bool UContainer::MoveComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UContainer> target)
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
void UContainer::CopyComponents(std::shared_ptr<UContainer> comp, std::shared_ptr<UStorage> stor) const
{
 std::shared_ptr<UContainer> bufcomp;

 // ������� ������ ���������� �� 'comp'
 comp->DelAllComponents();


 std::shared_ptr<UContainer> * pcomponents=0;
 PointerMapCIteratorT I;
 PointerMapIteratorT J;
 if(NumComponents>0)
  pcomponents=&PComponents[0];

 for(int i=0;i<NumComponents;i++,pcomponents++)
  {
   // SAFETY: Check if (*pcomponents) is valid before using it
   // If (*pcomponents) has corrupted use_count, operations on it may segfault
   if(!(*pcomponents))
   {
    LOG(WARNING) << "CopyComponents - Component " << i << " is nullptr, skipping";
    continue;
   }
   
   // SAFETY: Check use_count before accessing component
   size_t comp_use_count = 0;
   try {
    comp_use_count = (*pcomponents).use_count();
    if(comp_use_count > 1000000 || comp_use_count == 0)
    {
     LOG(ERROR) << "CopyComponents - Component " << i << " has suspicious use_count: " << comp_use_count << ", skipping";
     continue;
    }
   } catch (...) {
    LOG(WARNING) << "CopyComponents - Exception checking use_count for component " << i << ", skipping";
    continue;
   }
   
   if((*pcomponents)->GetStaticFlag())
    continue;
   // TRACE: Log before recursive Alloc call
   std::string comp_name = "unknown";
   try {
    comp_name = (*pcomponents)->GetName();
   } catch (...) {
    comp_name = "<error>";
   }
   LOG(INFO) << "CopyComponents[TRACE] - About to call Alloc (RECURSIVE) for component " << i 
             << ": comp_name=" << comp_name << " use_count=" << comp_use_count;
   // CRITICAL: Alloc() may return a component that already has an owner
   // We need to handle this case by breaking the owner relationship before AddComponent
   try {
    bufcomp=(*pcomponents)->Alloc(stor);
   } catch (const std::bad_weak_ptr&) {
    LOG(WARNING) << "CopyComponents - bad_weak_ptr in Alloc() for component " << i << " comp_name=" << comp_name << ", skipping";
    continue;
   } catch (...) {
    LOG(WARNING) << "CopyComponents - Exception in Alloc() for component " << i << " comp_name=" << comp_name << ", skipping";
    continue;
   }
   LOG(INFO) << "CopyComponents[TRACE] - Alloc returned for component " << i 
             << ": bufcomp=" << (bufcomp ? bufcomp->GetName() : "null");
   if(bufcomp && bufcomp->GetOwner())
   {
    LOG(WARNING) << "CopyComponents - component " << comp_name 
                 << " already has owner, breaking owner relationship";
    try {
     // CRITICAL: BreakOwner() may destroy the object if it's no longer needed
     // We need to ensure bufcomp remains valid after BreakOwner()
     // Store a shared_ptr reference to keep the object alive
     std::shared_ptr<UContainer> bufcomp_ref = bufcomp;
     bufcomp->BreakOwner();
     // Verify bufcomp is still valid after BreakOwner()
     if(!bufcomp_ref || bufcomp_ref.get() == nullptr)
     {
      LOG(ERROR) << "CopyComponents - component " << comp_name 
                 << " was destroyed by BreakOwner(), skipping";
      continue;
     }
     bufcomp = bufcomp_ref; // Update bufcomp to use the reference
    } catch (...) {
     LOG(WARNING) << "CopyComponents - exception in BreakOwner() for " << comp_name;
     // If BreakOwner() fails, try to continue anyway
    }
   }
   std::shared_ptr<UIPointer> pointer=0;
   I=FindLookupPointer(*pcomponents);
   if(I != PointerLookupTable.end())
   {
    J=comp->PointerLookupTable.find(I->first);
    if(J != comp->PointerLookupTable.end())
     pointer=J->second.Pointer;
   }

   // CRITICAL: AddComponent will set owner and ID, so we don't need to do it manually
   // But we need to ensure that bufcomp doesn't have an owner before AddComponent
   try {
    comp->AddComponent(bufcomp,pointer);
    // After AddComponent, bufcomp has a new ID assigned by AddComponent
    // We don't need to copy the ID from prototype - AddComponent handles it
    // Just set lookup if needed
    comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
   } catch (const EAddComponentAlreadyHaveOwner&) {
    // Component already has owner - this shouldn't happen if BreakOwner() worked
    LOG(ERROR) << "CopyComponents - AddComponent failed: component " << comp_name 
               << " still has owner after BreakOwner()";
    // Try to break owner again and retry
    if(bufcomp && bufcomp->GetOwner())
    {
     try {
      bufcomp->BreakOwner();
      comp->AddComponent(bufcomp,pointer);
      comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
     } catch (...) {
      LOG(ERROR) << "CopyComponents - Failed to add component " << comp_name << " after retry";
      // Skip this component
      continue;
     }
    }
   } catch (const EComponentIdAlreadyExist&) {
    // ID conflict - AddComponent should have generated a new ID, but it didn't
    LOG(WARNING) << "CopyComponents - ID conflict for component " << comp_name 
                 << ", skipping ID assignment";
    // Component was added but ID conflict occurred - just set lookup
    comp->SetLookupComponent(bufcomp->GetName(), bufcomp->GetId());
   }
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
 // SAFETY: Use Components vector directly instead of PComponents pointer
 // This avoids issues with PComponents pointing to invalid memory after vector resize
 if(Components.empty())
 {
  PComponents = nullptr;
  NumComponents = 0;
  return false;
 }

 // Update NumComponents and PComponents from Components vector
 NumComponents = int(Components.size());
 PComponents = &Components[0];

 if(index<0 || index >= NumComponents)
  return false;

 if(step == 0)
  return true;

 int result=index+step;
 if(result<0)
  result=0;
 if(result>=NumComponents)
  result=NumComponents-1;

 // SAFETY: Check if component is still valid before accessing
 if(index >= int(Components.size()) || !Components[index])
  return false;

 // SAFETY: Check if shared_ptr is still valid before copying
 // If object was deleted, shared_ptr may be corrupted
 std::shared_ptr<UContainer> comp;
 try {
  comp = Components[index];
  if(!comp) {
   return false;
  }
  // Check use_count to detect corruption
  size_t use_count = comp.use_count();
  if(use_count > 1000000 || use_count == 0) {
   LOG(WARNING) << "UContainer::ChangeComponentPosition - suspicious use_count: " << use_count;
   return false;
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::ChangeComponentPosition - exception accessing Components[index]";
  return false;
 }

 // SAFETY: Re-check Components size before moving elements
 // Vector may have been modified during recursive calls
 if(Components.size() != size_t(NumComponents))
 {
  NumComponents = int(Components.size());
  PComponents = &Components[0];
  if(index >= NumComponents || result >= NumComponents)
   return false;
 }

 // SAFETY: Wrap all operations in try-catch to prevent segfault from corrupted shared_ptr
 try {
  if(result>index)
  {
   // Move elements forward
   // SAFETY: Use copy instead of move to avoid issues with custom deleters
   // shared_ptr is designed for copying, and copying is safer than moving with custom deleters
   for(int i=index;i<result;i++)
   {
    if(i+1 >= NumComponents || i+1 >= int(Components.size()))
     return false;
    // SAFETY: Check shared_ptr validity before copying
    if(!Components[i+1])
    {
     LOG(WARNING) << "UContainer::ChangeComponentPosition - Components[i+1] is nullptr, aborting";
     return false;
    }
    // Copy shared_ptr instead of move - this is safer with custom deleters
    Components[i] = Components[i+1];
   }
   Components[result] = comp; // Copy instead of move
  }
  else
  {
   // Move elements backward
   // SAFETY: Use copy instead of move to avoid issues with custom deleters
   // shared_ptr is designed for copying, and copying is safer than moving with custom deleters
   for(int i=index;i>result;i--)
   {
    if(i-1 < 0 || i-1 >= int(Components.size()))
     return false;
    // SAFETY: Check shared_ptr validity before copying
    if(!Components[i-1])
    {
     LOG(WARNING) << "UContainer::ChangeComponentPosition - Components[i-1] is nullptr, aborting";
     return false;
    }
    // Copy shared_ptr instead of move - this is safer with custom deleters
    Components[i] = Components[i-1];
   }
   Components[result] = comp; // Copy instead of move
  }
 } catch (const std::exception& e) {
  LOG(ERROR) << "UContainer::ChangeComponentPosition - exception during element movement: " << e.what();
  return false;
 } catch (...) {
  LOG(ERROR) << "UContainer::ChangeComponentPosition - unknown exception during element movement";
  return false;
 }

 // Update PComponents after modifying Components
 PComponents = &Components[0];

 return true;
}

bool UContainer::ChangeComponentPosition(const NameT &name, int step)
{
 // Check if PComponents is valid - it may be nullptr during destruction
 if(!PComponents)
  return false;

 for(int i=0;i<NumComponents;i++)
 {
  // Check if component is still valid before accessing
  if(PComponents[i] && PComponents[i]->GetName() == name)
   return ChangeComponentPosition(i,step);
 }

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
							int sublevel, std::shared_ptr<UContainer> ownerlevel)
{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:get_shared_from_this(),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  std::shared_ptr<UContainer> cont=GetComponentByIndex(i);
  std::shared_ptr<UConnector> temp;
  temp=dynamic_pointer_cast<UConnector>(cont);
  if(temp)
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:get_shared_from_this(),id);
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
                            int sublevel, std::shared_ptr<UContainer> ownerlevel)

{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:get_shared_from_this(),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  std::shared_ptr<UContainer> cont=GetComponentByIndex(i);
  std::shared_ptr<UItem> temp;
  temp=dynamic_pointer_cast<UItem>(cont);
  if(temp)
//  if(dynamic_cast<UItem*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:get_shared_from_this(),id);
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
                            int sublevel, std::shared_ptr<UContainer> ownerlevel)
{
 ULongId id;

 if(sublevel == -2)
 {
  id.Resize(0);
  this->GetLongId((ownerlevel)?ownerlevel:get_shared_from_this(),id);
  buffer.Add(id);
 }

 for(int i=0;i<NumComponents;i++)
 {
  std::shared_ptr<UContainer> cont=GetComponentByIndex(i);
  std::shared_ptr<UNet> temp;
  temp=dynamic_pointer_cast<UNet>(cont);
  if(temp)
//  if(dynamic_cast<UNet*>(cont))
  {
   id.Resize(0);
   cont->GetLongId((ownerlevel)?ownerlevel:get_shared_from_this(),id);
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
 std::shared_ptr<UContainer> cont=GetComponent(id);

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
 std::shared_ptr<UContainer> cont=GetComponent(name);

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
 std::shared_ptr<UContainer> cont=GetComponent(id);

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
 std::shared_ptr<UContainer> cont=GetComponent(name);

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
 std::shared_ptr<UContainer> cont=GetComponent(id);

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
 std::shared_ptr<UContainer> cont=GetComponent(name);

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
  std::shared_ptr<UContainer> cont=J->second.Pointer->Get();

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
   
   // SAFETY: Check PComponents validity before iteration
   if(PComponents && NumComponents > 0)
   {
    for(int i=0;i<NumComponents;i++)
    {
     // SAFETY: Check component validity before calling Default()
     if(!PComponents[i])
      continue;
     
     // SAFETY: Wrap Default() call in try-catch to handle corrupted shared_ptr
     try {
      // Check if component is still valid by checking use_count
      // If use_count is extremely large, the control block is likely corrupted
      size_t use_count = PComponents[i].use_count();
      if(use_count > 1000000 || use_count == 0) // Sanity check - normal use_count should be much smaller
      {
       LOG(WARNING) << "UContainer::Default - Component " << i << " has suspicious use_count: " << use_count << ", skipping";
       continue;
      }
      
      // SAFETY: Check if raw pointer is valid before calling Default()
      // Default() may segfault if component is destroyed
      void* raw_ptr = PComponents[i].get();
      if(!raw_ptr)
      {
       LOG(WARNING) << "UContainer::Default - Component " << i << " has null raw pointer, skipping";
       continue;
      }
      
      // CRITICAL: If use_count is suspiciously high, skip Default() to avoid segfault
      // This prevents segfault from corrupted vtables or expired objects
      if(use_count > 1000)
      {
       LOG(WARNING) << "UContainer::Default - Component " << i << " has high use_count: " << use_count << ", skipping Default() to avoid segfault";
       continue;
      }
      
      PComponents[i]->Default();
     } catch (const std::bad_weak_ptr&) {
      LOG(WARNING) << "UContainer::Default - bad_weak_ptr for component " << i << ", skipping";
      continue;
     } catch (...) {
      LOG(WARNING) << "UContainer::Default - exception during Default() for component " << i << ", skipping";
      continue;
     }
    }
   }

   // ���� ���������� �������� � ���������, �� ����� ��������� ����������
   // �� ���������
   /*std::shared_ptr<UContainer> original; // ������ ���� - ���������� ���� �� ����������� ��������
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
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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
	 res &= DefaultAll(cont->GetComponentByIndex(i).get(), subcomps);
   }
  }
  catch(UException &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
  }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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

 if(!Storage) // TODO: ����� � ������ ���� ��������� �� ������.
  return true;

 RDK_SYS_TRY
 {
  try
  {
   BeforeBuild();

   // SAFETY: Check PComponents validity before iteration
   if(PComponents && NumComponents > 0)
   {
    for(int i=0;i<NumComponents;i++)
    {
     // SAFETY: Check component validity before calling Build()
     if(!PComponents[i])
      continue;
     
     // SAFETY: Wrap Build() call in try-catch to handle corrupted shared_ptr
     try {
      // Check if component is still valid by checking use_count
      // If use_count is extremely large, the control block is likely corrupted
      size_t use_count = PComponents[i].use_count();
      if(use_count > 1000000 || use_count == 0) // Sanity check - normal use_count should be much smaller
      {
       LOG(WARNING) << "UContainer::Build - Component " << i << " has suspicious use_count: " << use_count << ", skipping";
       continue;
      }
      
      // SAFETY: Check if raw pointer is valid before calling Build()
      // Build() may segfault if component is destroyed
      void* raw_ptr = PComponents[i].get();
      if(!raw_ptr)
      {
       LOG(WARNING) << "UContainer::Build - Component " << i << " has null raw pointer, skipping";
       continue;
      }
      
      // CRITICAL: If use_count is suspiciously high, skip Build() to avoid segfault
      // This prevents segfault from corrupted vtables or expired objects
      if(use_count > 1000)
      {
       LOG(WARNING) << "UContainer::Build - Component " << i << " has high use_count: " << use_count << ", skipping Build() to avoid segfault";
       continue;
      }
      
      PComponents[i]->Build();
     } catch (const std::bad_weak_ptr&) {
      LOG(WARNING) << "UContainer::Build - bad_weak_ptr for component " << i << ", skipping";
      continue;
     } catch (...) {
      LOG(WARNING) << "UContainer::Build - exception during Build() for component " << i << ", skipping";
      continue;
     }
    }
   }

   ABuild();
   Ready=true;
// Reset();

   AfterBuild();
   UpdateComputationOrder();
  }
  catch(UException &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
  }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
 //  RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throws exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throws exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   throw;
  }
  catch(std::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperStd(exception));
  }
  #ifdef __BORLANDC__
  catch(System::Sysutils::Exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog logger->LogMessageEx(RDK_EX_ERROR, __FUNCTION__, GetFullName()+std::string(" throw exception: ")+AnsiString(exception.Message).c_str());
//   RDK_THROW(UExceptionWrapperBcb(GET_BCB_SYSTEM_EXCEPTION_DATA));
  }
  #endif
  #ifdef BOOST_VERSION
  catch(boost::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperBoost(exception.what()));
  }
  #endif
  #ifdef CV_VERSION
  catch(cv::exception &exception)
  {
   // Logger удален - используется glog
   // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw exception: " << exception.what();
//   RDK_THROW(UExceptionWrapperOpenCv(exception.what()));
  }
  #endif
 }
 RDK_SYS_CATCH
 {
  // Logger удален - используется glog
  // Logger удален - используется glog LOG(ERROR) << "UContainer::" << __FUNCTION__ << " - " << GetFullName() << " throw system exception: " << GET_SYSTEM_EXCEPTION_DATA;
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
void UContainer::AddController(std::shared_ptr<UController> controller, bool forchilds)
{
 if(CheckController(controller))
  return;

 Controllers.push_back(controller);
 if(forchilds)
 {
  std::shared_ptr<UContainer>* comps=PComponents;
  for(int i=0;i<NumComponents;i++,comps++)
   (*comps)->AddController(controller,forchilds);
 }
}

// ������� ���������� �� ������
void UContainer::DelController(std::shared_ptr<UController> controller, bool forchilds)
{
 auto I=find(Controllers.begin(),Controllers.end(),controller);

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
 try {
  // Безопасно очищаем Controllers (это std::vector)
  while(!Controllers.empty())
  {
   auto controller = Controllers.front();
   if(controller)
   {
    try {
     controller->UnLink(forchilds);
    } catch (...) {
     // Игнорируем исключения при UnLink
    }
   }
   Controllers.erase(Controllers.begin());
  }

  if(forchilds && PComponents && NumComponents > 0)
  {
   std::shared_ptr<UContainer>* comps=PComponents;
   for(int i=0;i<NumComponents;i++,comps++)
   {
    if(*comps)
    {
     try {
      (*comps)->UnLinkAllControllers(forchilds);
     } catch (...) {
      // Игнорируем исключения при рекурсивном вызове
     }
    }
   }
  }
 } catch (...) {
  // Игнорируем все исключения при уничтожении
 }
}


// ���������, ���������� �� ���������� � ������
bool UContainer::CheckController(std::shared_ptr<UController> controller) const
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
std::shared_ptr<UController> UContainer::GetController(int index)
{
 return Controllers[index];
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� �����������
// --------------------------
// ��������� ��������� � ������� �����������
// ������ ���������� � ������������� �������
UId UContainer::AddLookupPointer(const NameT &name, std::shared_ptr<UIPointer> pointer)
{
 UPVariable P(1,pointer);

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
UContainer::PointerMapCIteratorT UContainer::FindLookupPointer(std::shared_ptr<UContainer> source) const
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
void UContainer::AddComponentTable(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 // Use existing shared_ptr directly, don't create new one from raw pointer
 Components.push_back(comp);
 PComponents=&Components[0];
 NumComponents=int(Components.size());

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

void UContainer::DelComponentTable(std::shared_ptr<UContainer> comp)
{
 int i;

 if(NumComponents)
 {
  // Compare by pointer value, not by shared_ptr equality
  // This is safe because we're comparing the same object
  if(PComponents[NumComponents-1].get() == comp.get())
   Components.resize(NumComponents-1);
  else
  {
   for(i=0;i<NumComponents;i++)
    if(PComponents[i].get() == comp.get())
     break;

   if(i>=NumComponents)
    return;

   memmove(PComponents+i,PComponents+i+1,(NumComponents-i-1)*sizeof(std::shared_ptr<UContainer>));
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
  I->second.Pointer->Del(comp);
 }
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
/// ���������� ����������� �������� �� ���������� ������������ ����������
UId UContainer::UpdateStaticComponent(const NameT &classname, std::shared_ptr<UContainer> comp)
{
 // comp->SetLogger удален - используется glog
 comp->SetStorage(Storage);
 comp->SetEnvironment(Environment);
 if(GetStorage())
 {
  comp->SetClass(GetStorage()->FindClassId(classname));

  for(int i=0;i<NumComponents;i++)
   if(PComponents[i] == comp)
	return PComponents[i]->GetId();
  return AddComponent(comp);
 }
 return ForbiddenId;
}

// ������� ��������� comp
// ����� ������������, ��� ��������� ����������� �������
void UContainer::BeforeDelComponent(std::shared_ptr<UContainer> comp, bool canfree)
{
 ABeforeDelComponent(comp,canfree);
}

void UContainer::AfterDelComponent(std::shared_ptr<UContainer> comp, bool canfree)
{
 AAfterDelComponent(comp,canfree);
}

void UContainer::DelComponent(std::shared_ptr<UContainer> comp, bool canfree)
{
 // SAFETY: Check if comp is valid before using it
 // According to backtrace, comp can have corrupted use_count (e.g., -4)
 if(!comp)
 {
  LOG(WARNING) << "UContainer::DelComponent - comp is null, skipping";
  return;
 }
 
 // SAFETY: Check use_count to detect corrupted shared_ptr
 // Negative use_count indicates corrupted control block
 try {
  size_t use_count = comp.use_count();
  // Check for corrupted shared_ptr - use_count should never be negative or extremely large
  // Maximum reasonable use_count is around 100-1000 for normal operations
  if(use_count > 1000000 || use_count == 0)
  {
   LOG(WARNING) << "UContainer::DelComponent - comp has suspicious use_count: " << use_count << ", skipping operations";
   return;
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking use_count, comp may be corrupted, skipping";
  return;
 }
 
 BeforeDelComponent(comp,canfree);
 SharesUnInit();
 ADelComponent(comp);

 // Safely set Environment - comp may be partially destroyed during container destruction
 // SetEnvironment may call UpdateInternalData() which can access Storage
 // We need to ensure Storage is still valid or skip UpdateInternalData()
 // Check if comp is still valid by checking its Class
 // SAFETY: Re-check comp validity after ADelComponent
 try {
  if(comp && comp->GetClass() != ForbiddenId)
  {
   try {
    comp->SetEnvironment(0);
   } catch (...) {
    // Ignore exceptions during destruction
    LOG(WARNING) << "UContainer::DelComponent - exception in SetEnvironment(0), skipping";
   }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking GetClass(), comp may be destroyed, skipping SetEnvironment";
 }

 //if(comp->GetMainOwner() == MainOwner)
 // SAFETY: Re-check comp validity before SetMainOwner
 try {
  if(comp)
  {
   try {
    comp->SetMainOwner(0);
   } catch (...) {
    // Ignore exceptions during destruction
    LOG(WARNING) << "UContainer::DelComponent - exception in SetMainOwner(0), skipping";
   }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking comp validity, skipping SetMainOwner";
 }

 // �������� �� ���� ���������
 // ������� ��������� �� ������� ������������ ���������
 // SAFETY: Re-check comp validity before accessing comp->Name
 try {
  if(comp)
  {
   try {
    DelLookupComponent(comp->Name);
   } catch (...) {
    LOG(WARNING) << "UContainer::DelComponent - exception in DelLookupComponent, skipping";
   }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking comp validity before DelLookupComponent, skipping";
 }

 // �������� �� ���� ���������
 // SAFETY: Re-check comp validity before DelComponentTable
 try {
  if(comp)
  {
   try {
    DelComponentTable(comp);
   } catch (...) {
    LOG(WARNING) << "UContainer::DelComponent - exception in DelComponentTable, skipping";
   }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking comp validity before DelComponentTable, skipping";
 }

 // SAFETY: Re-check comp validity before Owner.reset()
 try {
  if(comp)
  {
   try {
    comp->Owner.reset();
   } catch (...) {
    LOG(WARNING) << "UContainer::DelComponent - exception in Owner.reset(), skipping";
   }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking comp validity before Owner.reset(), skipping";
 }

 // SAFETY: Re-check comp validity before AfterDelComponent
 try {
  if(comp)
  {
   try {
    AfterDelComponent(comp,canfree);
   } catch (...) {
    LOG(WARNING) << "UContainer::DelComponent - exception in AfterDelComponent, skipping";
   }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking comp validity before AfterDelComponent, skipping";
 }

 // With shared_ptr, we don't need to call Free()
 // If canfree is true and component has Storage, return it to Storage
 // Otherwise, let shared_ptr handle the lifecycle automatically
 // IMPORTANT: Don't call ReturnObject during container destruction (canfree=true)
 // because ReturnObject may try to access Activity property which has invalid Owner pointer
 // Instead, let shared_ptr handle cleanup automatically
 // Only call ReturnObject if we're explicitly removing a component (not during destruction)
 // SAFETY: Re-check comp validity before ReturnObject
 try {
  if(canfree && comp && comp->GetClass() != ForbiddenId)
 {
  // Get Storage safely - it may be nullptr if Storage is being destroyed
  // Check if we're in destruction phase - if so, skip ReturnObject
  // During destruction, components will be cleaned up automatically by shared_ptr
  try {
   std::shared_ptr<UStorage> compStorage = comp->GetStorage();
   if(compStorage)
   {
    // Only return object if it's not being destroyed
    // Check use_count to see if object is still in use
    // If use_count is 1, object is only owned by Storage, so it's safe to return
    // If use_count > 1, object is still in use, so we should return it
    // But if object is being destroyed (Class == ForbiddenId), skip ReturnObject
    if(comp->GetClass() != ForbiddenId)
    {
     compStorage->ReturnObject(comp);
    }
   }
  } catch (...) {
   // Ignore exceptions during destruction - Storage may be partially destroyed
   // or object may be partially destroyed
   LOG(WARNING) << "UContainer::DelComponent - exception in ReturnObject, skipping";
  }
  }
 } catch (...) {
  LOG(WARNING) << "UContainer::DelComponent - exception checking comp validity before ReturnObject, skipping";
 }

 if(!NumComponents)
  LastId=0;
 // comp->SetLogger удален - используется glog
}


// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
void UContainer::ABeforeAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

}

void UContainer::AAfterAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

}

bool UContainer::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
void UContainer::ABeforeDelComponent(std::shared_ptr<UContainer> comp, bool canfree)
{

}

void UContainer::AAfterDelComponent(std::shared_ptr<UContainer> comp, bool canfree)
{

}

bool UContainer::ADelComponent(std::shared_ptr<UContainer> comp)
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
   variable.Property->Save(safe_shared_cast<USerStorage>(&xml),true);
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


