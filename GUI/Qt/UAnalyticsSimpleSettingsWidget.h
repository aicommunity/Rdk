#ifndef UANALYTICSSIMPLESETTINGSWIDGET_H
#define UANALYTICSSIMPLESETTINGSWIDGET_H

#include "UVisualControllerWidget.h"
#include "UImagesWidget.h"

#include <QWidget>

namespace Ui {
  class UAnalyticsSimpleSettingsWidget;
}

/// Виджет настройки аналитики, замена граси эдитора
class UAnalyticsSimpleSettingsWidget : public UVisualControllerWidget
{
  Q_OBJECT

public:
  explicit UAnalyticsSimpleSettingsWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UAnalyticsSimpleSettingsWidget");
  ~UAnalyticsSimpleSettingsWidget();

private:
  Ui::UAnalyticsSimpleSettingsWidget *ui;

  UImagesWidget* images;
};

#endif // UANALYTICSSIMPLESETTINGSWIDGET_H
