#ifndef UServerTransportTcpQt_H
#define UServerTransportTcpQt_H

#include "../UServerTransportTcp.h"
#include "../UServerControl.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

namespace RDK{

class UServerSocketQt: public QObject
{
    Q_OBJECT
public:
    UServerSocketQt();
    UServerSocketQt(QTcpSocket* s);
    ~UServerSocketQt();

    void SetSocket(QTcpSocket *s);
    QTcpSocket* GetSocket();

    //void WriteData(/*???*/);
    //void GetIncomingData(const char* data, int& size);

signals:
    //void onConnected(std::string bind);
    void onDisconnected(std::string bind);
    void onReadyRead(std::string bind);
private slots:
    virtual void readyRead();
    virtual void disconnected();

private:
    QTcpSocket *socket;
};

class UServerTransportTcpQt: public QTcpServer, public RDK::UServerTransportTcp
{
    Q_OBJECT
public:
    UServerTransportTcpQt();
    ~UServerTransportTcpQt();
    /// ������ �������� ����� �� ���������� ���������, �������� ��������
    /// ������ ������������ ������� ��� ����������� �� ���� ����������
    virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
    /// ��������� ����� �� ������� ���������������� ����������
    virtual void SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr);
    /// ������ ����� � ���� ��������� ���������� �������
    virtual void SetServerBinding(std::string &interface_address, int port);
    /// ��������� ������ ���������� ���������� ��������
    virtual std::string GetServerBindingInterfaceAddress(void);
    /// �������� ���� �������
    int GetServerBindingPort(void) const;
    /// ������������ ��������� �������, ��������� ��� ���������
    virtual void ServerStop();
    /// ������������ ������ �������
    virtual void ServerStart();
    //virtual void ConnectClient(std::string &bind);
    //virtual void DisconnectClient(std::string &bind);

    virtual int GetSocketState(std::string bind);

    bool ServerIsActive();

    virtual void timerEvent(QTimerEvent *event);

    virtual void ProcessEventQueueExternally();

public slots:
    void ServerNewConnection();
    void SocketReadyRead(std::string bind);
    void SocketDisconnected(std::string bind);
    void CommandQueueTimerTimeout();
private:
    QTcpServer *server;
    std::map<std::string, UServerSocketQt*> serverSockets;

    std::string server_address;
    int server_port;

    QBasicTimer *commandQueueTimer;

};

class RDK_LIB_TYPE UServerControlQt: public RDK::UServerControl
{
public:
// --------------------------
// ������ ���������� ����������
// --------------------------
/// ������������ ��������� �������� ����������
virtual int RegisterMetadataReceiver(const std::string &address, int port);

/// ������� ��������� �������� ����������
virtual int UnRegisterMetadataReceiver(const std::string &address, int port);
// --------------------------

private:
// --------------------------
/// ���������� ������ �������
/// ���������� ��������������� �������
/// ���������� �� UApplication
// --------------------------
virtual bool ASetNumChannels(int old_num);
virtual bool AInsertChannel(int index);
virtual bool ADeleteChannel(int index);
// --------------------------

public: // TODO: �������
// --------------------------
// ��������������� ������
// --------------------------
// �����, ���������� ����� ������ ������
virtual void AfterReset(void);

// �����, ���������� ����� ���� �������
virtual void AfterCalculate(void);
// --------------------------


};


class URpcDecoderCommonQt: public RDK::URpcDecoderCommon
{
/// ������ ��������������� ������ �� ����������� �������
std::string ControlResponseString;

std::vector<RDK::UParamT> binary_data;

public:
// --------------------------
// ������������ � �����������
// --------------------------
URpcDecoderCommonQt(void);
virtual ~URpcDecoderCommonQt(void);
// --------------------------

// --------------------------
// ������ ���������� ���������
// --------------------------
/// ���������, �������������� �� ������� �����������
/// �������, ��� ������� ��� ������������ ����� ������ ���������� false
virtual bool IsCmdSupported(const std::shared_ptr<RDK::URpcCommand> &command) const;

/// ������� ����� ����� ��������
virtual URpcDecoderCommonQt* New(void);

virtual std::string ARemoteCall(const std::string &cmd, RDK::USerStorageXML &xml, const std::string &component_name, int engine_index, int &return_value);

/*
void SetEngine(QMainWindow* e);
QMainWindow* GetEngine();
*/

// --------------------------
//private:
//QMainWindow *engine;

};

/*
namespace Ui {
  class UTcpServerControlWidget;
}


class UTcpServerControlWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UTcpServerControlWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
  ~UTcpServerControlWidget();

  /// ������ ����� ��������
  virtual void ASaveParameters();
  /// ���������� ����� ��������
  virtual void ALoadParameters();
  /// ���������� ����������
  virtual void AUpdateInterface();

private slots:

    void PushButtonResetClicked();
    void PushButtonApplyClicked();
    void PushButtonServerStartClicked();
    void PushButtonServerStopClicked();

    void TcpCommandTimerTick();
    void TcpServerRestartTimerTick();


private:
  Ui::UTcpServerControlWidget *ui;

  QTimer *tcpCommandTimer;
  QTimer *tcpServerRestartTimer;


};
*/

}//namespace RDK

#endif //UServerTransportTcpQt_H
