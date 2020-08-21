#ifndef UTcpServerControlWidget_CPP
#define UTcpServerControlWidget_CPP

#include "UTcpServerControlWidget.h"
#include "ui_UTcpServerControlWidget.h"
#include "UGEngineControllWidget.h"
#include <QTcpSocket>


/// Экзепляр класса приложения
extern RDK::UApplication RdkApplication;

/// Экземпляр класса контроллера расчета
//extern UEngineControlVcl RdkEngineControl;

UServerSocketQt::UServerSocketQt()
{

}

UServerSocketQt::UServerSocketQt(QTcpSocket* s)
{
    this->socket = s;
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

UServerSocketQt::~UServerSocketQt()
{

}

void UServerSocketQt::SetSocket(QTcpSocket *s)
{
  /*if(socket!=NULL)
  {
      if(socket->isOpen())
      {
          socket->close();
      }
      delete socket;
      socket=NULL;
  }*/

    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    this->socket = s;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

QTcpSocket* UServerSocketQt::GetSocket()
{
    return socket;
}

void UServerSocketQt::readyRead()
{
    //Рассчитать bind
    QString peer_name = socket->peerAddress().toString();
    QString peer_port = QString::number(socket->peerPort());
    QString b = peer_name+":"+peer_port;
    std::string bnd = b.toUtf8().constData();
    emit(onReadyRead(bnd));
}

void UServerSocketQt::disconnected()
{
    //Рассчитать bind
    QString peer_name = socket->peerAddress().toString();
    QString peer_port = QString::number(socket->peerPort());
    QString b = peer_name+":"+peer_port;
    std::string bnd = b.toUtf8().constData();
    emit(onDisconnected(bnd));
}

UServerTransportTcpQt::UServerTransportTcpQt()
{
    server=new QTcpServer();
    connect(server, SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));
}
UServerTransportTcpQt::~UServerTransportTcpQt()
{
    if(server)
    {
     delete server;
    }
}

//Задает адрес и порт входящего интерфейса сервера
void UServerTransportTcpQt::SetServerBinding(std::string &interface_address, int port)
{
 if(interface_address == GetServerBindingInterfaceAddress() && port == GetServerBindingPort())
    return;

 server_address=interface_address;
 server_port = port;

 QHostAddress ha;
 ha.setAddress(QString(interface_address.c_str()));

 server->listen(ha, port);
 server->close();
 QString s = server->serverAddress().toString();
 int i=0;
}

//Получение адреса интерфейса управления сервером
std::string UServerTransportTcpQt::GetServerBindingInterfaceAddress()
{
 if(server->isListening())
 {
  QString s = server->serverAddress().toString();
  std::string ss = s.toUtf8().constData();
  return ss;
 }
 else
 {
  return server_address;
 }
}

//Получение адреса интерфейса управления сервером
int UServerTransportTcpQt::GetServerBindingPort(void) const
{
 if(server->isListening())
 {
  int p = server->serverPort();
  return server->serverPort();
 }
 else
 {
  return server_port;
 }
}

void UServerTransportTcpQt::ServerNewConnection()
{
    if(server->hasPendingConnections())
    {
     bool stop=false;
     while(!stop)
     {
      QTcpSocket *socket = server->nextPendingConnection();
      if(socket==nullptr)
       break;

      QString peer_name = socket->peerAddress().toString();
      QString peer_port = QString::number(socket->peerPort());
      QString b = peer_name+":"+peer_port;
      std::string bnd = b.toUtf8().constData();

      bool b3 = Application!=NULL;
      bool b1 = Application->GetServerControl()!=NULL;
      bool b2 = Application->GetServerControl()->GetServerTransport()!=NULL;

      std::map<std::string, UServerSocketQt*>::iterator I = serverSockets.find(bnd);

      if(I!=serverSockets.end())
      {
        delete(I->second);
        serverSockets.erase(I);
      }

      UServerSocketQt* sqt = new UServerSocketQt(socket);
      connect(sqt, SIGNAL(onReadyRead(std::string)), this, SLOT(SocketReadyRead(std::string)));
      connect(sqt, SIGNAL(onDisconnected(std::string)), this, SLOT(SocketDisconnected(std::string)));

      serverSockets[bnd] = sqt;
      Application->GetServerControl()->GetServerTransport()->ConnectClient(bnd);
      //Application->GetServerControl()->ProcessIncomingData(bnd, Application->GetServerControl()->GetServerTransport());
     }
    }
}

void UServerTransportTcpQt::SocketReadyRead(std::string bind)
{
    Application->GetServerControl()->ProcessIncomingData(bind, Application->GetServerControl()->GetServerTransport());
}

void UServerTransportTcpQt::SocketDisconnected(std::string bind)
{
    Application->GetServerControl()->GetServerTransport()->DisconnectClient(bind);
    //Получить ссылку на сокет
    std::map<std::string, UServerSocketQt*>::iterator I = serverSockets.find(bind);
    if(I!=serverSockets.end())
    {
        delete I->second;
        serverSockets.erase(I);
    }
}

int UServerTransportTcpQt::GetSocketState(std::string bind)
{
    std::map<std::string, UServerSocketQt*>::iterator I = serverSockets.find(bind);
    if(I!=serverSockets.end())
    {
        if(I->second->GetSocket()->isValid() && I->second->GetSocket()->state()==QAbstractSocket::SocketState::ConnectedState)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return -1;
    }
}

bool UServerTransportTcpQt::ServerIsActive()
{
 server->isListening();
 return true;
}

///Инициировать остановку сервера, отключить все приемники
void UServerTransportTcpQt::ServerStop()
{
 server->pauseAccepting();
 for(std::map<std::string, UServerSocketQt*>::iterator I = serverSockets.begin(); I!=serverSockets.end(); I++)
 {
    delete I->second;
 }
 serverSockets.clear();
 server->close();
}

/// Инициировать запуск сервера
void UServerTransportTcpQt::ServerStart()
{
    if(server_address!="" && server_port!=0)
    {
        QHostAddress ha;
        ha.setAddress(QString(server_address.c_str()));

        server->listen(ha, server_port);
    }
}

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
int UServerTransportTcpQt::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
{
 if(server->isListening()&& !bind.empty())
 {
  try
  {
    //Здесь что-то вроде IdTcpServerExecute ???
    //Получить ссылку на сокет
    std::map<std::string, UServerSocketQt*>::iterator I = serverSockets.find(bind);
    if(I!=serverSockets.end())
    {
     QTcpSocket *socket = I->second->GetSocket();
     if(socket==nullptr)
      return 0;//?????

     QString peer_name = socket->peerAddress().toString();
     QString peer_port = QString::number(socket->peerPort());
     QString b = peer_name+":"+peer_port;
     if(b==QString(bind.c_str()))
     {
      QDataStream in;
      in.setDevice(socket);
      in.setVersion(QDataStream::Qt_5_0);
      in.startTransaction();
      char *buffer;
      uint length=0;
      in.readBytes(buffer, length);
      bytes.resize(length);
      memcpy(&bytes[0],buffer,length);
      Log_LogMessage(RDK_EX_DEBUG, (std::string("Data received from: ")+bind+std::string(" size (bytes)=")+QString::number(length).toUtf8().constData()).c_str());
      return length;
     }
    }
  }
  catch (...)
  {
   ///UServerControlForm->IdTCPServer->Contexts->UnlockList();
   throw;
   return 0;//?????
  }
 }
 return 0;
 /*
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
 */
}

/// Отправить ответ на команду соответствующему получателю
void UServerTransportTcpQt::SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr)
{
  /*TByteDynArray arr;
  arr.set_length(buffer.size());
  memcpy(&arr[0],&buffer[0],buffer.size());

 if(UServerControlForm->IdTCPServer->Active && !responce_addr.empty())*/

 if(server->isListening()&& !responce_addr.empty())
 {
  try
  {
     //Здесь что-то вроде IdTcpServerExecute ???
     //Получить ссылку на сокет
     std::map<std::string, UServerSocketQt*>::iterator I = serverSockets.find(responce_addr);
     if(I!=serverSockets.end())
     {
      QTcpSocket *socket = I->second->GetSocket();
      if(socket==nullptr)
       return;
      QString peer_name = socket->peerAddress().toString();
      QString peer_port = QString::number(socket->peerPort());
      QString b = peer_name+":"+peer_port;
      if(b==QString(responce_addr.c_str()))
      {
       QDataStream out;
       out.setDevice(socket);
       out.setVersion(QDataStream::Qt_5_0);
       out.startTransaction();

       char* buf;
       buf = new char[buffer.size()];
       memcpy(buf,&buffer[0],buffer.size());

       out.writeBytes(buf, buffer.size());
       return;
      }
     }
   }
   catch (...)
   {
    ///UServerControlForm->IdTCPServer->Contexts->UnlockList();
    throw;
    return;//?????
   }
  }
 /*
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
 */
}
/*
void UServerTransportTcpQt::ConnectClient(std::string &bind)
{
 UServerTransport::ConnectClient(bind);

}

void UServerTransportTcpQt::DisconnectClient(std::string &bind)
{

}
*/

