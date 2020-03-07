#ifndef UWATCHFORMWIDGET_H
#define UWATCHFORMWIDGET_H

#include <QWidget>

namespace Ui {
class UWatchFormWidget;
}

class UWatchFormWidget : public QWidget
{
 Q_OBJECT

public:
 explicit UWatchFormWidget(QWidget *parent = nullptr);
 ~UWatchFormWidget();

private slots:
    void on_pushButton_2_clicked();

private:
 Ui::UWatchFormWidget *ui;
};

#endif // UWATCHFORMWIDGET_H
