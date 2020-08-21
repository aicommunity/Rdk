#ifndef UTcpServerControlWidget_H
#define UTcpServerControlWidget_H

#include "UVisualControllerWidget.h"
#include "../../Core/Application/UServerTransportTcp.h"

#include <QTcpServer>

class UServerTransportTcpQt: public RDK::UServerTransportTcp
{
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

private:
    QTcpServer *server;

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

private:
  Ui::UTcpServerControlWidget *ui;
};

#endif //UTcpServerControlWidget_H
