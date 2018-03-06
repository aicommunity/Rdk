#ifndef UANALYTICSSIMPLESETTINGSWIDGET_H
#define UANALYTICSSIMPLESETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
  class UAnalyticsSimpleSettingsWidget;
}

class UAnalyticsSimpleSettingsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit UAnalyticsSimpleSettingsWidget(QWidget *parent = 0);
  ~UAnalyticsSimpleSettingsWidget();

private:
  Ui::UAnalyticsSimpleSettingsWidget *ui;
};

#endif // UANALYTICSSIMPLESETTINGSWIDGET_H
