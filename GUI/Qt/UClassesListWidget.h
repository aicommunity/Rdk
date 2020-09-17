#ifndef UCLASSESLISTWIDGET_H
#define UCLASSESLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include <QWidget>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QListWidget>

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
    void on_lineEdit_textChanged(const QString &arg1);

    // ������������ ������ ���������
    void on_listWidgetRTlibs_itemSelectionChanged();


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

#endif // UCLASSESLISTWIDGET_H
