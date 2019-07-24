#ifndef UCLASSESLISTWIDGET_H
#define UCLASSESLISTWIDGET_H

#include "UVisualControllerWidget.h"

#include <QWidget>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QModelIndex>


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

public slots:
    void dragEvent(QModelIndex index);

signals:
    void classSelectionChanged();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);



private:
    Ui::UClassesListWidget *ui;
};

#endif // UCLASSESLISTWIDGET_H
