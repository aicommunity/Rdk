#include "UWatchSeriesOption.h"
#include "ui_UWatchSeriesOption.h"

#include "UWatch.h"


UWatchSeriesOption::UWatchSeriesOption(QWidget *parent) :
   QDialog(parent),
    ui(new Ui::UWatchSeriesOption)
{
    ui->setupUi(this);
    WatchTab = dynamic_cast<UWatchTab*>(parent);

    loadGraphsName();
}

UWatchSeriesOption::~UWatchSeriesOption()
{
    delete ui;
}

void UWatchSeriesOption::on_okButton_clicked()
{
   saveParemeters();
   close();
   destroy();
   delete this;
}

void UWatchSeriesOption::on_applyButton_clicked()
{
    saveParemeters();
    int currentChartIndex = ui->graphsList->currentRow();
    updateGraphsSeries(currentChartIndex);
}

void UWatchSeriesOption::on_closeButton_clicked()
{
    close();
    destroy();
    delete this;
}

void UWatchSeriesOption::on_addSerieButton_clicked()
{
    if(!WatchTab)
     return;
    int currentChartIndex = ui->graphsList->currentRow();

    WatchTab->createSelectionDialog(currentChartIndex);
    updateGraphsSeries(currentChartIndex);
}

void UWatchSeriesOption::on_removeSerieButton_clicked()
{
    if(!WatchTab)
     return;
    //удаляем в выбранном графике выбранную среию
    int currentChartIndex = ui->graphsList->currentRow();
    int currentSerieIndex = ui->graphsSeriesList->currentRow();
    if(currentSerieIndex<0)
     return;
    WatchTab->getChart(currentChartIndex)->deleteSerie(currentSerieIndex);
    updateGraphsSeries(currentChartIndex);
}

void UWatchSeriesOption::on_graphsList_currentRowChanged(int currentRow)
{
    updateGraphsSeries(currentRow);
}

void UWatchSeriesOption::on_graphsSeriesList_currentRowChanged(int currentRow)
{
    updateSeriesProperties(currentRow);
}

void UWatchSeriesOption::loadGraphsName()
{
    if(!WatchTab)
     return;
    //загружаем имена графиков
    for (int i = 0; i < WatchTab->countGraphs(); i++)
    {
        ui->graphsList->addItem(WatchTab->getChart(i)->getChartTitle());
    }
    if (ui->graphsList->count()) ui->graphsList->setCurrentRow(0);
}

void UWatchSeriesOption::updateGraphsSeries(int currentChartIndex)
{
    if(!WatchTab)
     return;
    //загружаем имена серий выбранного графика
    ui->graphsSeriesList->clear();
    for (int i = 0; i < WatchTab->getChart(currentChartIndex)->countSeries(); i++)
    {
        ui->graphsSeriesList->addItem(WatchTab->getChart(currentChartIndex)->getSerieName(i));
    }
    if (ui->graphsSeriesList->count())  ui->graphsSeriesList->setCurrentRow(0);
}

void UWatchSeriesOption::updateSeriesProperties(int currentSerieIndex)
{
    //загружаем свойства выбранной серии
    if(currentSerieIndex == -1) return;
    int currentChartIndex = ui->graphsList->currentRow();

    updateSeriesNameProp(currentChartIndex, currentSerieIndex);
    updateSeriesColorProp(currentChartIndex, currentSerieIndex);
    updateSeriesWidthProp(currentChartIndex, currentSerieIndex);
    updateSeriesLineTypeProp(currentChartIndex, currentSerieIndex);
    updateSeriesYShift(currentChartIndex, currentSerieIndex);
}

void UWatchSeriesOption::updateSeriesNameProp(int currentChartIndex, int currentSerieIndex)
{
    if(!WatchTab)
     return;
    QString name = WatchTab->getChart(currentChartIndex)->getSerieName(currentSerieIndex);
    ui->serieName->setText(name);
}

void UWatchSeriesOption::updateSeriesColorProp(int currentChartIndex, int currentSerieIndex)
{
    if(!WatchTab)
     return;

    const QColor color = WatchTab->getChart(currentChartIndex)->getSerieColor(currentSerieIndex);
    for (int i = 0; i < 15; ++i)
        if (color == defaultColors[i])
        {
            ui->serieColor->setCurrentIndex(i);
            return;
        }
    ui->serieColor->setCurrentIndex(16);
}

void UWatchSeriesOption::updateSeriesWidthProp(int currentChartIndex, int currentSerieIndex)
{
    if(!WatchTab)
     return;

    int width = WatchTab->getChart(currentChartIndex)->getSerieWidth(currentSerieIndex);
    if (width<=6) ui->serieLneWidth->setCurrentIndex(width-1);
    else ui->serieLneWidth->setCurrentIndex(5); //6+
}

void UWatchSeriesOption::updateSeriesLineTypeProp(int currentChartIndex, int currentSerieIndex)
{
    if(!WatchTab)
     return;

    Qt::PenStyle lineStyle =  WatchTab->getChart(currentChartIndex)->getSerieLineType(currentSerieIndex);
     for (int i = 0; i < 4; ++i)
        if (lineStyle == defaultLineStyle[i]) ui->serieLineType->setCurrentIndex(i);
}

void UWatchSeriesOption::updateSeriesYShift(int currentChartIndex,int currentSerieIndex)
{
    if(!WatchTab)
     return;

    double y_shift = WatchTab->getChart(currentChartIndex)->getSerieYShift(currentSerieIndex);
    ui->serieYshift->setValue(y_shift);
}

void UWatchSeriesOption::saveParemeters()
{
    //сохранение параметров
    int currentChartIndex = ui->graphsList->currentRow();
    int currentSerieIndex = ui->graphsSeriesList->currentRow();

    if(currentChartIndex == -1) return;
    if(currentSerieIndex == -1) return;

    QString name = ui->serieName->text();
    int colorIndex = ui->serieColor->currentIndex();
    int width = ui->serieLneWidth->currentIndex();
    int lineTypeIndex = ui->serieLineType->currentIndex();
    double y_shift = ui->serieYshift->value();

    if(WatchTab)
    {
        WatchTab->getChart(currentChartIndex)->setSerieName(currentSerieIndex, name);
        WatchTab->getChart(currentChartIndex)->setSerieStyle(currentSerieIndex, defaultColors[colorIndex], width+1, defaultLineStyle[lineTypeIndex]);
        WatchTab->getChart(currentChartIndex)->setSerieYshift(currentSerieIndex, y_shift);
    }
}

