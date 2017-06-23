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

  /// ���������� ������ ��������
  void readSettings(QString file, QString group = "UComponentPropertyChanger");
  /// ������ ������ ��������
  void writeSettings(QString file, QString group = "UComponentPropertyChanger");

public:
  UComponentsListWidget *componentsList;

private:
  // methods
  bool emptySeletion();

  // data
  Ui::UComponentPropertyChanger *ui;

  /// ��������� �� ���������� ������ ����������� XML
  UPropertyXMLWidget *propertyXML;

  // buffers
  QString componentName;
  QString propertyName;
};

#endif // UCOMPONENTPROPERTYCHANGER_H
