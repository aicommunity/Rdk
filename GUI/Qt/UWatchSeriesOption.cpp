#include "UWatchSeriesOption.h"
#include "ui_UWatchSeriesOption.h"
#include "UWatch.h"
#include "UGuiTelemetry.h"
#include "UComponentPropertySelectionWidget.h"
#include <QPixmap>
#include <QIcon>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QDoubleValidator>
#include <QPointF>
#include <limits>
#include <cmath>
#include <algorithm>
#include <rdk_cpp_init.h>
#include <utility>
#include "../../Core/Engine/UNet.h"
#include "../../Core/Engine/UContainer.h"


UWatchSeriesOption::UWatchSeriesOption(QWidget *parent, RDK::UApplication *app) :
   QDialog(parent),
    ui(new Ui::UWatchSeriesOption),
    application(app)
{
    ui->setupUi(this);
    setWindowTitle(tr("Series Options"));
    WatchTab = dynamic_cast<UWatchTab*>(parent);

    // Инициализируем контролы
    initializeColorComboBox();
    initializeAxisControls();

    // Подключаем слоты
    if (ui->serieColor) {
        connect(ui->serieColor, SIGNAL(currentIndexChanged(int)), this, SLOT(onSerieColorChanged(int)));
    }
    if (ui->changeDataSourceButton) {
        connect(ui->changeDataSourceButton, SIGNAL(clicked()), this, SLOT(onChangeDataSourceClicked()));
    }

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
    NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UWatchSeriesOption"), QStringLiteral("SaveAndClose"));
    saveParemeters();
    close();
    destroy();
    delete this;
}

void UWatchSeriesOption::on_applyButton_clicked()
{
    NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UWatchSeriesOption"), QStringLiteral("Apply"));
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
    //удаляем в выбранном графике выбранную серию
    int currentChartIndex = ui->graphsList->currentRow();
    if (currentChartIndex < 0) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a chart first."));
        return;
    }

    int currentSerieIndex = ui->graphsSeriesList->currentRow();
    if(currentSerieIndex < 0) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a series to delete."));
        return;
    }

    // Получаем реальный индекс серии из UserRole
    QListWidgetItem *item = ui->graphsSeriesList->item(currentSerieIndex);
    if (item) {
        QVariant serieIndex = item->data(Qt::UserRole);
        if (serieIndex.isValid()) {
            currentSerieIndex = serieIndex.toInt();
        }
    }

    // Подтверждение удаления
    QString serieName = item ? item->text() : tr("Series");
    int ret = QMessageBox::question(this, tr("Delete Series"),
                                     tr("Are you sure you want to delete series \"%1\"?").arg(serieName),
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        WatchTab->getChart(currentChartIndex)->deleteSerie(currentSerieIndex);
        updateGraphsSeries(currentChartIndex);
    }
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
    // Получаем реальный индекс серии из UserRole, если доступен
    if (currentRow >= 0 && ui->graphsSeriesList->item(currentRow)) {
        QVariant serieIndex = ui->graphsSeriesList->item(currentRow)->data(Qt::UserRole);
        if (serieIndex.isValid()) {
            updateSeriesProperties(serieIndex.toInt());
        } else {
            updateSeriesProperties(currentRow);
        }
    } else {
        updateSeriesProperties(-1);
    }
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

void UWatchSeriesOption::initializeAxisControls()
{
    axisMultipliers = {1e-6, 1e-4, 1e-2, 1.0, 10.0, 100.0, 1000.0};

    if (ui->axisYmaxSlider) {
        ui->axisYmaxSlider->setRange(-AXIS_SLIDER_LIMIT, AXIS_SLIDER_LIMIT);
    }
    if (ui->axisYminSlider) {
        ui->axisYminSlider->setRange(-AXIS_SLIDER_LIMIT, AXIS_SLIDER_LIMIT);
    }

    if (ui->axisYMultiplierCombo) {
        int maxIndex = static_cast<int>(axisMultipliers.size()) - 1;
        int defaultIndex = std::min(3, std::max(0, maxIndex));
        ui->axisYMultiplierCombo->setCurrentIndex(defaultIndex);
        setAxisMultiplierByIndex(defaultIndex);
    } else {
        axisMultiplier = 1.0;
    }

    if (ui->axisYmaxEdit) {
        auto *validator = new QDoubleValidator(-1e12, 1e12, 10, this);
        validator->setNotation(QDoubleValidator::ScientificNotation);
        ui->axisYmaxEdit->setValidator(validator);
    }
    if (ui->axisYminEdit) {
        auto *validator = new QDoubleValidator(-1e12, 1e12, 10, this);
        validator->setNotation(QDoubleValidator::ScientificNotation);
        ui->axisYminEdit->setValidator(validator);
    }
}

