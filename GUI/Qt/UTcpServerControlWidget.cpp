#ifndef UTcpServerControlWidget_CPP
#define UTcpServerControlWidget_CPP

#include "UTcpServerControlWidget.h"
#include "ui_UTcpServerControlWidget.h"
#include "UGEngineControllWidget.h"


/// Ёкзепл€р класса приложени€
extern RDK::UApplication RdkApplication;

//==========================================================================================

UTcpServerControlWidget::UTcpServerControlWidget(QWidget *parent, RDK::UApplication *app) :
  UVisualControllerWidget(parent, app),
  ui(new Ui::UTcpServerControlWidget)
{
  ui->setupUi(this);
  UpdateInterval = 0;
  setAccessibleName("UTcpServerControlWidget");

  /*
  tcpCommandTimer = new QTimer(this);
  tcpCommandTimer->setInterval(10);
  connect(tcpCommandTimer, SIGNAL(timeout()), this, SLOT(TcpCommandTimerTick()));
  tcpCommandTimer->setSingleShot(false);
  tcpCommandTimer->start();
  */

  tcpServerRestartTimer = new QTimer(this);
  tcpServerRestartTimer->setInterval(1000);
  connect(tcpServerRestartTimer, SIGNAL(timeout()), this, SLOT(TcpServerRestartTimerTick()));
  tcpServerRestartTimer->setSingleShot(false);//«десь не уверен

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

/*
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
*/

void UTcpServerControlWidget::TcpServerRestartTimerTick()
{
 //¬от тут должна быть еще проверка на то что все уже запущено
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
