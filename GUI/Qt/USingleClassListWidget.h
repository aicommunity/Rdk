#ifndef USINGLECLASSLISTWIDGET_H
#define USINGLECLASSLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include <rdk_application.h>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QModelIndex>
#include <QApplication>
#include <QHeaderView>

namespace Ui {
class USingleClassListWidget;
}

/// USingleClassListWidget class - ������ ����������� ������ � ��� ��������� �����������
/// ��������� ������ UComponentsListWidget
/// ����������� ������ ����������� � ������ �����������, � ������� �������
/// �������� ������� ������������ � ����� ����, �� �� �������� ��������� �� �����, ������� ����������� � UGEngineControllWidget.
class USingleClassListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit USingleClassListWidget(std::string class_name, QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~USingleClassListWidget();

    /// �������������� ������ �������� ������ (� ��������� � RDK)
    void AUpdateInterface();

    /// ������ ����� ��������
    virtual void ASaveParameters();
    /// ���������� ����� ��������
    virtual void ALoadParameters();


    void ChangeClass(std::string class_name);

    /// ���������� ������ ��� ���������� ����������
    QString getSelectedComponentLongName();

    /// ������������� ����������� �������
    void setEnableTabN(int n, bool enable);

public slots:
    void componentListItemSelectionChanged();

    void parametersListSelectionChanged();
    void reloadPropertys();

    /// ������� ��������� UpdateInterval, ��� ���������� �������� �� ����
    void setUpdateInterval(long value);

signals:
    void parameterChanged(QString path);

private:
    std::string ClassName;

    /// ��� ����������, ��� �������� ����������
    QString currentDrawPropertyComponentName;

    /// ���������� ���������. ���������� �� currentDrawPropertyComponentName, ��� ��� ��������������
    /// ��� componentListItemSelectionChanged, ����� ������������ � currentDrawPropertyComponentName,
    /// ����� ������� ���������� ����������� ��� ������������� ��������� ������ ����������.
    QString selectedClass;

    QString selectedComponentLong;

    // ����� ��������� ����� Property
    QString selectedParameterName;

    /// ��������� �� ��������� ����� TreeWidget � ������������ ����������� ��� ������� shift
    QTreeWidget *componentsTree;

    /// ��������� �������� ������������ �����
    QString currentDrawComponentName;

    /// ������� ����������� ����� ���������� ������ �����������
    void addComponentSons(RDK::UEPtr<RDK::UContainer> cont, QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem);

    QString CalSelectedParameterPath();

    Ui::USingleClassListWidget *ui;
};



#endif // USINGLECLASSLISTWIDGET_H
