#ifndef UCREATEPROJECTWIZARDWIDGET_H
#define UCREATEPROJECTWIZARDWIDGET_H

#include <QWizard>

namespace Ui {
class UCreateConfigurationWizardWidget;
}

class UCreateConfigurationWizardWidget : public QWizard
{
    Q_OBJECT

public:
    explicit UCreateConfigurationWizardWidget(QWidget *parent = 0);
    ~UCreateConfigurationWizardWidget();

private:
    Ui::UCreateConfigurationWizardWidget *ui;
};

#endif // UCREATEPROJECTWIZARDWIDGET_H