//==========================================================================================

// --------------------------
// Методы управления вещателями
// --------------------------
/// Регистрирует удаленный приемник метаданных
int UServerControlQt::RegisterMetadataReceiver(const std::string &address, int port)
{
 return 0;
 /*

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
 */
 return 0;
}

/// Удаляет удаленный приемник метаданных
int UServerControlQt::UnRegisterMetadataReceiver(const std::string &address, int port)
{
 return 0;
 /*
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
 */
 return 0;
}
// --------------------------


// --------------------------
/// Управление числом каналов
/// Выполнение вспомогательных методов
/// Вызывается из UApplication
// --------------------------
bool UServerControlQt::ASetNumChannels(int old_num)
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

bool UServerControlQt::AInsertChannel(int index)
{
 return true;
}

bool UServerControlQt::ADeleteChannel(int index)
{
 return true;
}
// --------------------------

// --------------------------
// Вспомогательные методы
// --------------------------
// Метод, вызываемый после сброса модели
void UServerControlQt::AfterReset(void)
{
 UServerControl::AfterReset();
}

// Метод, вызываемый после шага расчета
void UServerControlQt::AfterCalculate(void)
{
 UServerControl::AfterCalculate();
}
// --------------------------

//==========================================================================================
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoderCommonQt::URpcDecoderCommonQt(void):
engine(0)
{

}

