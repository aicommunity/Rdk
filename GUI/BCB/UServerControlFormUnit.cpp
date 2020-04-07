//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UServerControlFormUnit.h"
#include "UGEngineControlFormUnit.h"
//#ifdef RDK_VIDEO
//#include "TVideoGrabberControlFormUnit.h"
//#endif
#include "UImagesFormUnit.h"
#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#ifdef RDK_VIDEO
#include "VideoOutputFormUnit.h"
#endif
#include "TIdTcpResultBroadcasterFormUnit.h"
#include "TUBitmap.h"
#ifdef DVA_GEVISCOPE
#include "TGeViScopeResultBroadcasterFormUnit.h"
#endif
#include "../../Deploy/Include/rdk_cpp_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUHttpServerUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"

#include <fstream>

using namespace RDK;

TUServerControlForm *UServerControlForm;

/// Экзепляр класса приложения
extern RDK::UApplication RdkApplication;

/// Экземпляр класса контроллера расчета
extern UEngineControlVcl RdkEngineControl;

//Задает адрес и порт входящего интерфейса сервера
void UServerTransportTcpVcl::SetServerBinding(std::string &interface_address, int port)
{
 if(interface_address == GetServerBindingInterfaceAddress() &&
	port == GetServerBindingPort())

 UServerControlForm->SetServerBinding(interface_address, port);
}

//Получение адреса интерфейса управления сервером
std::string UServerTransportTcpVcl::GetServerBindingInterfaceAddress()
{
 return UServerControlForm->GetServerBindingInterfaceAddress();
}

//Получение адреса интерфейса управления сервером
int UServerTransportTcpVcl::GetServerBindingPort(void) const
{
 return UServerControlForm->GetServerBindingPort();
}

///Инициировать остановку сервера, отключить все приемники
void UServerTransportTcpVcl::ServerStop()
{
 UServerControlForm->ServerStop();
}

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
int UServerTransportTcpVcl::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
{
 if(UServerControlForm->IdTCPServer->Active && !bind.empty())
 {
  try
  {
   TList *list=UServerControlForm->IdTCPServer->Contexts->LockList();
   std::string current_bind;
   for(int i=0;i<list->Count;i++)
   {
	TIdContext *context=static_cast<TIdContext*>(list->Items[i]);
	current_bind=AnsiString(context->Binding->PeerIP).c_str();
	current_bind+=":";
	current_bind+=RDK::sntoa(context->Binding->PeerPort);

	if(current_bind == bind)
	{
	  /// Это все уедет в транспорт, в платформозависимую часть
	  TIdBytes VBuffer;
	  int length=context->Connection->IOHandler->InputBuffer->Size;
	  if(length>0)
	  {
	   context->Connection->IOHandler->ReadBytes(VBuffer, length);
	   length=VBuffer.Length;
	   bytes.resize(length);
	   memcpy(&bytes[0],&VBuffer[0],length);
	   bytes.resize(length);
	   Log_LogMessage(RDK_EX_DEBUG, (std::string("Data received from: ")+bind+std::string(" size (bytes)=")+sntoa(length)).c_str());
	   //Отпустить список
	   UServerControlForm->IdTCPServer->Contexts->UnlockList();
	   //Вернуть длину
	   return length;
	   break;
	  }
	}
   }

   UServerControlForm->IdTCPServer->Contexts->UnlockList();
   return 0;
  }
  catch (...)
  {
   UServerControlForm->IdTCPServer->Contexts->UnlockList();
   return 0;//?????
   throw;
  }
 }
 return 0;
}

/// Отправить ответ на команду соответствующему получателю
void UServerTransportTcpVcl::SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr)
{
  TByteDynArray arr;
  arr.set_length(buffer.size());
  memcpy(&arr[0],&buffer[0],buffer.size());

 if(UServerControlForm->IdTCPServer->Active && !responce_addr.empty())
 {
  try
  {
   TList *list=UServerControlForm->IdTCPServer->Contexts->LockList();
   std::string current_bind;
   for(int i=0;i<list->Count;i++)
   {
	TIdContext *context=static_cast<TIdContext*>(list->Items[i]);
	current_bind=AnsiString(context->Binding->PeerIP).c_str();
	current_bind+=":";
	current_bind+=RDK::sntoa(context->Binding->PeerPort);

	if(current_bind == responce_addr)
	{
	 context->Connection->IOHandler->Write(arr, arr.get_length());
	 //context->Connection->IOHandler->WriteBufferFlush();  //Это было закомменчено до меня
	}
   }

   UServerControlForm->IdTCPServer->Contexts->UnlockList();
  }
  catch (...)
  {
   UServerControlForm->IdTCPServer->Contexts->UnlockList();
   throw;
  }
 }
}


