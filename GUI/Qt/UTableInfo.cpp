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
    ui->tableWidget->setColumnWidth(0, 200);
    ui->tableWidget->setColumnWidth(1, 40);

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
    addAction(ui->actionDeleteCurrentItem);


    //Соединение действия выше и слотав котором выполняется действие, кпо щелчку
    connect(ui->actionSelectComponent, SIGNAL(triggered()), this, SLOT(slotSelectComponent()));
    connect(ui->actionselectAllComponents, SIGNAL(triggered()), this, SLOT(slotSelectAllComponents()));
    connect(ui->actionDeleteAll, SIGNAL(triggered()), this, SLOT(slotDeleteAll()));
    connect(ui->actionDeleteCurrentItem, SIGNAL(triggered()), this, SLOT(slotDeleteCurrentItem()));

}

UTableInfo::~UTableInfo()
{
    delete ui;
}



void UTableInfo::AUpdateInterface()
{
    int sel_index=Core_GetSelectedChannelIndex();
    RDK::UChannelProfiler* profiler=application->GetEngineControl()->GetChannelProfiler(sel_index);
    RDK::UChannelProfiler* pefromance=application->GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
    if(!profiler)
        return;
    std::list<std::pair<std::string, RDK::UPerfomanceResults> > comp_perfomance = profiler->GetComponentsProfilerOutputData();
    std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator cI=comp_perfomance.begin(),cJ=comp_perfomance.end();

    int rowCount =ui->tableWidget->rowCount();
    for (int j=rowCount-1; j>=0; j--)
    {
        ui->tableWidget->removeRow(j);
    }


    for(int i=0;cI != cJ;++cI, i++)
    {
        // Вставляем строку
        ui->tableWidget->insertRow(i);

        ui->tableWidget->setItem(i,0, new QTableWidgetItem(cI->first.c_str()));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(QString::number(cI->second.AvgDuration*1000)));


    }
    /*pefromance->AddAllComponents(componentname);
    std::list<std::pair<std::string, RDK::UPerfomanceResults> > comp_perfomance2 = pefromance->GetComponentsProfilerOutputData();

    cI=comp_perfomance2.begin();
    cJ=comp_perfomance2.end();
    for(int i=0;cI != cJ;++cI, i++)
    {
        // Вставляем строку
        ui->tableWidget->insertRow(i);

        ui->tableWidget->setItem(i,0, new QTableWidgetItem(cI->first.c_str()));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(QString::number(cI->second.AvgDuration*1000)));
    }*/


    //pefromance->AddAllComponents(componentname);



}




void UTableInfo::ASaveParameters()
{
    if(!application) return;


}

void UTableInfo::ALoadParameters()
{
    if(!application) return;
    /*int sel_index=Core_GetSelectedChannelIndex();
    RDK::UChannelProfiler* profiler=application->GetEngineControl()->GetChannelProfiler(sel_index);
    if(!profiler)
        return;
    std::list<std::pair<std::string, RDK::UPerfomanceResults> > comp_perfomance = profiler->GetComponentsProfilerOutputData();
    std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator cI=comp_perfomance.begin(),cJ=comp_perfomance.end();*/
    int rowCount =ui->tableWidget->rowCount();
    for (int j=rowCount-1; j>=0; j--)
    {
        ui->tableWidget->removeRow(j);
    }
}

void UTableInfo::slotSelectComponent()
{

}



#endif