void UWatchSeriesOption::updateAxisControlsFromChart(int chartIndex)
{
    if (!WatchTab || chartIndex < 0 || chartIndex >= WatchTab->countGraphs()) {
        return;
    }

    UWatchChart *chart = WatchTab->getChart(chartIndex);
    if (!chart) {
        return;
    }

    double yMax = chart->getAxisYmax();
    double yMin = chart->getAxisYmin();

    setAxisLineEditValue(ui->axisYmaxEdit, yMax);
    setAxisLineEditValue(ui->axisYminEdit, yMin);
    syncSliderWithValue(ui->axisYmaxSlider, yMax);
    syncSliderWithValue(ui->axisYminSlider, yMin);
}

void UWatchSeriesOption::applyAxisInputsToChart(int chartIndex)
{
    if (!WatchTab || chartIndex < 0 || chartIndex >= WatchTab->countGraphs()) {
        return;
    }

    UWatchChart *chart = WatchTab->getChart(chartIndex);
    if (!chart) {
        return;
    }

    double fallbackMax = chart->getAxisYmax();
    double fallbackMin = chart->getAxisYmin();
    double newMax = readAxisValue(ui->axisYmaxEdit, fallbackMax);
    double newMin = readAxisValue(ui->axisYminEdit, fallbackMin);

    if (newMin >= newMax) {
        double delta = std::max(currentAxisMultiplier(), 1e-6);
        newMin = newMax - delta;
    }

    chart->setAxisYmax(newMax);
    chart->setAxisYmin(newMin);

    syncSliderWithValue(ui->axisYmaxSlider, newMax);
    syncSliderWithValue(ui->axisYminSlider, newMin);
    setAxisLineEditValue(ui->axisYmaxEdit, newMax);
    setAxisLineEditValue(ui->axisYminEdit, newMin);
}

void UWatchSeriesOption::syncSliderWithValue(QSlider *slider, double value)
{
    if (!slider) {
        return;
    }
    int sliderValue = sliderValueFromAxis(value);
    sliderValue = std::clamp(sliderValue, -AXIS_SLIDER_LIMIT, AXIS_SLIDER_LIMIT);
    QSignalBlocker blocker(slider);
    slider->setValue(sliderValue);
}

double UWatchSeriesOption::axisValueFromSlider(int sliderValue) const
{
    return sliderValue * currentAxisMultiplier();
}

int UWatchSeriesOption::sliderValueFromAxis(double value) const
{
    if (std::abs(currentAxisMultiplier()) < 1e-12) {
        return 0;
    }
    double normalized = value / currentAxisMultiplier();
    return static_cast<int>(std::round(normalized));
}

double UWatchSeriesOption::readAxisValue(QLineEdit *edit, double fallback) const
{
    if (!edit) {
        return fallback;
    }
    bool ok = false;
    double val = edit->text().toDouble(&ok);
    return ok ? val : fallback;
}

void UWatchSeriesOption::setAxisLineEditValue(QLineEdit *edit, double value)
{
    if (!edit) {
        return;
    }
    QSignalBlocker blocker(edit);
    edit->setText(QString::number(value, 'g', 10));
}

