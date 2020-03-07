#ifndef UWATCHWIDGETFORM_H
#define UWATCHWIDGETFORM_H

#include <QWidget>

namespace Ui {
class UWatchFormWidget;
}

class UWatchWidgetForm : public QWidget
{
 Q_OBJECT

public:
 explicit UWatchWidgetForm(QWidget *parent = nullptr);
 ~UWatchWidgetForm();

private:
 Ui::UWatchFormWidget *ui;
};

#endif // UWATCHWIDGETFORM_H
