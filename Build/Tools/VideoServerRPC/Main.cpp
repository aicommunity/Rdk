//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you

#include <vcl.h>
#include <windows.h>
#include "DllMainFormUnit.h"
#include "VideoServerRPCLib.h"

#pragma hdrstop
#pragma argsused

TDllMainForm *MainForm=0;

//----------------------------------------------------
//int ResponseTimeOut=100;
const char* ServerAnswerDebug=NULL;

/*
int RDK_CALL Rpc_GetResponseTimeOut(void)
{
 return ResponseTimeOut;
}

bool RDK_CALL Rpc_SetResponseTimeOut(const int &value)
{
 if(ResponseTimeOut == value)
  return true;

 ResponseTimeOut=value;
 return true;
}
*/
/// Управление параметрами
const char* RDK_CALL Rpc_GetServerAnswerDebug(void)
{
 return ServerAnswerDebug;
}

bool RDK_CALL Rpc_SetServerAnswerDebug(const char* value)
{
 if(ServerAnswerDebug == value)
  return true;

 ServerAnswerDebug=value;
 return true;
}
//----------------------------------------------------
/// Инициализация
int RDK_CALL Rpc_Init(void)
{
 if(!MainForm)
 {
  MainForm=new TDllMainForm(0);
 }
 return 0;
}

int RDK_CALL Rpc_UnInit(void)
{
 if(MainForm)
 {
  delete MainForm;
  MainForm=0;
 }
 return 0;
}

/// Коммуникация с сервером
int RDK_CALL Rpc_Connect(const char* serverAddress, int serverPort)
{
 if(MainForm)
 {
  if(MainForm->IdTCPClient->Connected())
   MainForm->IdTCPClient->Disconnect(true);

  //MainForm->IdTCPClient->Connect(serverAddress, serverPort);
  MainForm->IdTCPClient->Host=serverAddress;
  MainForm->IdTCPClient->Port=serverPort;
  MainForm->IdTCPClient->Connect();
  //MainForm->Timer1->Enabled=true;
  SetEvent(MainForm->Thread->CalcStarted);
  //MainForm->Thread->Start();
 }

 return 0;
}

int RDK_CALL Rpc_Disconnect(void)
{
 if(MainForm)
 {
  ResetEvent(MainForm->Thread->CalcStarted);
  MainForm->IdTCPClient->Disconnect(true);
  //MainForm->Timer1->Enabled=false;
  ResetEvent(MainForm->Thread->CalcStarted);
 }

 return 0;
}

/// timeOut, мс
/// 0 - работа без подтверждения
/// value - синхронный режим, время ожидания
int RDK_CALL Rpc_StartChannel(int channel_index, int timeout)
{
 RDK::USerStorageXML xml;
 int cmdId=MainForm->PrepareCommandXml(xml, "StartChannel", channel_index);
 MainForm->SendControlCommand(xml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }

}

int RDK_CALL Rpc_ResetChannel(int channel_index, int timeout)
{
 RDK::USerStorageXML xml;
 int cmdId=MainForm->PrepareCommandXml(xml, "ResetChannel", channel_index);
 MainForm->SendControlCommand(xml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }

}

int RDK_CALL Rpc_StopChannel(int channel_index, int timeout)
{
 RDK::USerStorageXML xml;
 int cmdId=MainForm->PrepareCommandXml(xml, "StopChannel", channel_index);
 MainForm->SendControlCommand(xml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }

}

/// Команды управления камерами
int RDK_CALL Rpc_CameraLeft(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePan", cameraname, channel_index);
 PtzControlXml.WriteFloat("PanSpeed",-10.0);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int RDK_CALL Rpc_CameraRight(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePan", cameraname, channel_index);
 PtzControlXml.WriteFloat("PanSpeed",10.0);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int RDK_CALL Rpc_CameraUp(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveTilt", cameraname, channel_index);
 PtzControlXml.WriteFloat("TiltSpeed",-10.0);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int RDK_CALL Rpc_CameraDown(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveTilt", cameraname, channel_index);
 PtzControlXml.WriteFloat("TiltSpeed",10.0);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int RDK_CALL Rpc_CameraStop(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_Stop", cameraname, channel_index);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int RDK_CALL Rpc_CameraZoomIn(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveZoom", cameraname, channel_index);
 PtzControlXml.WriteFloat("ZoomSpeed",-10.0);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int RDK_CALL Rpc_CameraZoomOut(int channel_index, const char* cameraname, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveZoom", cameraname, channel_index);
 PtzControlXml.WriteFloat("ZoomSpeed",10.0);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  if(WaitForSingleObject(MainForm->Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
  {
   RDK::USerStorageXML response;
   if(MainForm->FindPacketById(cmdId, response))
   {
	std::string answ;
	response.Save(answ);
	ServerAnswerDebug=answ.c_str();
   }
  }
  else
  {
   return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
  }
 }
}

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}