// --------------------------
// Методы управления вещателями
// --------------------------
/// Регистрирует удаленный приемник метаданных
int UServerControlVcl::RegisterMetadataReceiver(const std::string &address, int port)
{
 UnRegisterMetadataReceiver(address, port);
 TIdTcpResultBroadcasterFrame *broadcaster=IdTcpResultBroadcasterForm->FindBroadcasterFrame(address,port);
 if(broadcaster)
 {

 }
 else
 {
  IdTcpResultBroadcasterForm->AddBroadcaster();
  broadcaster=IdTcpResultBroadcasterForm->GetBroadcasterFrame(IdTcpResultBroadcasterForm->GetNumBroadcasters()-1);

  if(broadcaster)
  {
   string bind2=address;
   bind2+=":";
   bind2+=sntoa(port);
   broadcaster->ServerAddressLabeledEdit->Text=bind2.c_str();
   broadcaster->XmlComponentNameLabeledEdit->Text=MetaComponentName.c_str();//"Pipeline1.AggrPureOutput";
   broadcaster->XmlComponentStateNameLabeledEdit->Text=MetaComponentStateName.c_str();
   broadcaster->EnableXmlTranslationCheckBox->Checked=true;
   broadcaster->ConnectButtonClick(UServerControlForm);
   Log_LogMessage(RDK_EX_INFO, (std::string("Metadata receiver registered: ")+bind2).c_str());
  }
 }

 return 0;
}

/// Удаляет удаленный приемник метаданных
int UServerControlVcl::UnRegisterMetadataReceiver(const std::string &address, int port)
{
 TIdTcpResultBroadcasterFrame *broadcaster=IdTcpResultBroadcasterForm->FindBroadcasterFrame(address,port);
 if(broadcaster)
 {
  broadcaster->EnableXmlTranslationCheckBox->Checked=false;
 // broadcaster->ConnectButtonClick(this);
  int index=IdTcpResultBroadcasterForm->GetBroadcasterFrameIndex(broadcaster);
  if(index >= 0)
  {
   IdTcpResultBroadcasterForm->DelBroadcaster(index);
  }
  Log_LogMessage(RDK_EX_INFO, (std::string("Metadata receiver unregistered: ")+address+string(":")+sntoa(port)).c_str());
 }

 return 0;
}
// --------------------------


// --------------------------
/// Управление числом каналов
/// Выполнение вспомогательных методов
/// Вызывается из UApplication
// --------------------------
bool UServerControlVcl::ASetNumChannels(int old_num)
{
 int num=Core_GetNumChannels();
 //if(num<=0)
 // return false;

#ifdef RDK_VIDEO
 if(RdkApplication.GetProjectConfig().ProjectMode == 1 && VideoOutputForm)
 {
  if(VideoOutputForm->GetNumSources()<num)
  {
   for(int i=VideoOutputForm->GetNumSources();i<num;i++)
   {
	VideoOutputForm->AddSource();
	VideoOutputForm->GetVideoOutputFrame(i)->Init(0);
//	VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->VideoGrabberControlFrame->PipeUidEdit->Text=(std::string("USharedMemory")+RDK::sntoa(i)).c_str();
//	VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->VideoGrabberControlFrame->PipeIndexEdit->Text=IntToStr(i);
//    VideoOutputForm->GetVideoOutputFrame(i)->MyVideoGrabberControlForm->CloseButtonClick(this);
   }
   VideoOutputForm->UpdateInterface();
  }
  else
  {
   while(VideoOutputForm->GetNumSources()>num)
   {
	VideoOutputForm->DelSource(VideoOutputForm->GetNumSources()-1);
   }
  }

  if(IdTcpResultBroadcasterForm->GetNumBroadcasters()<num)
  {
   for(int i=IdTcpResultBroadcasterForm->GetNumBroadcasters();i<num;i++)
   {
	IdTcpResultBroadcasterForm->AddBroadcaster();

	TIdTcpResultBroadcasterFrame *frame=IdTcpResultBroadcasterForm->GetBroadcasterFrame(i);
	if(frame)
	{
	 frame->ChannelIndexLabeledEdit->Text=IntToStr(i);
	 frame->ServerAddressLabeledEdit->Text=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->ServerAddressLabeledEdit->Text;
	 frame->XmlComponentNameLabeledEdit->Text=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentNameLabeledEdit->Text;
	 frame->XmlComponentStateNameLabeledEdit->Text=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentStateNameLabeledEdit->Text;
	 frame->EnableXmlTranslationCheckBox->Checked=IdTcpResultBroadcasterForm->GetBroadcasterFrame(0)->EnableXmlTranslationCheckBox->Checked;
	 frame->EnableXmlTranslationCheckBoxClick(UServerControlForm);
	}
   }
   IdTcpResultBroadcasterForm->UpdateInterface();
  }
  else
  {
   while(IdTcpResultBroadcasterForm->GetNumBroadcasters()>num)
   {
	IdTcpResultBroadcasterForm->DelBroadcaster(IdTcpResultBroadcasterForm->GetNumBroadcasters()-1);
   }
  }
 }
#endif

#ifdef DVA_GEVISCOPE
  if(GeViScopeResultBroadcasterForm->GetNumBroadcasters()<value)
  {
   for(int i=GeViScopeResultBroadcasterForm->GetNumBroadcasters();i<value;i++)
   {
	GeViScopeResultBroadcasterForm->AddBroadcaster();

	TGeViScopeResultBroadcasterFrame *frame=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(i);
	if(frame)
	{
	 frame->ChannelIndexLabeledEdit->Text=IntToStr(i);
	 frame->ServerAddressLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->ServerAddressLabeledEdit->Text;
	 frame->UsernameLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->UsernameLabeledEdit->Text;
	 frame->PasswordLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->PasswordLabeledEdit->Text;
	 frame->MediaChannelLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->MediaChannelLabeledEdit->Text;
	 frame->XmlComponentNameLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentNameLabeledEdit->Text;
	 frame->XmlComponentStateNameLabeledEdit->Text=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->XmlComponentStateNameLabeledEdit->Text;
	 frame->EnableXmlTranslationCheckBox->Checked=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(0)->EnableXmlTranslationCheckBox->Checked;
	 frame->EnableXmlTranslationCheckBoxClick(this);
	}
   }
   GeViScopeResultBroadcasterForm->UpdateInterface();
  }
  else
  {
   while(GeViScopeResultBroadcasterForm->GetNumBroadcasters()>value)
   {
	GeViScopeResultBroadcasterForm->DelBroadcaster(GeViScopeResultBroadcasterForm->GetNumBroadcasters()-1);
   }
  }
#endif

// ChannelNames.resize(value);

 //UEngineMonitorForm->EngineMonitorFrame->SetNumChannels(value);

// AAfterReset();
 RDK::UIVisualControllerStorage::UpdateInterface();
 return true;
}

