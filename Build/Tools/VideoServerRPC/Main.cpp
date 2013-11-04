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

bool RDK_CALL Rpc_IsClientConnected(void)
{
 if(MainForm)
 {
  return MainForm->IdTCPClient->Connected();
 }

 return false;
}
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
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
  
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
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
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
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
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

int RDK_CALL Rpc_GetNumChannels(int &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML xml;
 int cmdId=MainForm->PrepareCommandXml(xml, "GetNumChannels", -1);
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
   results=StrToInt(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}
//------------------------------------------------------------------------------
/// Возвращает список текущих камер в виде строки разделяемой ','
int RDK_CALL Ptz_GetCameraNames(int channel_index, const char* &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results = NULL;
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
   results=response.ReadString("Data", "").c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Возвращает список доступных типов (классов) камер в виде строки разделяемой ','
int RDK_CALL Ptz_GetCameraTypes(int channel_index, const char* &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results = NULL;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PrepareCommandXml(PtzControlXml, "Ptz_GetCameraTypes", channel_index);
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
   results=response.ReadString("Data", "").c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Возвращает тип (класс) камеры по имени
int RDK_CALL Ptz_GetCameraType(int channel_index, const char* camera_name, const char* &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results = NULL;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetCameraType", camera_name, channel_index);
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
   results=response.ReadString("Data", "").c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Добавляет новую камеру
/// Инициализирует камеру с новым именем camera_name создавая управление типа camera_type
int RDK_CALL Ptz_AddCamera(int channel_index, const char* camera_name, const char* camera_type, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_AddCamera", camera_name, channel_index);
 PtzControlXml.WriteString("CameraType", camera_type);
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

/// Удаляет камеру
int RDK_CALL Ptz_DelCamera(int channel_index, const char* camera_name, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_DelCamera", camera_name, channel_index);
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

/// Удаляет все камеры
int RDK_CALL Ptz_DelAllCameras(int channel_index, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PrepareCommandXml(PtzControlXml, "Ptz_DelAllCameras", channel_index);
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

/// Задает значение параметра компонента управления камерой
int RDK_CALL Ptz_SetCameraParameter(int channel_index, const char* camera_name, const char* param_name, const char* param_value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_SetCameraParameter", camera_name, channel_index);
 PtzControlXml.WriteString("Parameter", param_name);
 PtzControlXml.WriteString("Value", param_value);
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

/// Считывает значение параметра компонента управления камерой
int Ptz_GetCameraParameter(int channel_index, const char* camera_name, const char* param_name, const char* &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results = NULL;
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
   results=response.ReadString("Data", "").c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Возвращает список поддерживаемых команд в виде списка, разделенного ','
int Ptz_GetImplementedCommands(int channel_index, const char* camera_name, const char* &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results = NULL;
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
   results=response.ReadString("Data", "").c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}





/// Выполняет действия по подключению к физической камере
int RDK_CALL Ptz_CameraConnect(int channel_index, const char* camera_name, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_CameraConnect", camera_name, channel_index);
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

/// Выполняет действия по отключению от физической камеры
int RDK_CALL Ptz_CameraDisconnect(int channel_index, const char* camera_name, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_CameraDisconnect", camera_name, channel_index);
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

/// Возвращает true если заданная функция реализована
//RDK_LIB_TYPE bool RDK_CALL Ptz_IsCmdImplementedById(int channel_index, const char* camera_name, int cmd);
int RDK_CALL Ptz_IsCmdImplemented(int channel_index, const char* camera_name, const char* cmd, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_IsCmdImplemented", camera_name, channel_index);
 PtzControlXml.WriteString("CommandName", cmd);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// -------------------------------
/// Функции чтения/записи параметров движения камеры
/// Список возможных имен параметров движения:
/// Pan, PanSpeed, Tilt, TiltSpeed, Zoom, ZoomSpeed, Focus, FocusSpeed,
/// Iris, IrisSpeed, Brightness, BrightnessSpeed
/// -------------------------------
/// Возвращает список поддерживаемых параметров, разделенных запятой
int RDK_CALL Ptz_GetImplementedMoveParamsList(int channel_index, const char* camera_name, const char* &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results = NULL;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetImplementedMoveParamsList", camera_name, channel_index);
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
   results=response.ReadString("Data", "").c_str();
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// results = true если параметр поддерживается
int RDK_CALL Ptz_IsMoveParamImplemented(int channel_index, const char* camera_name, const char *param_name, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_IsMoveParamImplemented", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

int RDK_CALL Ptz_SetMoveParamImplemented(int channel_index, const char* camera_name, const char *param_name, bool value, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_SetMoveParamImplemented", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
 PtzControlXml.WriteBool("ParamValue", value);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Минимальное "родное" значение параметра
int RDK_CALL Ptz_GetMoveParamMinNativeValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0.0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetMoveParamMinNativeValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
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
   results=StrToFloat(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

int RDK_CALL Ptz_SetMoveParamMinNativeValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_SetMoveParamMinNativeValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
 PtzControlXml.WriteFloat("ParamValue", value);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Максимальное "родное" значение параметра
int RDK_CALL Ptz_GetMoveParamMaxNativeValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0.0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetMoveParamMaxNativeValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
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
   results=StrToFloat(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

int RDK_CALL Ptz_SetMoveParamMaxNativeValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_SetMoveParamMaxNativeValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
 PtzControlXml.WriteFloat("ParamValue", value);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Минимальное общепринятое значение параметра
int RDK_CALL Ptz_GetMoveParamMinValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0.0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetMoveParamMinValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
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
   results=StrToFloat(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

int RDK_CALL Ptz_SetMoveParamMinValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_SetMoveParamMinValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
 PtzControlXml.WriteFloat("ParamValue", value);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

/// Максимальное общепринятое значение параметра
int RDK_CALL Ptz_GetMoveParamMaxValue(int channel_index, const char* camera_name, const char *param_name, double &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0.0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GetMoveParamMaxValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
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
   results=StrToFloat(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

int RDK_CALL Ptz_SetMoveParamMaxValue(int channel_index, const char* camera_name, const char *param_name, double value, bool &results, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;

 results=0;
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_SetMoveParamMaxValue", camera_name, channel_index);
 PtzControlXml.WriteString("ParamName", param_name);
 PtzControlXml.WriteFloat("ParamValue", value);
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
   results=StrToBool(response.ReadString("Data", "").c_str());
   return response.ReadInteger("Res", RDK_RPC_UNSUCCESSFULL_DECODING);
  }
  else
  {
   return res;
  }
 }
}

// ---------------------
// Общие функции управления движением
// ---------------------
/// Прекращает текущее движение камеры.
int RDK_CALL Ptz_Stop(int channel_index, const char* camera_name, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
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
  else if(res == 0)
   return RDK_RPC_RESPONSE_NOT_RECIEVED;

  else
  {
   return res;
  }
 }
}

/// Возвращает камеру в начальное положение.
int RDK_CALL Ptz_GotoHome(int channel_index, const char* camera_name, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GotoHome", camera_name, channel_index);
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

/// Запоминает текущее положение камеры как некоторое предустановленное.
int RDK_CALL Ptz_PresetPoint(int channel_index, const char* camera_name, int i, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_PresetPoint", camera_name, channel_index);
 PtzControlXml.WriteInteger("Point", i);
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

/// Перемещает камеру в предустановленное положение.
int RDK_CALL Ptz_GotoPoint(int channel_index, const char* camera_name, int i, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_GotoPoint", camera_name, channel_index);
 PtzControlXml.WriteInteger("Point", i);
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

/// Удаляет сохраненное предустановленное положение
int RDK_CALL Ptz_RemovePoint(int channel_index, const char* camera_name, int i, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_RemovePoint", camera_name, channel_index);
 PtzControlXml.WriteInteger("Point", i);
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
// Функции управления перемещением по скорости
// Скорость задается в станадартизованных библиотекой ед/с
// Непрерывно перемещает камеру в заданном знаком скорости направлении
// ---------------------
/// Перемещает камеру одновременно по 3 направлениям Pan, Tilt, Zoom, со
/// скоростями соответственно pan_speed, tilt_speed, zoom_speed, если камера поддерживает такой режим.
int RDK_CALL Ptz_MovePTZ(int channel_index, const char* camera_name, double pan_speed, double tilt_speed, double zoom_speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePTZ", camera_name, channel_index);
 PtzControlXml.WriteFloat("PanSpeed", pan_speed);
 PtzControlXml.WriteFloat("TiltSpeed", tilt_speed);
 PtzControlXml.WriteFloat("ZoomSpeed", zoom_speed);
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

/// Перемещает камеру одновременно по 2 направлениям Pan, Tilt со
/// скоростями соответственно pan_speed, tilt_speed, zoom_speed, если камера поддерживает такой режим.
int RDK_CALL Ptz_MovePT(int channel_index, const char* camera_name, double pan_speed, double tilt_speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePT", camera_name, channel_index);
 PtzControlXml.WriteFloat("PanSpeed", pan_speed);
 PtzControlXml.WriteFloat("TiltSpeed", tilt_speed);
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

/// Перемещает камеру в направлении TPtzDirection со скоростью speed
/// (здесь предполагается, что speed >=0).
int RDK_CALL Ptz_MoveDirection(int channel_index, const char* camera_name, TPtzDirection direction, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveDirection", camera_name, channel_index);
 PtzControlXml.WriteFloat("Speed", speed);
 PtzControlXml.WriteFloat("Direction", direction);
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

/// Перемещает камеру по горизонтальной оси с скоростью speed.
int RDK_CALL Ptz_MovePan(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
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

/// Перемещает камеру по вертикальной оси с скоростью speed.
int RDK_CALL Ptz_MoveTilt(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
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

/// Изменение поля зрения камеры  с скоростью speed.
int RDK_CALL Ptz_MoveZoom(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
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

/// Изменение фокусировки камеры  с скоростью speed.
int RDK_CALL Ptz_MoveFocus(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveFocus", camera_name, channel_index);
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

/// Изменение величины диафрагмы камеры  с скоростью speed.
int RDK_CALL Ptz_MoveIris(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveIris", camera_name, channel_index);
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

/// Изменение величины к-та усиления камеры  с скоростью speed.
int RDK_CALL Ptz_MoveBrightness(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveBrightness", camera_name, channel_index);
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
// Функции управления перемещением по скорости
// Скорость задается в родных для камеры единицах
/// Непрерывно перемещает камеру в заданном знаком скорости направлении
// ---------------------
int RDK_CALL Ptz_MovePTZNative(int channel_index, const char* camera_name, double pan_speed, double tilt_speed, double zoom_speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePTZNative", camera_name, channel_index);
 PtzControlXml.WriteFloat("PanSpeed", pan_speed);
 PtzControlXml.WriteFloat("TiltSpeed", tilt_speed);
 PtzControlXml.WriteFloat("ZoomSpeed", zoom_speed);
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

int RDK_CALL Ptz_MovePTNative(int channel_index, const char* camera_name, double pan_speed, double tilt_speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePTNative", camera_name, channel_index);
 PtzControlXml.WriteFloat("PanSpeed", pan_speed);
 PtzControlXml.WriteFloat("TiltSpeed", tilt_speed);
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

int RDK_CALL Ptz_MoveDirectionNative(int channel_index, const char* camera_name, TPtzDirection direction, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveDirectionNative", camera_name, channel_index);
 PtzControlXml.WriteFloat("Speed", speed);
 PtzControlXml.WriteFloat("Direction", direction);
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

int RDK_CALL Ptz_MovePanNative(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MovePanNative", camera_name, channel_index);
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

int RDK_CALL Ptz_MoveTiltNative(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveTiltNative", camera_name, channel_index);
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

int RDK_CALL Ptz_MoveZoomNative(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveZoomNative", camera_name, channel_index);
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

int RDK_CALL Ptz_MoveFocusNative(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveFocusNative", camera_name, channel_index);
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

int RDK_CALL Ptz_MoveIrisNative(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveIrisNative", camera_name, channel_index);
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

int RDK_CALL Ptz_MoveBrightnessNative(int channel_index, const char* camera_name, double speed, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_MoveBrightnessNative", camera_name, channel_index);
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
// Функции считывания состояния камеры в стандартизированных величинах
// ---------------------
int RDK_CALL Ptz_ReadPTZPosition(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadPTZPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   pan=response.ReadFloat("Pan",0.0);
   tilt=response.ReadFloat("Tilt",0.0);
   zoom=response.ReadFloat("Zoom",0.0);
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

int RDK_CALL Ptz_ReadPTPosition(int channel_index, const char* camera_name, double &pan, double &tilt, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadPTPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   pan=response.ReadFloat("Pan",0.0);
   tilt=response.ReadFloat("Tilt",0.0);
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

int RDK_CALL Ptz_ReadPanPosition(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadPanPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Pan",0.0);
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

int RDK_CALL Ptz_ReadTiltPosition(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadTiltPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Tilt",0.0);
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

int RDK_CALL Ptz_ReadZoomPosition(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;

 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
	
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadZoomPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Zoom",0.0);
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

int RDK_CALL Ptz_ReadFocusPosition(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadFocusPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Focus",0.0);
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

int RDK_CALL Ptz_ReadIrisPosition(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadIrisPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Iris",0.0);
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

int RDK_CALL Ptz_ReadBrightnessPosition(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadBrightnessPosition", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Brightness",0.0);
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
// Функции считывания состояния камеры в родных для камеры величинах
// ---------------------
int RDK_CALL Ptz_ReadPTZPositionNative(int channel_index, const char* camera_name, double &pan, double &tilt, double &zoom, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
  
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
  
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadPTZPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   pan=response.ReadFloat("Pan",0.0);
   tilt=response.ReadFloat("Tilt",0.0);
   zoom=response.ReadFloat("Zoom",0.0);
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

int RDK_CALL Ptz_ReadPTPositionNative(int channel_index, const char* camera_name, double &pan, double &tilt, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadPTPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   pan=response.ReadFloat("Pan",0.0);
   tilt=response.ReadFloat("Tilt",0.0);
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

int RDK_CALL Ptz_ReadPanPositionNative(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
  
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
  
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadPanPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Pan",0.0);
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

int RDK_CALL Ptz_ReadTiltPositionNative(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
  
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
  
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadTiltPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Tilt",0.0);
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

int RDK_CALL Ptz_ReadZoomPositionNative(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadZoomPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Zoom",0.0);
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

int RDK_CALL Ptz_ReadFocusPositionNative(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
  
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
  
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadFocusPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Focus",0.0);
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

int RDK_CALL Ptz_ReadIrisPositionNative(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadIrisPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Iris",0.0);
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

int RDK_CALL Ptz_ReadBrightnessPositionNative(int channel_index, const char* camera_name, double &value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_ReadBrightnessPositionNative", camera_name, channel_index);
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
   response.SelectNodeRoot("RpcResponse/Data/Position");
   value=response.ReadFloat("Brightness",0.0);
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
// Функции управления вспомогательными параметрами камеры
// Эти функции управляют параметрами, представляющими собой флаг
// включения/выключения некоторых режимов (функций).
// Соответственно, значения аргумента value во всех функциях обычно принимаются
// равными 0 (функция выключена), 1 (функция включена).
// ---------------------
/// Дворники
int RDK_CALL Ptz_Rain(int channel_index, const char* camera_name, double value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_Rain", camera_name, channel_index);
 PtzControlXml.WriteFloat("State", value);
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

/// Подсветка
int RDK_CALL Ptz_Light(int channel_index, const char* camera_name, double value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_Light", camera_name, channel_index);
 PtzControlXml.WriteFloat("State", value);
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

/// Автофокусировка
int RDK_CALL Ptz_AutoFocus(int channel_index, const char* camera_name, double value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_AutoFocus", camera_name, channel_index);
 PtzControlXml.WriteFloat("State", value);
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

/// Автодиафрагма
int RDK_CALL Ptz_AutoIris(int channel_index, const char* camera_name, double value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_AutoIris", camera_name, channel_index);
 PtzControlXml.WriteFloat("State", value);
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

/// Автоподстройка к-та усиления
int RDK_CALL Ptz_AutoBrightness(int channel_index, const char* camera_name, double value, int timeout)
{
 if(!MainForm)
  return RDK_RPC_LIBRARY_NOT_INIT;
 
 if(!MainForm->IdTCPClient->Connected())
  return RDK_RPC_CLIENT_NOT_CONNECTED;
   
 RDK::USerStorageXML PtzControlXml;
 int cmdId=MainForm->PreparePtzControlXml(PtzControlXml, "Ptz_AutoBrightness", camera_name, channel_index);
 PtzControlXml.WriteFloat("State", value);
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

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}

