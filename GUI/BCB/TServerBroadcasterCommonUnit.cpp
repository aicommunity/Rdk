//---------------------------------------------------------------------------

#pragma hdrstop

#include "TServerBroadcasterCommonUnit.h"
//---------------------------------------------------------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TResultBroadcasterThread::TResultBroadcasterThread(bool CreateSuspended)
: TThread(CreateSuspended)
{
 SendEnable=CreateEvent(0,TRUE,0,0);
 MetaUnlockEvent=CreateEvent(0,TRUE,TRUE,0);
}

__fastcall TResultBroadcasterThread::~TResultBroadcasterThread(void)
{
 CloseHandle(SendEnable);
 CloseHandle(MetaUnlockEvent);
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
bool __fastcall TResultBroadcasterThread::GenerateSendString(void)
{
 ResetEvent(MetaUnlockEvent);

 SendString.clear();

 SendString=std::string("<Meta Size=")+RDK::sntoa(MetaList.size())+">\r\n";
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
  SendString+="<Server>";
  SendString+="server_id";
  SendString+="</Server>\r\n";
  SendString+="<Response>\r\n";
  SendString+=I->Metadata+"\r\n";
  SendString+="</Response>\r\n";
  SendString+="</Packet>\r\n";
 }
 SendString+="</Meta>";

 SetEvent(MetaUnlockEvent);
}

bool __fastcall TResultBroadcasterThread::Send(void)
{
 if(ASend())
 {
 }

 SetEvent(SendEnable);
}



void __fastcall TResultBroadcasterThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(SendEnable,30) == WAIT_TIMEOUT)
   continue;
  ResetEvent(SendEnable);
  ResetEvent(MetaUnlockEvent);
  if(!MetaList.empty())
  {
   if(!GenerateSendString())
   {
	SetEvent(MetaUnlockEvent);
	Sleep(30);
	SetEvent(SendEnable);
   }
   else
   {
	Send();
    SetEvent(MetaUnlockEvent);
   }
  }
  else
  {
   SetEvent(MetaUnlockEvent);
   Sleep(30);
   SetEvent(SendEnable);
  }
 }
}
// --------------------------


#pragma package(smart_init)
