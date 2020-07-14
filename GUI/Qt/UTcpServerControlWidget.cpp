#ifndef UTcpServerControlWidget_CPP
#define UTcpServerControlWidget_CPP

#include "UTcpServerControlWidget.h"
#include "ui_UTcpServerControlWidget.h"
#include <QTcpSocket>


/// Экзепляр класса приложения
extern RDK::UApplication RdkApplication;

/// Экземпляр класса контроллера расчета
//extern UEngineControlVcl RdkEngineControl;


UServerTransportTcpQt::UServerTransportTcpQt()
{
    server=new QTcpServer();
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
 QHostAddress ha;
 ha.setAddress(QString(interface_address.c_str()));
 server->listen(ha, port);
 server->close();
}

//Получение адреса интерфейса управления сервером
std::string UServerTransportTcpQt::GetServerBindingInterfaceAddress()
{
 QString s = server->serverAddress().toString();
 std::string ss = s.toUtf8().constData();
 return ss;
}

//Получение адреса интерфейса управления сервером
int UServerTransportTcpQt::GetServerBindingPort(void) const
{
 return server->serverPort();
}

///Инициировать остановку сервера, отключить все приемники
void UServerTransportTcpQt::ServerStop()
{
 server->close();
}

/// Инициировать запуск сервера
void UServerTransportTcpQt::ServerStart()
{
 QHostAddress sa = server->serverAddress();
 qint16 sp = server->serverPort();
 server->listen(sa,sp);
}

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
int UServerTransportTcpQt::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
{
 if(server->isListening()&& !bind.empty())
 {
  try
  {
   if(server->hasPendingConnections())
   {
    bool stop=false;
    while(!stop)
    {
     QTcpSocket *socket = server->nextPendingConnection();
     if(socket==nullptr)
      break;

     QString peer_name = socket->peerName();
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
  }
  catch (...)
  {
   ///UServerControlForm->IdTCPServer->Contexts->UnlockList();
   throw;
   return 0;//?????
  }
 }
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
 return 0;
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
    if(server->hasPendingConnections())
    {
     bool stop=false;
     while(!stop)
     {
      QTcpSocket *socket = server->nextPendingConnection();
      if(socket==nullptr)
       break;
      QString peer_name = socket->peerName();
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








UTcpServerControlWidget::UTcpServerControlWidget(QWidget *parent, RDK::UApplication *app) :
  UVisualControllerWidget(parent, app),
  ui(new Ui::UTcpServerControlWidget)
{
  ui->setupUi(this);
  UpdateInterval = 0;
  setAccessibleName("UTcpServerControlWidget");
}

UTcpServerControlWidget::~UTcpServerControlWidget()
{
  delete ui;
}

/// обновление интерфейса
void UTcpServerControlWidget::AUpdateInterface()
{

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
