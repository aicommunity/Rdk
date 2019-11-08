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
    //�������
    ui->tableWidget->setColumnCount(2);
    //������
    ui->tableWidget->setRowCount(0);
    // ��������� ��������� ������ ������ ��������
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // ��������� ��������� ���������
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // ��������� �������� ������ �������
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  //  ui->tableWidget->setColumnWidth(0, 200);
  //  ui->tableWidget->setColumnWidth(1, 40);

    //���������
    QStringList headers;
    headers<<"Component name"<<"Time, ms";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    //ui->tableWidget->setItem(0,0, new QTableWidgetItem("Full step"));
    //ui->tableWidget->setItem(1,0, new QTableWidgetItem("Model"));

    //���������� �������� �� ������ ������
    addAction(ui->actionSelectComponent);
    addAction(ui->actionselectAllComponents);
    addAction(ui->actionDeleteAll);
    addAction(ui->actionAddGroupComponents);


    //���������� �������� ���� � ������ ������� ����������� ��������, ��� ������
    connect(ui->actionSelectComponent, SIGNAL(triggered()), this, SLOT(slotSelectComponent()));
    connect(ui->actionselectAllComponents, SIGNAL(triggered()), this, SLOT(slotSelectAllComponents()));
    connect(ui->actionDeleteAll, SIGNAL(triggered()), this, SLOT(slotDeleteAll()));
    connect(ui->actionAddGroupComponents, SIGNAL(triggered()), this, SLOT(slotAddGroupComponents()));

    //�������
    ui->tableWidget_2->setColumnCount(2);
    //������
    ui->tableWidget_2->setRowCount(0);
    // ��������� ��������� ������ ������ ��������
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
    // ��������� ��������� ���������
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    // ��������� �������� ������ �������
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

    // ��������� ������ ������������������ ���������
    int rowCount =ui->tableWidget->rowCount();
    for (int j=rowCount-1; j>=0; j--)
    {
        ui->tableWidget->removeRow(j);
    }


    for(int i=0;cI != cJ;++cI, i++)
    {
        // ��������� ������
        ui->tableWidget->insertRow(i);
        QString a =  cI->first.c_str();
        a.remove("\r\n");
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(a));

        //ui->tableWidget->setItem(i,0, new QTableWidgetItem(cI->first.c_str()));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(QString::number(cI->second.AvgDuration*1000)));
    }

    // ��������� ������ ������������������ Gui
    std::list<std::pair<std::string, RDK::UPerfomanceResults> > gui_perfomance=profiler->GetGuiProfilerOutputData();
    std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator gI=gui_perfomance.begin(),gJ=gui_perfomance.end();


    int rowCountGui =ui->tableWidget_2->rowCount();
    for (int j=rowCountGui-1; j>=0; j--)
    {
        ui->tableWidget_2->removeRow(j);
    }


    for(int i=0;gI != gJ;++gI, i++)
    {
        // ��������� ������
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


