#ifndef UCOMPONENTPROPERTYCHANGER_H
#define UCOMPONENTPROPERTYCHANGER_H

#include <QWidget>
#include "UComponentsListWidget.h"
#include "UPropertyXMLWidget.h"

namespace Ui {
  class UComponentPropertyChanger;
}

class UComponentPropertyChanger : public QWidget
{
  Q_OBJECT

public:
  explicit UComponentPropertyChanger(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UComponentPropertyChanger");
  ~UComponentPropertyChanger();

public slots:
  void actionSet();
  void actionSetGlobal();
  void actionSetGlobalOwner();
  void actionDefaultAll();
  void actionShowXML();

  void updateCurrentPropertyValue(QString value);

  /// считывание файлов настроек
  void readSettings(QString file, QString group = "UComponentPropertyChanger");
  /// запись файлов настроек
  void writeSettings(QString file, QString group = "UComponentPropertyChanger");

public:
  UComponentsListWidget *componentsList;

private:
  // methods
  bool emptySeletion();

  // data
  Ui::UComponentPropertyChanger *ui;

  /// ”казатель на диалоговое окошко отображени€ XML
  UPropertyXMLWidget *propertyXML;

  // buffers
  QString componentName;
  QString propertyName;
};

#endif // UCOMPONENTPROPERTYCHANGER_H
