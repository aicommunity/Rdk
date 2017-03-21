#ifndef UDRAWENGINEWIDGET_H
#define UDRAWENGINEWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include "UClassesListWidget.h"

#include <rdk_application.h>

#include <QWidget>

namespace Ui {
class UDrawEngineWidget;
}

/// UDrawEngineWidget class - виджет отрисовки схемы со списком компонентов хранилища.
///
/// Содержит один объект UDrawEngineImageWidget и один объект UClassesListWidget

class UDrawEngineWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UDrawEngineWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UDrawEngineWidget");
    virtual ~UDrawEngineWidget();

    void AUpdateInterface();

public slots:
    void readSettings(QString file, QString group = "UDrawEngineWidget");
    void writeSettings(QString file, QString group = "UDrawEngineWidget");

    void componentDoubleClick(QString name);
    void componentSingleClick(QString name);

    void updateScheme(bool reloadXml);

signals:
    void componentSelectedFromScheme(QString name);
    void componentDoubleClickFromScheme(QString name);
    void componentStapBackFromScheme();
    void updateComponentsListFromScheme();

    void viewLinksFromScheme(QString componentName);
    void createLinksFromScheme(QString firstComponentName, QString secondComponentName);

private:
    UDrawEngineImageWidget *modelScheme;
    UClassesListWidget *classesList;

    Ui::UDrawEngineWidget *ui;
};

#endif // UDRAWENGINEWIDGET_H
