#include "UComponentPropertySelectionWidget.h"
#include "ui_UComponentPropertySelectionWidget.h"

#include <QDebug>

UComponentPropertySelectionWidget::UComponentPropertySelectionWidget(QWidget *parent, int mode, QString settingsFile, QString settingsGroup) :
    QDialog(parent),
    ui(new Ui::UComponentPropertySelectionWidget)
{
    ui->setupUi(this);
    componentsList = NULL;
    readSettings(settingsFile, settingsGroup);
    componentsList = new UComponentsListWidget(this, settingsFileName, settingsGroupName+"_componentsList");
    componentsList->setVerticalOrientation(false);
    componentsList->openTabN(mode);
    ui->horizontalLayoutComponentsList->addWidget(componentsList);
}

UComponentPropertySelectionWidget::~UComponentPropertySelectionWidget()
{
    writeSettings(settingsFileName, settingsGroupName);
    delete ui;
}

void UComponentPropertySelectionWidget::readSettings(QString file, QString group)
{
    settingsFileName = file;
    settingsGroupName = group;
    QSettings settings(settingsFileName, QSettings::IniFormat);
    settings.beginGroup(settingsGroupName);
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
    if(componentsList) componentsList->readSettings(settingsFileName, settingsGroupName+"_componentsList");
}

void UComponentPropertySelectionWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}
