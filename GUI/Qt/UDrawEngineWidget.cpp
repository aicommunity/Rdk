#include "UDrawEngineWidget.h"
#include "ui_UDrawEngineWidget.h"
#include <QDebug>
#include <QThread>

UDrawEngineWidget::UDrawEngineWidget(QWidget *parent, QString settingsFile, QString settingsGroup) :
    UVisualControllerWidget(parent),
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
    ui->horizontalLayoutForModelScheme->addWidget(modelScheme);

    UpdateInterval = 0; // don't update by kore ticks
    setAccessibleName("UDrawEngineWidget"); // имя класса для сериализации
    readSettings(settingsFile, settingsGroup);

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

void UDrawEngineWidget::readSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    modelScheme->resize(settings.value("labelModelScheme_w").toInt(),
                                 settings.value("labelModelScheme_h").toInt());
    settings.endGroup();
}

void UDrawEngineWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("labelModelScheme_w", QVariant(modelScheme->width()));
    settings.setValue("labelModelScheme_h", QVariant(modelScheme->height()));
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
