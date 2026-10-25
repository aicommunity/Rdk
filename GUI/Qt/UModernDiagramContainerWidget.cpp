#include "UModernDiagramContainerWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QGuiApplication>
#include <QScreen>

namespace {
constexpr int kDefaultClassesPaneWidth = 280;
constexpr int kMinFloatHeight = 200;
constexpr int kMaxFloatHeight = 600;
}

UModernDiagramContainerWidget::UModernDiagramContainerWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app)
  , m_pinnedHost(nullptr)
  , m_pinnedLayout(nullptr)
  , m_unpinButton(nullptr)
  , m_floatDialog(nullptr)
  , m_floatLayout(nullptr)
  , m_pinButton(nullptr)
  , m_classesPinned(false)
  , m_classesVisible(false)
  , m_pinnedPaneWidth(kDefaultClassesPaneWidth)
{
    CheckModelFlag=false;

    // Создаем splitter для разделения диаграммы и списка классов
    splitter = new QSplitter(Qt::Horizontal, this);

    // Современная диаграмма
    modernScheme = new UModernDiagramWidget(splitter);
    modernScheme->SetApplication(app);
    modernScheme->SetComponentName("");
    splitter->addWidget(modernScheme);

    // Панель для закреплённой палитры классов
    m_pinnedHost = new QWidget(splitter);
    m_pinnedLayout = new QVBoxLayout(m_pinnedHost);
    m_pinnedLayout->setContentsMargins(0, 0, 0, 0);
    m_pinnedLayout->setSpacing(2);

    QHBoxLayout *unpinBar = new QHBoxLayout();
    unpinBar->setContentsMargins(4, 2, 4, 2);
    m_unpinButton = new QToolButton(m_pinnedHost);
    m_unpinButton->setText(tr("Unpin"));
    m_unpinButton->setToolTip(tr("Show classes palette as a floating window"));
    m_unpinButton->setAutoRaise(true);
    connect(m_unpinButton, &QToolButton::clicked, this, &UModernDiagramContainerWidget::unpinClassesList);
    unpinBar->addWidget(m_unpinButton);
    unpinBar->addStretch(1);
    m_pinnedLayout->addLayout(unpinBar);

    splitter->addWidget(m_pinnedHost);

    // Список классов (перемещается между float и pinned host)
    classesList = new UClassesListWidget(this, app);

    setupClassesHosts();

    // Настройка пропорций splitter
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    // Установка layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(splitter);

    // Подключение сигналов от современной диаграммы
    connect(modernScheme, SIGNAL(componentSelected(QString)), this, SIGNAL(componentSelectedFromScheme(QString)));
    connect(modernScheme, SIGNAL(componentDoubleClicked(QString)), this, SIGNAL(componentDoubleClickFromScheme(QString)));
    connect(modernScheme, SIGNAL(componentStapBack()), this, SIGNAL(componentStapBackFromScheme()));
    connect(modernScheme, SIGNAL(updateComponentsList()), this, SIGNAL(updateComponentsListFromScheme()));
    connect(modernScheme, SIGNAL(viewLinks(QString)), this, SIGNAL(viewLinksFromScheme(QString)));
    connect(modernScheme, SIGNAL(createLinks(QString,QString)), this, SIGNAL(createLinksFromScheme(QString,QString)));
    connect(modernScheme, SIGNAL(switchLinks(QString,QString)), this, SIGNAL(switchLinksFromScheme(QString,QString)));
    connect(modernScheme, SIGNAL(openComponentGuiRequested(UComponentGuiContext)), this, SIGNAL(openComponentGuiFromScheme(UComponentGuiContext)));
    connect(modernScheme, SIGNAL(openProjectDescriptionRequested()), this, SIGNAL(openProjectDescriptionRequested()));
    connect(modernScheme, &UModernDiagramWidget::classesListToggleRequested,
            this, &UModernDiagramContainerWidget::toggleClassesList);

    UpdateInterval = 0; // don't update by core ticks
    setAccessibleName("UModernDiagramContainerWidget"); // имя класса для сериализации
    ALoadParameters();
    applyClassesPresentation();

    UpdateInterface(true);
}

UModernDiagramContainerWidget::~UModernDiagramContainerWidget()
{
}

