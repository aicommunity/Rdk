#ifndef USTATUSPANEL_H
#define USTATUSPANEL_H

#include "UVisualControllerWidget.h"

#include <QWidget>

namespace Ui {
  class UStatusPanel;
}

/// ������ ������ ����������, ���������� ����� / ��� / ���, �������� ���������� ������� �������
///
/// � ������ ��������� ThreadMode ��������� ������ saveConfig(), ��� ���������� ������� ����������,
/// �� ��������� ��� ��������������, ��� ��� �� ����� ��������� ��������� ���������� ��������������.
class UStatusPanel : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UStatusPanel(QWidget *parent = 0, RDK::UApplication *app = NULL);
  virtual ~UStatusPanel();

  void AUpdateInterface();
  void AAfterLoadProject();

public slots:
  void setShowDebugMessages(bool checked);
  void setDetailedLog(bool checked);
  void setAutoupdateProperties(bool checked);
  void setAutosaveStates(bool checked);
  void setAutosaveProject(bool checked);

  /// Single/Multipe threaded - ��������� ������ saveConfig()
  void setThreadMode();

signals:
  void saveConfig();
  void setPropertyUpdateInterval(long value);

private:
  Ui::UStatusPanel *ui;
  RDK::UApplication *application;
};

#endif // USTATUSPANEL_H
