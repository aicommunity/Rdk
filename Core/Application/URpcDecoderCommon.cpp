#ifndef URpcDecoderCommon_CPP
#define URpcDecoderCommon_CPP

#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"
#include "../../Deploy/Include/myrdk.h"
#include "URpcDecoderCommon.h"
#include "URpcCommandInternal.h"
#include "UApplication.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderCommon::URpcDecoderCommon(void)
{

}

URpcDecoderCommon::~URpcDecoderCommon(void)
{

}
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Создает копию этого декодера
URpcDecoderCommon* URpcDecoderCommon::New(void)
{
 return new URpcDecoderCommon;
}

/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
bool URpcDecoderCommon::IsCmdSupported(const UEPtr<URpcCommand> &command) const
{
 if(!command || !command->IsDecoded)
  return false;

 std::string cmd=command->FunctionName;
 if(cmd == "RpcPing")
 {
  return true;
 }
 else
 if(cmd == "SetNumChannels")
 {
  return true;
 }
 else
 if(cmd == "GetNumChannels")
 {
  return true;
 }
 else
 if(cmd == "GetServerName")
 {
  return true;
 }
 else
 if(cmd == "GetServerId")
 {
  return true;
 }
 else
 if(cmd == "GetChannelName")
 {
  return true;
 }
 else
 if(cmd == "SetChannelName")
 {
  return true;
 }
 else
 if(cmd == "GetChannelVideoSource")
 {
  return true;
 }
 else
 if(cmd == "SetChannelVideoSource")
 {
  return true;
 }
 else
 if(cmd == "CheckChannelVideoSourceConnection")
 {
  return true;
 }
 else
 if(cmd == "GetChannelBroacaster")
 {
  return true;
 }
 else
 if(cmd == "SetChannelBroadcaster")
 {
  return true;
 }
 else
 if(cmd == "ResetChannel")
 {
  return true;
 }
 else
 if(cmd == "StartChannel")
 {
  return true;
 }
 else
 if(cmd == "StopChannel")
 {
  return true;
 }
 if(cmd == "StartTraining")
 {
  return true;
 }
 else
 if(cmd == "RegisterMetadataReceiver")
 {
  return true;
 }
 else
 if(cmd == "UnRegisterMetadataReceiver")
 {
  return true;
 }
 else
 if(cmd == "LoadProject")
 {
  return true;
 }
 else
 if(cmd == "SaveProject")
 {
  return true;
 }
 else
 if(cmd == "ReadImageFromVideoSource")
 {
  return true;
 }
 else
 if(cmd == "StartVideoSource")
 {
  return true;
 }
 else
 if(cmd == "DeployProject")
 {
  return true;
 }
 else
 if(cmd == "GetDeploymentState")
 {
  return true;
 }
 else
 if(cmd == "GetLastError")
 {
  return true;
 }
 else
 if(cmd == "PrepareProject")
 {
  return true;
 }
 else
 if(cmd == "GetPreparationResult")
 {
  return true;
 }
 else
 if(cmd == "OpenPreparedProject")
 {
  return true;
 }
 else
 if(cmd == "RunPreparedProject")
 {
  return true;
 }
 else
 if(cmd == "GetCalculationState")
 {
  return true;
 } 
 else
 if(cmd == "FinishCalculation")
 {
  return true;
 }
 else
 if(cmd == "UploadCalculationResults")
 {
  return true;
 }
 else
 if(cmd == "GetUploadState")
 {
  return true;
 }
 else
 if(cmd == "CloseSolver")
 {
  return true;
 }
 else
 if(cmd == "StopVideoSource")
  return true;

 return false;
}

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
bool URpcDecoderCommon::AProcessCommand(const UEPtr<URpcCommand> &command)
{
 if(!command)
 {
  // Ошибка - команда не задана
  return false;
 }

 UEPtr<URpcCommandInternal> cmd=dynamic_pointer_cast<URpcCommandInternal>(command);
 if(!cmd)
 {
  // Ошибка - команда не поддерживается декодером
  MLog_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Common Decoder : Command internal structure not supported. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command=")+command->FunctionName).c_str());
  return false;
 }

 const char* response=RemoteCall(cmd->Request.c_str(), cmd->ResponseStatus, cmd->ChannelIndex);

 if(response)
  cmd->Response=response;
 else
  cmd->Response=RDK::sntoa(cmd->ResponseStatus);

 MEngine_FreeBufString(cmd->ChannelIndex, response);

 if(cmd->ResponseStatus == 2001)
 {
  MLog_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Common Decoder: Unknown command. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command=")+command->FunctionName).c_str());
  return false;
 }

 cmd->IsProcessed=true;
 return true;
}

