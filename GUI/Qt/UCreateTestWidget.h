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
  explicit UCreateTestWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
  ~UCreateTestWidget();

  /// ������ ����� ��������
  virtual void ASaveParameters();
  /// ���������� ����� ��������
  virtual void ALoadParameters();

public slots:

  // buttons:
  void addProperty();
  void createTest();
  void deleteSelectedProperty();


private:
  Ui::UCreateTestWidget *ui;

  UComponentsListWidget *componentsList;
};

#endif // UCREATETESTWIDGET_H
