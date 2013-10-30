//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DllMainFormUnit.h"
#include "VideoServerRPCLib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDllMainForm *DllMainForm;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread::TEngineThread(TIdTCPClient* client, RDK::UTransferReader &packetreader, bool CreateSuspended)
: IdTCPClient(client), PacketReader(&packetreader), TThread(CreateSuspended)
{
 CalcEnable=CreateEvent(0,TRUE,0,0);
 CalcStarted=CreateEvent(0,TRUE,0,0);
 CalculationNotInProgress=CreateEvent(0,TRUE,TRUE,0);
}

__fastcall TEngineThread::~TEngineThread(void)
{
 CloseHandle(CalcStarted);
 CloseHandle(CalcEnable);
 CloseHandle(CalculationNotInProgress);
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TEngineThread::BeforeCalculate(void)
{

}

void __fastcall TEngineThread::AfterCalculate(void)
{

}



void __fastcall TEngineThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(CalcStarted,30) == WAIT_TIMEOUT)
   continue;

  if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
   continue;

  if((IdTCPClient->Connected()) && !(IdTCPClient->IOHandler->InputBufferIsEmpty()))
  {
   ClientBuffer.resize(1);
   TStream* dataInputStream=new TMemoryStream();
   int len=IdTCPClient->IOHandler->InputBuffer->Size;
   IdTCPClient->IOHandler->ReadStream(dataInputStream, len, false);
   ClientBuffer.resize(len/sizeof(ClientBuffer[0]));
   dataInputStream->Position=0;
   dataInputStream->ReadBuffer(&ClientBuffer[0], len);
   dataInputStream->Free();
   if(WaitForSingleObject(PacketReaderUnlockEvent, 10) != WAIT_TIMEOUT)
   {
	ResetEvent(PacketReaderUnlockEvent);
	PacketReader->ProcessDataPart(ClientBuffer);
	if(PacketReader->GetNumPackets()>0)
	 PulseEvent(PacketReceivedEvent);

	SetEvent(PacketReaderUnlockEvent);
   }
   ResetEvent(CalcEnable);
  }
 // else
 //  Sleep(10);
 // IdTCPClient
//  if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
//   continue;
//  ResetEvent(CalcEnable);
  ResetEvent(CalculationNotInProgress);
  BeforeCalculate();
  AfterCalculate();
  SetEvent(CalculationNotInProgress);
//  Sleep(1);
 }
}
// --------------------------


//---------------------------------------------------------------------------
// Подготовка запроса
int TDllMainForm::PrepareCommandXml(RDK::USerStorageXML &xml, const char* cmd, int channel)
{
 xml.Create("RpcRequest");
 xml.WriteString("Cmd",cmd);
 int id=GetTickCount();
 xml.WriteInteger("Id",id);
 xml.WriteInteger("Channel",channel);
 return id;
}

// Подготовка команды для камеры
int TDllMainForm::PreparePtzControlXml(RDK::USerStorageXML &xml, const std::string &cmd, const std::string &camera, int channel)
{
 xml.Create("RpcRequest");
 xml.WriteString("Cmd",cmd);
 int id=GetTickCount();
 xml.WriteInteger("Id",GetTickCount());
 xml.WriteInteger("Channel",channel);
 xml.WriteString("Component",camera);
 return id;
}

// Отсылка запроса
void TDllMainForm::SendControlCommand(RDK::USerStorageXML &xml)
{
 std::string request;
 xml.Save(request);
 Packet.SetNumParams(1);
 Packet.SetParamSize(0,request.size());
 Packet.SetParam(0,request);
 RDK::UParamT buffer;
 Packet.Save(buffer);

 if(IdTCPClient->Connected() && !buffer.empty())
 {
  TStream* dataOutputStream=new TMemoryStream();
  dataOutputStream->WriteBuffer(&buffer[0], buffer.size()*sizeof(buffer[0]));
  dataOutputStream->Position=0;
  IdTCPClient->IOHandler->Write(dataOutputStream, dataOutputStream->Size, true);
  dataOutputStream->Free();
  SetEvent(Thread->CalcEnable);
 }
}

// Поиск пакета по id комманды
int TDllMainForm::FindPacketById(int cmdId, RDK::USerStorageXML &xml)
{
 if(WaitForSingleObject(Thread->PacketReaderUnlockEvent,10) != WAIT_TIMEOUT)
 {
  ResetEvent(Thread->PacketReaderUnlockEvent);
  std::list<RDK::UTransferPacket> packetList=PacketReader.GetPacketList();
  RDK::USerStorageXML xmlTemp;

  for (std::list<RDK::UTransferPacket>::iterator  it = packetList.begin(), end = packetList.end(); it != end; ++it)
  {
   PacketXml.resize(it->GetParamSize(0));
   memcpy(&PacketXml[0],&(it->operator ()((0),0)), it->GetParamSize(0));
   xmlTemp.Load(PacketXml, "RpcResponse");

   if( xmlTemp.ReadInteger("Id", 0) == cmdId)
   {
    packetList.erase(it);
	xml=xmlTemp;
	SetEvent(Thread->PacketReaderUnlockEvent);
	return 1;
   }
  }
  SetEvent(Thread->PacketReaderUnlockEvent);
 }
 else
 {
  return RDK_RPC_PACKET_READER_ACCESS_TIMEOUT;
 }

 // пакет не найден
 return 0;
}
//---------------------------------------------------------------------------
// Ожидание ответа сервера
int TDllMainForm::WaitServerResponse(int cmdId, RDK::USerStorageXML &response, int timeout)
{
 if(WaitForSingleObject(Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
 {
  int res=FindPacketById(cmdId, response);
  return res;
 }
 else
 {
  return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
 }
}
//---------------------------------------------------------------------------
__fastcall TDllMainForm::TDllMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDllMainForm::FormCreate(TObject *Sender)
{
 Thread=new TEngineThread(IdTCPClient, PacketReader, false);
 Thread->PacketReceivedEvent=CreateEvent(0,TRUE,0,0);
 Thread->PacketReaderUnlockEvent=CreateEvent(0,TRUE,TRUE,0);
}
//---------------------------------------------------------------------------
void __fastcall TDllMainForm::FormDestroy(TObject *Sender)
{
 if(Thread)
 {
  Thread->Terminate();
  WaitForSingleObject(Thread->CalculationNotInProgress,1000);
  CloseHandle(Thread->PacketReceivedEvent);
  CloseHandle(Thread->PacketReaderUnlockEvent);
  delete Thread;
 }
}
//---------------------------------------------------------------------------