URpcDecoderCommonQt::~URpcDecoderCommonQt(void)
{

}
// --------------------------

// --------------------------
// Методы управления командами
// --------------------------
/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
bool URpcDecoderCommonQt::IsCmdSupported(const RDK::UEPtr<RDK::URpcCommand> &command) const
{
 if(!command || !command->IsDecoded)
  return false;

 std::string cmd=command->FunctionName;
 if(cmd == "LoadProject")
 {
  return true;
 }
 else
 if(cmd == "SaveProject")
 {
  return true;
 }
 return URpcDecoderCommon::IsCmdSupported(command);
}

/// Создает копию этого декодера
URpcDecoderCommonQt* URpcDecoderCommonQt::New(void)
{
 return new URpcDecoderCommonQt;
}

std::string URpcDecoderCommonQt::ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int channel_index, int &return_value)
{
 ControlResponseString.clear();
 RDK::USerStorageXML result;

 result.Create("RpcResponse");
 result.WriteString("Id", xml.ReadString("Id",""));
 if(cmd == "GetChannelVideoSource")
 {
  return_value=1;
 }
 else
 if(cmd == "SetChannelVideoSource")
 {
  return_value=1;
 }
 else
 if(cmd == "CheckChannelVideoSourceConnection")
 {
    //result.WriteInteger("State",UServerControlForm->CheckChannelVideoSourceConnection(channel_index));
     result.WriteInteger("State",-1);
    return_value=1;
 }
 else
 if(cmd == "GetChannelBroacaster")
 {
  int type=xml.ReadInteger("BroadcasterType",0);
  result.SelectNodeForce("Data");
  result.SelectUp();
  return_value=0;
 }
 else
 if(cmd == "SetChannelBroadcaster")
 {
  //int type=xml.ReadInteger("BroadcasterType",0);
  //result.WriteInteger("BroadcasterType",type);
  return_value=1;
 }
 else
 if(cmd == "ReadImageFromVideoSource")
 {
   return_value=1;
 }
 else
 if(cmd == "StartVideoSource")
 {
   return_value=1;
 }
 else
 if(cmd == "StopVideoSource")
 {
   return_value=1;
 }
 else
 if(cmd == "LoadProject")
 {
  /*std::string file_name=xml.ReadString("FileName","");
  if(!file_name.empty())
  {
      //GetApplication()->OpenProject(file_name);
      //RDK::UIVisualControllerStorage::UpdateInterface(true);

      if(!engine)
      {
          return_value=1;
      }
      else
      {
          UGEngineControllWidget* eng = dynamic_cast<UGEngineControllWidget*>(engine);
          eng->loadProjectExternal(QString(file_name.c_str()));
          return_value=0;
      }

      //UServerControlForm->LoadProject(channel_index,file_name);
  }*/
     return_value=1;
 }
 else
 if(cmd == "SaveProject")
 {
  //GetApplication()->SaveProject();
     /*
  if(!engine)
  {
      return_value=1;
  }
  else
  {
      UGEngineControllWidget* eng = dynamic_cast<UGEngineControllWidget*>(engine);
      eng->actionSaveConfig();
      return_value=0;
  }
  */
  //return_value=0;//UServerControlForm->SaveProject();
     return_value=1;
 }
 else
  return_value=2001;

 if(return_value=2001)
 {
    return URpcDecoderCommon::ARemoteCall(cmd, xml, component_name, channel_index, return_value);
 }

 return ControlResponseString;

}
// --------------------------

void URpcDecoderCommonQt::SetEngine(QMainWindow* e)
{
    engine = e;
}
QMainWindow* URpcDecoderCommonQt::GetEngine()
{
    return engine;
}

