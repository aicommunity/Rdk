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

  /// запись файла настроек
  virtual void ASaveParameters();
  /// считывание файла настроек
  virtual void ALoadParameters();
  /// обновление интерфейса
  virtual void AUpdateInterface();

private:
  Ui::UTcpServerControlWidget *ui;
};

#endif //UTcpServerControlWidget_H
