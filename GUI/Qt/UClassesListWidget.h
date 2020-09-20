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
#include <QComboBox>

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

    void SetModelScheme(UDrawEngineImageWidget* model);

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
    UDrawEngineImageWidget* ModelScheme;
    Ui::UClassesListWidget *ui;

};

// ���������� ���� ��� �������� ����� ����������
class CrLibDialog: public QDialog
{
    Q_OBJECT
private:
    QLineEdit* InputLibName;
    QLabel* Message;
    QPushButton* AddButton;

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

    QLabel* MessageLib;
    QComboBox* Libraries;
    QLabel* MessageComp;
    QLineEdit* InputCompName;
    QLabel* MessageClass;
    QLineEdit* InputClassName;
    QPushButton* AddButton;
    QPushButton* ReplaceButton;
    bool Replace;
public:
    CrClassDialog(QStringList libs, QString cur_lib, QString cur_comp_name, QWidget* pwgt = 0);
    const std::string GetClassName() const;
    const std::string GetCompName() const;
    const std::string GetLibName() const;
    const bool GetReplace() const;
public slots:
    void ProcessInput();
};

class DeleteDialog: public QDialog
{
    Q_OBJECT
public:
     DeleteDialog(QString title, QString message, QWidget* pwgt = 0);

};
#endif // UCLASSESLISTWIDGET_H