bool UServerControlVcl::AInsertChannel(int index)
{
 return true;
}

bool UServerControlVcl::ADeleteChannel(int index)
{
 return true;
}
// --------------------------

// --------------------------
// Вспомогательные методы
// --------------------------
// Метод, вызываемый после сброса модели
void UServerControlVcl::AfterReset(void)
{
 UServerControl::AfterReset();
}

// Метод, вызываемый после шага расчета
void UServerControlVcl::AfterCalculate(void)
{
 UServerControl::AfterCalculate();
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderCommonVcl::URpcDecoderCommonVcl(void)
{

}

URpcDecoderCommonVcl::~URpcDecoderCommonVcl(void)
{

}
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
bool URpcDecoderCommonVcl::IsCmdSupported(const RDK::UEPtr<RDK::URpcCommand> &command) const
{
 return URpcDecoderCommon::IsCmdSupported(command);
}

/// Создает копию этого декодера
URpcDecoderCommonVcl* URpcDecoderCommonVcl::New(void)
{
 return new URpcDecoderCommonVcl;
}

std::string URpcDecoderCommonVcl::ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int channel_index, int &return_value)
{
 ControlResponseString.clear();
 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
 if(cmd == "GetChannelVideoSource")
 {
#ifdef RDK_VIDEO
 TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_index);
 if(!frame)
 {
  return_value=1;
 }
 else
 {
  result.WriteString("Mode",RDK::sntoa(frame->CaptureThread->GetSourceMode()));
  result.SelectNodeForce("Data");
  result.WriteString("Mode",RDK::sntoa(frame->CaptureThread->GetSourceMode()));
  frame->CaptureThread->SaveParametersEx(result);
  result.SelectUp();
  return_value=0;
 }
#else
  return_value=1;
#endif
 }
 else
 if(cmd == "SetChannelVideoSource")
 {
#ifdef RDK_VIDEO
  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_index);
  if(!frame)
   return_value=1;
  int mode=xml.ReadInteger("Mode",0);
  if(!xml.SelectNode("Data"))
  {
   return_value=1;
  }
  else
  {
   mode=xml.ReadInteger("Mode",mode);
   frame->Init(mode,xml);
//   frame->Init(mode);
//   frame->CaptureThread->LoadParametersEx(xml);
   xml.SelectUp();
   return_value=0;//SetChannelVideoSource(channel_index,mode);
  }
