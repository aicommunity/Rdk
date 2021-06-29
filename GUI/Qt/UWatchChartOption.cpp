#include "UWatchChartOption.h"
#include "ui_UWatchChartOption.h"

#include "UWatch.h"

UWatchChartOption::UWatchChartOption(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UWatchChartOption)
{
    ui->setupUi(this);

    Watch = dynamic_cast<UWatch*>(parent);
    updateChartList();
    updateLayoutBox();
    if(Watch->getCurrentTab())
     ui->updateIntervalMs->setText(QString::number(Watch->getCurrentTab()->UpdateIntervalMs));
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
 if(!Watch->getCurrentTab())
  return;
    //обновл€ем имена в списке графиков
    ui->allChartsList->clear();
    int chartsAmount = Watch->getCurrentTab()->countGraphs();

    for (int i = 0; i < chartsAmount; i++)
    {
        QString title = Watch->getCurrentTab()->getChart(i)->getChartTitle();
        ui->allChartsList->addItem(title);
    }
    ui->allChartsList->setCurrentRow(0);
}

void UWatchChartOption::updateLayoutBox()
{
    if(!Watch->getCurrentTab())
     return;
    //настраиваем пол€ выбора кол-ва колонок и строк
    int colNumber = Watch->getCurrentTab()->getColNumber();
    int rowNumber = Watch->getCurrentTab()->getRowNumber();
    if (colNumber && rowNumber)
    {
        ui->chartColNumber_spin->setValue(colNumber);
        ui->chartRowNumber_spin->setValue(rowNumber);
    }
}

void UWatchChartOption::updateParameters(int chartIndex)
{
    if(!Watch->getCurrentTab())
     return;
    ui->graphNameEditor->setText(Watch->getCurrentTab()->getChart(chartIndex)->getChartTitle());

    /*ƒќƒ≈Ћј“№*/
    ui->legendVisibilitSB->setDisabled(true);
    ui->tittleVisibilityCB->setDisabled(true);
    //ui->legendVisibilitSB->setTristate(Watch->getCurrentTab()->getChart(currentRow)->)
    //добавить видимость легенды и названи€

    //загрузка параметров графика и его осей
    ui->axisXNameEditor->setText(Watch->getCurrentTab()->getChart(chartIndex)->getAxisXName());
    ui->axisYNameEditor->setText(Watch->getCurrentTab()->getChart(chartIndex)->getAxisYName());

    ui->axisYzoomCB->setChecked(Watch->getCurrentTab()->getChart(chartIndex)->isAxisYzoomable);
    ui->axisYscrollCB->setChecked(Watch->getCurrentTab()->getChart(chartIndex)->isAxisYscrollable);
    ui->axisXtrackCB->setChecked(Watch->getCurrentTab()->getChart(chartIndex)->isAxisXtrackable);

    if(ui->axisXtrackCB->isChecked()) ui->axisXrangeSB->setEnabled(true);
    else ui->axisXrangeSB->setDisabled(true);

    ui->axisYmaxSB->setValue(Watch->getCurrentTab()->getChart(chartIndex)->getAxisYmax());
    ui->axisYminSB->setValue(Watch->getCurrentTab()->getChart(chartIndex)->getAxisYmin());
    ui->axisXrangeSB->setValue(Watch->getCurrentTab()->getChart(chartIndex)->getAxisXmax() - Watch->getCurrentTab()->getChart(chartIndex)->getAxisXmin());
}

void UWatchChartOption::createLayout()
{
    if(!Watch->getCurrentTab())
     return;

    //спрашиваем юзера, точно ли он хочет изменить параметры сетки
    //но не спрашиваем если там нет серий
    if (Watch->getCurrentTab()->getChart(0)->countSeries() == 0);
    else
    {
        //спрашиваем юзера точно ли он уверен в закрытие
        QMessageBox messageBox;
        messageBox.setText("Are you sure you want to Grid layout params?");
        messageBox.setInformativeText("All data about current charts will be lost");
        messageBox.setWindowTitle("Change Grid layout params");
        messageBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        messageBox.setIcon(QMessageBox::Question);

        if(messageBox.exec() != QMessageBox::Yes) return;
    }

    //создание нового расположени€
    int colNumber = ui->chartColNumber_spin->value();
    int rowNumber = ui->chartRowNumber_spin->value();

    Watch->getCurrentTab()->createGridLayout(rowNumber,colNumber);
}

void UWatchChartOption::saveParameters()
{
    if(!Watch->getCurrentTab())
     return;

    //сохранение всех параметров
   int index = ui->allChartsList->currentRow();
   if (index == -1) return;

   Watch->getCurrentTab()->getChart(index)->setChartTitle(ui->graphNameEditor->text());
   Watch->getCurrentTab()->getChart(index)->setAxisXname(ui->axisXNameEditor->text());
   Watch->getCurrentTab()->getChart(index)->setAxisYname(ui->axisYNameEditor->text());

   Watch->getCurrentTab()->getChart(index)->setAxisYmax(ui->axisYmaxSB->value());
   Watch->getCurrentTab()->getChart(index)->setAxisYmin(ui->axisYminSB->value());
   Watch->getCurrentTab()->getChart(index)->axisXrange = ui->axisXrangeSB->value();
   Watch->getCurrentTab()->getChart(index)->updateTimeIntervals(ui->axisXrangeSB->value());
   Watch->getCurrentTab()->getChart(index)->isAxisYzoomable = ui->axisYzoomCB->isChecked();
   Watch->getCurrentTab()->getChart(index)->isAxisYscrollable= ui->axisYscrollCB->isChecked();
   Watch->getCurrentTab()->getChart(index)->isAxisXtrackable= ui->axisXtrackCB->isChecked();

   Watch->getCurrentTab()->saveUpdateInterval(ui->updateIntervalMs->text().toInt());
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