void UModernDiagramContainerWidget::setupClassesHosts()
{
    m_floatDialog = new QDialog(this, Qt::Tool | Qt::WindowCloseButtonHint);
    m_floatDialog->setWindowTitle(tr("Component classes"));
    m_floatDialog->setWindowModality(Qt::NonModal);
    m_floatDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    m_floatDialog->resize(kDefaultClassesPaneWidth, 480);

    m_floatLayout = new QVBoxLayout(m_floatDialog);
    m_floatLayout->setContentsMargins(4, 4, 4, 4);
    m_floatLayout->setSpacing(4);

    QHBoxLayout *pinBar = new QHBoxLayout();
    pinBar->setContentsMargins(0, 0, 0, 0);
    m_pinButton = new QToolButton(m_floatDialog);
    m_pinButton->setText(tr("Pin"));
    m_pinButton->setToolTip(tr("Dock classes palette to the right side of the diagram"));
    m_pinButton->setAutoRaise(true);
    connect(m_pinButton, &QToolButton::clicked, this, &UModernDiagramContainerWidget::pinClassesList);
    pinBar->addWidget(m_pinButton);
    pinBar->addStretch(1);
    m_floatLayout->addLayout(pinBar);

    connect(m_floatDialog, &QDialog::finished, this, [this](int) {
        if(m_classesVisible && !m_classesPinned)
        {
            m_classesVisible = false;
            applyClassesPresentation();
        }
    });
}

void UModernDiagramContainerWidget::ensureClassesListIn(QWidget* host, QVBoxLayout* layout)
{
    if(!classesList || !host || !layout)
        return;

    if(classesList->parentWidget() == host)
        return;

    if(QWidget* oldParent = classesList->parentWidget())
    {
        if(QVBoxLayout* oldLayout = qobject_cast<QVBoxLayout*>(oldParent->layout()))
            oldLayout->removeWidget(classesList);
    }

    classesList->setParent(host);
    layout->addWidget(classesList, 1);
}

void UModernDiagramContainerWidget::collapseClassesSplitterPane()
{
    if(!splitter || !m_pinnedHost)
        return;

    m_pinnedHost->hide();
    QList<int> sizes = splitter->sizes();
    if(sizes.size() >= 2)
    {
        const int total = sizes[0] + sizes[1];
        if(sizes[1] > 40)
            m_pinnedPaneWidth = sizes[1];
        sizes[0] = qMax(1, total);
        sizes[1] = 0;
        splitter->setSizes(sizes);
    }
}

void UModernDiagramContainerWidget::expandClassesSplitterPane()
{
    if(!splitter || !m_pinnedHost)
        return;

    m_pinnedHost->show();
    QList<int> sizes = splitter->sizes();
    if(sizes.size() >= 2)
    {
        const int total = qMax(sizes[0] + sizes[1], width());
        const int pane = qBound(m_pinnedPaneWidth, 180, qMax(180, total / 2));
        sizes[0] = qMax(1, total - pane);
        sizes[1] = pane;
        splitter->setSizes(sizes);
    }
}

void UModernDiagramContainerWidget::syncClassesListButton()
{
    if(modernScheme)
        modernScheme->setClassesListButtonChecked(m_classesVisible);
}

void UModernDiagramContainerWidget::applyClassesPresentation()
{
    if(!classesList)
        return;

    if(!m_classesVisible)
    {
        if(m_floatDialog && m_floatDialog->isVisible())
            m_floatGeometry = m_floatDialog->saveGeometry();
        if(m_floatDialog)
            m_floatDialog->hide();
        collapseClassesSplitterPane();
        if(QWidget* oldParent = classesList->parentWidget())
        {
            if(QVBoxLayout* oldLayout = qobject_cast<QVBoxLayout*>(oldParent->layout()))
                oldLayout->removeWidget(classesList);
        }
        classesList->setParent(this);
        classesList->hide();
        syncClassesListButton();
        return;
    }

    classesList->show();

    if(m_classesPinned)
    {
        if(m_floatDialog && m_floatDialog->isVisible())
            m_floatGeometry = m_floatDialog->saveGeometry();
        if(m_floatDialog)
            m_floatDialog->hide();

        ensureClassesListIn(m_pinnedHost, m_pinnedLayout);
        expandClassesSplitterPane();
    }
    else
    {
        collapseClassesSplitterPane();
        ensureClassesListIn(m_floatDialog, m_floatLayout);

        if(m_floatDialog)
        {
            if(!m_floatGeometry.isEmpty())
                m_floatDialog->restoreGeometry(m_floatGeometry);
            else
            {
                QPoint anchor = modernScheme ? modernScheme->mapToGlobal(QPoint(modernScheme->width() - kDefaultClassesPaneWidth - 48, 48))
                                             : mapToGlobal(QPoint(width() - kDefaultClassesPaneWidth - 20, 40));
                QScreen *screen = nullptr;
                if(window())
                    screen = window()->screen();
                if(!screen)
                    screen = QGuiApplication::primaryScreen();
                QRect avail = screen ? screen->availableGeometry() : QRect(anchor, QSize(800, 600));
                int h = qBound(avail.height() - (anchor.y() - avail.top()) - 20, kMinFloatHeight, kMaxFloatHeight);
                if(anchor.x() + kDefaultClassesPaneWidth > avail.right())
                    anchor.setX(avail.right() - kDefaultClassesPaneWidth);
                if(anchor.x() < avail.left())
                    anchor.setX(avail.left());
                m_floatDialog->setGeometry(anchor.x(), anchor.y(), kDefaultClassesPaneWidth, h);
            }
            m_floatDialog->show();
            m_floatDialog->raise();
            m_floatDialog->activateWindow();
        }
    }

    syncClassesListButton();
}

