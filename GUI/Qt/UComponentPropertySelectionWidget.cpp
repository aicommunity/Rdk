#include "UComponentPropertySelectionWidget.h"
#include "ui_UComponentPropertySelectionWidget.h"

#include <QDebug>

UComponentPropertySelectionWidget::UComponentPropertySelectionWidget(QWidget *parent, int mode, RDK::UApplication *app, int treeExpansionPolicy) :
    QDialog(parent),
    ui(new Ui::UComponentPropertySelectionWidget)
{
    ui->setupUi(this);
    componentsList = NULL;
    application = app;
    componentsList = new UComponentsListWidget(this, application, 0);
    // Отключаем автоматические обновления в диалоге, чтобы дерево не сворачивалось автоматически
    componentsList->setUpdateInterval(0);
    componentsList->setVerticalOrientation(false);
    componentsList->setTreeExpansionPolicy(treeExpansionPolicy);
    componentsList->openTabN(mode);
    ui->horizontalLayoutComponentsList->addWidget(componentsList);
    if(application)
    {
     readSettings(QString::fromLocal8Bit(
                   application->GetProjectPath().c_str())+"settings.qt");
     componentsList->ALoadParameters();
    }
    // Вызываем обновление интерфейса один раз для первоначальной загрузки дерева
    componentsList->UpdateInterface(true);
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
}

void UComponentPropertySelectionWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}
