#include "RdkQTcpClientWidget.h"
#include "ui_RdkQTcpClientWidget.h"
#include <iostream>

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
    res = Rpc_Connect(1, server_addr, port);
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
    //WriteLog("")
}

/*
void RdkQTcpClientWidget::onPushButtonPingClick()
{
 int sum = Rtcl_SumTest(5,10);
 std::cerr<<"onPushButtonTestClick sum: "<<sum;
}
*/
