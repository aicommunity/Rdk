#ifndef UCOMPONENTPROPERTYCHANGER_H
#define UCOMPONENTPROPERTYCHANGER_H

#include "UVisualControllerWidget.h"
#include "UComponentsListWidget.h"
#include "UPropertyXMLWidget.h"

namespace Ui {
  class UComponentPropertyChanger;
}

class UComponentPropertyChanger : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UComponentPropertyChanger(QWidget *parent = 0, RDK::UApplication *app = NULL);
  ~UComponentPropertyChanger();

  /// ������ ����� ��������
  virtual void ASaveParameters();
  /// ���������� ����� ��������
  virtual void ALoadParameters();

public slots:
  void actionSet();
  void actionSetGlobal();
  void actionSetGlobalOwner();
  void actionShowXML();

  void updateCurrentPropertyValue(QString value);

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