const char* URpcDecoderCommon::RemoteCall(const char *request, int &return_value, int &res_channel_index)
{
 return_value=INT_MAX;

 RDK::USerStorageXML xml,xml_data;

 xml.Load(request,"RpcRequest");

 int channel_index=-1;
 std::string cmd;
 std::string component_name;
 res_channel_index=0;


 if(!ExtractCmd(xml,cmd) || cmd.empty())
 {
  return_value=2000;
  return 0;
 }

 if(!ExtractCC(xml,channel_index,component_name))
 {
  ExtractChannel(xml,channel_index);
  ExtractComponent(xml,component_name);
 }
  /*
 if((channel_index < 0 || channel_index >= GetNumEngines()) &&
	(cmd != "GetNumChannels" && cmd !="SetNumChannels"))
 {
  return_value=2001;
  return 0;
 }  */

 res_channel_index=channel_index;
 std::string response,temp;
 std::string name;

 RDK::USerStorageXML result;
 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
// if(cmd == "EngineInit")
// {
// }
// else
// {
 response=ARemoteCall(cmd,xml,component_name, channel_index, return_value);
 if(return_value == 2001)
 {
  if(cmd == "RpcPing")
  {
   return_value=0;
  }
  else
  if(cmd == "SetNumChannels")
  {
   int num_engines=xml.ReadInteger("NumChannels",Core_GetNumChannels());
   if(GetApplication()->SetNumChannels(num_engines))
	return_value=0;//UServerControlForm->SetNumChannels(num_engines);
   else
	return_value=112112;//UServerControlForm->SetNumChannels(num_engines);
  }
  else
  if(cmd == "GetNumChannels")
  {
   response=RDK::sntoa(GetApplication()->GetNumChannels());
   return_value=0;
  }
  else
  if(cmd == "GetServerName")
  {
   response=GetApplication()->GetServerControl()->GetServerName();
   return_value=0;
  }
  else
  if(cmd == "GetServerId")
  {
   response=GetApplication()->GetServerControl()->GetServerId();
   return_value=0;
  }
  else
  if(cmd == "GetChannelName")
  {
   response=GetApplication()->GetProjectConfig().ChannelsConfig[channel_index].ChannelName;
   return_value=0;
  }
  else
  if(cmd == "SetChannelName")
  {
   std::string ch_name=xml.ReadString("Name","");
   if(!ch_name.empty())
   {
	TProjectConfig config=GetApplication()->GetProjectConfig();
	config.ChannelsConfig[channel_index].ChannelName=ch_name;
	GetApplication()->SetProjectConfig(config);
   }
   return_value=0;
  }
  else
  if(cmd == "ResetChannel")
  {
   GetApplication()->ResetChannel(channel_index);
   return_value=0;
  }
  else
  if(cmd == "StartChannel")
  {
   GetApplication()->StartChannel(channel_index);
   return_value=0;
  }
  else
  if(cmd == "StopChannel")
  {
   GetApplication()->PauseChannel(channel_index);
   return_value=0;
  }
  if(cmd == "StartTraining")
  {
   std::stringstream ss;
   Model_SetComponentPropertyValue("TPyClassifierTrainer", "StartTraining", "1");
   return_value=0;
  }
  else
  if(cmd == "RegisterMetadataReceiver")
  {
   string address=xml.ReadString("Address","");
   int port=xml.ReadInteger("Port",8888);
   return_value=GetApplication()->GetServerControl()->RegisterMetadataReceiver(address, port);
  }
  else
  if(cmd == "UnRegisterMetadataReceiver")
  {
   string address=xml.ReadString("Address","");
   int port=xml.ReadInteger("Port",1000);
   return_value=GetApplication()->GetServerControl()->UnRegisterMetadataReceiver(address, port);
  }
  else
  if(cmd == "LoadProject")
  {
   std::string file_name=xml.ReadString("FileName","");
   if(!file_name.empty())
   {
       GetApplication()->OpenProject(file_name);
       RDK::UIVisualControllerStorage::UpdateInterface(true);
       return_value=0;//UServerControlForm->LoadProject(channel_index,file_name);
   }
  }
  else
  if(cmd == "SaveProject")
  {
   GetApplication()->SaveProject();
   return_value=0;//UServerControlForm->SaveProject();
  }
  else
  if(cmd == "DeployProject")
  {
   int task_id=xml.ReadInteger("TaskId",-1);
   int resp = GetApplication()->GetProjectDeployer()->StartProjectDeployment(task_id);
   std::stringstream ss;
   ss<<resp;
   response = ss.str();
   return_value=0;
  }
  else
  if(cmd == "GetDeploymentState")
  {
   //GetApplication()->SaveProject();
   int d_state = GetApplication()->GetProjectDeployer()->GetDeploymentState();
   int stage_max = GetApplication()->GetProjectDeployer()->GetStageCap();
   int stage_pos = GetApplication()->GetProjectDeployer()->GetStageProgress();

   std::string le = GetApplication()->GetProjectDeployer()->GetLastError();

   std::stringstream ss;
   ss<<d_state<<"|-|"<<stage_pos<<"|-|"<<stage_max<<"|-|"<<le;
   response=ss.str();
   return_value=0;//UServerControlForm->SaveProject();
  }
  else
  if(cmd == "GetLastError")
  {
   std::string last_error=GetApplication()->GetProjectDeployer()->GetLastError();
   response=last_error;
   return_value=0;
  }
  else
  if(cmd == "PrepareProject")
  {
    std::string resp="";
    int rs = GetApplication()->GetProjectDeployer()->PrepareProject(resp);
    response=resp;
    return_value=0;
  }
  else
  if(cmd == "GetPreparationResult")
  {
    std::string resp="";
    int rs = GetApplication()->GetProjectDeployer()->GetPreparationResult(resp);
    std::stringstream ss;
    ss<<rs<<"|-|"<<resp.c_str();
    response=ss.str();
    return_value=0;
  }
  else
  if(cmd=="OpenPreparedProject")
  {
    std::string resp="";
    std::cerr<<"Open prepared project!\n";
    int rs = GetApplication()->GetProjectDeployer()->OpenPreparedProject(resp);
    std::cerr<<"Project opened successful!\n";
    response=resp;
    return_value=0;
  }
  else
  if(cmd=="RunPreparedProject")
  {
    int rs = GetApplication()->GetProjectDeployer()->RunPreparedProject();
    std::stringstream ss;
    ss<<rs;
    response = ss.str();
    return_value=0;
  }
  else
  if(cmd == "GetCalculationState")
  {
    /*std::string resp="";
    int rs = GetApplication()->GetProjectDeployer()->GetPreparationResult(resp);
    std::stringstream ss;
    ss<<rs<<"|-|"<<resp.c_str();
    response=ss.str();*/
    //Формирует ответ такой:
    //"<calculation_state>|<capture_state>|<capture_frid>|<capture_maxfrid>|<capture_finished>|<message>"
    int calculation_state=-1;
    int capture_state=-1;
    unsigned long long capture_frid=0;
    unsigned long long capture_maxfrid=0;
    std::string message = "test state message";


    //Проанализировать состояние расчета - а вдруг ошибка
    /// Состояние тредов расчета
    /// -1 - пустое состояние
    /// 0 - запущен
    /// 1 - расчет остановлен
    /// 2 - расчет запущен, но не выполняется
    /// 3 - ошибка найдена в логах
    int calc_state = GetApplication()->GetProjectDeployer()->GetCalculationState();
    if(calc_state>=0)
    {
        calculation_state = calc_state;
        //Выгребаем логи, проверяем нет ли ошибок там:
        std::string log_err="";
        bool process_log_res = GetApplication()->GetProjectDeployer()->ProcessCalculationLog(log_err);
        if(!process_log_res)
        {
            message = "Log error: "+log_err;
            calculation_state = 3;
        }
        else
        {
            if(calc_state==2)
            {
                message = "Calculation started but not active (2).";
            }
            else if(calc_state==1 || calc_state==0)
            {
                unsigned long long capture_frame_id=0;
                unsigned long long capture_max_frame_id=0;
                int cap_state_out = -1;
                bool res_capstate = GetApplication()->GetProjectDeployer()->GetCaptureState(cap_state_out, capture_frame_id, capture_max_frame_id);
                if(!res_capstate)
                {
                    std::string err="";
                    err = GetApplication()->GetProjectDeployer()->GetLastError();
                    message = "Get capture state error: "+err;
                }
                else
                {
                    capture_state = cap_state_out;
                    capture_frid = capture_frame_id;
                    capture_maxfrid = capture_max_frame_id;
                    message = "Calculation is active";
                }
            }
            else
            {
                message = "Error: Wrong calculation state number";
            }
        }

    }
    else
    {
        std::string err="";
        err = GetApplication()->GetProjectDeployer()->GetLastError();
        message = "GetCalculationState() error: "+err;
    }

    std::stringstream res_ss;
    res_ss<<calculation_state<<"|-|"<<capture_state<<"|-|"<<capture_frid<<"|-|"<<capture_maxfrid<<"|-|"<<message.c_str();
    response = res_ss.str();
    return_value=0;
  }
  else
  if(cmd=="FinishCalculation")
  {
    bool rs = GetApplication()->GetProjectDeployer()->FinishCalculation();
    std::string le = GetApplication()->GetProjectDeployer()->GetLastError();
    std::stringstream ss;
    ss<<rs<<"|-|"<<le;
    response = ss.str();
    return_value=0;
  }
  else
  if(cmd=="UploadCalculationResults")
  {
    bool rs = GetApplication()->GetProjectDeployer()->UploadCalculationResults();
    std::string le = GetApplication()->GetProjectDeployer()->GetLastError();
    std::stringstream ss;
    ss<<rs<<"|-|"<<le;
    response = ss.str();
    return_value=0;
  }
  else
  if(cmd=="GetUploadState")
  {
    int us = GetApplication()->GetProjectDeployer()->GetUploadState();
    std::string le = GetApplication()->GetProjectDeployer()->GetLastError();
    std::stringstream ss;
    ss<<us<<"|-|"<<le;
    response = ss.str();
    return_value=0;
  }
  else
  if(cmd=="CloseSolver")
  {
    bool rs = GetApplication()->GetProjectDeployer()->CloseSolver();
    std::string le = GetApplication()->GetProjectDeployer()->GetLastError();
    std::stringstream ss;
    ss<<rs<<"|-|"<<le;
    response = ss.str();
    exit(0);
    return_value=0;
  }
 }

 result.SelectRoot();
 if(!response.empty())
 {
  result.WriteString("Data",response);
//  MEngine_FreeBufString(channel_index,response);
 }
 else
  result.WriteString("Data","");
 result.WriteInteger("Res",return_value);
 MCore_LockChannel(0);
 std::string &buffer=GetEngine(0)->CreateTempString();
 result.Save(buffer);
 MCore_UnLockChannel(0);

 return buffer.c_str();
}

std::string URpcDecoderCommon::ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int channel_index, int &return_value)
{
 return_value=2001;
 return "";
}
// --------------------------

}
#endif

