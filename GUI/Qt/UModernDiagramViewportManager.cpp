#include "UModernDiagramViewportManager.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramView.h"
#include "UModernDiagramScene.h"
#include "UStyleManager.h"

#include <QSettings>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>
#include <QRectF>
#include <QSize>
#include <QPushButton>
#include <QTimer>
#include <QStyle>
#include <QApplication>
#include "UGuiTelemetry.h"

UModernDiagramViewportManager::UModernDiagramViewportManager(UModernDiagramWidget* owner)
    : m_owner(owner)
    , m_projectDescriptionButton(nullptr)
    , m_resetZoomButton(nullptr)
    , m_classesListButton(nullptr)
{
}

void UModernDiagramViewportManager::saveCurrentViewState(const QString& componentName)
{
    if(componentName.isEmpty() || !m_owner || !m_owner->m_mainView)
        return;

    UModernDiagramViewState state;
    QTransform transform = m_owner->m_mainView->transform();
    state.scale = transform.m11();  // Масштаб по X (обычно равен масштабу по Y)
    state.center = m_owner->m_mainView->mapToScene(m_owner->m_mainView->viewport()->rect().center());
    state.isValid = true;

    m_viewStates[componentName] = state;
}

void UModernDiagramViewportManager::restoreViewState(const QString& componentName)
{
    if(componentName.isEmpty() || !m_owner || !m_owner->m_mainView || !m_owner->m_scene || m_owner->m_scene->items().isEmpty())
        return;

    // Проверяем, есть ли сохраненное состояние для этого компонента
    if(m_viewStates.contains(componentName))
    {
        const UModernDiagramViewState& state = m_viewStates[componentName];
        if(state.isValid)
        {
            // Восстанавливаем масштаб
            m_owner->m_mainView->resetTransform();
            m_owner->m_mainView->scale(state.scale, state.scale);

            // Восстанавливаем центр
            m_owner->m_mainView->centerOn(state.center);

            // DEBUG: Commented out to reduce log flood - Logging for debugging viewport restoration
            // QTransform transform = m_owner->m_mainView->transform();
            // QString logMsg = QString("[UModernDiagramViewportManager::restoreViewState] Restored state for '%1': scale=%2, center=(%3, %4), transform.m11()=%5")
            //     .arg(componentName)
            //     .arg(state.scale)
            //     .arg(state.center.x()).arg(state.center.y())
            //     .arg(transform.m11());
            // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
            return;
        }
    }

    // Если сохраненного состояния нет, устанавливаем начальный масштаб
    QRectF bounds = m_owner->m_scene->itemsBoundingRect();
    if(!bounds.isNull())
    {
        QRectF padded = bounds.adjusted(-200, -200, 200, 200);
        m_owner->m_scene->setSceneRect(padded);

        // Устанавливаем начальный масштаб
        m_owner->m_mainView->resetTransform();
        m_owner->m_mainView->scale(DEFAULT_SCALE, DEFAULT_SCALE);

        // Центрируем на содержимом
        m_owner->m_mainView->centerOn(bounds.center());

        // Сохраняем это состояние
        UModernDiagramViewState state;
        state.scale = DEFAULT_SCALE;
        state.center = bounds.center();
        state.isValid = true;
        m_viewStates[componentName] = state;

        // DEBUG: Commented out to reduce log flood - Logging for debugging initial state setup
        // QTransform transform = m_owner->m_mainView->transform();
        // QString logMsg = QString("[UModernDiagramViewportManager::restoreViewState] Set initial state for '%1': bounds=(%2, %3, %4, %5), padded=(%6, %7, %8, %9), scale=%10, center=(%11, %12), transform.m11()=%13")
        //     .arg(componentName)
        //     .arg(bounds.x()).arg(bounds.y()).arg(bounds.width()).arg(bounds.height())
        //     .arg(padded.x()).arg(padded.y()).arg(padded.width()).arg(padded.height())
        //     .arg(DEFAULT_SCALE)
        //     .arg(state.center.x()).arg(state.center.y())
        //     .arg(transform.m11());
        // MLog_LogMessageEx(RDK_GLOB_MESSAGE, RDK_EX_INFO, logMsg.toStdString().c_str(), 0);
    }
}

void UModernDiagramViewportManager::resetZoom()
{
    if(!m_owner || !m_owner->m_mainView || !m_owner->m_scene || m_owner->m_scene->items().isEmpty())
        return;

    QRectF bounds = m_owner->m_scene->itemsBoundingRect();
    if(!bounds.isNull())
    {
        // Сбрасываем масштаб к начальному значению
        m_owner->m_mainView->resetTransform();
        m_owner->m_mainView->scale(DEFAULT_SCALE, DEFAULT_SCALE);

        // Центрируем на содержимом
        m_owner->m_mainView->centerOn(bounds.center());

        // Обновляем сохраненное состояние
        if(!m_owner->m_componentName.isEmpty())
        {
            UModernDiagramViewState state;
            state.scale = DEFAULT_SCALE;
            state.center = bounds.center();
            state.isValid = true;
            m_viewStates[m_owner->m_componentName] = state;
        }
    }
}

