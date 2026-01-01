#ifndef UMODERNDIAGRAMCONTAINERWIDGET_H
#define UMODERNDIAGRAMCONTAINERWIDGET_H

#include "UVisualControllerWidget.h"
#include "UModernDiagramWidget.h"
#include "UClassesListWidget.h"

#include <rdk_application.h>
#include <QWidget>
#include <QSplitter>

/// UModernDiagramContainerWidget class - виджет-контейнер для современной диаграммы со списком компонентов хранилища.
///
/// Содержит один объект UModernDiagramWidget и один объект UClassesListWidget

class UModernDiagramContainerWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UModernDiagramContainerWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UModernDiagramContainerWidget();

    void AUpdateInterface();

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();

public slots:
    void componentDoubleClick(QString name);
    void componentSingleClick(QString name);

    void updateScheme(bool reloadXml);
    void updateClassesList();

signals:
    void componentSelectedFromScheme(QString name);
    void componentDoubleClickFromScheme(QString name);
    void componentStapBackFromScheme();
    void updateComponentsListFromScheme();

    void viewLinksFromScheme(QString componentName);
    void createLinksFromScheme(QString firstComponentName, QString secondComponentName);
    void switchLinksFromScheme(QString firstComponentName, QString secondComponentName);

private:
    UModernDiagramWidget *modernScheme;
    UClassesListWidget *classesList;
    QSplitter *splitter;
};

#endif // UMODERNDIAGRAMCONTAINERWIDGET_H

