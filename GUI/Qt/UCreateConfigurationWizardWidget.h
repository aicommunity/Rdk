#ifndef UCREATEPROJECTWIZARDWIDGET_H
#define UCREATEPROJECTWIZARDWIDGET_H

#include <QWizard>

#include <rdk_application.h>
#include "UClassesListWidget.h"

namespace Ui {
class UCreateConfigurationWizardWidget;
}

class UCreateConfigurationWizardWidget : public QWizard
{
  Q_OBJECT

public:
  explicit UCreateConfigurationWizardWidget(QWidget *parent = 0);
  ~UCreateConfigurationWizardWidget();

public slots:
  // radio buttons Channel Properties - Model selection
  void onMSPredefinedModel(bool checked);
  void onMSLoadModelFromFile(bool checked);
  void onMSModelFromComponent(bool checked);

  // channels number stuff
  void setApplySettingToAllChannels(bool checked);
  void setCalculationChannelsNumber(int value);

  // Browse buttons
  void browseNewProjectDirectory();
  void browseModelXML();

  // radio buttons Project Type
  void onPTUniversal(bool checked);
  void onPTVideoAnalysis(bool checked);
  void onPTModeling(bool checked);

  // radio buttons Calculation Mode
  void onCMSimple   (bool checked);
  void onCMRealTime (bool checked);
  void onCMBySignal (bool checked);
  void onCMFastest  (bool checked);

private:
  Ui::UCreateConfigurationWizardWidget *ui;

  /// Конфигурация сервера
  RDK::TProjectConfig ProjectConfig;

  UClassesListWidget *classesList;
};

#endif // UCREATEPROJECTWIZARDWIDGET_H