void UModernDiagramContainerWidget::toggleClassesList()
{
    m_classesVisible = !m_classesVisible;
    applyClassesPresentation();
}

void UModernDiagramContainerWidget::pinClassesList()
{
    m_classesPinned = true;
    m_classesVisible = true;
    if(m_floatDialog && m_floatDialog->isVisible())
        m_floatGeometry = m_floatDialog->saveGeometry();
    applyClassesPresentation();
}

void UModernDiagramContainerWidget::unpinClassesList()
{
    if(splitter)
    {
        QList<int> sizes = splitter->sizes();
        if(sizes.size() >= 2 && sizes[1] > 40)
            m_pinnedPaneWidth = sizes[1];
    }
    m_classesPinned = false;
    m_classesVisible = true;
    applyClassesPresentation();
}

void UModernDiagramContainerWidget::AUpdateInterface()
{
    modernScheme->Reload();
}

void UModernDiagramContainerWidget::AClearInterface()
{
    if(modernScheme)
        modernScheme->clearDiagram();
    if(classesList)
        classesList->ClearInterface();
}

void UModernDiagramContainerWidget::ASaveParameters()
{
    if(!application) return;

    if(m_floatDialog && m_floatDialog->isVisible())
        m_floatGeometry = m_floatDialog->saveGeometry();

    if(m_classesPinned && m_classesVisible && splitter)
    {
        QList<int> sizes = splitter->sizes();
        if(sizes.size() >= 2 && sizes[1] > 40)
            m_pinnedPaneWidth = sizes[1];
    }

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    settings.setValue("splitterState", splitter->saveState());
    settings.setValue("classesPinned", m_classesPinned);
    settings.setValue("classesVisible", m_classesVisible);
    settings.setValue("classesPaneWidth", m_pinnedPaneWidth);
    settings.setValue("classesFloatGeometry", m_floatGeometry);
    settings.endGroup();

    // Сохраняем состояние viewport для современной диаграммы
    if(modernScheme)
    {
        modernScheme->SaveViewState();
    }
}

void UModernDiagramContainerWidget::ALoadParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    m_classesPinned = settings.value("classesPinned", false).toBool();
    m_classesVisible = settings.value("classesVisible", false).toBool();
    m_pinnedPaneWidth = settings.value("classesPaneWidth", kDefaultClassesPaneWidth).toInt();
    m_floatGeometry = settings.value("classesFloatGeometry").toByteArray();
    const QByteArray splitterState = settings.value("splitterState").toByteArray();
    settings.endGroup();

    // Загружаем состояние viewport для современной диаграммы
    if(modernScheme)
    {
        modernScheme->LoadViewState();
    }

    // Сначала прячем палитру, затем применяем сохранённый режим (без мигания широкой панели).
    collapseClassesSplitterPane();
    if(m_classesPinned && m_classesVisible && !splitterState.isEmpty())
    {
        applyClassesPresentation();
        splitter->restoreState(splitterState);
        QList<int> sizes = splitter->sizes();
        if(sizes.size() >= 2 && sizes[1] > 40)
            m_pinnedPaneWidth = sizes[1];
    }
    else
    {
        applyClassesPresentation();
    }
}

void UModernDiagramContainerWidget::componentDoubleClick(QString name)
{
    modernScheme->SetComponentName(name);
    modernScheme->Reload();
}

void UModernDiagramContainerWidget::componentSingleClick(QString name)
{
    modernScheme->componentSingleClick(name);
}

void UModernDiagramContainerWidget::updateScheme(bool reloadXml)
{
    modernScheme->updateScheme(reloadXml);
}

void UModernDiagramContainerWidget::updateClassesList()
{
    // Обновление списка классов - можно вызвать AUpdateInterface если нужно
    if(classesList)
    {
        classesList->AUpdateInterface();
    }
}

void UModernDiagramContainerWidget::updateTheme()
{
    if(modernScheme)
    {
        modernScheme->updateTheme();
    }
}

void UModernDiagramContainerWidget::invalidatePortsCache(const QString& componentFullName)
{
    if(!modernScheme)
        return;
    if(componentFullName.isEmpty())
        modernScheme->invalidatePortsCache();
    else
        modernScheme->invalidatePortsCache(componentFullName);
}
