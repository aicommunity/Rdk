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
const char* ServerAnswerDebug=NULL;

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
  SetEvent(MainForm->Thread->CalcStarted);
 }

 return 0;
}

int RDK_CALL Rpc_Disconnect(void)
{
 if(MainForm)
 {
  ResetEvent(MainForm->Thread->CalcStarted);
  MainForm->IdTCPClient->Disconnect(true);
  ResetEvent(MainForm->Thread->CalcStarted);
 }

 return 0;
}

int RDK_CALL Rpc_StartChannel(int channel_index, int timeout)
{
 RDK::USerStorageXML xml;
 int cmdId=MainForm->PrepareCommandXml(xml, "StartChannel", channel_index);
 MainForm->SendControlCommand(xml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
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
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
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
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}

int RDK_CALL Rpc_GetNumChannels(int channel_index, int timeout)
{
 RDK::USerStorageXML xml;
 int cmdId=MainForm->PrepareCommandXml(xml, "GetNumChannels", channel_index);
 MainForm->SendControlCommand(xml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return StrToInt(response.ReadString("Data", "").c_str());
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}
//------------------------------------------------------------------------------
/// ¬озвращает список текущих камер в виде строки раздел€емой ','
const char* RDK_CALL Ptz_GetCameraNames(int channel_index, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PrepareCommandXml(PtzControlXml, "Ptz_GetCameraNames", channel_index);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return (response.ReadString("Data", "")).c_str();
  }
  else
  {
   // заглушка
   return 0;
  }
 }
}

/// —читывает значение параметра компонента управлени€ камерой
const char* Ptz_GetCameraParameter(int channel_index, const char* camera_name, const char* param_name, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetCameraParameter", camera_name, channel_index);
 PtzControlXml.WriteString("Parameter", param_name);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadString("Data", "").c_str();
  }
  else
  {
   // заглушка
   return 0;
  }
 }
}

/// ¬озвращает список поддерживаемых команд в виде списка, разделенного ','
const char* Ptz_GetImplementedCommands(int channel_index, const char* camera_name, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetImplementedCommands", camera_name, channel_index);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadString("Data", "").c_str();
  }
  else
  {
   // заглушка
   return 0;
  }
 }
}
// ---------------------
// ќбщие функции управлени€ движением
// ---------------------
/// ѕрекращает текущее движение камеры.
int RDK_CALL Ptz_Stop(int channel_index, const char* camera_name, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_Stop", camera_name, channel_index);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}
// ---------------------
// ‘ункции управлени€ перемещением по скорости
// —корость задаетс€ в станадартизованных библиотекой ед/с
// Ќепрерывно перемещает камеру в заданном знаком скорости направлении
// ---------------------
/// ѕеремещает камеру одновременно по 3 направлени€м Pan, Tilt, Zoom, со
/// скорост€ми соответственно pan_speed, tilt_speed, zoom_speed, если камера поддерживает такой режим.
//int RDK_CALL Ptz_Move(int channel_index, const char* camera_name, double pan_speed=0, double tilt_speed=0, double zoom_speed=0);

/// ѕеремещает камеру в направлении TPtzDirection со скоростью speed
/// (здесь предполагаетс€, что speed >=0).
//RDK_LIB_TYPE int RDK_CALL Ptz_MoveDirection(int channel_index, const char* camera_name, TPtzDirection direction, double speed);

/// ѕеремещает камеру по горизонтальной оси с скоростью speed.
int RDK_CALL Ptz_MovePan(int channel_index, const char* camera_name, double speed, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePan", camera_name, channel_index);
 PtzControlXml.WriteFloat("Speed", speed);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}

/// ѕеремещает камеру по вертикальной оси с скоростью speed.
int RDK_CALL Ptz_MoveTilt(int channel_index, const char* camera_name, double speed, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveTilt", camera_name, channel_index);
 PtzControlXml.WriteFloat("Speed", speed);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}

/// »зменение пол€ зрени€ камеры  с скоростью speed.
int RDK_CALL Ptz_MoveZoom(int channel_index, const char* camera_name, double speed, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveZoom", camera_name, channel_index);
 PtzControlXml.WriteFloat("Speed", speed);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}

// ---------------------
// ‘ункции считывани€ состо€ни€ камеры в стандартизированных величинах
// ---------------------
int RDK_CALL Ptz_ReadPTZPosition(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom, int timeout)
{
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveZoom", camera_name, channel_index);
 MainForm->SendControlCommand(PtzControlXml);

 if(timeout > 0)
 {
  RDK::USerStorageXML response;
  int res=MainForm->WaitServerResponse(cmdId, response, timeout);
  if(res == 1)
  {
   std::string answ;
   response.Save(answ);
   ServerAnswerDebug=answ.c_str();
   response.SelectNodeRoot("Data/Position");
   pan=response.ReadFloat("Pan",RDK_RPC_UNSUCCESSFULL_DECODING);
   tilt=response.ReadFloat("Tilt",RDK_RPC_UNSUCCESSFULL_DECODING);
   zoom=response.ReadFloat("Zoom",RDK_RPC_UNSUCCESSFULL_DECODING);
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}

