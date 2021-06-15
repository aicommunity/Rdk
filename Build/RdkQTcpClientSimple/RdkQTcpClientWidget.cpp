#include "RdkQTcpClientWidget.h"
#include "ui_RdkQTcpClientWidget.h"
#include <iostream>

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

RdkQTcpClientWidget::RdkQTcpClientWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RdkQTcpClientWidget)
{
    ui->setupUi(this);

    InitLibrary();

    ui->lineEditServerAddress->setText("127.0.0.1");
    ui->lineEditServerPort->setText("45045");

    // заполнение массива ошибок
    Errors[RDK_RPC_SERVER_NOT_EXIST]="Server not exist";
    Errors[RDK_RPC_CONNECTION_ERROR]="Connection error";
    Errors[RDK_RPC_SERVER_NOT_ACTIVE]="Server is not active";
    Errors[RDK_RPC_CLIENT_NOT_CONNECTED]="Client not connected";
    Errors[RDK_RPC_LIBRARY_NOT_INIT]="Library not init";
    Errors[RDK_RPC_RESPONSE_RECEIVE_TIMEOUT]="Exceeded response receive timeout";
    Errors[RDK_RPC_PACKET_READER_ACCESS_TIMEOUT]="Fatal error: exceeded packet reader access timeout";
    Errors[RDK_RPC_RESPONSE_NOT_RECIEVED]="Response not recieved";
    Errors[RDK_RPC_UNSUCCESSFULL_DECODING]="unsuccessfull packet decoding";

    connect(ui->pushButtonPing, SIGNAL(clicked()), this, SLOT(onPushButtonPingClick()));

    connect(ui->pushButtonConnect, SIGNAL(clicked()), this, SLOT(onPushButtonConnectClick()));
    connect(ui->pushButtonDisconnect, SIGNAL(clicked()), this, SLOT(onPushButtonDisconnectClick()));


    connect(ui->pushButtonStartChannel, SIGNAL(clicked()), this, SLOT(onPushButtonStartChannelClick()));
    connect(ui->pushButtonStopChannel, SIGNAL(clicked()), this, SLOT(onPushButtonStopChannelClick()));
    connect(ui->pushButtonResetChannel, SIGNAL(clicked()), this, SLOT(onPushButtonResetChannelClick()));
    connect(ui->pushButtonGetNumChannels, SIGNAL(clicked()), this, SLOT(onPushButtonGetNumChannelsClick()));
    connect(ui->pushButtonSaveServer, SIGNAL(clicked()), this, SLOT(onPushButtonSaveServerClick()));

    connect(ui->pushButtonGetChannelName, SIGNAL(clicked()), this, SLOT(onPushButtonGetChannelNameClick()));
    connect(ui->pushButtonSetChannelName, SIGNAL(clicked()), this, SLOT(onPushButtonSetChannelNameClick()));

    connect(ui->pushButtonLoadServerConfig, SIGNAL(clicked()), this, SLOT(onPushButtonLoadConfigClick()));

    ui->lineEditChannelIndex->setText("0");

}

RdkQTcpClientWidget::~RdkQTcpClientWidget()
{
    delete ui;
}

void RdkQTcpClientWidget::WriteLog(const QString& s)
{
    ui->plainTextEditLog->appendPlainText(GetCurrentTime()+" "+s);
}

bool RdkQTcpClientWidget::WriteLogError(int err_code)
{
 if(Errors[err_code]!="")
 {
   WriteLog(Errors[err_code]);
   return true;
 }
 return false;
}

void RdkQTcpClientWidget::onPushButtonPingClick()
{
    int res = Rpc_Ping(0, 10000);
    if(!WriteLogError(res))
    {
        WriteLog("Server is active");
    }
}

QString RdkQTcpClientWidget::GetCurrentTime()
{
 QDateTime t = QDateTime::currentDateTime();
 QString s = t.toString("dd.MM.yyyy hh:mm:ss.z");
 return s;
}

void RdkQTcpClientWidget::InitLibrary()
{
    int res = Rpc_Init(3);
}

void RdkQTcpClientWidget::onPushButtonConnectClick()
{
 std::string address = ui->lineEditServerAddress->text().toUtf8().constData();
 int port=0;
 try {
    port = ui->lineEditServerPort->text().toInt();
 } catch (...) {
    QMessageBox::warning(this, "Convert error", "Error on port reading, check contents of the field!");
    return;
 }

 if(address!="" && port!=0)
 {
    const char* server_addr = address.c_str();
    int res = Rpc_Connect(0, server_addr, port);
    //res = Rpc_Connect(1, server_addr, port);
    if(Rpc_IsClientConnected(0))
    {
        WriteLog("Client connected");
    }

    WriteLogError(res);
 }
}

