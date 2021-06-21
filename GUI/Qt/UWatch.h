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


QT_BEGIN_NAMESPACE
namespace Ui { class UWatch; }
QT_END_NAMESPACE

using namespace QtCharts;

//////////////////////////////////////////////////////////////////////////////
// Отдельное окно для отображения графиков по заданному источнику данных
//////////////////////////////////////////////////////////////////////////////


class UWatch : public QMainWindow, public RDK::UIVisualController
{
    Q_OBJECT

public:
    UWatch(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatch();
    UWatchTab *getCurrentTab();

    // Обновление интерфейса
    virtual void UpdateInterface(bool force_update=false);

    // Возврат интерфейса в исходное состояние
    virtual void ClearInterface(void);

    // Возвращает интервал обновления интерфейса
    virtual long GetUpdateInterval(void);

    // Задает интервал обновления интерфейса
    virtual bool SetUpdateInterval(long value);

    // Возвращает флаг разрешения обновления интерфейса даже если он не виден
    virtual bool GetAlwaysUpdateFlag(void);

    // Служебные методы управления интерфейсом
    /// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
    virtual void ResetCalculationStepUpdatedFlag(void);

    /// Выставляет флаг прошедшей перерисовки в этой итерации счета
    virtual void SetCalculationStepUpdatedFlag(void);

    /// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
    virtual bool GetCalculationStepUpdatedFlag(void);

    /// Возвращает время обновления интерфейса (мс)
    virtual unsigned long long GetUpdateTime(void);

    // Метод, вызываемый после загрузки проекта
    virtual void AfterLoadProject(void);

    // Метод, вызываемый перед закрытием проекта
    virtual void BeforeCloseProject(void);

    // Метод, вызываемый перед сбросом модели
    virtual void BeforeReset(void);

    // Метод, вызываемый после сброса модели
    virtual void AfterReset(void);

    // Метод, вызываемый перед шагом расчета
    virtual void BeforeCalculate(void);

    // Метод, вызываемый после шага расчета
    virtual void AfterCalculate(void);

    // Возвращает уникальное имя интерфейса
    virtual std::string GetName(void);

    // Возвращает полное уникальное имя интерфейса
    virtual std::string CalcFullName(void);

    // Возвращает имя класса интерфейса
    virtual std::string GetClassName(void);

    // Сохраняет параметры интерфейса в xml
    virtual void SaveParameters(RDK::USerStorageXML &xml);

    // Загружает параметры интерфейса из xml
    virtual void LoadParameters(RDK::USerStorageXML &xml);

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
