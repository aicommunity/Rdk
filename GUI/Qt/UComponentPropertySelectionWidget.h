#ifndef UCOMPONENTPROPERTYSELECTIONWIDGET_H
#define UCOMPONENTPROPERTYSELECTIONWIDGET_H

#include <QDialog>
#include <UComponentsListWidget.h>

namespace Ui {
class UComponentPropertySelectionWidget;
}

class UComponentPropertySelectionWidget : public QDialog
{
    Q_OBJECT

public:
    explicit UComponentPropertySelectionWidget(QWidget *parent = 0, int mode = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UComponentPropertySelectionWidget");
    virtual ~UComponentPropertySelectionWidget();

    UComponentsListWidget *componentsList;

public slots:
    ///считывание файлов настроек
    void readSettings(QString file, QString group = "UComponentPropertySelectionWidget");
    ///запись файлов настроек
    void writeSettings(QString file, QString group = "UComponentPropertySelectionWidget");

private:
    Ui::UComponentPropertySelectionWidget *ui;
};

#endif // UCOMPONENTPROPERTYSELECTIONWIDGET_H
