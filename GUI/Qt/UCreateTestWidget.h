#ifndef UCREATETESTWIDGET_H
#define UCREATETESTWIDGET_H

#include <QWidget>
#include <UVisualControllerWidget.h>
#include <UComponentsListWidget.h>
#include <rdk_application.h>

namespace Ui {
  class UCreateTestWidget;
}

class UCreateTestWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UCreateTestWidget(QWidget *parent = 0, RDK::UApplication *app = NULL, QString settingsFile = "settings.qt", QString settingsGroup = "UCreateTestWidget");
  ~UCreateTestWidget();

public slots:

  /// ���������� ������ ��������
  void readSettings(QString file, QString group = "UCreateTestWidget");
  /// ������ ������ ��������
  void writeSettings(QString file, QString group = "UCreateTestWidget");

  // buttons:
  void addProperty();
  void createTest();


private:
  Ui::UCreateTestWidget *ui;

  RDK::UApplication *application;
  UComponentsListWidget *componentsList;
};

#endif // UCREATETESTWIDGET_H
