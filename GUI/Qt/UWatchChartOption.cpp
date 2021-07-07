#include "UWatchChartOption.h"
#include "ui_UWatchChartOption.h"

#include "UWatch.h"

UWatchChartOption::UWatchChartOption(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UWatchChartOption)
{
    ui->setupUi(this);

    WatchTab = dynamic_cast<UWatchTab*>(parent);
    updateChartList();
    updateLayoutBox();
    if(WatchTab)
     ui->updateIntervalMs->setText(QString::number(WatchTab->UpdateIntervalMs));
}

UWatchChartOption::~UWatchChartOption()
{
    delete ui;
}


void UWatchChartOption::on_createLayot_button_clicked()
{
    createLayout();
    updateChartList();
}

void UWatchChartOption::updateChartList()
{
 if(!WatchTab)
  return;
    //обновляем имена в списке графиков
    ui->allChartsList->clear();
    int chartsAmount = WatchTab->countGraphs();

    for (int i = 0; i < chartsAmount; i++)
    {
        QString title = WatchTab->getChart(i)->getChartTitle();
        ui->allChartsList->addItem(title);
    }
    ui->allChartsList->setCurrentRow(0);
}

void UWatchChartOption::updateLayoutBox()
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

void UWatchChartOption::updateParameters(int chartIndex)
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

    ui->axisYmaxSB->setValue(WatchTab->getChart(chartIndex)->getAxisYmax());
    ui->axisYminSB->setValue(WatchTab->getChart(chartIndex)->getAxisYmin());
    ui->axisXrangeSB->setValue(WatchTab->getChart(chartIndex)->getAxisXmax() - WatchTab->getChart(chartIndex)->getAxisXmin());
}

void UWatchChartOption::createLayout()
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

void UWatchChartOption::saveParameters()
{
    if(!WatchTab)
     return;

    //сохранение всех параметров
   int index = ui->allChartsList->currentRow();
   if (index == -1) return;

   WatchTab->getChart(index)->setChartTitle(ui->graphNameEditor->text());
   WatchTab->getChart(index)->setAxisXname(ui->axisXNameEditor->text());
   WatchTab->getChart(index)->setAxisYname(ui->axisYNameEditor->text());

   WatchTab->getChart(index)->setAxisYmax(ui->axisYmaxSB->value());
   WatchTab->getChart(index)->setAxisYmin(ui->axisYminSB->value());
   WatchTab->getChart(index)->updateTimeIntervals(ui->axisXrangeSB->value());
   WatchTab->getChart(index)->fixInitialAxesState();

   WatchTab->getChart(index)->isAxisYzoomable = ui->axisYzoomCB->isChecked();
   WatchTab->getChart(index)->isAxisYscrollable= ui->axisYscrollCB->isChecked();
   WatchTab->getChart(index)->isAxisXtrackable= ui->axisXtrackCB->isChecked();

   WatchTab->saveUpdateInterval(ui->updateIntervalMs->text().toInt());
}
void UWatchChartOption::on_allChartsList_currentRowChanged(int currentRow)
{
    if(currentRow == -1) return;
    updateParameters(currentRow);
}

void UWatchChartOption::on_okButton_clicked()
{
    saveParameters();
    close();
    destroy();
    delete this;
}

void UWatchChartOption::on_applyButton_clicked()
{
    saveParameters();
    updateChartList();
}

void UWatchChartOption::on_closeButoon_clicked()
{
    close();
    destroy();
    delete this;
}

void UWatchChartOption::on_axisXtrackCB_stateChanged(int arg1)
{
    if(arg1) ui->axisXrangeSB->setEnabled(true);
    else ui->axisXrangeSB->setDisabled(true);
}
