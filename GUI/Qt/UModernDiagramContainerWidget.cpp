#include "UModernDiagramContainerWidget.h"
#include <QHBoxLayout>
#include <QSettings>

UModernDiagramContainerWidget::UModernDiagramContainerWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app)
{
    CheckModelFlag=false;

    // Создаем splitter для разделения диаграммы и списка классов
    splitter = new QSplitter(Qt::Horizontal, this);
    
    // Современная диаграмма
    modernScheme = new UModernDiagramWidget(splitter);
    modernScheme->SetApplication(app);
    modernScheme->SetComponentName("");
    splitter->addWidget(modernScheme);
    
    // Список классов
    classesList = new UClassesListWidget(splitter, app);
    splitter->addWidget(classesList);
    
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

    UpdateInterval = 0; // don't update by core ticks
    setAccessibleName("UModernDiagramContainerWidget"); // имя класса для сериализации
    ALoadParameters();

    UpdateInterface(true);
}

UModernDiagramContainerWidget::~UModernDiagramContainerWidget()
{
}

void UModernDiagramContainerWidget::AUpdateInterface()
{
    modernScheme->Reload();
}

void UModernDiagramContainerWidget::ASaveParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    settings.setValue("splitterState", splitter->saveState());
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
    splitter->restoreState(settings.value("splitterState").toByteArray());
    settings.endGroup();
    
    // Загружаем состояние viewport для современной диаграммы
    if(modernScheme)
    {
        modernScheme->LoadViewState();
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

