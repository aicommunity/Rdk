#ifndef USTATUSPANEL_H
#define USTATUSPANEL_H

#include "UVisualControllerWidget.h"

#include <QWidget>

namespace Ui {
  class UStatusPanel;
}

/// Виджет панели статусбара, отображает Время / Шаг / Фпс, чекбоксы глобальных настрок проекта
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
  void setAutosaveStates(bool checked);
  void setAutosaveProject(bool checked);

private:
  Ui::UStatusPanel *ui;
};

#endif // USTATUSPANEL_H