std::pair<double, double> UWatchSeriesOption::calculateAutoYAxis(UWatchChart *chart) const
{
    if (!chart) {
        return {0.0, 1.0};
    }

    double minVal = std::numeric_limits<double>::max();
    double maxVal = std::numeric_limits<double>::lowest();
    bool hasData = false;

    for (int i = 0; i < chart->countSeries(); ++i) {
        UWatchSerie *serie = chart->getSerie(i);
        if (!serie || serie->pointsVector().isEmpty()) {
            continue;
        }

        const auto points = serie->pointsVector();
        for (const QPointF &point : points) {
            hasData = true;
            minVal = std::min(minVal, point.y());
            maxVal = std::max(maxVal, point.y());
        }
    }

    if (!hasData) {
        return {chart->getAxisYmin(), chart->getAxisYmax()};
    }

    if (std::abs(maxVal - minVal) < 1e-9) {
        double padding = std::max(1.0, std::abs(maxVal) * 0.1);
        return {minVal - padding, maxVal + padding};
    }

    double margin = (maxVal - minVal) * 0.1;
    if (margin <= 0.0) {
        margin = currentAxisMultiplier();
    }
    return {minVal - margin, maxVal + margin};
}

double UWatchSeriesOption::currentAxisMultiplier() const
{
    return axisMultiplier <= 0.0 ? 1.0 : axisMultiplier;
}

void UWatchSeriesOption::setAxisMultiplierByIndex(int index)
{
    if (axisMultipliers.isEmpty()) {
        axisMultipliers = {1.0};
    }
    int maxIndex = static_cast<int>(axisMultipliers.size()) - 1;
    if (index < 0 || index > maxIndex) {
        index = std::min(3, std::max(0, maxIndex));
    }
    axisMultiplier = axisMultipliers.value(index, 1.0);
    if (axisMultiplier <= 0.0) {
        axisMultiplier = 1.0;
    }
}

void UWatchSeriesOption::on_axisYMultiplierCombo_currentIndexChanged(int index)
{
    setAxisMultiplierByIndex(index);
    syncSliderWithValue(ui->axisYmaxSlider, readAxisValue(ui->axisYmaxEdit, 0.0));
    syncSliderWithValue(ui->axisYminSlider, readAxisValue(ui->axisYminEdit, 0.0));
}

void UWatchSeriesOption::on_axisYmaxSlider_valueChanged(int value)
{
    double axisValue = axisValueFromSlider(value);
    setAxisLineEditValue(ui->axisYmaxEdit, axisValue);
    applyAxisInputsToChart(ui->graphsList->currentRow());
}

void UWatchSeriesOption::on_axisYminSlider_valueChanged(int value)
{
    double axisValue = axisValueFromSlider(value);
    setAxisLineEditValue(ui->axisYminEdit, axisValue);
    applyAxisInputsToChart(ui->graphsList->currentRow());
}

void UWatchSeriesOption::on_axisYmaxEdit_editingFinished()
{
    syncSliderWithValue(ui->axisYmaxSlider, readAxisValue(ui->axisYmaxEdit, 0.0));
    applyAxisInputsToChart(ui->graphsList->currentRow());
}

void UWatchSeriesOption::on_axisYminEdit_editingFinished()
{
    syncSliderWithValue(ui->axisYminSlider, readAxisValue(ui->axisYminEdit, 0.0));
    applyAxisInputsToChart(ui->graphsList->currentRow());
}

void UWatchSeriesOption::on_axisAutoButton_clicked()
{
    if (!WatchTab) {
        return;
    }
    int chartIndex = ui->graphsList->currentRow();
    if (chartIndex < 0 || chartIndex >= WatchTab->countGraphs()) {
        return;
    }

    UWatchChart *chart = WatchTab->getChart(chartIndex);
    auto range = calculateAutoYAxis(chart);
    setAxisLineEditValue(ui->axisYminEdit, range.first);
    setAxisLineEditValue(ui->axisYmaxEdit, range.second);
    syncSliderWithValue(ui->axisYminSlider, range.first);
    syncSliderWithValue(ui->axisYmaxSlider, range.second);
    applyAxisInputsToChart(chartIndex);
}

