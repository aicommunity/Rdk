#ifndef U_TABLE_INFO_WIDGET_CPP
#define U_TABLE_INFO_WIDGET_CPP
#include "UTableInfo.h"
#include "ui_UTableInfo.h"
#include <iostream>


UTableInfo::UTableInfo(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent,app),
    ui(new Ui::UTableInfo)
{
    setAccessibleName("UTableInfo");
    ui->setupUi(this);
    //столбцы
    ui->tableWidget->setColumnCount(2);
    //строки
    ui->tableWidget->setRowCount(0);
    // Разрешаем выделение только одного элемента
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Разрешаем изменять ширину колонок
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  //  ui->tableWidget->setColumnWidth(0, 200);
  //  ui->tableWidget->setColumnWidth(1, 40);

    //Заголовки
    QStringList headers;
    headers<<"Component name"<<"Time, ms";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    //ui->tableWidget->setItem(0,0, new QTableWidgetItem("Full step"));
    //ui->tableWidget->setItem(1,0, new QTableWidgetItem("Model"));

    //Добавление действий по правой кнопке
    addAction(ui->actionSelectComponent);
    addAction(ui->actionselectAllComponents);
    addAction(ui->actionDeleteAll);
    addAction(ui->actionAddGroupComponents);


    //Соединение действия выше и слотав котором выполняется действие, кпо щелчку
    connect(ui->actionSelectComponent, SIGNAL(triggered()), this, SLOT(slotSelectComponent()));
    connect(ui->actionselectAllComponents, SIGNAL(triggered()), this, SLOT(slotSelectAllComponents()));
    connect(ui->actionDeleteAll, SIGNAL(triggered()), this, SLOT(slotDeleteAll()));
    connect(ui->actionAddGroupComponents, SIGNAL(triggered()), this, SLOT(slotAddGroupComponents()));

    //столбцы
    ui->tableWidget_2->setColumnCount(2);
    //строки
    ui->tableWidget_2->setRowCount(0);
    // Разрешаем выделение только одного элемента
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Разрешаем изменять ширину колонок
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_2->setColumnWidth(0, 200);
    ui->tableWidget_2->setColumnWidth(1, 40);

    QStringList headers_gui;
    headers<<"Gui Widget name"<<"Time, ms";
    ui->tableWidget_2->setHorizontalHeaderLabels(headers_gui);

}

UTableInfo::~UTableInfo()
{
    delete ui;
}



void UTableInfo::AUpdateInterface()
{
    int sel_index=Core_GetSelectedChannelIndex();
    RDK::UChannelProfiler* profiler=application->GetEngineControl()->GetChannelProfiler(sel_index);

    if(!profiler)
        return;


    std::list<std::pair<std::string, RDK::UPerfomanceResults> > comp_perfomance = profiler->GetComponentsProfilerOutputData();
    std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator cI=comp_perfomance.begin(),cJ=comp_perfomance.end();

    // Обновляем список производительности компонент
    int rowCount =ui->tableWidget->rowCount();
    for (int j=rowCount-1; j>=0; j--)
    {
        ui->tableWidget->removeRow(j);
    }


    for(int i=0;cI != cJ;++cI, i++)
    {
        // Вставляем строку
        ui->tableWidget->insertRow(i);
        QString a =  cI->first.c_str();
        a.remove("\r\n");
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(a));

        //ui->tableWidget->setItem(i,0, new QTableWidgetItem(cI->first.c_str()));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(QString::number(cI->second.AvgDuration*1000)));
    }

    // Обновляем список производительности Gui
    std::list<std::pair<std::string, RDK::UPerfomanceResults> > gui_perfomance=profiler->GetGuiProfilerOutputData();
    std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator gI=gui_perfomance.begin(),gJ=gui_perfomance.end();


    int rowCountGui =ui->tableWidget_2->rowCount();
    for (int j=rowCountGui-1; j>=0; j--)
    {
        ui->tableWidget_2->removeRow(j);
    }


    for(int i=0;gI != gJ;++gI, i++)
    {
        // Вставляем строку
        ui->tableWidget_2->insertRow(i);
        QString a =  gI->first.c_str();
        a.remove("\r\n");
        ui->tableWidget_2->setItem(i,0, new QTableWidgetItem(a));

        //ui->tableWidget->setItem(i,0, new QTableWidgetItem(cI->first.c_str()));
        ui->tableWidget_2->setItem(i,1, new QTableWidgetItem(QString::number(gI->second.AvgDuration*1000)));
    }

}




void UTableInfo::ASaveParameters()
{
    if(!application) return;


}

void UTableInfo::ALoadParameters()
{
    if(!application) return;
    int rowCount =ui->tableWidget->rowCount();
    for (int j=rowCount-1; j>=0; j--)
    {
        ui->tableWidget->removeRow(j);
    }
}

void UTableInfo::updateVetorComponent (std::string compName)
{
    RDK::UChannelProfiler* pefromance=application->GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());

    pefromance->AddComponent(compName);

}


void UTableInfo::slotSelectComponent()
{
    if(!application)
        return;

    UComponentPropertySelectionWidget dialog(this, 3, application);
    if (dialog.exec())
    {
        std::string componentName = dialog.componentsList->getSelectedComponentLongName().toLocal8Bit().data();
        //componentNameVector.push_back(componentName);
        updateVetorComponent(componentName);


        /*selectedImage->setComponentName(dialog.componentsList->getSelectedComponentLongName());
        selectedImage->setComponentPropertyName(dialog.componentsList->getSelectedPropertyName());
        selectedImage->setCalcChannel(dialog.componentsList->getSelectedChannelIndex());*/
    }
    dialog.writeSettings(QString::fromLocal8Bit(
                           application->GetProjectPath().c_str())+"settings.qt");

}


void UTableInfo::slotSelectAllComponents()
{
    if(!application)
        return;
    RDK::UChannelProfiler* pefromance=application->GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
    pefromance->AddAllComponents("");

}
void UTableInfo::slotDeleteAll()
{
    if(!application)
        return;
    RDK::UChannelProfiler* pefromance=application->GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
    pefromance->DelAllComponents();
}
void UTableInfo::slotAddGroupComponents()
{
    if(!application)
        return;

    UComponentPropertySelectionWidget dialog(this, 3, application);
    if (dialog.exec())
    {
        std::string componentName = dialog.componentsList->getSelectedComponentLongName().toLocal8Bit().data();

        RDK::UChannelProfiler* pefromance=application->GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
        pefromance->AddAllComponents(componentName);

    }
    dialog.writeSettings(QString::fromLocal8Bit(
                           application->GetProjectPath().c_str())+"settings.qt");

}


#endif


