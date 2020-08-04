#ifndef UTcpServerControlWidget_H
#define UTcpServerControlWidget_H

#include "UVisualControllerWidget.h"
#include "../../Core/Application/UServerTransportTcp.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>


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
    /// Читает входящие байты из выбранного источника, контекст привязки
    /// всегда определяется строкой вне зависимости от типа транспорта
    virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);
    /// Отправить ответ на команду соответствующему получателю
    virtual void SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr);
    /// Задает адрес и порт входящего интерфейса сервера
    virtual void SetServerBinding(std::string &interface_address, int port);
    /// Получение адреса интерфейса управления сервером
    virtual std::string GetServerBindingInterfaceAddress(void);
    /// Получить порт сервера
    int GetServerBindingPort(void) const;
    /// Инициировать остановку сервера, отключить все приемники
    virtual void ServerStop();
    /// Инициировать запуск сервера
    virtual void ServerStart();
    //virtual void ConnectClient(std::string &bind);
    //virtual void DisconnectClient(std::string &bind);

    virtual int GetSocketState(std::string bind);

    bool ServerIsActive();

public slots:
    void ServerNewConnection();
    void SocketReadyRead(std::string bind);
    void SocketDisconnected(std::string bind);
private:
    QTcpServer *server;
    std::map<std::string, UServerSocketQt*> serverSockets;

    std::string server_address;
    int server_port;



};

class RDK_LIB_TYPE UServerControlQt: public RDK::UServerControl
{
public:
// --------------------------
// Методы управления вещателями
// --------------------------
/// Регистрирует удаленный приемник метаданных
virtual int RegisterMetadataReceiver(const std::string &address, int port);

/// Удаляет удаленный приемник метаданных
virtual int UnRegisterMetadataReceiver(const std::string &address, int port);
// --------------------------

private:
// --------------------------
/// Управление числом каналов
/// Выполнение вспомогательных методов
/// Вызывается из UApplication
// --------------------------
virtual bool ASetNumChannels(int old_num);
virtual bool AInsertChannel(int index);
virtual bool ADeleteChannel(int index);
// --------------------------

public: // TODO: костыль
// --------------------------
// Вспомогательные методы
// --------------------------
// Метод, вызываемый после сброса модели
virtual void AfterReset(void);

// Метод, вызываемый после шага расчета
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

  /// запись файла настроек
  virtual void ASaveParameters();
  /// считывание файла настроек
  virtual void ALoadParameters();
  /// обновление интерфейса
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
