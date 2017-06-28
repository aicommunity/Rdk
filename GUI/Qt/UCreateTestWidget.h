#ifndef UCREATETESTWIDGET_H
#define UCREATETESTWIDGET_H

#include <QWidget>
#include <UVisualControllerWidget.h>
#include <UComponentsListWidget.h>

namespace Ui {
  class UCreateTestWidget;
}

class UCreateTestWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UCreateTestWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UCreateTestWidget");
  ~UCreateTestWidget();

public slots:

  /// считывание файлов настроек
  void readSettings(QString file, QString group = "UCreateTestWidget");
  /// запись файлов настроек
  void writeSettings(QString file, QString group = "UCreateTestWidget");

  // buttons:
  void addProperty();
  void createTest();


private:
  Ui::UCreateTestWidget *ui;

  UComponentsListWidget *componentsList;
};

#endif // UCREATETESTWIDGET_H
