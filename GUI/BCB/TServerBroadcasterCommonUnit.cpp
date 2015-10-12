//---------------------------------------------------------------------------

#pragma hdrstop

#include "TServerBroadcasterCommonUnit.h"
#include "UServerControlFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "TBcbSupport.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
//---------------------------------------------------------------------------

/// ��������� ������ ����������� �������
extern UEngineControlVcl RdkEngineControl;

// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TResultBroadcasterThread::TResultBroadcasterThread(bool CreateSuspended)
: TThread(CreateSuspended)
{
 SendEnableFlag=false;
 Priority=RDK_DEFAULT_THREAD_PRIORITY;
 SendEnable=CreateEvent(0,TRUE,0,0);
 MetaUnlockEvent=CreateEvent(0,TRUE,TRUE,0);
 SendNotInProgressEvent=CreateEvent(0,TRUE,TRUE,0);
}

__fastcall TResultBroadcasterThread::~TResultBroadcasterThread(void)
{
 Terminate();
 WaitForSingleObject(SendNotInProgressEvent,INFINITE);
 WaitFor();

 CloseHandle(SendEnable);
 CloseHandle(MetaUnlockEvent);
 CloseHandle(SendNotInProgressEvent);
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
bool __fastcall TResultBroadcasterThread::GenerateSendString(void)
{
 if(WaitForSingleObject(MetaUnlockEvent,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(MetaUnlockEvent);

 SendString.clear();

 SendString=std::string("<Meta Size=")+RDK::sntoa(MetaList.size())+">\r\n";
  SendString+="<Server>";
  SendString+=UServerControlForm->ServerId;
  SendString+="</Server>\r\n";
 int i=0;
 std::list<TServerMetadata>::iterator I=MetaList.begin();
 for(;I != MetaList.end();++I)
 {
  SendString+=std::string("<Packet Index=")+RDK::sntoa(i++)+">";
  SendString+="<TimeStamp>";
  SendString+=RDK::sntoa(I->TimeStamp);
  SendString+="</TimeStamp>\r\n";
  SendString+="<Channel>";
  SendString+=RDK::sntoa(I->ChannelIndex);
  SendString+="</Channel>\r\n";
  SendString+="<Response>\r\n";
  SendString+=I->Metadata+"\r\n";
  SendString+="</Response>\r\n";
  SendString+="</Packet>\r\n";
 }
 SendString+="</Meta>";
 MetaList.clear();

 SetEvent(MetaUnlockEvent);
 return true;
}

bool __fastcall TResultBroadcasterThread::Send(void)
{
 SendPacket.SetCmdId(0);
 SendPacket.SetNumParams(1);
 SendPacket.SetParam(0,SendString);
 SendBuffer.resize(SendPacket.GetPacketSize());
 SendPacket.Save(SendBuffer);

 if(ASend())
 {
  return true;
 }

 return false;
}

bool __fastcall TResultBroadcasterThread::PeriodicallyActions(void)
{
 return true;
}


void __fastcall TResultBroadcasterThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(SendEnable,30) == WAIT_TIMEOUT)
   continue;
  ResetEvent(SendEnable);
  PeriodicallyActions();

  if(WaitForSingleObject(MetaUnlockEvent,30) == WAIT_TIMEOUT)
  {
   continue;
  }
  ResetEvent(MetaUnlockEvent);  //TODO
  ResetEvent(SendNotInProgressEvent); //TODO
  if(!MetaList.empty())
  {
   SetEvent(MetaUnlockEvent);
   if(!GenerateSendString())
   {
//	SetEvent(MetaUnlockEvent);
	Sleep(30);
   }
   else
   {
	if(SendEnableFlag)
	{
	 Send();
	 Sleep(5);
//	 SetEvent(MetaUnlockEvent);
	}
	else
	{
//	 SetEvent(MetaUnlockEvent);
	 Sleep(30);
	}
   }
  }
  else
  {
   SetEvent(MetaUnlockEvent);
   Sleep(30);
  }
//  SetEvent(MetaUnlockEvent);
  SetEvent(SendNotInProgressEvent);
 }
}

/// ��������� ���������� � �������
bool __fastcall TResultBroadcasterThread::AddMetadataSafe(int channel_index, RDK::ULongTime time_stamp, const std::string &component_name, const std::string &property_name)
{
 if(!SendEnableFlag)
  return true;

 if(channel_index>GetNumEngines())
  return false;

 const char* xml_data=0;
 if(!property_name.empty())
  xml_data=MModel_GetComponentStateValue(channel_index, component_name.c_str(),
												property_name.c_str());
 else
  xml_data=MModel_GetComponentState(channel_index, component_name.c_str());

 if(!xml_data)
 {
  return false;
 }

 TServerMetadata meta;

 meta.TimeStamp=time_stamp;

 meta.ServerId="";

 meta.ChannelId="";

 meta.ChannelIndex=channel_index;
 meta.Metadata=xml_data;
 MEngine_FreeBufString(channel_index,xml_data);


 if(WaitForSingleObject(MetaUnlockEvent,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(MetaUnlockEvent);

 MetaList.push_back(meta);

 SetEvent(MetaUnlockEvent);
 return true;
}

/// ���� ���������� ��������
bool TResultBroadcasterThread::GetSendEnableFlag(void) const
{
 return SendEnableFlag;
}

bool TResultBroadcasterThread::SetSendEnableFlag(bool value)
{
 SendEnableFlag=value;
 return true;
}
// --------------------------

// --------------------------
__fastcall TBroadcasterForm::TBroadcasterForm(TComponent* Owner)
 : TUVisualControllerForm(Owner)
{
 BroadcastEnableFlag=false;
}

__fastcall TBroadcasterForm::~TBroadcasterForm(void)
{
 UnRegisterFromEngineMonitor();
}
// --------------------------

// ---------------------------
// ������ ������� � ����������
// ---------------------------
bool TBroadcasterForm::GetBroadcastEnableFlag(void) const
{
 return BroadcastEnableFlag;
}

bool TBroadcasterForm::SetBroadcastEnableFlag(bool value)
{
 if(BroadcastEnableFlag == value)
  return true;

 BroadcastEnableFlag=value;
 if(BroadcastEnableFlag)
  return RegisterToEngineMonitor();
 else
  return UnRegisterFromEngineMonitor();

 return true;
}
// ---------------------------

// --------------------------

// ��������� ��������� ���������� � xml
void TBroadcasterForm::ASaveParameters(RDK::USerStorageXML &xml)
{
 AASaveParameters(xml);
 xml.WriteBool("BroadcastEnableFlag",GetBroadcastEnableFlag());
}

// ��������� ��������� ���������� �� xml
void TBroadcasterForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 AALoadParameters(xml);
 bool value=false;
 value=xml.ReadBool("BroadcastEnableFlag",false);
 SetBroadcastEnableFlag(value);
}
// --------------------------



// --------------------------
bool TBroadcasterForm::RegisterToEngineMonitor(void)
{
 if(UEngineMonitorForm)
 {
  if(UEngineMonitorForm->EngineMonitorFrame)
  {
   RdkEngineControl.RegisterMetadataBroadcaster(this);
   return true;
  }
 }
 return false;
}

bool TBroadcasterForm::UnRegisterFromEngineMonitor(void)
{
 if(UEngineMonitorForm)
 {
  if(UEngineMonitorForm->EngineMonitorFrame)
  {
   RdkEngineControl.UnRegisterMetadataBroadcaster(this);
   return true;
  }
 }
 return false;
}
// --------------------------



#pragma package(smart_init)
