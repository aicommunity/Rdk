#ifndef UTcpServerControlWidget_H
#define UTcpServerControlWidget_H

#include "UVisualControllerWidget.h"
#include "../../Core/Application/UServerTransportTcp.h"

#include <QTcpServer>
#include <QTimer>



class UServerTransportTcpQt: public QObject, public RDK::UServerTransportTcp
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
    /*
    virtual void ConnectClient(std::string &bind);
    virtual void DisconnectClient(std::string &bind);
    */

public slots:
    void ServerNewConnection();
private:
    QTcpServer *server;
    std::string server_address;
    int server_port;

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

#endif //UTcpServerControlWidget_H
