#ifndef UTcpServerControlWidget_H
#define UTcpServerControlWidget_H

#include "UVisualControllerWidget.h"
#include "../../Core/Application/Qt/UServerTransportTcpQt.h"
#include <QTimer>

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

    //void TcpCommandTimerTick();
    void TcpServerRestartTimerTick();


private:
  Ui::UTcpServerControlWidget *ui;

  QTimer *tcpCommandTimer;
  QTimer *tcpServerRestartTimer;
};

#endif //UTcpServerControlWidget_H
