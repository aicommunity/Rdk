#ifndef RDK_RPC_CPP
#define RDK_RPC_CPP

#include "rdk_rpc.h"
#include "rdk.h"
#include "rdk_cpp_init.h"


namespace RDK{

//std::vector<std::string> RpcReturnString;

/// Возвращает набор из имени компонента и индекса канала
/// Если определен параметр "CC"
/// Содержимое параметра представляет собой
/// текст вида: индекс_канала@Имя компонента
bool RDK_CALL ExtractCC(USerStorageXML &xml, int &channel_index, std::string &component_name)
{
 if(!xml.SelectNode("Component"))
  return false;
 component_name=xml.GetNodeText();
 xml.SelectUp();
 std::string::size_type i=component_name.find_first_of("@");
 if(i == string::npos)
  return false;
 channel_index=atoi(component_name.substr(0,i));
 component_name=component_name.substr(i+1);

 return true;
}

/// Возвращает имя компонента
/// Если определен параметр "Component"
bool RDK_CALL ExtractComponent(USerStorageXML &xml, std::string &component_name)
{
 if(!xml.SelectNode("Component"))
  return false;
 component_name=xml.GetNodeText();
 xml.SelectUp();
 return true;
}

/// Возвращает индекса канала
/// Если определен параметр "Channel"
bool RDK_CALL ExtractChannel(USerStorageXML &xml, int &channel_index)
{
 if(!xml.SelectNode("Channel"))
  return false;
 channel_index=atoi(xml.GetNodeText());
 xml.SelectUp();
 return true;
}

/// Возвращает имя команды
/// Если определен параметр "Cmd"
bool RDK_CALL ExtractCmd(USerStorageXML &xml, std::string &cmd_name)
{
 if(!xml.SelectNode("Cmd"))
  return false;
 cmd_name=xml.GetNodeText();
 xml.SelectUp();
 return true;
}

/// <RPC_Request>
///     <Channel>индекс движка</Channel>
///     <Cmd>имя вызываемой функции</Cmd>
///     <Component>имя компонента</Component>
///     <Class>имя класса</Class>
///     <Data>xml-описание данных функции, например xml с параметрами компонента</Data>
/// </RPC_Request>
const char* RDK_CALL RemoteCallInternal(const char *request, int &return_value, int &channel_index)
{
 const char* ReturnString=0;
 return_value=INT_MAX;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int engine_index=-1;
 std::string cmd;
 std::string component_name;
 channel_index=0;


 if(!ExtractCmd(xml,cmd) || cmd.empty())
 {
  return_value=2000;
  return 0;
 }

 if(!ExtractCC(xml,engine_index,component_name))
 {
  ExtractChannel(xml,engine_index);
  ExtractComponent(xml,component_name);
 }

 if((engine_index < 0 || engine_index >= GetNumEngines()) &&
	(cmd != "GetNumChannels" || cmd !="SetNumChannels"))
 {
  return_value=2001;
  return 0;
 }

// if(engine_index>=0)
//  MEngine_LogMessage(engine_index, RDK_EX_INFO, (std::string("Rec cmd id: ")+xml.ReadString("Id","")+std::string(" Cmd=")+cmd).c_str());
 channel_index=engine_index;
 std::string response,temp;
 std::string name;

 RDK::USerStorageXML result;
 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
 if(cmd == "EngineInit")
 {
  int predefined_structure=xml.ReadInteger("PredefinedStructure",0);

  return_value=MEngineInit(engine_index, predefined_structure);
 }
 else
 if(cmd == "EngineUnInit")
 {
  return_value=MEngineUnInit(engine_index);
 }
 else
 if(cmd == "Env_Reset")
 {
  return_value=MEnv_Reset(engine_index, component_name.c_str());
 }
 else
 if(cmd == "Env_Calculate")
 {
  return_value=MEnv_Calculate(engine_index, component_name.c_str());
 }
 else
 if(cmd == "Model_Create")
 {
  std::string class_name=xml.ReadString("Class","");
  if(!class_name.empty())
  {
   return_value=MModel_Create(engine_index, class_name.c_str());
  }
  else
   return_value=100;
 }
 else
 if(cmd == "Model_LoadComponent")
 {
  xml.SelectNodeRoot("RpcRequest/Data/Save");
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_LoadComponent(engine_index, component_name.c_str(),data.c_str());
 }
 else
 if(cmd == "Model_LoadComponentParameters")
 {
  xml.SelectNodeRoot("RpcRequest/Data/SaveProperties");
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_LoadComponentParameters(engine_index, component_name.c_str(),data.c_str());
 }
 else
 if(cmd == "Model_GetComponentParameters")
 {
  ReturnString=MModel_GetComponentParameters(engine_index, component_name.c_str());
  if(ReturnString)
  {
   return_value=0;
  }
  else
   return_value=2002;
 }
 else
 if(cmd == "Model_GetComponentsNameList")
 {
  ReturnString=MModel_GetComponentsNameList(engine_index, component_name.c_str());
  if(ReturnString)
  {
   return_value=0;
  }
  else
   return_value=2002;
 }
 else
 if(cmd == "Model_GetComponentStates")
 {
  ReturnString=MModel_GetComponentState(engine_index, component_name.c_str());
  if(ReturnString)
  {
   return_value=0;
  }
  else
   return_value=2002;
 }
 else
 if(cmd == "Model_SetComponentParameters")
 {
  xml.SelectNodeRoot(std::string("RpcRequest/Data/")+component_name);
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_SetComponentParameters(engine_index, component_name.c_str(),data.c_str());
 }
 else
 if(cmd == "Model_SetComponentState")
 {
  xml.SelectNodeRoot(std::string("RpcRequest/Data/")+component_name);
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_SetComponentState(engine_index, component_name.c_str(),data.c_str());
 }
 else
  return_value=2001;

 result.SelectRoot();
 if(ReturnString)
 {
  result.WriteString("Data",ReturnString);
  MEngine_FreeBufString(engine_index,ReturnString);
 }
 else
  result.WriteString("Data","");
 result.WriteInteger("Res",return_value);
 MLockEngine(engine_index);
 std::string &buffer=GetEngine(engine_index)->CreateTempString();
 result.Save(buffer);
 MUnLockEngine(engine_index);

 return buffer.c_str();
}

const char* RDK_CALL PtzRemoteCall(const char *request, int &return_value, int &channel_index)
{
 const char* ReturnString=0;
 return_value=2001;

 channel_index=0;

 RDK::USerStorageXML xml,xml_data;
 USerStorageXML response;

 xml.Load(request,"RpcRequest");

 int engine_index=-1;
 std::string cmd;
 std::string camera;

 if(!ExtractCmd(xml,cmd) || cmd.empty())
 {
  return_value=2000;
  return 0;
 }

 if(!ExtractCC(xml,engine_index, camera))
 {
  if(!ExtractChannel(xml,engine_index))
   return 0;
  ExtractComponent(xml,camera);
 }

 if(engine_index < 0 || engine_index >= GetNumEngines())
 {
  return_value=2001;
  return 0;
 }
 channel_index=engine_index;

 response.Create("RpcResponse");
 response.WriteString("Id", xml.ReadString("Id",""));

 if(cmd == "Ptz_GetCameraNames")
 {
  ReturnString=Ptz_GetCameraNames(engine_index);
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetCameraType")
 {
  ReturnString=Ptz_GetCameraType(engine_index,camera.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetCameraTypes")
 {
  ReturnString=Ptz_GetCameraTypes(engine_index);
  return_value=0;
 }
 else
 if(cmd == "Ptz_AddCamera")
 {
  std::string camera_type=xml.ReadString("CameraType","");
  return_value=Ptz_AddCamera(engine_index,camera.c_str(),camera_type.c_str());
 }
 else
 if(cmd == "Ptz_DelCamera")
 {
  return_value=Ptz_DelCamera(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_DelAllCameras")
 {
  return_value=Ptz_DelAllCameras(engine_index);
 }
 else
 if(cmd == "Ptz_GetCameraParameter")
 {
  std::string param_name=xml.ReadString("Parameter","");
  ReturnString=Ptz_GetCameraParameter(engine_index,camera.c_str(),param_name.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetCameraParameter")
 {
  std::string param_name=xml.ReadString("Parameter","");
  std::string value=xml.ReadString("Value","");
  return_value=Ptz_SetCameraParameter(engine_index,camera.c_str(),param_name.c_str(),value.c_str());
 }
 else
 if(cmd == "Ptz_GetImplementedCommands")
 {
  ReturnString=Ptz_GetImplementedCommands(engine_index,camera.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_CameraConnect")
 {
  return_value=Ptz_CameraConnect(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_CameraDisconnect")
 {
  return_value=Ptz_CameraDisconnect(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_IsCmdImplemented")
 {
  std::string cmd_name=xml.ReadString("CommandName","");
  response.WriteString("Data",sntoa(Ptz_IsCmdImplemented(engine_index,camera.c_str(),cmd_name.c_str())));
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetImplementedMoveParamsList")
 {
  ReturnString=Ptz_GetImplementedMoveParamsList(engine_index,camera.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_IsMoveParamImplemented")
 {
  std::string param_name=xml.ReadString("ParamName","");
  response.WriteString("Data",sntoa(Ptz_IsMoveParamImplemented(engine_index,camera.c_str(),param_name.c_str())));
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetMoveParamImplemented")
 {
  std::string param_name=xml.ReadString("ParamName","");
  bool param_value=xml.ReadBool("ParamValue",false);
  if(Ptz_SetMoveParamImplemented(engine_index,camera.c_str(),param_name.c_str(),param_value))
   return_value=0;
  else
   return_value=11000; // Исходная функция вернула false
 }
 else
 if(cmd == "Ptz_GetMoveParamMinNativeValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  response.WriteString("Data",sntoa(Ptz_GetMoveParamMinNativeValue(engine_index,camera.c_str(),param_name.c_str())));
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetMoveParamMinNativeValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  double param_value=xml.ReadFloat("ParamValue",0.0);
  if(Ptz_SetMoveParamMinNativeValue(engine_index,camera.c_str(),param_name.c_str(),param_value))
   return_value=0;
  else
   return_value=11000; // Исходная функция вернула false
 }
 else
 if(cmd == "Ptz_GetMoveParamMaxNativeValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  response.WriteString("Data",sntoa(Ptz_GetMoveParamMaxNativeValue(engine_index,camera.c_str(),param_name.c_str())));
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetMoveParamMaxNativeValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  double param_value=xml.ReadFloat("ParamValue",0.0);
  if(Ptz_SetMoveParamMaxNativeValue(engine_index,camera.c_str(),param_name.c_str(),param_value))
   return_value=0;
  else
   return_value=11000; // Исходная функция вернула false
 }
 else
 if(cmd == "Ptz_GetMoveParamMinValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  response.WriteString("Data",sntoa(Ptz_GetMoveParamMinValue(engine_index,camera.c_str(),param_name.c_str())));
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetMoveParamMinValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  double param_value=xml.ReadFloat("ParamValue",0.0);
  if(Ptz_SetMoveParamMinValue(engine_index,camera.c_str(),param_name.c_str(),param_value))
   return_value=0;
  else
   return_value=11000; // Исходная функция вернула false
 }
 else
 if(cmd == "Ptz_GetMoveParamMaxValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  response.WriteString("Data",sntoa(Ptz_GetMoveParamMaxValue(engine_index,camera.c_str(),param_name.c_str())));
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetMoveParamMaxValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  double param_value=xml.ReadFloat("ParamValue",0.0);
  if(Ptz_SetMoveParamMaxValue(engine_index,camera.c_str(),param_name.c_str(),param_value))
   return_value=0;
  else
   return_value=11000; // Исходная функция вернула false
 }
 else
 if(cmd == "Ptz_Stop")
 {
  return_value=Ptz_Stop(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_GotoHome")
 {
  return_value=Ptz_GotoHome(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_PresetPoint")
 {
  int point_index=xml.ReadInteger("Point",0);
  return_value=Ptz_PresetPoint(engine_index,camera.c_str(),point_index);
 }
 else
 if(cmd == "Ptz_GotoPoint")
 {
  int point_index=xml.ReadInteger("Point",0);
  return_value=Ptz_GotoPoint(engine_index,camera.c_str(),point_index);
 }
 else
 if(cmd == "Ptz_RemovePoint")
 {
  int point_index=xml.ReadInteger("Point",0);
  return_value=Ptz_RemovePoint(engine_index,camera.c_str(),point_index);
 }
 else
 if(cmd == "Ptz_GetNumPresetPoints")
 {
  int value=0;
  return_value=Ptz_GetNumPresetPoints(engine_index,camera.c_str(),value);
  response.WriteInteger("NumPresetPoints",value);
 }
 else
 if(cmd == "Ptz_SetPresetPointName")
 {
  int point_index=xml.ReadInteger("Point",0);
  std::string name=xml.ReadString("PointName","");
  return_value=Ptz_SetPresetPointName(engine_index,camera.c_str(),point_index,name.c_str());
 }
 else
 if(cmd == "Ptz_GetPresetPointsList")
 {
  const char *str=Ptz_GetPresetPointsList(engine_index,camera.c_str());
  if(str)
  {
   return_value=0;
   response.WriteString("PresetPointsList",str);
  }
  else
   return_value=11000;
 }
 else
 if(cmd == "Ptz_ReadPTZPosition")
 {
  double pan,tilt,zoom;
  unsigned long long pan_time_stamp=0, tilt_time_stamp=0, zoom_time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPTZPosition(engine_index,camera.c_str(),pan, tilt, zoom,
		pan_time_stamp, tilt_time_stamp, zoom_time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
  response.WriteFloat("Zoom",zoom);
  response.WriteFloat("PanTimeStamp",pan_time_stamp);
  response.WriteFloat("TiltTimeStamp",tilt_time_stamp);
  response.WriteFloat("ZoomTimeStamp",zoom_time_stamp);
 }
 else
 if(cmd == "Ptz_ReadPTPosition")
 {
  double pan,tilt;
  unsigned long long pan_time_stamp=0, tilt_time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPTPosition(engine_index,camera.c_str(),pan, tilt, pan_time_stamp, tilt_time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
  response.WriteFloat("PanTimeStamp",pan_time_stamp);
  response.WriteFloat("TiltTimeStamp",tilt_time_stamp);
 }
 else
 if(cmd == "Ptz_ReadPanPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPanPosition(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",value);
  response.WriteFloat("PanTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadTiltPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadTiltPosition(engine_index,camera.c_str(),value,time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Tilt",value);
  response.WriteFloat("TiltTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadZoomPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadZoomPosition(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Zoom",value);
  response.WriteFloat("ZoomTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadFocusPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadFocusPosition(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Focus",value);
  response.WriteFloat("FocusTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadIrisPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadIrisPosition(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Iris",value);
  response.WriteFloat("IrisTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadBrightnessPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadBrightnessPosition(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Brightness",value);
  response.WriteFloat("BrightnessTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadPTZPositionNative")
 {
  double pan,tilt,zoom;
  unsigned long long pan_time_stamp=0, tilt_time_stamp=0, zoom_time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPTZPositionNative(engine_index,camera.c_str(),pan, tilt, zoom,
			pan_time_stamp, tilt_time_stamp, zoom_time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
  response.WriteFloat("Zoom",zoom);
  response.WriteFloat("PanTimeStamp",pan_time_stamp);
  response.WriteFloat("TiltTimeStamp",tilt_time_stamp);
  response.WriteFloat("ZoomTimeStamp",zoom_time_stamp);
 }
 else
 if(cmd == "Ptz_ReadPTPositionNative")
 {
  double pan,tilt;
  unsigned long long time_stamp=0;
  unsigned long long pan_time_stamp=0, tilt_time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPTPositionNative(engine_index,camera.c_str(),pan, tilt, pan_time_stamp, tilt_time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
  response.WriteFloat("PanTimeStamp",pan_time_stamp);
  response.WriteFloat("TiltTimeStamp",tilt_time_stamp);
 }
 else
 if(cmd == "Ptz_ReadPanPositionNative")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPanPositionNative(engine_index,camera.c_str(),value, time_stamp,async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",value);
  response.WriteFloat("PanTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadTiltPositionNative")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadTiltPositionNative(engine_index,camera.c_str(),value, time_stamp,async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Tilt",value);
  response.WriteFloat("TiltTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadZoomPositionNative")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadZoomPositionNative(engine_index,camera.c_str(),value, time_stamp,async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Zoom",value);
  response.WriteFloat("ZoomTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadFocusPositionNative")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadFocusPositionNative(engine_index,camera.c_str(),value, time_stamp,async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Focus",value);
  response.WriteFloat("FocusTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadIrisPositionNative")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadIrisPositionNative(engine_index,camera.c_str(),value, time_stamp,async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Iris",value);
  response.WriteFloat("IrisTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadBrightnessPositionNative")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadBrightnessPositionNative(engine_index,camera.c_str(),value, time_stamp,async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Brightness",value);
  response.WriteFloat("BrightnessTimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_MovePTZ")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=Ptz_MovePTZ(engine_index,camera.c_str(),pan_speed,tilt_speed,zoom_speed);
 }
 else
 if(cmd == "Ptz_MovePT")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  return_value=Ptz_MovePT(engine_index,camera.c_str(),pan_speed,tilt_speed);
 }
 else
 if(cmd == "Ptz_MoveDirection")
 {
  int direction=xml.ReadFloat("Direction",0);
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveDirection(engine_index,camera.c_str(),(TPtzDirection)direction,speed);
 }
 else
 if(cmd == "Ptz_MovePan")
 {
  double pan_speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MovePan(engine_index,camera.c_str(),pan_speed);
 }
 else
 if(cmd == "Ptz_MoveTilt")
 {
  double tilt_speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveTilt(engine_index,camera.c_str(),tilt_speed);
 }
 else
 if(cmd == "Ptz_MoveZoom")
 {
  double zoom_speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveZoom(engine_index,camera.c_str(),zoom_speed);
 }
 else
 if(cmd == "Ptz_MoveFocus")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveFocus(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_MoveIris")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveIris(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_MoveBrightness")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveBrightness(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_MovePTZNative")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=Ptz_MovePTZNative(engine_index,camera.c_str(),pan_speed,tilt_speed,zoom_speed);
 }
 else
 if(cmd == "Ptz_MovePTNative")
 {
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  return_value=Ptz_MovePTNative(engine_index,camera.c_str(),pan_speed,tilt_speed);
 }
 else
 if(cmd == "Ptz_MoveDirectionNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  int direction=xml.ReadFloat("Direction",0);
  return_value=Ptz_MoveDirectionNative(engine_index,camera.c_str(),(TPtzDirection)direction, speed);
 }
 else
 if(cmd == "Ptz_MovePanNative")
 {
  double pan_speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MovePanNative(engine_index,camera.c_str(),pan_speed);
 }
 else
 if(cmd == "Ptz_MoveTiltNative")
 {
  double tilt_speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveTiltNative(engine_index,camera.c_str(),tilt_speed);
 }
 else
 if(cmd == "Ptz_MoveZoomNative")
 {
  double zoom_speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveZoomNative(engine_index,camera.c_str(),zoom_speed);
 }
 else
 if(cmd == "Ptz_MoveFocusNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveFocusNative(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_MoveIrisNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveIrisNative(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_MoveBrightnessNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_MoveBrightnessNative(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_Rain")
 {
  double state=xml.ReadFloat("State",0.0);
  return_value=Ptz_Rain(engine_index,camera.c_str(),state);
 }
 else
 if(cmd == "Ptz_Light")
 {
  double state=xml.ReadFloat("State",0.0);
  return_value=Ptz_Light(engine_index,camera.c_str(),state);
 }
 else
 if(cmd == "Ptz_AutoFocus")
 {
  double state=xml.ReadFloat("State",0.0);
  return_value=Ptz_AutoFocus(engine_index,camera.c_str(),state);
 }
 else
 if(cmd == "Ptz_AutoIris")
 {
  double state=xml.ReadFloat("State",0.0);
  return_value=Ptz_AutoIris(engine_index,camera.c_str(),state);
 }
 else
 if(cmd == "Ptz_AutoBrightness")
 {
  double state=xml.ReadFloat("State",0.0);
  return_value=Ptz_AutoBrightness(engine_index,camera.c_str(),state);
 }
 else
 if(cmd == "Ptz_SetRelayState")
 {
  int id=xml.ReadInteger("Index",0);
  int value=xml.ReadInteger("Value",0);
  return_value=Ptz_SetRelayState(engine_index,camera.c_str(),id,value);
 }
 else
 if(cmd == "Ptz_SetPowerOn")
 {
  int value=xml.ReadInteger("Value",0);
  return_value=Ptz_SetPowerOn(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_SetPowerOff")
 {
  int value=xml.ReadInteger("Value",0);
  return_value=Ptz_SetPowerOff(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_SetZoomSpeed")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_SetZoomSpeed(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_SetFocusSpeed")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  return_value=Ptz_SetFocusSpeed(engine_index,camera.c_str(),speed);
 }
 else
 if(cmd == "Ptz_SetPTZ")
 {
  double pan=xml.ReadFloat("Pan",0.0);
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt=xml.ReadFloat("Tilt",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  double zoom=xml.ReadFloat("Zoom",0.0);
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=Ptz_SetPTZ(engine_index,camera.c_str(),pan,tilt,zoom,pan_speed,tilt_speed,zoom_speed);
 }
 else
 if(cmd == "Ptz_SetPT")
 {
  double pan=xml.ReadFloat("Pan",0.0);
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt=xml.ReadFloat("Tilt",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  return_value=Ptz_SetPT(engine_index,camera.c_str(),pan,tilt,pan_speed,tilt_speed);
 }
 else
 if(cmd == "Ptz_SetPan")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetPan(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetTilt")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetTilt(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetPTZNative")
 {
  double pan=xml.ReadFloat("Pan",0.0);
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt=xml.ReadFloat("Tilt",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  double zoom=xml.ReadFloat("Zoom",0.0);
  double zoom_speed=xml.ReadFloat("ZoomSpeed",0.0);
  return_value=Ptz_SetPTZNative(engine_index,camera.c_str(),pan,tilt,zoom,pan_speed,tilt_speed,zoom_speed);
 }
 else
 if(cmd == "Ptz_SetPTNative")
 {
  double pan=xml.ReadFloat("Pan",0.0);
  double pan_speed=xml.ReadFloat("PanSpeed",0.0);
  double tilt=xml.ReadFloat("Tilt",0.0);
  double tilt_speed=xml.ReadFloat("TiltSpeed",0.0);
  return_value=Ptz_SetPTNative(engine_index,camera.c_str(),pan,tilt,pan_speed,tilt_speed);
 }
 else
 if(cmd == "Ptz_SetPanNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetPanNative(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetTiltNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetTiltNative(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetZoom")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetZoom(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetZoomNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetZoomNative(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetFocus")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetFocus(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetFocusNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetFocusNative(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetIris")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetIris(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetIrisNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetIrisNative(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetBrightness")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetBrightness(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SetBrightnessNative")
 {
  double speed=xml.ReadFloat("Speed",0.0);
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetBrightnessNative(engine_index,camera.c_str(),value,speed);
 }
 else
 if(cmd == "Ptz_SyncPanAbsolutePosition")
 {
  return_value=Ptz_SyncPanAbsolutePosition(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_SyncTiltAbsolutePosition")
 {
  return_value=Ptz_SyncTiltAbsolutePosition(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_SyncPTAbsolutePosition")
 {
  return_value=Ptz_SyncPTAbsolutePosition(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_ReadSyncPanState")
 {
  double value;
  unsigned long long time_stamp=0;

  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadSyncPanState(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadSyncTiltState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadSyncTiltState(engine_index,camera.c_str(),value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadSyncPTState")
 {
  double sync_pan_state, sync_tilt_state;
  unsigned long long pan_time_stamp=0, tilt_time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadSyncPTState(engine_index,camera.c_str(),sync_pan_state, sync_tilt_state, pan_time_stamp, tilt_time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("SyncPanState",sync_pan_state);
  response.WriteFloat("SyncTiltState",sync_tilt_state);
  response.WriteFloat("PanTimeStamp",pan_time_stamp);
  response.WriteFloat("TiltTimeStamp",tilt_time_stamp);
 }
 else
 if(cmd == "Ptz_SetStabilizationState")
 {
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetStabilizationState(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_ReadStabilizationState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadStabilizationState(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ResetServo")
 {
  return_value=Ptz_ResetServo(engine_index,camera.c_str());
 }
 else
 if(cmd == "Ptz_SetAbsoluteLatitudePosition")
 {
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetAbsoluteLatitudePosition(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_ReadAbsoluteLatitudePosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadAbsoluteLatitudePosition(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_SetAbsoluteNorthPosition")
 {
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetAbsoluteNorthPosition(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_ReadAbsoluteNorthPosition")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadAbsoluteNorthPosition(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadPanGyroDriftState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPanGyroDriftState(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadTiltGyroDriftState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadTiltGyroDriftState(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_SetPanGyroDriftCorrection")
 {
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetPanGyroDriftCorrection(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_SetTiltGyroDriftCorrection")
 {
  double value=xml.ReadFloat("Value",0.0);
  return_value=Ptz_SetTiltGyroDriftCorrection(engine_index,camera.c_str(),value);
 }
 else
 if(cmd == "Ptz_SetSerialLineMode")
 {
  int line_number, rate, data_bits, stop_bits;
  bool parity;
  line_number=xml.ReadInteger("LineNumber",0);
  rate=xml.ReadInteger("Rate",0);
  data_bits=xml.ReadInteger("DataBits",0);
  stop_bits=xml.ReadInteger("StopBits",0);
  parity=xml.ReadInteger("Parity",0);
  return_value=Ptz_SetSerialLineMode(engine_index,camera.c_str(), line_number, rate, data_bits, stop_bits, parity);
 }
 else
 if(cmd == "Ptz_SendDataToSerialLine")
 {
  int line_number, size;
  char * data=0;
  line_number=xml.ReadInteger("LineNumber",0);
  size=xml.ReadInteger("DataSize",0);
  //data_bits=xml.ReadInteger("DataBits",0);
  return_value=Ptz_SendDataToSerialLine(engine_index,camera.c_str(), line_number, size, data);
 }
 else
 if(cmd == "Ptz_ReadDataFromSerialLine")
 {
  int line_number=xml.ReadInteger("LineNumber",0);
  double value;
  char* data=0;
  int data_size=0;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadDataFromSerialLine(engine_index,camera.c_str(), line_number, data_size, data, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("DataSize",data_size);
  response.WriteFloat("TimeStamp",time_stamp);
 }
/* else
 if(cmd == "Ptz_ReadPanGyroDriftState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPanGyroDriftState(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }*/
 else
 if(cmd == "Ptz_ReadPanErrorState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadPanErrorState(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
 if(cmd == "Ptz_ReadTiltErrorState")
 {
  double value;
  unsigned long long time_stamp=0;
  int async_mode=xml.ReadInteger("AsyncMode",0);
  return_value=Ptz_ReadTiltErrorState(engine_index,camera.c_str(), value, time_stamp, async_mode);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Value",value);
  response.WriteFloat("TimeStamp",time_stamp);
 }
 else
  return_value=2001;

 response.SelectRoot();
 response.WriteInteger("Res",return_value);
 if(ReturnString)
 {
  response.WriteString("Data",ReturnString);
  MEngine_FreeBufString(engine_index, ReturnString);
 }

// MLockEngine(engine_index);
 UELockPtr<UEngine> eng=GetEngineLock(engine_index);
 std::string &buffer=eng->CreateTempString();
 response.Save(buffer);
// MUnLockEngine(engine_index);

 return buffer.c_str();
}



}

#endif