void RdkQTcpClientWidget::onPushButtonDisconnectClick()
{
    int res = Rpc_Disconnect(0);
    if(!Rpc_IsClientConnected(0))
    {
        WriteLog("Client disconnected");
    }
    WriteLogError(res);
    //WriteLog("")
}

void RdkQTcpClientWidget::onPushButtonStartChannelClick()
{
    const char* serverAnswer=NULL;
    int res=Rpc_StartChannel(0, ui->lineEditChannelIndex->text().toInt(), 60000);
    if(WriteLogError(res))
    {
     serverAnswer=Rpc_GetServerAnswerDebug(0);
     if(serverAnswer != NULL && ui->checkBoxWriteServerResponse->isChecked())
         ui->plainTextEditLog->appendPlainText(QString(serverAnswer));
    }
}

void RdkQTcpClientWidget::onPushButtonStopChannelClick()
{
    const char* serverAnswer=NULL;
    int res=Rpc_StopChannel(0, ui->lineEditChannelIndex->text().toInt(), 5000);
    if(WriteLogError(res))
    {
     serverAnswer=Rpc_GetServerAnswerDebug(0);
     if(serverAnswer != NULL && ui->checkBoxWriteServerResponse->isChecked())
      ui->plainTextEditLog->appendPlainText(QString(serverAnswer));
    }
}

void RdkQTcpClientWidget::onPushButtonResetChannelClick()
{
    const char* serverAnswer=NULL;
    int res=Rpc_ResetChannel(0, ui->lineEditChannelIndex->text().toInt(), 5000);
    if(WriteLogError(res))
    {
     serverAnswer=Rpc_GetServerAnswerDebug(0);
     if(serverAnswer != NULL && ui->checkBoxWriteServerResponse->isChecked())
      ui->plainTextEditLog->appendPlainText(QString(serverAnswer));
    }
}

void RdkQTcpClientWidget::onPushButtonGetNumChannelsClick()
{
    int res=0;
    Rpc_GetNumChannels(0, res, 10000);
    ui->lineEditNumChannels->setText(QString::number(res));
}

void RdkQTcpClientWidget::onPushButtonSaveServerClick()
{
   int res=Rpc_SaveProject(0, 10000);
}

void RdkQTcpClientWidget::onPushButtonGetChannelNameClick()
{
    const char *res=0;
    Rpc_GetChannelName(0, ui->lineEditChannelIndex->text().toInt(), res, 1000);
     if(res)
      ui->lineEditChannelName->setText(QString(res));
}

void RdkQTcpClientWidget::onPushButtonSetChannelNameClick()
{
   std::string name = ui->lineEditChannelName->text().toUtf8().constData();
   Rpc_SetChannelName(0, ui->lineEditChannelIndex->text().toInt(), name.c_str(), 1000);
}

void RdkQTcpClientWidget::onPushButtonLoadConfigClick()
{
    QFileDialog fd(this);
    fd.setWindowTitle("Open config file");
    fd.setDirectory("/home/ivan");
    fd.selectNameFilter("*.ini");


    QString fileName = QFileDialog::getOpenFileName(this, tr("Open config file"), QCoreApplication::applicationDirPath(), tr("*.ini"), nullptr, QFileDialog::DontUseNativeDialog);
        if (fileName.isEmpty())
          return;

        std::string name = fileName.toUtf8().constData();
        Rpc_LoadProject(0, name.c_str(), 1000);

}


/*
void RdkQTcpClientWidget::onPushButtonPingClick()
{
 int sum = Rtcl_SumTest(5,10);
 std::cerr<<"onPushButtonTestClick sum: "<<sum;
}
*/

void RdkQTcpClientWidget::on_pushButtonStartTraining_clicked()
{
    const char* serverAnswer=NULL;
    int res=Rpc_StartTraining(0, ui->lineEditChannelIndex->text().toInt(), 60000);
    if(WriteLogError(res))
    {
     serverAnswer=Rpc_GetServerAnswerDebug(0);
     if(serverAnswer != NULL && ui->checkBoxWriteServerResponse->isChecked())
         ui->plainTextEditLog->appendPlainText(QString(serverAnswer));
    }
}
