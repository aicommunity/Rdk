#ifndef UCLASSESLISTWIDGET_H
#define UCLASSESLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include <QWidget>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QListWidget>
#include <QDialog>

namespace Ui {
class UClassesListWidget;
}


/// UClassesListWidget class - ������ ����������� ������ ��������� ����������� �� UStorage
///
/// �������� ��� ������ ��������������� �� �����:
/// - By Name - ������ ���� ��������� ���������
/// - By Libs - ����������� ������ ��������� �� �����������
/// ���������� �� ������ ����� ���������� �� �������, ����������� dropEvent

class UClassesListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UClassesListWidget(QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~UClassesListWidget();

    QString selctedClass() const;

    virtual void AUpdateInterface(void);

public slots:
    void dragEvent(QModelIndex index);

signals:
    void classSelectionChanged();

private slots:

    // ������������ ������ ��������� �� ������� LibsCtrl
    void on_listWidgetRTlibs_itemSelectionChanged();

    // ��������� ������ ��������� ������
    void on_lineEditSearch_textChanged(const QString &arg1);

    // ������� ������ ������� � ����������� �� �� ���������� �� ��������� ������ ��������� ������
    void tab0_textChanged(const QString &arg1);
    void tab1_textChanged(const QString &arg1);
    void tab2_textChanged(const QString &arg1);


public slots:
    //������� ������������ ����
    // ��������/�������� ����������
    void CreateRTlibrary();
    void DeleteRTlibrary();

    //��������/�������� ������
    void AddNewClass();
    void DeleteClass();

private:

    Ui::UClassesListWidget *ui;

};

// ���������� ���� ��� �������� ����� ����������
class CrLibDialog: public QDialog
{
    Q_OBJECT
private:
    QLineEdit* InputLibName;
    QLabel* Message;

public:
    CrLibDialog(QWidget* pwgt = 0);

    const std::string GetLibName() const;

public slots:
    void ProcessInput();
};

// ���������� ���� ��� �������� ������ ������
class CrClassDialog: public QDialog
{
    Q_OBJECT
private:
    QLineEdit* InputClassName;
    QLineEdit* ComponentName;
    QLabel* Info;
    QLabel* MessageClass;
    QLabel* MessageComp;
    // ����� �� ������ replace ������
    bool Replace;

public:
    CrClassDialog(QString lib_name, QWidget* pwgt = 0);
    const bool GetReplace() const;
    const std::string GetClassName() const;
    const std::string GetCompName() const;

public slots:
    void ProcessInput();
};

#endif // UCLASSESLISTWIDGET_H
