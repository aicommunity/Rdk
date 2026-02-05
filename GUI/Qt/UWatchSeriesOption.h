#ifndef UWATCHSERIESOPTION_H
#define UWATCHSERIESOPTION_H

#include <QDialog>
#include <QStyle>
#include <QPen>
#include <QColor>
#include <QtCharts/QChart>
#include <QVector>
#include <QSlider>
#include <QLineEdit>

#include "UVisualControllerWidget.h"



namespace Ui {
class UWatchSeriesOption;
}
using namespace QtCharts;

class UWatchTab;
class UWatchChart;

//////////////////////////////////////////////////////////////////////
// Один отдельно взятый график с одной или несколькими сериями данных
//////////////////////////////////////////////////////////////////////

class UWatchSeriesOption : public QDialog
{
    Q_OBJECT

public:
    explicit UWatchSeriesOption(QWidget *parent = nullptr, RDK::UApplication *app = nullptr);
    ~UWatchSeriesOption();


private slots:
    void on_okButton_clicked();
    void on_applyButton_clicked();
    void on_closeButton_clicked();
    void on_addSerieButton_clicked();
    void on_removeSerieButton_clicked();
    void on_graphsList_currentRowChanged(int currentRow);
    void on_graphsSeriesList_currentRowChanged(int currentRow);

    void on_createLayot_button_clicked();

    void on_axisXtrackCB_stateChanged(int arg1);

    void onSerieColorChanged(int index);
    void onChangeDataSourceClicked();
    void on_axisYMultiplierCombo_currentIndexChanged(int index);
    void on_axisYmaxSlider_valueChanged(int value);
    void on_axisYminSlider_valueChanged(int value);
    void on_axisYmaxEdit_editingFinished();
    void on_axisYminEdit_editingFinished();
    void on_axisAutoButton_clicked();
    void on_axisResetButton_clicked();
    void on_axisApplyAllButton_clicked();

private:
    Ui::UWatchSeriesOption *ui;
    UWatchTab *WatchTab;
    RDK::UApplication *application;

   //void loadGraphsName();//загрузить имена графиков
    void updateGraphsSeries(int currentChartIndex); //обновить имена серий выбранного графика в листе серий

    void updateSeriesProperties(int currentSerieIndex); //Обновить свойства для выбранной серии
    void updateSeriesNameProp(int currentChartIndex, int currentSerieIndex);
    void updateSeriesColorProp(int currentChartIndex, int currentSerieIndex);
    void updateSeriesWidthProp(int currentChartIndex, int currentSerieIndex);
    void updateSeriesLineTypeProp(int currentChartIndex,int currentSerieIndex);
    void updateSeriesYShift(int currentChartIndex,int currentSerieIndex);
    void updateSeriesDataSourceProp(int currentChartIndex, int currentSerieIndex); // Обновить информацию об источнике данных
    void updateColorPreview(const QColor &color); // Обновить предпросмотр цвета

    void updateChartList(); //обновить имена в лист графиков
    void updateLayoutBox();
    void updateParameters(int chartIndex);//обновить параметры выбранного в листе графика
    void createLayout(); //создать новое расположение
    void saveChartParameters(); //сохранить изменения

    //сохранить параметры
    void saveParemeters();
    
    // Изменить источник данных серии
    void changeSeriesDataSource(int chartIndex, int serieIndex);

    // Инициализировать ComboBox цветов с иконками
    void initializeColorComboBox();
    void initializeAxisControls();
    void updateAxisControlsFromChart(int chartIndex);
    void applyAxisInputsToChart(int chartIndex);
    void syncSliderWithValue(QSlider *slider, double value);
    double axisValueFromSlider(int sliderValue) const;
    int sliderValueFromAxis(double value) const;
    double readAxisValue(QLineEdit *edit, double fallback) const;
    void setAxisLineEditValue(QLineEdit *edit, double value);
    std::pair<double, double> calculateAutoYAxis(UWatchChart *chart) const;
    double currentAxisMultiplier() const;
    void setAxisMultiplierByIndex(int index);

    //массивы цветов и типов линий
    const QColor defaultColors[15]={Qt::red, Qt::darkRed, Qt::yellow, Qt::darkYellow, Qt::green,
                                    Qt::darkGreen, Qt::cyan, Qt::darkCyan, Qt::blue, Qt::darkBlue,
                                    Qt::magenta,Qt::darkMagenta, Qt::gray, Qt::darkGray, Qt::black};

    const Qt::PenStyle defaultLineStyle[4] = {Qt::SolidLine, Qt::DotLine, Qt::DashLine, Qt::DashDotLine};

    QVector<double> axisMultipliers;
    double axisMultiplier = 1.0;
    static constexpr int AXIS_SLIDER_LIMIT = 10000;
};

#endif // UWATCHSERIESOPTION_H
