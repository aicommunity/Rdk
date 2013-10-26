//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DllMainFormUnit.h"
//#include "TUBitmap.h"
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
  }
 // IdTCPClient
//  if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
//   continue;
//  ResetEvent(CalcEnable);
  ResetEvent(CalculationNotInProgress);
  BeforeCalculate();
  AfterCalculate();
  SetEvent(CalculationNotInProgress);
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
 xml.WriteString("Camera",camera);
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
 }
}

// Поиск пакета по id комманды
bool TDllMainForm::FindPacketById(int id, RDK::USerStorageXML &xml)
{
 if(WaitForSingleObject(Thread->PacketReaderUnlockEvent,10) != WAIT_TIMEOUT)
 {
  ResetEvent(Thread->PacketReaderUnlockEvent);
  std::list<RDK::UTransferPacket> packetList=PacketReader.GetPacketList();
  RDK::UTransferPacket packet;
  RDK::USerStorageXML xmlTemp;

  for (std::list<RDK::UTransferPacket>::iterator  it = packetList.begin(), end = packetList.end(); it != end; ++it)
  {
   packet=*it;
   packetList.erase(it);
   PacketXml.resize(packet.GetParamSize(0));
   memcpy(&PacketXml[0],&packet(0)[0],packet.GetParamSize(0));
   xmlTemp.Load(PacketXml, "RpcResponse");

   if( xmlTemp.ReadInteger("Id", 0) == id)
   {
	xml=xmlTemp;
	SetEvent(Thread->PacketReaderUnlockEvent);
	return true;
   }
   /*if(packet.GetCmdId() == id)
   {
	PacketXml.resize(packet.GetParamSize(0));
	memcpy(&PacketXml[0],&packet(0)[0],packet.GetParamSize(0));
	xml.Load(PacketXml, "RpcResponse");
	//packetList.remove(it);  ?
	SetEvent(Thread->PacketReaderUnlockEvent);
	return true;
   } */
  }
  SetEvent(Thread->PacketReaderUnlockEvent);
 }
 else
 {
  // Ошибка
 }

 return false;
}
/*
const char* TDllMainForm::WaitData()
{
 if((IdTCPClient->Connected()) && !(IdTCPClient->IOHandler->InputBufferIsEmpty()))
 {
  bool ReadReady = true;
  //if( TcpClient->Select(&ReadReady, NULL, NULL, 10) )
  //{
  // if(ReadReady)
  // {
	ClientBuffer.resize(1);
	TStream* dataInputStream=new TMemoryStream();
	int len=IdTCPClient->IOHandler->InputBuffer->Size;
	IdTCPClient->IOHandler->ReadStream(dataInputStream, len, false);
	ClientBuffer.resize(len/sizeof(ClientBuffer[0]));
    dataInputStream->Position=0;
	dataInputStream->ReadBuffer(&ClientBuffer[0], len);
	dataInputStream->Free();
	//ClientBuffer.resize(20000);
	//int len=TcpClient->ReceiveBuf(&ClientBuffer[0],ClientBuffer.size(),0);
	//if(len>0)
	//{
	// ClientBuffer.resize(len);
	 PacketReader.ProcessDataPart(ClientBuffer);
	 if(PacketReader.GetNumPackets()>0)
	 {
	  Packet=PacketReader.GetLastPacket();
	  PacketReader.DelLastPacket();
	  if(Packet.GetNumParams()>0)
	  {
	   PacketXml.resize(Packet.GetParamSize(0));
	   memcpy(&PacketXml[0],&Packet(0)[0],Packet.GetParamSize(0));
	   //StaticText1->Caption=String("Последний XML размера: ")+IntToStr(int(PacketXml.size()));
	   //StaticText2->Caption=String("Пакетов в очереди: ")+IntToStr(int(PacketReader.GetPacketList().size()));
	   //RichEdit1->Lines->Clear();
	   //RichEdit1->Lines->Add(PacketXml.c_str());
	   return PacketXml.c_str();
	  }
	 }
	//}
   //}
  //}
 }

 return NULL;
}  */
//---------------------------------------------------------------------------
__fastcall TDllMainForm::TDllMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDllMainForm::FormCreate(TObject *Sender)
{
 //PacketReceivedEvent=CreateEvent(0,TRUE,0,0);
 //PacketReaderUnlockEvent=CreateEvent(0,TRUE,TRUE,0);

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
 //CloseHandle(ThreadPacketReceivedEvent);
 //CloseHandle(ThreadPacketReaderUnlockEvent);
}
//---------------------------------------------------------------------------
void __fastcall TDllMainForm::Timer1Timer(TObject *Sender)
{
 if((IdTCPClient->Connected()) && !(IdTCPClient->IOHandler->InputBufferIsEmpty()))
 { /*
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
   PacketReader.ProcessDataPart(ClientBuffer);
   if(PacketReader.GetNumPackets()>0)
	PulseEvent(PacketReceivedEvent);

   SetEvent(PacketReaderUnlockEvent);
  }  */
 }
}
//---------------------------------------------------------------------------

