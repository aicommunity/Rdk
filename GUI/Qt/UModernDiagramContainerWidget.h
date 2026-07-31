#ifndef UMODERNDIAGRAMCONTAINERWIDGET_H
#define UMODERNDIAGRAMCONTAINERWIDGET_H

#include "UVisualControllerWidget.h"
#include "UModernDiagramWidget.h"
#include "UClassesListWidget.h"
#include "UComponentGuiContext.h"

#include <rdk_application.h>
#include <QWidget>
#include <QSplitter>
#include <QDialog>
#include <QToolButton>
#include <QByteArray>

class QVBoxLayout;

/// UModernDiagramContainerWidget class - виджет-контейнер для современной диаграммы со списком компонентов хранилища.
///
/// Содержит один объект UModernDiagramWidget и один объект UClassesListWidget.
/// Палитра классов по умолчанию скрыта; открывается кнопкой на диаграмме (float Tool)
/// и может быть закреплена в правой панели splitter.

class UModernDiagramContainerWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UModernDiagramContainerWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UModernDiagramContainerWidget();

    void AUpdateInterface();
    virtual void AClearInterface() override;

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();
    /// Обновление темы - инвалидирует кэш всех узлов и обновляет сцену
    void updateTheme();

    UModernDiagramWidget* modernDiagramWidget() const { return modernScheme; }

public slots:
    void componentDoubleClick(QString name);
    void componentSingleClick(QString name);

    void updateScheme(bool reloadXml);
    void updateClassesList();
    void toggleClassesList();
    void pinClassesList();
    void unpinClassesList();

signals:
    void componentSelectedFromScheme(QString name);
    void componentDoubleClickFromScheme(QString name);
    void componentStapBackFromScheme();
    void updateComponentsListFromScheme();

    void viewLinksFromScheme(QString componentName);
    void createLinksFromScheme(QString firstComponentName, QString secondComponentName);
    void switchLinksFromScheme(QString firstComponentName, QString secondComponentName);
    void openComponentGuiFromScheme(const UComponentGuiContext& context);
    /// Запрос на открытие окна описания проекта (от кнопки на диаграмме)
    void openProjectDescriptionRequested();

private:
    void setupClassesHosts();
    void applyClassesPresentation();
    void collapseClassesSplitterPane();
    void expandClassesSplitterPane();
    void ensureClassesListIn(QWidget* host, QVBoxLayout* layout);
    void syncClassesListButton();

    UModernDiagramWidget *modernScheme;
    UClassesListWidget *classesList;
    QSplitter *splitter;

    QWidget *m_pinnedHost;
    QVBoxLayout *m_pinnedLayout;
    QToolButton *m_unpinButton;

    QDialog *m_floatDialog;
    QVBoxLayout *m_floatLayout;
    QToolButton *m_pinButton;

    bool m_classesPinned;
    bool m_classesVisible;
    int m_pinnedPaneWidth;
    QByteArray m_floatGeometry;
};

#endif // UMODERNDIAGRAMCONTAINERWIDGET_H