//==========================================================================================

UTcpServerControlWidget::UTcpServerControlWidget(QWidget *parent, RDK::UApplication *app) :
  UVisualControllerWidget(parent, app),
  ui(new Ui::UTcpServerControlWidget)
{
  ui->setupUi(this);
  UpdateInterval = 0;
  setAccessibleName("UTcpServerControlWidget");


  tcpCommandTimer = new QTimer(this);
  tcpCommandTimer->setInterval(10);
  connect(tcpCommandTimer, SIGNAL(timeout()), this, SLOT(TcpCommandTimerTick()));
  tcpCommandTimer->setSingleShot(false);
  tcpCommandTimer->start();

  tcpServerRestartTimer = new QTimer(this);
  tcpServerRestartTimer->setInterval(1000);
  connect(tcpServerRestartTimer, SIGNAL(timeout()), this, SLOT(TcpServerRestartTimerTick()));
  tcpServerRestartTimer->setSingleShot(false);//Здесь не уверен

  connect(ui->pushButtonReset, SIGNAL(clicked()), this, SLOT(PushButtonResetClicked()));
  connect(ui->pushButtonApply, SIGNAL(clicked()), this, SLOT(PushButtonApplyClicked()));
  connect(ui->pushButtonServerStop, SIGNAL(clicked()), this, SLOT(PushButtonServerStopClicked()));
  connect(ui->pushButtonServerStart, SIGNAL(clicked()), this, SLOT(PushButtonServerStartClicked()));

  ui->lineEditServerAddress->setText("127.0.0.1");
  ui->lineEditServerPort->setText("45045");
  PushButtonApplyClicked();
}

UTcpServerControlWidget::~UTcpServerControlWidget()
{
  delete ui;
}

void UTcpServerControlWidget::PushButtonResetClicked()
{
 AUpdateInterface();
}

void UTcpServerControlWidget::PushButtonApplyClicked()
{
 if(UpdateInterfaceFlag)
     return;

 int new_port = ui->lineEditServerPort->text().toInt();
 std::string new_ip = ui->lineEditServerAddress->text().toUtf8().constData();

 RDK::TProjectConfig config = application->GetProjectConfig();

 config.ServerInterfaceAddress = new_ip;
 config.ServerInterfacePort = new_port;

 application->SetProjectConfig(config);

 application->GetServerControl()->GetServerTransport()->SetServerBinding(new_ip, new_port);
}

void UTcpServerControlWidget::PushButtonServerStartClicked()
{
 PushButtonApplyClicked();
 application->GetServerControl()->GetServerTransport()->ServerStart();
 AUpdateInterface();
}

void UTcpServerControlWidget::PushButtonServerStopClicked()
{
 application->GetServerControl()->GetServerTransport()->ServerStop();
 AUpdateInterface();
}

void UTcpServerControlWidget::TcpCommandTimerTick()
{
 try
 {
    application->GetServerControl()->ProcessCommandQueue(application->GetServerControl()->GetServerTransport());
 }
 catch(...)
 {
  Log_LogMessage(RDK_EX_WARNING, "UTcpServerControlWidget::TcpCommandTimerTick() Global catcher error");
 }
}

void UTcpServerControlWidget::TcpServerRestartTimerTick()
{
 //Вот тут должна быть еще проверка на то что все уже запущено
 //но так как все переехало в класс - она дб там

 if(application->GetServerControl()->GetAutoStartFlag())
     PushButtonServerStartClicked();
}



/// обновление интерфейса
void UTcpServerControlWidget::AUpdateInterface()
{
 std::string addr = application->GetServerControl()->GetServerTransport()->GetServerBindingInterfaceAddress();
 ui->lineEditServerAddress->setText(QString(addr.c_str()));
 ui->lineEditServerPort->setText(QString::number(application->GetServerControl()->GetServerTransport()->GetServerBindingPort()));
 std::string bnd = addr+":"+ui->lineEditServerPort->text().toUtf8().constData();

 if(application->GetServerControl()->GetServerTransport().Get()!=nullptr)
 {
     bool state = application->GetServerControl()->GetServerTransport()->GetSocketState(bnd);
     if(!application->GetServerControl()->GetServerTransport()->ServerIsActive())
     {
         ui->labelServerState->setText("Server is disabled");
     }
     else
     {
         ui->labelServerState->setText("Server is active");
     }
 }
}

void UTcpServerControlWidget::ASaveParameters()
{
  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt", QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  settings.setValue("geometry", saveGeometry());
  settings.endGroup();
}

void UTcpServerControlWidget::ALoadParameters()
{
  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt", QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  restoreGeometry(settings.value("geometry").toByteArray());
  settings.endGroup();
}
#endif //UTcpServerControlWidget_CPP
