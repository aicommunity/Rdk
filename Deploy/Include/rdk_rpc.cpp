#ifndef RDK_RPC_CPP
#define RDK_RPC_CPP

#include "rdk_rpc.h"
#include "rdk.h"


namespace RDK{

std::vector<std::string> RpcReturnString;

/// ���������� ����� �� ����� ���������� � ������� ������
/// ���� ��������� �������� "CC"
/// ���������� ��������� ������������ �����
/// ����� ����: ������_������@��� ����������
bool ExtractCC(USerStorageXML &xml, int &channel_index, std::string &component_name)
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

/// ���������� ��� ����������
/// ���� ��������� �������� "Component"
bool ExtractComponent(USerStorageXML &xml, std::string &component_name)
{
 if(!xml.SelectNode("Component"))
  return false;
 component_name=xml.GetNodeText();
 xml.SelectUp();
 return true;
}

/// ���������� ������� ������
/// ���� ��������� �������� "Channel"
bool ExtractChannel(USerStorageXML &xml, int &channel_index)
{
 if(!xml.SelectNode("Channel"))
  return false;
 channel_index=atoi(xml.GetNodeText());
 xml.SelectUp();
 return true;
}

/// ���������� ��� �������
/// ���� ��������� �������� "Cmd"
bool ExtractCmd(USerStorageXML &xml, std::string &cmd_name)
{
 if(!xml.SelectNode("Cmd"))
  return false;
 cmd_name=xml.GetNodeText();
 xml.SelectUp();
 return true;
}

/// <RPC_Request>
///     <Channel>������ ������</Channel>
///     <Cmd>��� ���������� �������</Cmd>
///     <Component>��� ����������</Component>
///     <Class>��� ������</Class>
///     <Data>xml-�������� ������ �������, �������� xml � ����������� ����������</Data>
/// </RPC_Request>
const char* RemoteCallInternal(const char *request, int &return_value)
{
 return_value=INT_MAX;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int engine_index=-1;
 std::string cmd;
 std::string component_name;


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
 else
  RpcReturnString[engine_index].clear();

// std::map<std::string,std::vector<char> >::const_iterator I;
// int res=0;
 std::string response,temp;
 std::string name;

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
  const char * data=MModel_GetComponentParameters(engine_index, component_name.c_str());
  if(data)
  {
   RpcReturnString[engine_index]=data;
   return_value=0;
  }
  else
   return_value=2002;
 }
 else
 if(cmd == "Model_GetComponentStates")
 {
  const char * data=MModel_GetComponentState(engine_index, component_name.c_str());
  if(data)
  {
   RpcReturnString[engine_index]=data;
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

 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
 result.WriteString("Data",RpcReturnString[engine_index]);
 result.WriteInteger("Res",return_value);
 result.Save(RpcReturnString[engine_index]);

 return RpcReturnString[engine_index].c_str();
}

const char* PtzRemoteCall(const char *request, int &return_value)
{
 return_value=2001;

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
 else
 {
  if(RpcReturnString.size()<=engine_index)
   RpcReturnString.resize(engine_index+1);
  RpcReturnString[engine_index].clear();
 }

 RpcReturnString[engine_index].clear();
 response.Create("RpcResponse");

 if(cmd == "Ptz_GetCameraNames")
 {
  RpcReturnString[engine_index]=Ptz_GetCameraNames(engine_index);
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetCameraType")
 {
  RpcReturnString[engine_index]=Ptz_GetCameraType(engine_index,camera.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetCameraTypes")
 {
  RpcReturnString[engine_index]=Ptz_GetCameraTypes(engine_index);
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
  RpcReturnString[engine_index]=Ptz_GetCameraParameter(engine_index,camera.c_str(),param_name.c_str());
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
  RpcReturnString[engine_index]=Ptz_GetImplementedCommands(engine_index,camera.c_str());
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
  RpcReturnString[engine_index]=sntoa(Ptz_IsCmdImplemented(engine_index,camera.c_str(),cmd_name.c_str()));
  return_value=0;
 }
 else
 if(cmd == "Ptz_GetImplementedMoveParamsList")
 {
  RpcReturnString[engine_index]=Ptz_GetImplementedMoveParamsList(engine_index,camera.c_str());
  return_value=0;
 }
 else
 if(cmd == "Ptz_IsMoveParamImplemented")
 {
  std::string param_name=xml.ReadString("ParamName","");
  RpcReturnString[engine_index]=sntoa(Ptz_IsMoveParamImplemented(engine_index,camera.c_str(),param_name.c_str()));
  return_value=0;
 }
 else
 if(cmd == "Ptz_SetMoveParamImplemented")
 {
  std::string param_name=xml.ReadString("ParamName","");
  bool param_value=xml.ReadBool("ParamValue","");
  if(Ptz_SetMoveParamImplemented(engine_index,camera.c_str(),param_name.c_str(),param_value))
   return_value=0;
  else
   return_value=11000; // �������� ������� ������� false
 }
 else
 if(cmd == "Ptz_GetMoveParamMinNativeValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  RpcReturnString[engine_index]=sntoa(Ptz_GetMoveParamMinNativeValue(engine_index,camera.c_str(),param_name.c_str()));
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
   return_value=11000; // �������� ������� ������� false
 }
 else
 if(cmd == "Ptz_GetMoveParamMaxNativeValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  RpcReturnString[engine_index]=sntoa(Ptz_GetMoveParamMaxNativeValue(engine_index,camera.c_str(),param_name.c_str()));
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
   return_value=11000; // �������� ������� ������� false
 }
 else
 if(cmd == "Ptz_GetMoveParamMinValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  RpcReturnString[engine_index]=sntoa(Ptz_GetMoveParamMinValue(engine_index,camera.c_str(),param_name.c_str()));
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
   return_value=11000; // �������� ������� ������� false
 }
 else
 if(cmd == "Ptz_GetMoveParamMaxValue")
 {
  std::string param_name=xml.ReadString("ParamName","");
  RpcReturnString[engine_index]=sntoa(Ptz_GetMoveParamMaxValue(engine_index,camera.c_str(),param_name.c_str()));
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
   return_value=11000; // �������� ������� ������� false
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
 if(cmd == "Ptz_ReadPTZPosition")
 {
  double pan,tilt,zoom;
  return_value=Ptz_ReadPTZPosition(engine_index,camera.c_str(),pan, tilt, zoom);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
  response.WriteFloat("Zoom",zoom);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadPTPosition")
 {
  double pan,tilt;
  return_value=Ptz_ReadPTPosition(engine_index,camera.c_str(),pan, tilt);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadPanPosition")
 {
  double value;
  return_value=Ptz_ReadPanPosition(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadTiltPosition")
 {
  double value;
  return_value=Ptz_ReadTiltPosition(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Tilt",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadZoomPosition")
 {
  double value;
  return_value=Ptz_ReadZoomPosition(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Zoom",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadFocusPosition")
 {
  double value;
  return_value=Ptz_ReadFocusPosition(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Focus",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadIrisPosition")
 {
  double value;
  return_value=Ptz_ReadIrisPosition(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Iris",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadBrightnessPosition")
 {
  double value;
  return_value=Ptz_ReadBrightnessPosition(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Brightness",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadPTZPositionNative")
 {
  double pan,tilt,zoom;
  return_value=Ptz_ReadPTZPositionNative(engine_index,camera.c_str(),pan, tilt, zoom);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
  response.WriteFloat("Zoom",zoom);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadPTPositionNative")
 {
  double pan,tilt;
  return_value=Ptz_ReadPTPositionNative(engine_index,camera.c_str(),pan, tilt);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",pan);
  response.WriteFloat("Tilt",tilt);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadPanPositionNative")
 {
  double value;
  return_value=Ptz_ReadPanPositionNative(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Pan",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadTiltPositionNative")
 {
  double value;
  return_value=Ptz_ReadTiltPositionNative(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Tilt",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadZoomPositionNative")
 {
  double value;
  return_value=Ptz_ReadZoomPositionNative(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Zoom",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadFocusPositionNative")
 {
  double value;
  return_value=Ptz_ReadFocusPositionNative(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Focus",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadIrisPositionNative")
 {
  double value;
  return_value=Ptz_ReadIrisPositionNative(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Iris",value);
//  response.Save(RpcReturnString[engine_index]);
 }
 else
 if(cmd == "Ptz_ReadBrightnessPositionNative")
 {
  double value;
  return_value=Ptz_ReadBrightnessPositionNative(engine_index,camera.c_str(),value);
  response.SelectNodeRoot("RpcResponse/Data/Position");
  response.WriteFloat("Brightness",value);
//  response.Save(RpcReturnString[engine_index]);
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

 response.SelectRoot();
 response.WriteString("Id", xml.ReadString("Id",""));
 response.WriteInteger("Res",return_value);
 if(!RpcReturnString[engine_index].empty())
 {
  response.WriteString("Data",RpcReturnString[engine_index]);
 }
 response.Save(RpcReturnString[engine_index]);

 return RpcReturnString[engine_index].c_str();
}



}

#endif

