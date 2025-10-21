#ifndef USETTINGSREADERWIDGET_H
#define USETTINGSREADERWIDGET_H

#include "UVisualControllerWidget.h"

/// ������������� ����������� ����������� �������� �� QObject �������� ������� load/save Parameters �� RDK
///
/// �������� �� ���� ������ � ��� �������, ��� ��������:
/// RDK::UIVisualControllerStorage::LoadParameters
/// RDK::UIVisualControllerStorage::SaveParameters
/// �������� ���������� ��� � ����������� �������

class USettingsReaderWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit USettingsReaderWidget(QWidget *parent = 0) :
    UVisualControllerWidget(parent)
  {

  }

  virtual ~USettingsReaderWidget();

  /// ������ ����� ��������
  virtual void ASaveParameters()
  {
    emit writeSetting();
  }

  /// ���������� ����� ��������
  virtual void ALoadParameters()
  {
    emit readSetting();
  }

signals:
  void readSetting();
  void writeSetting();
};

#endif // USETTINGSREADERWIDGET_H