void UWatchSeriesOption::on_axisResetButton_clicked()
{
    if (!WatchTab) {
        return;
    }
    int chartIndex = ui->graphsList->currentRow();
    if (chartIndex < 0 || chartIndex >= WatchTab->countGraphs()) {
        return;
    }

    UWatchChart *chart = WatchTab->getChart(chartIndex);
    setAxisLineEditValue(ui->axisYminEdit, chart->getInitialAxisYmin());
    setAxisLineEditValue(ui->axisYmaxEdit, chart->getInitialAxisYmax());
    syncSliderWithValue(ui->axisYminSlider, chart->getInitialAxisYmin());
    syncSliderWithValue(ui->axisYmaxSlider, chart->getInitialAxisYmax());
    applyAxisInputsToChart(chartIndex);
}

void UWatchSeriesOption::on_axisApplyAllButton_clicked()
{
    if (!WatchTab) {
        return;
    }
    int currentChartIndex = ui->graphsList->currentRow();
    if (currentChartIndex < 0 || currentChartIndex >= WatchTab->countGraphs()) {
        return;
    }

    double targetMax = readAxisValue(ui->axisYmaxEdit, WatchTab->getChart(currentChartIndex)->getAxisYmax());
    double targetMin = readAxisValue(ui->axisYminEdit, WatchTab->getChart(currentChartIndex)->getAxisYmin());
    if (targetMin >= targetMax) {
        targetMin = targetMax - std::max(currentAxisMultiplier(), 1e-6);
    }

    for (int i = 0; i < WatchTab->countGraphs(); ++i) {
        UWatchChart *chart = WatchTab->getChart(i);
        if (!chart) {
            continue;
        }
        chart->setAxisYmax(targetMax);
        chart->setAxisYmin(targetMin);
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

    updateAxisControlsFromChart(chartIndex);
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

   applyAxisInputsToChart(index);
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

    // Сохраняем текущий выбор
    int currentSerieRow = ui->graphsSeriesList->currentRow();

    //загружаем имена серий выбранного графика с расширенной информацией
    ui->graphsSeriesList->clear();
    UWatchChart *chart = WatchTab->getChart(currentChartIndex);
    
    for (int i = 0; i < chart->countSeries(); i++)
    {
        UWatchSerie *serie = chart->getSerie(i);
        if (!serie) {
            continue;
        }

        // Формируем текст с информацией об источнике данных
        QString itemText = serie->name();
        if (!serie->nameComponent.isEmpty() && !serie->nameProperty.isEmpty()) {
            itemText += QString(" - %1.%2").arg(serie->nameComponent).arg(serie->nameProperty);
            if (serie->Jx != 0 || serie->Jy != 0) {
                itemText += QString(" [%1, %2]").arg(serie->Jx).arg(serie->Jy);
            }
        }

        QListWidgetItem *item = new QListWidgetItem(itemText, ui->graphsSeriesList);
        
        // Добавляем цветовую иконку
        QPixmap colorPixmap(16, 16);
        colorPixmap.fill(serie->color());
        item->setIcon(QIcon(colorPixmap));
        
        // Добавляем индикацию статуса (активна/неактивна)
        if (!serie->isOnline) {
            // Неактивная серия - серый цвет текста
            item->setForeground(QBrush(QColor(Qt::gray)));
            item->setToolTip(tr("Series is offline (data source unavailable)"));
        } else {
            item->setToolTip(tr("Component: %1\nProperty: %2\nIndices: [%3, %4]")
                            .arg(serie->nameComponent)
                            .arg(serie->nameProperty)
                            .arg(serie->Jx)
                            .arg(serie->Jy));
        }
        
        // Сохраняем индекс серии в UserRole для быстрого доступа
        item->setData(Qt::UserRole, i);
    }
    
    // Восстанавливаем выбор
    if (ui->graphsSeriesList->count() > 0) {
        if (currentSerieRow >= 0 && currentSerieRow < ui->graphsSeriesList->count()) {
            ui->graphsSeriesList->setCurrentRow(currentSerieRow);
        } else {
            ui->graphsSeriesList->setCurrentRow(0);
        }
    }
}

void UWatchSeriesOption::updateSeriesProperties(int currentSerieIndex)
{
    //загружаем свойства выбранной серии
    if(currentSerieIndex == -1) {
        // Очищаем поля, если серия не выбрана
        if (ui->serieName) ui->serieName->clear();
        if (ui->serieColor) ui->serieColor->setCurrentIndex(0);
        if (ui->serieLneWidth) ui->serieLneWidth->setCurrentIndex(0);
        if (ui->serieLineType) ui->serieLineType->setCurrentIndex(0);
        if (ui->serieYshift) ui->serieYshift->setValue(0.0);
        return;
    }
    
    int currentChartIndex = ui->graphsList->currentRow();
    if (currentChartIndex < 0) return;

    updateSeriesNameProp(currentChartIndex, currentSerieIndex);
    updateSeriesColorProp(currentChartIndex, currentSerieIndex);
    updateSeriesWidthProp(currentChartIndex, currentSerieIndex);
    updateSeriesLineTypeProp(currentChartIndex, currentSerieIndex);
    updateSeriesYShift(currentChartIndex, currentSerieIndex);
    updateSeriesDataSourceProp(currentChartIndex, currentSerieIndex);
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
            // Обновляем предпросмотр цвета
            updateColorPreview(color);
            return;
        }
    ui->serieColor->setCurrentIndex(16);
    updateColorPreview(color);
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

    QString name = ui->serieName->text().trimmed();
    
    // Валидация имени серии
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"), tr("Series name cannot be empty."));
        return;
    }

    int colorIndex = ui->serieColor->currentIndex();
    if (colorIndex < 0 || colorIndex >= 15) {
        colorIndex = 0; // Используем красный по умолчанию
    }
    
    int width = ui->serieLneWidth->currentIndex();
    int lineTypeIndex = ui->serieLineType->currentIndex();
    if (lineTypeIndex < 0 || lineTypeIndex >= 4) {
        lineTypeIndex = 0; // Используем сплошную линию по умолчанию
    }
    
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