#endif
 }
 else
 if(cmd == "CheckChannelVideoSourceConnection")
 {
	result.WriteInteger("State",UServerControlForm->CheckChannelVideoSourceConnection(channel_index));
	return_value=0;
 }
 else
 if(cmd == "GetChannelBroacaster")
 {
  int type=xml.ReadInteger("BroadcasterType",0);
  result.SelectNodeForce("Data");
#ifdef DVA_GEVISCOPE
  if(type == 1000)
  {
	TGeViScopeResultBroadcasterFrame *frame=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(channel_index);
	if(frame)
	{
	 result.WriteString("ChannelIndex",AnsiString(frame->ChannelIndexLabeledEdit->Text).c_str());
	 result.WriteString("ServerUrl",AnsiString(frame->ServerAddressLabeledEdit->Text).c_str());
	 result.WriteString("Username",AnsiString(frame->UsernameLabeledEdit->Text).c_str());
	 result.WriteString("Password",AnsiString(frame->PasswordLabeledEdit->Text).c_str());
	 result.WriteString("MediaChannel",AnsiString(frame->MediaChannelLabeledEdit->Text).c_str());
	 result.WriteString("XmlComponentName",AnsiString(frame->XmlComponentNameLabeledEdit->Text).c_str());
	 result.WriteString("XmlComponentStateName",AnsiString(frame->XmlComponentStateNameLabeledEdit->Text).c_str());
	 result.WriteBool("EnableXmlTranslationFlag",frame->EnableXmlTranslationCheckBox->Checked);
	}
  }
#endif
  result.SelectUp();
  return_value=0;
 }
 else
 if(cmd == "SetChannelBroadcaster")
 {
  int type=xml.ReadInteger("BroadcasterType",0);
  result.WriteInteger("BroadcasterType",type);
  if(!xml.SelectNode("Data"))
  {
   return_value=1;
  }
  else
  {
#ifdef DVA_GEVISCOPE
  if(type == 1000)
  {
	TGeViScopeResultBroadcasterFrame *frame=GeViScopeResultBroadcasterForm->GetBroadcasterFrame(channel_index);
	if(frame)
	{
	 frame->DisconnectButtonClick(this);
	 frame->ChannelIndexLabeledEdit->Text=xml.ReadString("ChannelIndex",AnsiString(frame->ChannelIndexLabeledEdit->Text).c_str()).c_str();
	 frame->ServerAddressLabeledEdit->Text=xml.ReadString("ServerUrl",AnsiString(frame->ServerAddressLabeledEdit->Text).c_str()).c_str();
	 frame->UsernameLabeledEdit->Text=xml.ReadString("Username",AnsiString(frame->UsernameLabeledEdit->Text).c_str()).c_str();
	 frame->PasswordLabeledEdit->Text=xml.ReadString("Password",AnsiString(frame->PasswordLabeledEdit->Text).c_str()).c_str();
	 frame->MediaChannelLabeledEdit->Text=xml.ReadString("MediaChannel",AnsiString(frame->MediaChannelLabeledEdit->Text).c_str()).c_str();
	 frame->XmlComponentNameLabeledEdit->Text=xml.ReadString("XmlComponentName",AnsiString(frame->XmlComponentNameLabeledEdit->Text).c_str()).c_str();
	 frame->XmlComponentStateNameLabeledEdit->Text=xml.ReadString("XmlComponentStateName",AnsiString(frame->XmlComponentStateNameLabeledEdit->Text).c_str()).c_str();
	 frame->EnableXmlTranslationCheckBox->Checked=xml.ReadBool("EnableXmlTranslationFlag",frame->EnableXmlTranslationCheckBox->Checked);
	 frame->EnableXmlTranslationCheckBoxClick(this);
	 frame->ConnectButtonClick(this);
	}
  }
#endif
   result.SelectUp();
  }
  return_value=0;
 }
 else
 if(cmd == "ReadImageFromVideoSource")
 {
#ifdef RDK_VIDEO
  int video_source_index=channel_index;
  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_index);
  if(!frame)
   return_value=2;

  double time_stamp;
  frame->CaptureThread->ReadSourceSafe(UServerControlForm->TempUBitmap,time_stamp,false);

  UServerControlForm->TempUBitmap>>UServerControlForm->Bitmap;

  int type=xml.ReadInteger("Type",0);
  binary_data.clear();
  if(type == 0)
  {
   UServerControlForm->MemStream->Clear();
   UServerControlForm->Bitmap->SaveToStream(UServerControlForm->MemStream);
   UServerControlForm->MemStream->Position=0;
   binary_data.resize(1);
   binary_data[0].resize(UServerControlForm->MemStream->Size);
   if(binary_data[0].size()>0)
	UServerControlForm->MemStream->ReadBuffer(&binary_data[0][0],binary_data[0].size());
  }
  else
  if(type == 1)
  {
   int quality=xml.ReadInteger("Quality",75);
   TJPEGImage *jpeg=new TJPEGImage;
   jpeg->Assign(UServerControlForm->Bitmap);
   jpeg->CompressionQuality=quality;
   jpeg->Compress();
   UServerControlForm->MemStream->Clear();
   jpeg->SaveToStream(UServerControlForm->MemStream);
   UServerControlForm->MemStream->Position=0;
   binary_data.resize(1);
   binary_data[0].resize(UServerControlForm->MemStream->Size);
   if(binary_data[0].size()>0)
	UServerControlForm->MemStream->ReadBuffer(&binary_data[0][0],binary_data[0].size());
   delete jpeg;
  }

  return_value=0;
#else
   return_value=1;
#endif
 }
 else
 if(cmd == "StartVideoSource")
 {
#ifdef RDK_VIDEO
  TVideoOutputFrame* frame=VideoOutputForm->GetVideoOutputFrame(channel_index);
  if(frame)
  {
   frame->StartButtonClick(UServerControlForm);
   return_value=0;
  }
  else
   return_value=1;
#else
   return_value=1;
#endif
 }
 else
 if(cmd == "StopVideoSource")
 {
#ifdef RDK_VIDEO
  TVideoOutputFrame* frame=VideoOutputForm->GetVideoOutputFrame(channel_index);
  if(frame)
  {
   frame->StopButtonClick(UServerControlForm);
   return_value=0;
  }
  else
   return_value=1;
#else
   return_value=1;
#endif
 }
 else
  return_value=2001;

 return ControlResponseString;

}
// --------------------------

//---------------------------------------------------------------------------
__fastcall TUServerControlForm::TUServerControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 MemStream=0;

 Bitmap=0;

}

__fastcall TUServerControlForm::~TUServerControlForm(void)
{

}

/// Отправляет ответ на команду
/*void TUServerControlForm::SendCommandResponse(TIdContext *context, UParamT &dest, std::vector<RDK::UParamT> &binary_data)
{
 UTransferPacket packet;

 packet.SetNumParams(1+binary_data.size());
 packet.SetParamSize(0,dest.size());
 packet.SetParam(0,dest);
 for(size_t i=0;i<binary_data.size();i++)
 {
  packet.SetParamSize(i+1,binary_data[i].size());
  packet.SetParam(i+1,binary_data[i]);
 }
 RDK::UParamT buffer;
 packet.Save(buffer);

  TByteDynArray arr;
  arr.set_length(buffer.size());
  memcpy(&arr[0],&buffer[0],buffer.size());

	 context->Connection->IOHandler->Write(arr, arr.get_length());
//	 context->Connection->IOHandler->WriteBufferFlush();
	 std::string str;
	str.resize(packet.GetParamSize(0));
	memcpy(&str[0],&(packet.operator ()((0),0)), packet.GetParamSize(0));
	Log_LogMessage(RDK_EX_DEBUG,(string("Response Sent: ")+str).c_str());
} */


