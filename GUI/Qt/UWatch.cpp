#include "UWatch.h"
#include "ui_UWatch.h"

UWatch::UWatch(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UWatch)
{
    ui->setupUi(this);
    //создаем первую вкладку
    createTab();
}

UWatch::~UWatch()
{
    delete ui;
}

UWatchTab *UWatch::getCurrentTab()
{
    return tab[ui->tabWidget->currentIndex()];
}


void UWatch::on_actionCreate_tab_triggered()
{
    createTab();
}

void UWatch::on_actionSeries_option_triggered()
{
    seriesOption = new UWatchSeriesOption(this);
    //seriesOption->setModal(true);
    seriesOption->show();

    //UWatchSeriesOption seriesOption(this);
    //seriesOption.setModal(true);
    //seriesOption.show();
    //seriesOption.exec();
}

void UWatch::on_actionCharts_option_triggered()
{
    chartOption = new UWatchChartOption(this);
    chartOption->setModal(true);
    chartOption->show();
    //chartOption->exec();

    //UWatchChartOption chartOption(this);
    //chartOption.setModal(true);
    //chartOption.show();
    //chartOption.exec();
}

void UWatch::createTab()
{
    //создаем каждую новую вкладку с именем tab
    tab.push_back(new UWatchTab(this));
    ui->tabWidget->addTab(tab.last(), QString("tab %0").arg(ui->tabWidget->count()+1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
}

void UWatch::deleteTab(int index)
{
    ui->tabWidget->removeTab(index);
    delete tab[index];
    tab.remove(index);
}

void UWatch::on_tabWidget_tabCloseRequested(int index)
{
    deleteTab(index);
}
