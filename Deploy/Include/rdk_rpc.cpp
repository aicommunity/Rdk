#ifndef RDK_RPC_CPP
#define RDK_RPC_CPP

#include "rdk_rpc.h"
#include "rdk.h"


namespace RDK{

std::vector<std::string> RpcReturnString;

/// <RPC_Request>
///     <Engine>индекс движка</Engine>
///     <Cmd>имя вызываемой функции</Cmd>
///     <Name>имя компонента</Name>
///     <Class>имя класса</Class>
///     <Data>xml-описание данных функции, например xml с параметрами компонента</Data>
/// </RPC_Request>
const char* RemoteCallInternal(const char *request, int &return_value)
{
 return_value=0;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RPC_Request");

 int engine_index=xml.ReadInteger("Engine",0);
 std::string cmd=xml.ReadString("Cmd","");

 if((engine_index < 0 || engine_index >= GetNumEngines()) &&
	(cmd != "GetNumEngines" || cmd !="SetNumEngines"))
 {
  return_value=2001;
  return 0;
 }
 else
  RpcReturnString[engine_index].clear();

 if(cmd.empty())
 {
  return_value=2000;
  return 0;
 }

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
  std::string component_name=xml.ReadString("Name","");

  return_value=MEnv_Reset(engine_index, component_name.c_str());
 }
 else
 if(cmd == "Env_Calculate")
 {
  std::string component_name=xml.ReadString("Name","");
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
  std::string component_name=xml.ReadString("Name","");
  xml.SelectNodeRoot("RPC_Request/Data/Save");
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_LoadComponent(engine_index, component_name.c_str(),data.c_str());
 }
 else
 if(cmd == "Model_LoadComponentParameters")
 {
  std::string component_name=xml.ReadString("Name","");
  xml.SelectNodeRoot("RPC_Request/Data/SaveProperties");
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_LoadComponentParameters(engine_index, component_name.c_str(),data.c_str());
 }
 else
 if(cmd == "Model_GetComponentParameters")
 {
  std::string component_name=xml.ReadString("Name","");

  const char * data=MModel_GetComponentParameters(engine_index, component_name.c_str());
  if(data)
   RpcReturnString[engine_index]=data;
  else
   return_value=2002;
 }
 else
 if(cmd == "Model_GetComponentStates")
 {
  std::string component_name=xml.ReadString("Name","");

  const char * data=MModel_GetComponentState(engine_index, component_name.c_str());
  if(data)
   RpcReturnString[engine_index]=data;
  else
   return_value=2002;
 }
 else
 if(cmd == "Model_SetComponentParameters")
 {
  std::string component_name=xml.ReadString("Name","");
  xml.SelectNodeRoot(std::string("RPC_Request/Data/")+component_name);
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_SetComponentParameters(engine_index, component_name.c_str(),data.c_str());
 }
 else
 if(cmd == "Model_SetComponentState")
 {
  std::string component_name=xml.ReadString("Name","");
  xml.SelectNodeRoot(std::string("RPC_Request/Data/")+component_name);
  std::string data;
  xml.SaveFromNode(data);

  return_value=MModel_SetComponentState(engine_index, component_name.c_str(),data.c_str());
 }

 return RpcReturnString[engine_index].c_str();
}


}

#endif

