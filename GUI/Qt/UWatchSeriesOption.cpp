#include "UWatchSeriesOption.h"
#include "ui_UWatchSeriesOption.h"

#include "UWatch.h"


UWatchSeriesOption::UWatchSeriesOption(QWidget *parent) :
   QDialog(parent),
    ui(new Ui::UWatchSeriesOption)
{
    ui->setupUi(this);
    WatchTab = dynamic_cast<UWatchTab*>(parent);

    updateChartList();
    updateLayoutBox();
    if(WatchTab)
     ui->updateIntervalMs->setText(QString::number(WatchTab->UpdateIntervalMs));

//    loadGraphsName();
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
    if(currentRow == -1)
        return;
    updateParameters(currentRow);
    updateGraphsSeries(currentRow);
}

void UWatchSeriesOption::on_graphsSeriesList_currentRowChanged(int currentRow)
{
    updateSeriesProperties(currentRow);
}

void UWatchSeriesOption::updateChartList()
{
 if(!WatchTab)
  return;
    //обновляем имена в списке графиков
    int current_row = ui->graphsList->currentRow();
    ui->graphsList->clear();
    int chartsAmount = WatchTab->countGraphs();

    for (int i = 0; i < chartsAmount; i++)
    {
        QString title = WatchTab->getChart(i)->getChartTitle();
        ui->graphsList->addItem(title);
    }
    if (ui->graphsList->count())
    {
        if(current_row >=0 && current_row < ui->graphsList->count())
            ui->graphsList->setCurrentRow(current_row);
        else
            ui->graphsList->setCurrentRow(0);
    }

}

void UWatchSeriesOption::updateLayoutBox()
{
    if(!WatchTab)
     return;
    //настраиваем поля выбора кол-ва колонок и строк
    int colNumber = WatchTab->getColNumber();
    int rowNumber = WatchTab->getRowNumber();
    if (colNumber && rowNumber)
    {
        ui->chartColNumber_spin->setValue(colNumber);
        ui->chartRowNumber_spin->setValue(rowNumber);
    }
}

void UWatchSeriesOption::updateParameters(int chartIndex)
{
    if(!WatchTab)
     return;
    ui->graphNameEditor->setText(WatchTab->getChart(chartIndex)->getChartTitle());

    /*ДОДЕЛАТЬ*/
    ui->legendVisibilitSB->setDisabled(true);
    ui->tittleVisibilityCB->setDisabled(true);
    //ui->legendVisibilitSB->setTristate(WatchTab->getChart(currentRow)->)
    //добавить видимость легенды и названия

    //загрузка параметров графика и его осей
    ui->axisXNameEditor->setText(WatchTab->getChart(chartIndex)->getAxisXName());
    ui->axisYNameEditor->setText(WatchTab->getChart(chartIndex)->getAxisYName());

    ui->axisYzoomCB->setChecked(WatchTab->getChart(chartIndex)->isAxisYzoomable);
    ui->axisYscrollCB->setChecked(WatchTab->getChart(chartIndex)->isAxisYscrollable);
    ui->axisXtrackCB->setChecked(WatchTab->getChart(chartIndex)->isAxisXtrackable);

    if(ui->axisXtrackCB->isChecked()) ui->axisXrangeSB->setEnabled(true);
    else ui->axisXrangeSB->setDisabled(true);

    ui->axisYmaxSB->setValue(WatchTab->getChart(chartIndex)->getInitialAxisYmax());
    ui->axisYminSB->setValue(WatchTab->getChart(chartIndex)->getInitialAxisYmin());
    ui->axisXrangeSB->setValue(WatchTab->getChart(chartIndex)->getAxisXrange());
}

void UWatchSeriesOption::createLayout()
{
    if(!WatchTab)
     return;

    //спрашиваем юзера, точно ли он хочет изменить параметры сетки
    //но не спрашиваем если там нет серий
    if (WatchTab->getChart(0)->countSeries() == 0)
    {
        ;
    }
    else
    {
        // Если сетка графиков уменьшается (прозойдет удаление графиков)
        QString grid_reduce = "";
        if(ui->chartColNumber_spin->value()*ui->chartRowNumber_spin->value() < WatchTab->getRowNumber()*WatchTab->getColNumber())
            grid_reduce =   "New layout contains fewer charts than it was, so "
                            + QString::number(WatchTab->getRowNumber()*WatchTab->getColNumber() - ui->chartColNumber_spin->value()*ui->chartRowNumber_spin->value()) +
                            " charts will be deleted";

        //спрашиваем юзера точно ли он уверен
        QMessageBox messageBox;
        messageBox.setText("Are you sure you want to change Grid layout params?");
        messageBox.setInformativeText
                    ("Current layout is: " + QString::number(WatchTab->getRowNumber())
                    + " by " + QString::number(WatchTab->getColNumber())
                    + "\nWill be changed to layout: "
                    + QString::number(ui->chartRowNumber_spin->value()) + " by " + QString::number(ui->chartColNumber_spin->value())
                    + "\n" +grid_reduce
                    );
        messageBox.setWindowTitle("Change Grid layout params");
        messageBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        messageBox.setIcon(QMessageBox::Question);

        if(messageBox.exec() != QMessageBox::Yes) return;
    }

    //создание нового расположения
    int colNumber = ui->chartColNumber_spin->value();
    int rowNumber = ui->chartRowNumber_spin->value();

    WatchTab->createGridLayout(rowNumber,colNumber);
}

void UWatchSeriesOption::saveChartParameters()
{
    if(!WatchTab)
     return;

    //сохранение всех параметров
   int index = ui->graphsList->currentRow();
   if (index == -1) return;

   WatchTab->getChart(index)->setChartTitle(ui->graphNameEditor->text());
   WatchTab->getChart(index)->setAxisXname(ui->axisXNameEditor->text());
   WatchTab->getChart(index)->setAxisYname(ui->axisYNameEditor->text());

   WatchTab->getChart(index)->setAxisYmax(ui->axisYmaxSB->value());
   WatchTab->getChart(index)->setAxisYmin(ui->axisYminSB->value());
   WatchTab->getChart(index)->setAxisXrange(ui->axisXrangeSB->value());
   WatchTab->getChart(index)->updateTimeIntervals(ui->axisXrangeSB->value());
   WatchTab->getChart(index)->fixInitialAxesState();

   WatchTab->getChart(index)->isAxisYzoomable = ui->axisYzoomCB->isChecked();
   WatchTab->getChart(index)->isAxisYscrollable= ui->axisYscrollCB->isChecked();
   WatchTab->getChart(index)->isAxisXtrackable= ui->axisXtrackCB->isChecked();

   WatchTab->saveUpdateInterval(ui->updateIntervalMs->text().toInt());
}
/*
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
}*/

void UWatchSeriesOption::updateGraphsSeries(int currentChartIndex)
{
    if(!WatchTab)
     return;

    if(currentChartIndex < 0 || currentChartIndex >= WatchTab->countGraphs())
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
    saveChartParameters();

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


void UWatchSeriesOption::on_createLayot_button_clicked()
{
    createLayout();
    updateChartList();
}


void UWatchSeriesOption::on_axisXtrackCB_stateChanged(int arg1)
{
    if(arg1)
     ui->axisXrangeSB->setEnabled(true);
    else
     ui->axisXrangeSB->setDisabled(true);
}

