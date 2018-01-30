#include "UCreateConfigurationWizardWidget.h"
#include "ui_UCreateConfigurationWizardWidget.h"

#include <QFileDialog>

UCreateConfigurationWizardWidget::UCreateConfigurationWizardWidget(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::UCreateConfigurationWizardWidget)
{
  ui->setupUi(this);

  classesList = new UClassesListWidget(this);
  ui->horizontalLayoutFromComponent->addWidget(classesList);

  // radio buttons Channel Properties - Model selection
  connect(ui->radioButtonMSPredefinedModel, SIGNAL(toggled(bool)), this, SLOT(onMSPredefinedModel(bool)));
  connect(ui->radioButtonMSFromComponent  , SIGNAL(toggled(bool)), this, SLOT(onMSModelFromComponent(bool)));
  connect(ui->radioButtonMSFromFile       , SIGNAL(toggled(bool)), this, SLOT(onMSLoadModelFromFile(bool)));

  // channels number stuff
  connect(ui->checkBoxSettingToAllChannels, SIGNAL(toggled(bool)), this, SLOT(setApplySettingToAllChannels(bool)));
  connect(ui->spinBoxChannelsNumber, SIGNAL(valueChanged(int)), this, SLOT(setCalculationChannelsNumber(int)));

  // Browse buttons
  connect(ui->pushButtonBrowseProjectDirectory, SIGNAL(pressed()), this, SLOT(browseNewProjectDirectory()));
  connect(ui->pushButtonBrowseModel, SIGNAL(pressed()), this, SLOT(browseModelXML()));

  // radio buttons Project Type
  connect(ui->radioButtonPTUniversal, SIGNAL(toggled(bool)), this, SLOT(onPTUniversal(bool)));
  connect(ui->radioButtonPTVideoAnalysis, SIGNAL(toggled(bool)), this, SLOT(onPTVideoAnalysis(bool)));
  connect(ui->radioButtonPTModeling, SIGNAL(toggled(bool)), this, SLOT(onPTModeling(bool)));

  // radio buttons Calculation Mode
  connect(ui->radioButtonCMSimple, SIGNAL(toggled(bool)), this, SLOT(onCMSimple(bool)));
  connect(ui->radioButtonCMRealTime, SIGNAL(toggled(bool)), this, SLOT(onCMRealTime(bool)));
  connect(ui->radioButtonCMBySignal, SIGNAL(toggled(bool)), this, SLOT(onCMBySignal(bool)));
  connect(ui->radioButtonCMFastest, SIGNAL(toggled(bool)), this, SLOT(onCMFastest(bool)));

  ui->radioButtonMSFromComponent->setChecked(true);
  ui->frameMaxCalcModelTime->hide();
  ui->radioButtonPTUniversal->setChecked(true);
}

UCreateConfigurationWizardWidget::~UCreateConfigurationWizardWidget()
{
  delete ui;
}

void UCreateConfigurationWizardWidget::onMSPredefinedModel(bool checked)
{
  if(checked)
  {
    ui->frameFromComponent->hide();
    ui->frameFromFile->hide();
    ui->framePredefinedModel->show();
  }
}

void UCreateConfigurationWizardWidget::onMSLoadModelFromFile(bool checked)
{
  if(checked)
  {
    ui->frameFromComponent->hide();
    ui->framePredefinedModel->hide();
    ui->frameFromFile->show();
  }
}

void UCreateConfigurationWizardWidget::onMSModelFromComponent(bool checked)
{
  if(checked)
  {
    ui->frameFromFile->hide();
    ui->framePredefinedModel->hide();
    ui->frameFromComponent->show();
  }
}

void UCreateConfigurationWizardWidget::setApplySettingToAllChannels(bool checked)
{
  if(checked)
    ui->listWidgetChannels->hide();
  else
    ui->listWidgetChannels->show();
}

void UCreateConfigurationWizardWidget::setCalculationChannelsNumber(int value)
{
  int listSize = ui->listWidgetChannels->count();

  for(int i = listSize; i > value; --i)
  {
    delete ui->listWidgetChannels->takeItem(i-1);
  }

  for(int i = listSize; i < value; ++i)
  {
    ui->listWidgetChannels->addItem(QString::number(i));
  }
}

void UCreateConfigurationWizardWidget::browseNewProjectDirectory()
{
  ui->lineEditProjectDirectory->setText(
        QFileDialog::getExistingDirectory(this, tr("Create project directory"), ""));
}

void UCreateConfigurationWizardWidget::browseModelXML()
{
  ui->lineEditModelFromFile->setText(
        QFileDialog::getOpenFileName(this, tr("Open model xml-file"),
                                     QApplication::applicationDirPath()+"/../../../Configs", tr("model*.xml")));
}

void UCreateConfigurationWizardWidget::onPTUniversal(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->clear();
    ui->lineEditTimeStepDurationGlobal ->clear();
    ui->lineEditMinInterstepInterval   ->clear();
    ui->radioButtonCMSimple->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::onPTVideoAnalysis(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->setText("30");
    ui->lineEditTimeStepDurationGlobal->setText("30");
    ui->lineEditMinInterstepInterval->setText("30");
    ui->radioButtonCMRealTime->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::onPTModeling(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->setText("2000");
    ui->lineEditTimeStepDurationGlobal->setText("2000");
    ui->lineEditMinInterstepInterval->setText("30");
    ui->radioButtonCMFastest->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::onCMSimple(bool checked)
{

}

void UCreateConfigurationWizardWidget::onCMRealTime(bool checked)
{
  if(checked)
    ui->frameMaxCalcModelTime->show();
  else
    ui->frameMaxCalcModelTime->hide();
}

void UCreateConfigurationWizardWidget::onCMBySignal(bool checked)
{

}

void UCreateConfigurationWizardWidget::onCMFastest(bool checked)
{
  if(checked)
    ui->frameMaxCalcModelTime->show();
  else
    ui->frameMaxCalcModelTime->hide();
}
