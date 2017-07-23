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
    componentsList = new UComponentsListWidget(this, settingsFile, settingsGroup+"_componentsList");
    componentsList->UpdateInterval = 0;
    componentsList->setVerticalOrientation(false);
    componentsList->openTabN(mode);
    ui->horizontalLayoutComponentsList->addWidget(componentsList);
}

UComponentPropertySelectionWidget::~UComponentPropertySelectionWidget()
{
    delete ui;
}

void UComponentPropertySelectionWidget::readSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();

    if(componentsList) componentsList->readSettings(file, group+"_componentsList");
}

void UComponentPropertySelectionWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    if(componentsList) componentsList->writeSettings(file, group+"_componentsList");
}