void UWatchSeriesOption::updateSeriesDataSourceProp(int currentChartIndex, int currentSerieIndex)
{
    if(!WatchTab || currentChartIndex < 0 || currentSerieIndex < 0)
        return;

    UWatchSerie *serie = WatchTab->getChart(currentChartIndex)->getSerie(currentSerieIndex);
    if (!serie) {
        // Очищаем поля источника данных
        if (ui->serieComponentName) ui->serieComponentName->clear();
        if (ui->seriePropertyName) ui->seriePropertyName->clear();
        if (ui->serieJx) ui->serieJx->setValue(0);
        if (ui->serieJy) ui->serieJy->setValue(0);
        if (ui->serieChannelIndex) ui->serieChannelIndex->setValue(0);
        return;
    }

    // Обновляем поля источника данных (read-only для отображения)
    if (ui->serieComponentName) {
        ui->serieComponentName->setText(serie->nameComponent);
    }
    if (ui->seriePropertyName) {
        ui->seriePropertyName->setText(serie->nameProperty);
    }
    if (ui->serieJx) {
        ui->serieJx->setValue(serie->Jx);
    }
    if (ui->serieJy) {
        ui->serieJy->setValue(serie->Jy);
    }
    if (ui->serieChannelIndex) {
        ui->serieChannelIndex->setValue(serie->indexChannel);
    }
}

void UWatchSeriesOption::updateColorPreview(const QColor &color)
{
    // Обновляем предпросмотр цвета, если есть соответствующий виджет
    QLabel *colorPreview = findChild<QLabel*>("serieColorPreview");
    if (colorPreview) {
        QPixmap pixmap(24, 24);
        pixmap.fill(color);
        colorPreview->setPixmap(pixmap);
    }
}

void UWatchSeriesOption::initializeColorComboBox()
{
    if (!ui->serieColor) {
        return;
    }

    // Добавляем иконки цветов для каждого элемента
    for (int i = 0; i < 15 && i < ui->serieColor->count(); ++i) {
        QPixmap pixmap(16, 16);
        pixmap.fill(defaultColors[i]);
        ui->serieColor->setItemIcon(i, QIcon(pixmap));
    }
}

