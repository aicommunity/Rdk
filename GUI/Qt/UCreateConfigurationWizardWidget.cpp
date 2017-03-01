#include "UCreateConfigurationWizardWidget.h"
#include "ui_UCreateConfigurationWizardWidget.h"

UCreateConfigurationWizardWidget::UCreateConfigurationWizardWidget(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::UCreateConfigurationWizardWidget)
{
    ui->setupUi(this);
}

UCreateConfigurationWizardWidget::~UCreateConfigurationWizardWidget()
{
    delete ui;
}