//void TUServerControlForm::SendCommandResponse(const std::string &client_binding, UParamT &dest, std::vector<RDK::UParamT> &binary_data)
//{
 ///Это все напрямую улетит в транспорт, когда будет готов его прототип и дочерний класс
 ///видимо будет размещен в этом файле, ведь транспорт только про содержимое
 ///не должен знать, а адрес отправителя - очень даже
 /*
 if(IdTCPServer->Active && !dest.empty())
 {
  try
  {
   TList *list=IdTCPServer->Contexts->LockList();
   std::string current_bind;
   for(int i=0;i<list->Count;i++)
   {
	TIdContext *context=static_cast<TIdContext*>(list->Items[i]);
	current_bind=AnsiString(context->Binding->PeerIP).c_str();
	current_bind+=":";
	current_bind+=RDK::sntoa(context->Binding->PeerPort);

	if(current_bind == client_binding)
	{
	 SendCommandResponse(context, dest, binary_data);
	 Log_LogMessage(RDK_EX_DEBUG,(string("Response Sent: ")+string(" To: ")+current_bind).c_str());
	 break;
	}
   }

   IdTCPServer->Contexts->UnlockList();
  }
  catch (...)
  {
   IdTCPServer->Contexts->UnlockList();
   throw;
  }
 }
 */
//}

/// Устанавливает параметры сервера
bool TUServerControlForm::SetServerBinding(const std::string &interface_address, int port)
{
 //TODO: Это теперь должно вызываться только напрямую из транвпорта

 IdTCPServer->Active=false;
 IdTCPServer->Bindings->Items[0]->Port=port;
 IdTCPServer->Bindings->Items[0]->IP=interface_address.c_str();

 //02.04.2020 Закомментировано
 //if(RdkApplication.GetServerControl()->GetAutoStartFlag())
 // ServerStartButtonClick(this);

 return true;
}

/// Возвращает параметры сервера
//Вызов только через интерфейс транспорта, потом мб уберется совсем
std::string TUServerControlForm::GetServerBindingInterfaceAddress(void)
{

 return AnsiString(IdTCPServer->Bindings->Items[0]->IP).c_str();
}
//Вызов только через интерфейс транспорта, потом мб уберется совсем
int TUServerControlForm::GetServerBindingPort(void) const
{
 return IdTCPServer->Bindings->Items[0]->Port;
}


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUServerControlForm::AAfterReset(void)
{
// RDK::dynamic_pointer_cast<UServerControlVcl>(RdkApplication.GetServerControl())->AfterReset();
}

// Метод, вызываемый после шага расчета
void TUServerControlForm::AAfterCalculate(void)
{
// RDK::dynamic_pointer_cast<UServerControlVcl>(RdkApplication.GetServerControl())->AfterCalculate();
}

// Обновление интерфейса
void TUServerControlForm::AUpdateInterface(void)
{
 PerformanceChart->Series[0]->Clear();
 PerformanceChart->Series[1]->Clear();
 PerformanceChart->Series[2]->Clear();

 const std::vector<RDK::ULongTime> &model_avg=RdkApplication.GetServerControl()->GetModelAvg(),
							&transport_avg=RdkApplication.GetServerControl()->GetTransportAvg();

 for(size_t i=0;i<model_avg.size();i++)
 {
  PerformanceChart->Series[0]->AddY(model_avg[i]);
  PerformanceChart->Series[1]->AddY(transport_avg[i]);
  PerformanceChart->Series[2]->AddY(model_avg[i]+transport_avg[i]);
 }


 ChannelNamesStringGrid->RowCount=RdkApplication.GetProjectConfig().ChannelsConfig.size()+1;
 if(ChannelNamesStringGrid->RowCount>1)
  ChannelNamesStringGrid->FixedRows=1;

 ChannelNamesStringGrid->Cells[0][0]="#";
 ChannelNamesStringGrid->Cells[1][0]="Channel Name";
 ChannelNamesStringGrid->ColWidths[0]=25;
 ChannelNamesStringGrid->ColWidths[1]=ChannelNamesStringGrid->Width-ChannelNamesStringGrid->ColWidths[0]-25;
 for(int i=0;i<int(RdkApplication.GetProjectConfig().ChannelsConfig.size());i++)
 {
  ChannelNamesStringGrid->Cells[0][i+1]=StrToInt(i);
  ChannelNamesStringGrid->Cells[1][i+1]=RdkApplication.GetProjectConfig().ChannelsConfig[i].ChannelName.c_str();
 }

 NumberOfChannelsLabeledEdit->Text=IntToStr(Core_GetNumChannels());
 ServerNameLabeledEdit->Text=RdkApplication.GetServerControl()->GetServerName().c_str();
 ServerIdLabeledEdit->Text=RdkApplication.GetServerControl()->GetServerId().c_str();
 ServerControlPortLabeledEdit->Text=IdTCPServer->Bindings->Items[0]->Port;
 BindingAddressLabeledEdit->Text=IdTCPServer->Bindings->Items[0]->IP;

 MetadataComponentNameLabeledEdit->Text=RdkApplication.GetServerControl()->GetMetaComponentName().c_str();
 MetadataComponentStateNameLabeledEdit->Text=RdkApplication.GetServerControl()->GetMetaComponentStateName().c_str();

 if(IdTCPServer->Active==false)
 {
  TcpServerIndicatorPanel->Color=clRed;
  TcpServerIndicatorLabel->Caption = "Tcp server stopped";
 }
 else
 {
  TList *list=IdTCPServer->Contexts->LockList();
  if(list->Count>0)
  {
   bool connected = false;
   for(int i=0; i<list->Count; i++)
   {
	TIdContext *AContext = reinterpret_cast<TIdContext *>(list->Items[i]);
	if(AContext->Connection->Connected())
	{
	 connected=true;
	 break;
	}
   }

   if(connected)
   {
	TcpServerIndicatorPanel->Color=clLime;
	TcpServerIndicatorLabel->Caption = "Tcp server active and connected";
   }
   else
   {
	TcpServerIndicatorPanel->Color=clGreen;
	TcpServerIndicatorLabel->Caption = "Tcp server waiting for connection";
   }
  }
  else
  {
   TcpServerIndicatorPanel->Color=clGreen;
   TcpServerIndicatorLabel->Caption = "Tcp server waiting for connection";
  }
  IdTCPServer->Contexts->UnlockList();
 }
}

