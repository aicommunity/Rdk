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
// ��������� ���� ��� ����������� �������� �� ��������� ��������� ������
//////////////////////////////////////////////////////////////////////////////


class UWatch : public QMainWindow, public RDK::UIVisualController
{
    Q_OBJECT

public:
    UWatch(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatch();
    UWatchTab *getCurrentTab();

    // ���������� ����������
    virtual void UpdateInterface(bool force_update=false);

    // ������� ���������� � �������� ���������
    virtual void ClearInterface(void);

    // ���������� �������� ���������� ����������
    virtual long GetUpdateInterval(void);

    // ������ �������� ���������� ����������
    virtual bool SetUpdateInterval(long value);

    // ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
    virtual bool GetAlwaysUpdateFlag(void);

    // ��������� ������ ���������� �����������
    /// ���������� ���� ��������� ����������� � ���� �������� �����
    virtual void ResetCalculationStepUpdatedFlag(void);

    /// ���������� ���� ��������� ����������� � ���� �������� �����
    virtual void SetCalculationStepUpdatedFlag(void);

    /// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
    virtual bool GetCalculationStepUpdatedFlag(void);

    /// ���������� ����� ���������� ���������� (��)
    virtual unsigned long long GetUpdateTime(void);

    // �����, ���������� ����� �������� �������
    virtual void AfterLoadProject(void);

    // �����, ���������� ����� ��������� �������
    virtual void BeforeCloseProject(void);

    // �����, ���������� ����� ������� ������
    virtual void BeforeReset(void);

    // �����, ���������� ����� ������ ������
    virtual void AfterReset(void);

    // �����, ���������� ����� ����� �������
    virtual void BeforeCalculate(void);

    // �����, ���������� ����� ���� �������
    virtual void AfterCalculate(void);

    // ���������� ���������� ��� ����������
    virtual std::string GetName(void);

    // ���������� ������ ���������� ��� ����������
    virtual std::string CalcFullName(void);

    // ���������� ��� ������ ����������
    virtual std::string GetClassName(void);

    // ��������� ��������� ���������� � xml
    virtual void SaveParameters(RDK::USerStorageXML &xml);

    // ��������� ��������� ���������� �� xml
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
