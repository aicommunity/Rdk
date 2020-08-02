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
}

UWatchChartOption::~UWatchChartOption()
{
    delete ui;
}

void UWatchChartOption::on_chartLayot_CB_currentIndexChanged(int index)
{
    //настройка spin box'ов в зависимости от типа расположения
    if (index == 0) {ui->chartRowNumber_spin->setDisabled(true);ui->chartColNumber_spin->setDisabled(true);}
    else if (index == 1) {ui->chartColNumber_spin->setEnabled(true);ui->chartRowNumber_spin->setDisabled(true);}
    else if (index == 2) {ui->chartRowNumber_spin->setEnabled(true);ui->chartColNumber_spin->setDisabled(true);}
    else if (index == 3) {ui->chartRowNumber_spin->setEnabled(true);ui->chartColNumber_spin->setEnabled(true);}
}

void UWatchChartOption::on_createLayot_button_clicked()
{
    createLayout();
    updateChartList();
}

void UWatchChartOption::updateChartList()
{
    //обновляем имена в списке графиков
    ui->allChartsList->clear();
    int chartsAmount = Watch->getCurrentTab()->countGraphs();

    for (int i = 0; i < chartsAmount; i++)
    {
        QString title = Watch->getCurrentTab()->getChart(i)->getChartTitle();
        ui->allChartsList->addItem(title);
    }
    ui->allChartsList->setCurrentRow(0);
}

void UWatchChartOption::updateParameters(int chartIndex)
{
    ui->graphNameEditor->setText(Watch->getCurrentTab()->getChart(chartIndex)->getChartTitle());

    //ui->legendVisibilitSB->setTristate(Watch->getCurrentTab()->getChart(currentRow)->)
    //добавить видимость легенды и названия

    //загрузка параметров графика и его осей
    ui->axisXNameEditor->setText(Watch->getCurrentTab()->getChart(chartIndex)->getAxisXName());
    ui->axisYNameEditor->setText(Watch->getCurrentTab()->getChart(chartIndex)->getAxisYName());

    ui->axisYzoomCB->setChecked(Watch->getCurrentTab()->getChart(chartIndex)->isAxisYzoomable);
    ui->axisYscrollCB->setChecked(Watch->getCurrentTab()->getChart(chartIndex)->isAxisYscrollable);

    ui->axisYmaxSB->setValue(Watch->getCurrentTab()->getChart(chartIndex)->getAxisYmax());
    ui->axisYminSB->setValue(Watch->getCurrentTab()->getChart(chartIndex)->getAxisYmin());
    ui->axisXrangeSB->setValue(Watch->getCurrentTab()->getChart(chartIndex)->axisXrange);
}

void UWatchChartOption::createLayout()
{
    //создание нового расположения
    int currentMethod =ui->chartLayot_CB->currentIndex();
    int colNumber = ui->chartColNumber_spin->value();
    int rowNumber = ui->chartRowNumber_spin->value();

    if(currentMethod == 0) Watch->getCurrentTab()->createSingleLayout();
    else if(currentMethod == 1) Watch->getCurrentTab()->createColLayout(colNumber);
    else if(currentMethod == 2) Watch->getCurrentTab()->createRowLayout(rowNumber);
    else if(currentMethod == 3) Watch->getCurrentTab()->createGridLayout(rowNumber,colNumber);
}

void UWatchChartOption::saveParameters()
{
    //сохранение всех параметров
   int index = ui->allChartsList->currentRow();
   if (index == -1) return;

   Watch->getCurrentTab()->getChart(index)->setChartTitle(ui->graphNameEditor->text());
   Watch->getCurrentTab()->getChart(index)->setAxisXname(ui->axisXNameEditor->text());
   Watch->getCurrentTab()->getChart(index)->setAxisYname(ui->axisYNameEditor->text());

   Watch->getCurrentTab()->getChart(index)->setAxisYmax(ui->axisYmaxSB->value());
   Watch->getCurrentTab()->getChart(index)->setAxisYmin(ui->axisYminSB->value());
   Watch->getCurrentTab()->getChart(index)->axisXrange = ui->axisXrangeSB->value();
   Watch->getCurrentTab()->getChart(index)->isAxisYzoomable = ui->axisYzoomCB->isChecked();
   Watch->getCurrentTab()->getChart(index)->isAxisYscrollable= ui->axisYscrollCB->isChecked();
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