// Возврат интерфейса в исходное состояние
void TUServerControlForm::AClearInterface(void)
{
 ServerStopButtonClick(this);
}


// Сохраняет параметры интерфейса в xml
void TUServerControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
/*
 xml.WriteInteger("AverageIterations",RdkApplication.GetServerControl()->GetAverageIterations());
// xml.WriteInteger("ServerControlPort", GetServerBindingPort());
// xml.WriteString("ServerControlAddress", GetServerBindingInterfaceAddress());

 xml.WriteInteger("NumberOfChannels",RdkApplication.Core_GetNumChannels());
 xml.WriteInteger("AutoStartFlag",RdkApplication.GetServerControl()->GetAutoStartFlag());
 xml.WriteString("ServerName",RdkApplication.GetServerControl()->GetServerName());
 xml.WriteString("ServerId",RdkApplication.GetServerControl()->GetServerId());

 xml.WriteString("MetadataComponentName",RdkApplication.GetServerControl()->GetMetaComponentName());
 xml.WriteString("MetadataComponentStateName",RdkApplication.GetServerControl()->GetMetaComponentStateName());
                      */
 for(size_t i=0;i<RdkApplication.GetProjectConfig().ChannelsConfig.size();i++)
 {
  xml.WriteString(std::string("ChannelName_")+RDK::sntoa(i),RdkApplication.GetProjectConfig().ChannelsConfig[i].ChannelName);
 }
}

// Загружает параметры интерфейса из xml
/// TODO: Этот код должен частично мигрировать RdkApplication.GetServerControl()
void TUServerControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
 RdkApplication.GetServerControl()->SetAverageIterations(xml.ReadInteger("AverageIterations",RdkApplication.GetServerControl()->GetAverageIterations()));

 RdkApplication.GetServerControl()->SetMetaComponentName(xml.ReadString("MetadataComponentName",""));
 RdkApplication.GetServerControl()->SetMetaComponentStateName(xml.ReadString("MetadataComponentStateName",""));

 int source_num_channels=RdkApplication.GetNumChannels();//xml.ReadInteger("NumberOfChannels",RdkApplication.Core_GetNumChannels());
// RdkApplication.SetNumEngines(source_num_channels);
 TProjectConfig config=RdkApplication.GetProjectConfig();
 for(size_t i=0;i<RdkApplication.GetProjectConfig().ChannelsConfig.size();i++)
 {
  config.ChannelsConfig[i].ChannelName=xml.ReadString(std::string("ChannelName_")+RDK::sntoa(i),RDK::sntoa(i));
 }
 RdkApplication.SetProjectConfig(config);

 RdkApplication.GetServerControl()->SetAutoStartFlag(xml.ReadInteger("AutoStartFlag",true));
 RdkApplication.GetServerControl()->SetServerName(xml.ReadString("ServerName","Server"));
 RdkApplication.GetServerControl()->SetServerId(xml.ReadString("ServerId","Server"));

 UpdateInterface(true);

 if(RdkApplication.GetServerControl()->GetAutoStartFlag())
  ServerStartButtonClick(this);
}
// -----------------------------


// -----------------------------
// Обработчики команд сервера
// -----------------------------
/// Возвращает тип источника видео для канала
/// в соответствии с режимами VideoOutputFrame
int TUServerControlForm::GetChannelVideoSource(int channel_id)
{
#ifdef RDK_VIDEO
 TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
 if(!frame)
  return -1;

 return frame->CaptureThread->GetSourceMode();// MyVideoGrabberControlForm->VideoGrabberControlFrame->GetMode();
#else
 return -1;
#endif
}

/// Задает источник видео для канала
/// в соответствии с режимами VideoOutputFrame
int TUServerControlForm::SetChannelVideoSource(int channel_id, int source_mode)
{
#ifdef RDK_VIDEO
 int num=RdkApplication.GetNumChannels();
 if(channel_id<0)
 {
  for(int i=0;i<num;i++)
  {
   TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(i);
   if(!frame)
	return 1;

   frame->Init(source_mode);
//   frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(source_mode);
  }
 }
 else
 {
  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
  if(!frame)
   return 1;

  frame->Init(source_mode);
//  frame->MyVideoGrabberControlForm->VideoGrabberControlFrame->SelectMode(source_mode);
 }

 return 0;
#else
 return 1000;
#endif
}


