#include "UDrawEngineWidget.h"
#include "ui_UDrawEngineWidget.h"
#include <QDebug>
#include <QThread>

UDrawEngineWidget::UDrawEngineWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UDrawEngineWidget)
{
    ui->setupUi(this);

    classesList = new UClassesListWidget(this);
    ui->verticalLayoutForClassesList->addWidget(classesList);

    modelScheme = new UDrawEngineImageWidget(this);
    connect(modelScheme, SIGNAL(componentSelected(QString)), this, SIGNAL(componentSelectedFromScheme(QString)));
    connect(modelScheme, SIGNAL(componentDoubleClick(QString)), this, SIGNAL(componentDoubleClickFromScheme(QString)));
    connect(modelScheme, SIGNAL(componentStapBack()), this, SIGNAL(componentStapBackFromScheme()));
    connect(modelScheme, SIGNAL(updateComponentsList()), this, SIGNAL(updateComponentsListFromScheme()));
    connect(modelScheme, SIGNAL(viewLinks(QString)), this, SIGNAL(viewLinksFromScheme(QString)));
    connect(modelScheme, SIGNAL(createLinks(QString,QString)), this, SIGNAL(createLinksFromScheme(QString,QString)));
    connect(modelScheme, SIGNAL(switchLinks(QString,QString)), this, SIGNAL(switchLinksFromScheme(QString,QString)));
    ui->horizontalLayoutForModelScheme->addWidget(modelScheme);

    UpdateInterval = 0; // don't update by core ticks
    setAccessibleName("UDrawEngineWidget"); // имя класса для сериализации
    ALoadParameters();

    UpdateInterface(true);
}

UDrawEngineWidget::~UDrawEngineWidget()
{
    delete ui;
}

void UDrawEngineWidget::AUpdateInterface()
{
  modelScheme->reDrawScheme(true);
}

void UDrawEngineWidget::ASaveParameters()
{
  if(!application) return;

  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt",
                     QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  settings.setValue("splitterState", ui->splitter->saveState());
  settings.setValue("labelModelScheme_w", QVariant(modelScheme->width()));
  settings.setValue("labelModelScheme_h", QVariant(modelScheme->height()));
  settings.endGroup();
}

void UDrawEngineWidget::ALoadParameters()
{
  if(!application) return;

  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt",
                     QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  ui->splitter->restoreState(settings.value("splitterState").toByteArray());
  modelScheme->resize(settings.value("labelModelScheme_w").toInt(),
                               settings.value("labelModelScheme_h").toInt());
  settings.endGroup();
}

void UDrawEngineWidget::componentDoubleClick(QString name)
{
    modelScheme->setComponentName(name);
}

void UDrawEngineWidget::componentSingleClick(QString name)
{
    modelScheme->selectComponent(name);
    //updateScheme(true);
}

void UDrawEngineWidget::updateScheme(bool reloadXml)
{
    modelScheme->reDrawScheme(reloadXml);
}