void UWatchSeriesOption::onSerieColorChanged(int index)
{
    if (index >= 0 && index < 15) {
        updateColorPreview(defaultColors[index]);
    } else if (index == 16) {
        // "other" - используем черный по умолчанию
        updateColorPreview(Qt::black);
    }
}

void UWatchSeriesOption::onChangeDataSourceClicked()
{
    int currentChartIndex = ui->graphsList->currentRow();
    int currentSerieIndex = ui->graphsSeriesList->currentRow();

    if (currentChartIndex < 0 || currentSerieIndex < 0) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a series first."));
        return;
    }

    changeSeriesDataSource(currentChartIndex, currentSerieIndex);
}

void UWatchSeriesOption::changeSeriesDataSource(int chartIndex, int serieIndex)
{
    if(!WatchTab || chartIndex < 0 || serieIndex < 0)
        return;

    UWatchChart *chart = WatchTab->getChart(chartIndex);
    if (!chart) return;

    UWatchSerie *serie = chart->getSerie(serieIndex);
    if (!serie) return;

    // Сохраняем текущие параметры серии
    QString oldName = serie->name();
    QColor oldColor = serie->color();
    int oldWidth = serie->pen().width();
    Qt::PenStyle oldLineType = serie->pen().style();
    double oldYShift = serie->YShift;

    // Используем application, переданный в конструктор
    // Открываем диалог выбора источника данных
    int channelIndex = 0;
    QString componentName;
    QString componentProperty;
    int jx = 0, jy = 0;

    UComponentPropertySelectionWidget dialog(this, 3, application);
    dialog.setModal(true);
    if (dialog.exec())
    {
        channelIndex = dialog.componentsList->getSelectedChannelIndex();
        componentName = dialog.componentsList->getSelectedComponentLongName();
        componentProperty = dialog.componentsList->getSelectedPropertyName();
    }

    // Проверяем, что выбран не пустой элемент
    if(componentName.isEmpty() || componentProperty.isEmpty())
        return;

    // Проверяем тип свойства
    bool is_int_or_double = false;
    {
        RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>();
        if (model) {
            RDK::UContainer *cont = model->GetComponentL(componentName.toStdString());
            if(cont)
            {
                RDK::UEPtr<RDK::UIProperty> prop = cont->FindProperty(componentProperty.toStdString());
                if(prop)
                {
                    if(prop->GetLanguageType() == typeid(double) || prop->GetLanguageType() == typeid(int))
                    {
                        is_int_or_double = true;
                        jx = 0;
                        jy = 0;
                    }
                }
            }
        }
    }

    if (!is_int_or_double) {
        // Для матричных свойств нужно выбрать индексы
        // Пока используем существующие индексы или 0,0
        jx = serie->Jx;
        jy = serie->Jy;
    }

    // Удаляем старую серию и создаем новую с новым источником данных
    double time_interval = chart->getAxisXmax() - chart->getAxisXmin();
    
    // Удаляем старую серию
    chart->deleteSerie(serieIndex);
    
    // Создаем новую серию с новым источником данных
    chart->createSerie(channelIndex, componentName, componentProperty, "type", jx, jy, time_interval, oldYShift);
    
    // Восстанавливаем визуальные параметры
    int newSerieIndex = chart->countSeries() - 1;
    if (newSerieIndex >= 0) {
        chart->setSerieName(newSerieIndex, oldName);
        // Восстанавливаем цвет и стиль
        for (int i = 0; i < 15; ++i) {
            if (defaultColors[i] == oldColor) {
                chart->setSerieColor(newSerieIndex, i);
                break;
            }
        }
        chart->setSerieWidth(newSerieIndex, oldWidth);
        chart->setSerieLineType(newSerieIndex, oldLineType);
    }

    // Обновляем список серий
    updateGraphsSeries(chartIndex);
    if (ui->graphsSeriesList->count() > 0) {
        ui->graphsSeriesList->setCurrentRow(newSerieIndex);
    }
}

