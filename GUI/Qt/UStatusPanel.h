#ifndef USTATUSPANEL_H
#define USTATUSPANEL_H

#include "UVisualControllerWidget.h"

#include <QWidget>

namespace Ui {
  class UStatusPanel;
}

/// Виджет панели статусбара, отображает Время / Шаг / Фпс, чекбоксы глобальных настрок проекта
///
/// В случае изменения ThreadMode испускает сигнал saveConfig(), для сохранения конфига приложения,
/// не сохраняет его самостоятельно, так как не может сохранить состояния интерфейса самостоятельно.
class UStatusPanel : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UStatusPanel(QWidget *parent = 0, RDK::UApplication *app = NULL);
  virtual ~UStatusPanel();

  void AUpdateInterface();
  void AAfterLoadProject();

  void ChangeAutoupdateProperties(bool value);

public slots:
  void setShowDebugMessages(bool checked);
  void setDetailedLog(bool checked);
  void setAutoupdateProperties(bool checked);
  void setAutosaveStates(bool checked);
  void setAutosaveProject(bool checked);

  /// Single/Multipe threaded - испускает сигнал saveConfig()
  void setThreadMode();

signals:
  void saveConfig();
  void setPropertyUpdateInterval(long value);

private:
  Ui::UStatusPanel *ui;
};

#endif // USTATUSPANEL_H
