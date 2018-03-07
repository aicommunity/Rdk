#include "UAnalyticsSimpleSettingsWidget.h"
#include "ui_UAnalyticsSimpleSettingsWidget.h"

UAnalyticsSimpleSettingsWidget::UAnalyticsSimpleSettingsWidget(QWidget *parent, QString settingsFile, QString settingsGroup) :
  UVisualControllerWidget(parent),
  ui(new Ui::UAnalyticsSimpleSettingsWidget)
{
  ui->setupUi(this);

  UpdateInterval = -1;
  setAccessibleName("UAnalyticsSimpleSettingsWidget"); // имя класса для сериализации

  images = new UImagesWidget(this, settingsFile);
  ui->verticalLayoutImagesWidget->addWidget(images);
}

UAnalyticsSimpleSettingsWidget::~UAnalyticsSimpleSettingsWidget()
{
  delete ui;
}
