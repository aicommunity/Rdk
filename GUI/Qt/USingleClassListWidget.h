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
class USingleClassListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit USingleClassListWidget(std::string class_name, QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~USingleClassListWidget();

    /// ������ ����� ��������
    virtual void ASaveParameters();
    /// ���������� ����� ��������
    virtual void ALoadParameters();

    /// ��������� ������ ��� �����������
    void ChangeClass(std::string class_name);

    /// ���������� ������ ��� ���������� ����������
    QString getSelectedComponentLongName();

public slots:
    void componentListItemSelectionChanged();

    void parametersListSelectionChanged();

    /// ���������� ������������� ������ �����������
    void reloadClassTree();

    /// ���������� ������������� �������
    void reloadPropertys();

    /// ������� ��������� UpdateInterval, ��� ���������� �������� �� ����
    void setUpdateInterval(long value);

signals:
    void parameterChanged(QString path);

private:
    /// �������� ������
    std::string ClassName;

    /// ��� ����������� ������
    QString selectedClass;

    /// ������� ��� ������������ ���������� ������������ �������� ������
    QString selectedComponentLong;

    /// ����� ��������� ������ Property
    QString selectedParameterName;

    /// ������ �����������
    QTreeWidget *componentsTree;

    /// ������� ����������� ����� ���������� ������ �����������
    void addComponentSons(std::shared_ptr<RDK::UContainer> cont, QString componentName, QTreeWidgetItem *treeWidgetFather);

    /// ������ �������� ������� ���� ��� ���������� ����������
    QString CalSelectedParameterPath();

    Ui::USingleClassListWidget *ui;
};



#endif // USINGLECLASSLISTWIDGET_H