void UModernDiagramViewportManager::saveToSettings()
{
    if(!m_owner || !m_owner->m_application)
        return;

    // Сохраняем текущее состояние перед сохранением
    if(!m_owner->m_componentName.isEmpty())
    {
        saveCurrentViewState(m_owner->m_componentName);
    }

    QSettings settings(QString::fromLocal8Bit(
                         m_owner->m_application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup("UModernDiagramWidget_ViewStates");

    // Сохраняем количество состояний
    settings.setValue("count", m_viewStates.size());

    // Сохраняем каждое состояние
    int index = 0;
    for(auto it = m_viewStates.begin(); it != m_viewStates.end(); ++it, ++index)
    {
        QString key = QString("component_%1").arg(index);
        settings.setValue(key + "_name", it.key());
        settings.setValue(key + "_scale", it.value().scale);
        settings.setValue(key + "_center_x", it.value().center.x());
        settings.setValue(key + "_center_y", it.value().center.y());
        settings.setValue(key + "_valid", it.value().isValid);
    }

    settings.endGroup();
}

void UModernDiagramViewportManager::loadFromSettings()
{
    if(!m_owner || !m_owner->m_application)
        return;

    QSettings settings(QString::fromLocal8Bit(
                         m_owner->m_application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup("UModernDiagramWidget_ViewStates");

    // Очищаем текущие состояния
    m_viewStates.clear();

    // Загружаем количество состояний
    int count = settings.value("count", 0).toInt();

    // Загружаем каждое состояние
    for(int i = 0; i < count; ++i)
    {
        QString key = QString("component_%1").arg(i);
        QString componentName = settings.value(key + "_name").toString();
        if(!componentName.isEmpty())
        {
            UModernDiagramViewState state;
            state.scale = settings.value(key + "_scale", DEFAULT_SCALE).toDouble();
            state.center.setX(settings.value(key + "_center_x", 0.0).toDouble());
            state.center.setY(settings.value(key + "_center_y", 0.0).toDouble());
            state.isValid = settings.value(key + "_valid", false).toBool();
            m_viewStates[componentName] = state;
        }
    }

    settings.endGroup();
}

QPushButton* UModernDiagramViewportManager::createProjectDescriptionButton(QWidget* parent)
{
    if(m_projectDescriptionButton)
        return m_projectDescriptionButton;

    m_projectDescriptionButton = new QPushButton(parent);
    QStyle* style = QApplication::style();
    if(style)
        m_projectDescriptionButton->setIcon(style->standardIcon(QStyle::SP_FileIcon));
    m_projectDescriptionButton->setIconSize(QSize(20, 20));
    m_projectDescriptionButton->setToolTip(QCoreApplication::translate("UModernDiagramViewportManager", "Project Description"));
    m_projectDescriptionButton->setFixedSize(32, 32);
    m_projectDescriptionButton->raise();

    if(m_owner)
    {
        QObject::connect(m_projectDescriptionButton, &QPushButton::clicked, m_owner, &UModernDiagramWidget::requestOpenProjectDescription);
    }

    updateOverlayButtonsStyle();

    QTimer::singleShot(0, parent, [this, parent]() {
        if((m_projectDescriptionButton || m_resetZoomButton) && parent)
            updateOverlayButtonsPosition(parent->width(), parent->height());
    });

    return m_projectDescriptionButton;
}

QPushButton* UModernDiagramViewportManager::createResetZoomButton(QWidget* parent)
{
    if(m_resetZoomButton)
        return m_resetZoomButton;

    m_resetZoomButton = new QPushButton(parent);
    QStyle* style = QApplication::style();
    if(style)
        m_resetZoomButton->setIcon(style->standardIcon(QStyle::SP_BrowserReload));
    m_resetZoomButton->setIconSize(QSize(20, 20));
    m_resetZoomButton->setToolTip(QCoreApplication::translate("UModernDiagramViewportManager", "Reset zoom"));
    m_resetZoomButton->setFixedSize(32, 32);
    updateOverlayButtonsStyle();
    m_resetZoomButton->raise();

    // Подключаем сигнал к слоту виджета
    if(m_owner)
    {
        QObject::connect(m_resetZoomButton, &QPushButton::clicked, m_owner, [this]() {
            resetZoom();
        });
    }

    // Позиционируем кнопки при первом создании
    QTimer::singleShot(0, parent, [this, parent]() {
        if(parent)
            updateOverlayButtonsPosition(parent->width(), parent->height());
    });

    return m_resetZoomButton;
}

QPushButton* UModernDiagramViewportManager::createClassesListButton(QWidget* parent)
{
    if(m_classesListButton)
        return m_classesListButton;

    m_classesListButton = new QPushButton(parent);
    QStyle* style = QApplication::style();
    if(style)
        m_classesListButton->setIcon(style->standardIcon(QStyle::SP_FileDialogDetailedView));
    m_classesListButton->setIconSize(QSize(20, 20));
    m_classesListButton->setToolTip(QCoreApplication::translate("UModernDiagramViewportManager", "Component classes"));
    m_classesListButton->setFixedSize(32, 32);
    m_classesListButton->setCheckable(true);
    m_classesListButton->setChecked(false);
    m_classesListButton->raise();

    if(m_owner)
    {
        QObject::connect(m_classesListButton, &QPushButton::clicked, m_owner, &UModernDiagramWidget::requestToggleClassesList);
    }

    updateOverlayButtonsStyle();

    QTimer::singleShot(0, parent, [this, parent]() {
        if(parent)
            updateOverlayButtonsPosition(parent->width(), parent->height());
    });

    return m_classesListButton;
}

void UModernDiagramViewportManager::setClassesListButtonChecked(bool checked)
{
    if(m_classesListButton)
        m_classesListButton->setChecked(checked);
}

void UModernDiagramViewportManager::updateOverlayButtonsPosition(int width, int height)
{
    Q_UNUSED(height);
    const int margin = 10;
    const int buttonWidth = 32;   // ширина кнопки (горизонталь)
    const int buttonHeight = 32;  // высота кнопки (вертикаль)
    const int gap = 8;

    int x = width - margin - buttonWidth;
    int y = margin;
    // Сверху вниз: zoom, project description, classes
    if(m_resetZoomButton)
    {
        m_resetZoomButton->move(x, y);
        y += buttonHeight + gap;
    }
    if(m_projectDescriptionButton)
    {
        m_projectDescriptionButton->move(x, y);
        y += buttonHeight + gap;
    }
    if(m_classesListButton)
        m_classesListButton->move(x, y);
}

void UModernDiagramViewportManager::updateResetZoomButtonPosition(int width, int height)
{
    updateOverlayButtonsPosition(width, height);
}

void UModernDiagramViewportManager::updateOverlayButtonsStyle()
{
    UStyleManager* styleManager = UStyleManager::instance();
    QString themeName = styleManager->getThemeName();

    QString darkStyle =
            "QPushButton {"
            "    min-width: 32px; max-width: 32px; min-height: 32px; max-height: 32px;"
            "    padding: 0;"
            "    background-color: rgba(33, 37, 43, 220);"
            "    border: 1px solid #5C6370;"
            "    border-radius: 4px;"
            "    font-size: 18px;"
            "    color: #ABB2BF;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(44, 49, 58, 240);"
            "    border-color: #61AFEF;"
            "    color: #61AFEF;"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(30, 58, 95, 250);"
            "    border-color: #61AFEF;"
            "    color: #61AFEF;"
            "}";
    QString lightStyle =
            "QPushButton {"
            "    min-width: 32px; max-width: 32px; min-height: 32px; max-height: 32px;"
            "    padding: 0;"
            "    background-color: rgba(255, 255, 255, 200);"
            "    border: 1px solid #ccc;"
            "    border-radius: 4px;"
            "    font-size: 18px;"
            "    color: #374151;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(240, 240, 240, 220);"
            "    border-color: #5B8DEF;"
            "    color: #1E40AF;"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(220, 220, 220, 240);"
            "    border-color: #3B82F6;"
            "    color: #1E40AF;"
            "}";

    if(themeName == "Modern Dark" || themeName == "dark")
    {
        if(m_resetZoomButton)
            m_resetZoomButton->setStyleSheet(darkStyle);
        if(m_projectDescriptionButton)
            m_projectDescriptionButton->setStyleSheet(darkStyle);
        if(m_classesListButton)
            m_classesListButton->setStyleSheet(darkStyle);
    }
    else
    {
        if(m_resetZoomButton)
            m_resetZoomButton->setStyleSheet(lightStyle);
        if(m_projectDescriptionButton)
            m_projectDescriptionButton->setStyleSheet(lightStyle);
        if(m_classesListButton)
            m_classesListButton->setStyleSheet(lightStyle);
    }
}

void UModernDiagramViewportManager::updateResetZoomButtonStyle()
{
    updateOverlayButtonsStyle();
}

