#include "UAnalyticsSimpleSettingsWidget.h"
#include "ui_UAnalyticsSimpleSettingsWidget.h"

UAnalyticsSimpleSettingsWidget::UAnalyticsSimpleSettingsWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::UAnalyticsSimpleSettingsWidget)
{
  ui->setupUi(this);
}

UAnalyticsSimpleSettingsWidget::~UAnalyticsSimpleSettingsWidget()
{
  delete ui;
}
