#ifndef UWATCH_H
#define UWATCH_H

#include <QMainWindow>
#include <QVector>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>

#include "UWatchTab.h"
#include "UWatchChartOption.h"
#include "UWatchSeriesOption.h"
#include "UVisualControllerMainWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class UWatch; }
QT_END_NAMESPACE

using namespace QtCharts;

//////////////////////////////////////////////////////////////////////////////
// Отдельное окно для отображения графиков по заданному источнику данных
//////////////////////////////////////////////////////////////////////////////


class UWatch : public UVisualControllerMainWidget
{
    Q_OBJECT

public:
    UWatch(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatch();
    UWatchTab *getCurrentTab();

    // Обновление интерфейса
    virtual void AUpdateInterface(void);

    // Возврат интерфейса в исходное состояние
    virtual void AClearInterface(void);

    // Метод, вызываемый после загрузки проекта
    virtual void AAfterLoadProject(void);

    // Метод, вызываемый перед закрытием проекта
    virtual void ABeforeCloseProject(void);

    // Метод, вызываемый перед сбросом модели
    virtual void ABeforeReset(void);

    // Метод, вызываемый после сброса модели
    virtual void AAfterReset(void);

    // Метод, вызываемый перед шагом расчета
    virtual void ABeforeCalculate(void);

    // Метод, вызываемый после шага расчета
    virtual void AAfterCalculate(void);

    // Сохраняет параметры интерфейса в xml
    virtual void ASaveParameters(RDK::USerStorageXML &xml);

    // Загружает параметры интерфейса из xml
    virtual void ALoadParameters(RDK::USerStorageXML &xml);

private slots:
    void on_actionCreate_tab_triggered();
    void on_actionSeries_option_triggered();
    void on_actionCharts_option_triggered();
    void on_tabWidget_tabCloseRequested(int index);

    void on_actionTake_screenshot_triggered();

private:
    QVector <UWatchTab*> tab;
    UWatchChartOption *chartOption;
    UWatchSeriesOption *seriesOption;

    void createTab();
    void deleteTab(int index);

    Ui::UWatch *ui;
};
#endif // UWATCH_H
