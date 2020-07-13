#ifndef UVISUALCONTROLLERWIDGET_H
#define UVISUALCONTROLLERWIDGET_H

//#include <rdk_cpp_initlib.h>
#include <rdk_application.h>
#include "../../Core/Application/UIVisualController.h"
#include <QWidget>
#include <QSettings>
#include <QDebug>
#undef GetClassName //�������� �� ��������� �������

/// The UVisualControllerWidget class - ������� ����� �������
///
/// ������ ������, ����������� �� ����� ������ ������������ � ���� RDK

class UVisualControllerWidget:public QWidget, public RDK::UIVisualController
{
    Q_OBJECT

public:
    UVisualControllerWidget(QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~UVisualControllerWidget();

protected:
    /// �������� ������ ����������
    RDK::UApplication *application;

    /// true - �� ����� ���������� AUpdateInterface, ��� ��������� ����� - false
    ///
    /// ��������� ������ ������������ ���������� ����������,
    /// ������������ � ������ ������� �������������� ���������� �� ���������� ���������� � �� ���� ����.
    bool UpdateInterfaceFlag;

    ///��� ������� � ��� ������, ������������ � ������� �����������
    std::string Name, ClassName;

    /// ����, ����������� ���������� ����������, ���� ���� ������ �� �����
    bool AlwaysUpdateFlag;

    /// �������� ���������� ���������� � ������������� ��� �����
    /// \details (���� =0) �� ��������� �� ����������� ��� �����
    RDK::UELockVar<long> UpdateInterval;

    /// ����� ���������� ����������
    unsigned long long LastUpdateTime;

    /// ������������ ���� ����� ���� ������������� (���������)
    bool CalculationStepUpdatedFlag;

    /// ����, ����������� �������� �� ������������� ������ ����� ����������� ����������
    bool CheckModelFlag;

    /// �����, ����������� �� ���������� ����������
    unsigned long long UpdateTime;

    public:
    /// ����, ���������� ��� ���� ������
    static RDK::UELockVar<bool> CalculationModeFlag;

    public:
    // -----------------------------
    // ������ ���������� ���������� �����������
    // -----------------------------
    /// �����, ���������� ����� �������� �������
    /// \details �������� ����� AAfterLoadProject() � ����� ��������� ����������
    virtual void AfterLoadProject(void);
    virtual void AAfterLoadProject(void);

    /// �����, ���������� ����� ��������� �������
    /// \details �������� ����� ABeforeCloseProject() � ����� ��������� ����������
    virtual void BeforeCloseProject(void);
    virtual void ABeforeCloseProject(void);

    /// �����, ���������� ����� ������� ������
    /// \details �������� ����� ABeforeReset() � ����� ��������� ����������
    virtual void BeforeReset(void);
    virtual void ABeforeReset(void);

    /// �����, ���������� ����� ������ ������
    /// \details �������� ����� AAfterReset() � ����� ��������� ����������
    virtual void AfterReset(void);
    virtual void AAfterReset(void);

    /// �����, ���������� ����� ����� �������
    /// \details �������� ����� ABeforeCalculate() � ����� ��������� ����������
    virtual void BeforeCalculate(void);
    virtual void ABeforeCalculate(void);

    /// �����, ���������� ����� ���� �������
    /// \details �������� ����� AAfterCalculate() � ����� ��������� ����������
    virtual void AfterCalculate(void);
    virtual void AAfterCalculate(void);

    /// ���������� ����������
    /*! �������� ����� AUpdateInterface() � ����� ��������� ����������.
     ���� force_update == true, �� ��������� �����������
     ��� ����������� �� UpdateInterval*/
    virtual void UpdateInterface(bool force_update=false);
    virtual void AUpdateInterface(void);

    /// ������� ���������� � �������� ���������
    /// \details �������� ����� AClearInterface() � ����� ��������� ����������
    virtual void ClearInterface(void);
    virtual void AClearInterface(void);

    /// ���������� ���������� ��� ����������
    virtual std::string GetName(void);

    /// ���������� ��� ������ ����������
    virtual std::string GetClassName(void);

    virtual std::string CalcFullName(void);

    /// ���������� �������� ���������� ����������
    virtual long GetUpdateInterval(void);

    /// ������ �������� ���������� ����������
    virtual bool SetUpdateInterval(long value);

    /// ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
    virtual bool GetAlwaysUpdateFlag(void);

    /// ��������� ��������� ���������� � xml
    /// \details �������� ����� ASaveParameters() � ����� ��������� ����������
    virtual void SaveParameters(RDK::USerStorageXML &);
    virtual void ASaveParameters();

    /// ��������� ��������� ���������� �� xml
    /// \details �������� ����� ALoadParameters() � ����� ��������� ����������
    virtual void LoadParameters(RDK::USerStorageXML &);
    virtual void ALoadParameters();

    // ��������� ������ ���������� �����������
    /// ���������� ���� ��������� ����������� � ���� �������� �����
    virtual void ResetCalculationStepUpdatedFlag(void);

    /// ���������� ���� ��������� ����������� � ���� �������� �����
    virtual void SetCalculationStepUpdatedFlag(void);

    /// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
    virtual bool GetCalculationStepUpdatedFlag(void);

    /// ���������� ����� ���������� ���������� (��)
    virtual unsigned long long GetUpdateTime(void);

public slots:
    virtual void unInit();

signals:
    void closeWindow();
};

#endif // UVISUALCONTROLLERWIDGET_H