/// Проверяет подключен ли видеоисточник
int TUServerControlForm::CheckChannelVideoSourceConnection(int channel_id)
{
#ifdef RDK_VIDEO
 int num=RdkApplication.GetNumChannels();

  TVideoOutputFrame *frame=VideoOutputForm->GetVideoOutputFrame(channel_id);
  if(!frame)
   return 0;

 return frame->CaptureThread->CheckConnection();
#else
 return 0;
#endif
}

/// Загружает проект аналитики для канала
/// или загружает проект для всех каналов, если channel_id<0
int TUServerControlForm::LoadProject(int channel_id, const std::string &project_file_name)
{
 //UGEngineControlForm->OpenProject(project_file_name.c_str());
 return 0;
}

/// Сохраняет проект
int TUServerControlForm::SaveProject(void)
{
 //Это тоже надо бы с осторожностью
 UGEngineControlForm->SaveProject();
 return 0;
}
// -----------------------------

//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormCreate(TObject *Sender)
{
// CommandQueueUnlockEvent=CreateEvent(0,TRUE,TRUE,0);
 MemStream=new TMemoryStream;
 Bitmap=new Graphics::TBitmap;
 UGEngineControlForm->SpecialForms["TUServerControlForm"]=this;
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::FormDestroy(TObject *Sender)
{
 ServerStopButtonClick(Sender);
// UGEngineControlForm->SpecialForms.erase("TUServerControlForm");

 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;

// CloseHandle(CommandQueueUnlockEvent);
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ServerStartButtonClick(TObject *Sender)
{
//return; // TODO: запуск отключен.
 ServerRestartTimer->Enabled=true;
 try
 {
  //UHttpServerFrame->ServerListenOn();
  IdTCPServer->Active=true;
 }
 catch(EIdSocketError &ex)
 {
  Log_LogMessage(RDK_EX_ERROR, AnsiString(ex.ToString()).c_str());
 }
 catch(EIdCouldNotBindSocket &ex2)
 {
  Log_LogMessage(RDK_EX_ERROR, AnsiString(ex2.ToString()).c_str());
 }
 this->UpdateInterface();
// TcpServer->Active=true;
}
//---------------------------------------------------------------------------

void TUServerControlForm::ServerStop()
{
 ServerRestartTimer->Enabled=false;

 TList *list=IdTCPServer->Contexts->LockList();
 for(int i=0;i<list->Count;i++)
 {
	TIdContext *context=static_cast<TIdContext*>(list->Items[i]);
	context->Connection->Disconnect();
 }

 IdTCPServer->Contexts->UnlockList();

 IdTCPServer->Active=false;
 this->UpdateInterface();
}

void __fastcall TUServerControlForm::ServerStopButtonClick(TObject *Sender)
{
 RdkApplication.GetServerControl()->GetServerTransport()->ServerStop();
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ReturnOptionsButtonClick(TObject *Sender)
{
 UpdateInterface(true);
 PageControlChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::ApplyOptionsButtonClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 int new_num_channels=StrToInt(NumberOfChannelsLabeledEdit->Text);
 if(new_num_channels < 1)
  return;
 ///01.04.2020 - закрываю, потому что вообще непонятно, насколько допустимо в
 ///реалиях 2020 так делать
 /*
 if(new_num_channels != Core_GetNumChannels())
 {
  UGEngineControlForm->Pause1Click(Sender);


  ///RdkApplication.SetNumChannels(new_num_channels);
  ////////////////////////////////////
//  SetNumChannels(Core_GetNumChannels());///Это было раньше закрыто
 }*/

// UHttpServerFrame->SetListenPort(StrToInt(ServerControlPortLabeledEdit->Text));

 int new_port=StrToInt(ServerControlPortLabeledEdit->Text);
 String new_address=BindingAddressLabeledEdit->Text;

 RDK::TProjectConfig config=RdkApplication.GetProjectConfig();

 config.ServerInterfaceAddress=AnsiString(new_address).c_str();
 config.ServerInterfacePort=new_port;
 RdkApplication.SetProjectConfig(config);

 ///Это наверное будет работать так:
 RdkApplication.GetServerControl()->GetServerTransport()->SetServerBinding(config.ServerInterfaceAddress,config.ServerInterfacePort);
 ///А это уберется
 ///SetServerBinding(config.ServerInterfaceAddress,config.ServerInterfacePort);

 RdkApplication.GetServerControl()->SetServerName(AnsiString(ServerNameLabeledEdit->Text).c_str());
 RdkApplication.GetServerControl()->SetServerId(AnsiString(ServerIdLabeledEdit->Text).c_str());
 RdkApplication.GetServerControl()->SetMetaComponentName(AnsiString(MetadataComponentNameLabeledEdit->Text).c_str());
 RdkApplication.GetServerControl()->SetMetaComponentStateName(AnsiString(MetadataComponentStateNameLabeledEdit->Text).c_str());

 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::ChannelNamesStringGridKeyDown(TObject *Sender,
          WORD &Key, TShiftState Shift)
{
 if(UpdateInterfaceFlag)
  return;

 if(Key == VK_RETURN)
 {
  TProjectConfig config=RdkApplication.GetProjectConfig();
  config.ChannelsConfig[ChannelNamesStringGrid->Row-1].ChannelName=AnsiString(ChannelNamesStringGrid->Cells[1][ChannelNamesStringGrid->Row]).c_str();
  RdkApplication.SetProjectConfig(config);
  UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------
void __fastcall TUServerControlForm::PageControlChange(TObject *Sender)
{
// ServerControlPortLabeledEdit->Text=IntToStr(UHttpServerFrame->GetListenPort());
 ServerControlPortLabeledEdit->Text=IdTCPServer->Bindings->Items[0]->Port;//TcpServer->LocalPort;
 NumberOfChannelsLabeledEdit->Text=IntToStr(RdkApplication.GetNumChannels());

 TProjectConfig config=RdkApplication.GetProjectConfig();
 ChannelNamesStringGrid->RowCount=config.ChannelsConfig.size()+1;
 ChannelNamesStringGrid->ColWidths[0]=20;
 ChannelNamesStringGrid->ColWidths[1]=ChannelNamesStringGrid->Width-ChannelNamesStringGrid->ColWidths[0]-20;
 ChannelNamesStringGrid->Cells[0][0]="Channel #";
 ChannelNamesStringGrid->Cells[1][0]="Channel Name";
 for(int i=0;i<int(config.ChannelsConfig.size());i++)
 {
  ChannelNamesStringGrid->Cells[0][i+1]=IntToStr(i);
  ChannelNamesStringGrid->Cells[1][i+1]=config.ChannelsConfig[i].ChannelName.c_str();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::CommandTimerTimer(TObject *Sender)
{
try
{
 RdkApplication.GetServerControl()->ProcessCommandQueue(RdkApplication.GetServerControl()->GetServerTransport());
}
catch (...)
{
 Log_LogMessage(RDK_EX_WARNING, "TUServerControlForm::CommandTimerTimer Global catcher error");
// throw;
}
// SetEvent(CommandQueueUnlockEvent);
}
//---------------------------------------------------------------------------








void __fastcall TUServerControlForm::IdTCPServerDisconnect(TIdContext *AContext)
{
 std::string bind=AnsiString(AContext->Binding->PeerIP).c_str();
 bind+=":";
 bind+=RDK::sntoa(AContext->Binding->PeerPort);

 //UServerTransport *t = RdkApplication.GetServerControl()->GetServerTransport();
 //t->DisconnectClient(bind);

 RdkApplication.GetServerControl()->GetServerTransport()->DisсonnectClient(bind);

 Log_LogMessage(RDK_EX_INFO, (std::string("Client Disconnected: ")+bind).c_str());
 this->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::IdTCPServerExecute(TIdContext *AContext)
{
// if(WaitForSingleObject(ServerReceivingNotInProgress, 10000) == WAIT_TIMEOUT)
//  return;
// ResetEvent(ServerReceivingNotInProgress);

//Хз как сделать здесь... событие не хочется пробрасывать по цепочке, через базовые классы
//по типу UServerControl, так как потеряем контекст (TIdContext *AContext)
//поэтому делаем хитрый финт ушами: сначала парсим сообщение в функции в UServerTransportTcpVcl,
//где по факту просто пробиваем его в форму, и в похожей на эту функцию парсим
//как вариант, обращаемся к компоненту форму
//потом распарсенное сообщение отдаем в UServerControl  для обработки, по необходимости
//спуская часть работы на UServerControlVcl

//Распарсить сообщение:

try
{
 std::string bind=AnsiString(AContext->Binding->PeerIP.c_str()).c_str();
 bind+=":";
 bind+=RDK::sntoa(AContext->Binding->PeerPort);
 //Убираем отсюда всю обработку, пусть это и грозит потерей контекста
 RdkApplication.GetServerControl()->ProcessIncomingData( bind, RdkApplication.GetServerControl()->GetServerTransport() );

}
catch(...)
{
Log_LogMessage(RDK_EX_DEBUG, std::string("UServerControl data processing error").c_str());
}
//  Memo1.Lines.Add(LLine);
//  AContext.Connection.IOHandler.WriteLn('OK');
//  TIdNotify.NotifyMethod( StopStartServerdMessage );
// SetEvent(ServerReceivingNotInProgress);
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::IdTCPServerConnect(TIdContext *AContext)
{
 UnicodeString b = AContext->Binding->PeerIP;
 std::string bind=AnsiString(b).c_str();
 bind+=":";
 bind+=RDK::sntoa(AContext->Binding->PeerPort);
 RdkApplication.GetServerControl()->GetServerTransport()->ConnectClient(bind);
 Log_LogMessage(RDK_EX_INFO, (std::string("Client connected: ")+bind).c_str());
 this->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUServerControlForm::ServerRestartTimerTimer(TObject *Sender)
{
 if(IdTCPServer->Active)
  return;

 if(RdkApplication.GetServerControl()->GetAutoStartFlag())
 {
  ServerStartButtonClick(Sender);
 }
}
//---------------------------------------------------------------------------


void __fastcall TUServerControlForm::FormClose(TObject *Sender, TCloseAction &Action)

{
 ServerStopButtonClick(Sender);
// Sleep(100);
}
//---------------------------------------------------------------------------


void __fastcall TUServerControlForm::IdHTTPServerAuthorization(TObject *Sender, TIdAuthentication *Authentication, bool &Handled)
{
 Log_LogMessage(RDK_EX_INFO, (std::string(AnsiString("HTTP Server authorization attemp with login: "+Authentication->Username + " and password: "+Authentication->Password).c_str()).c_str());
}
//---------------------------------------------------------------------------



