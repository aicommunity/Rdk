#ifndef UWATCH_H
#define UWATCH_H

#include <QMainWindow>
#include <QVector>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>

#include "UWatchTab.h"
#include "UVisualControllerMainWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class UWatch; }
QT_END_NAMESPACE

using namespace QtCharts;

//////////////////////////////////////////////////////////////////////////////
// ��������� ���� ��� ����������� �������� �� ��������� ��������� ������
//////////////////////////////////////////////////////////////////////////////


class UWatch : public UVisualControllerMainWidget
{
    Q_OBJECT

public:
    UWatch(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatch();
    UWatchTab *getCurrentTab();

    // ���������� ����������
    virtual void AUpdateInterface(void);

    // ������� ���������� � �������� ���������
    virtual void AClearInterface(void);

    // �����, ���������� ����� �������� �������
    virtual void AAfterLoadProject(void);

    // �����, ���������� ����� ��������� �������
    virtual void ABeforeCloseProject(void);

    // �����, ���������� ����� ������� ������
    virtual void ABeforeReset(void);

    // �����, ���������� ����� ������ ������
    virtual void AAfterReset(void);

    // �����, ���������� ����� ����� �������
    virtual void ABeforeCalculate(void);

    // �����, ���������� ����� ���� �������
    virtual void AAfterCalculate(void);

    // ��������� ��������� ���������� � xml
    virtual void ASaveParameters(RDK::USerStorageXML &xml);

    // ��������� ��������� ���������� �� xml
    virtual void ALoadParameters(RDK::USerStorageXML &xml);

private slots:
    void on_actionCreate_tab_triggered();
    void on_actionSeries_option_triggered();
    void on_actionCharts_option_triggered();
    void on_tabWidget_tabCloseRequested(int index);


private:
    QVector <UWatchTab*> tab;

    void createTab();
    void deleteTab(int index);

    Ui::UWatch *ui;
};
#endif // UWATCH_H
