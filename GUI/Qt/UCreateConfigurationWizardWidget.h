#ifndef UCREATEPROJECTWIZARDWIDGET_H
#define UCREATEPROJECTWIZARDWIDGET_H

#include <QWizard>
#include <QListWidgetItem>

#include <rdk_application.h>
#include "UClassesListWidget.h"

namespace Ui {
class UCreateConfigurationWizardWidget;
}

class UCreateConfigurationWizardWidget : public QWizard
{
  Q_OBJECT

public:
  explicit UCreateConfigurationWizardWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
  ~UCreateConfigurationWizardWidget();

public slots:
  // radio buttons Channel Properties - Model selection
  void onMSPredefinedModel   (bool checked);
  void onMSLoadModelFromFile (bool checked);
  void onMSModelFromComponent(bool checked);
  void modelFromFileNameChanged(QString value);

  // channels number stuff
  void setApplySettingToAllChannels(bool checked);
  void setCalculationChannelsNumber(int value);
  void channelSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous);

  // Browse buttons
  void browseNewProjectDirectory();
  void browseModelXML();

  // radio buttons Project Type
  void onPTUniversal(bool checked);
  void onPTVideoAnalysis(bool checked);
  void onPTModeling(bool checked);

  // time step duration
  void defaultTimeChanged(QString value);
  void globalTimeChanged (QString value);

  // radio buttons Calculation Mode
  void onCMSimple   (bool checked);
  void onCMRealTime (bool checked);
  void onCMBySignal (bool checked);
  void onCMFastest  (bool checked);

  void maxCalculationModelTimeChanged(QString text);
  void minInterstepIntervalChanged(QString text);

  // checkboxes at the bottom
  void setInitAfterLoad (bool checked);
  void setResetAfterLoad(bool checked);
  void setDebugMode     (bool checked);

protected:
  virtual void accept() override;

private:
  Ui::UCreateConfigurationWizardWidget *ui;

  /// Конфигурация сервера
  RDK::TProjectConfig ProjectConfig;

  /// id текущего канала
  int channelNumber;

  /// Мапа моделей, которые необходимо загрузить из файла
  QMap<int, QString> modelsFromFile;

  UClassesListWidget *classesList;

  RDK::UApplication *application;
};

#endif // UCREATEPROJECTWIZARDWIDGET_H
