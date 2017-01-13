#ifndef UCOMPONENTLINKSWIDGET_H
#define UCOMPONENTLINKSWIDGET_H

#include "UVisualControllerWidget.h"
#include <QTreeWidgetItem>

namespace Ui {
class UComponentLinksWidget;
}


/// UComponentLinksWidget class - ������ ������ �� ������� ����������
///
/// ����� ��� ������ ������:
/// - ����� ������ � ����� ����������� - � ���� ������ ������������ ��� ����� �
/// ������ (������� ��������� �����/������) ������ ����������, � ���� ����������� ��������� ����� ����� ������������ ������� � ��������.
/// - ����� ������ � ����� ������������ - � ���� ������ ������������ ������ � ����� (������� ��������� ������/�����) ������� ����������,
/// � ����� ������� (������� ��������� �����)
/// �����! ������� ������ ��������� �����/������

class UComponentLinksWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UComponentLinksWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UComponentLinksWidget");
    virtual ~UComponentLinksWidget();
    void AUpdateInterface();    

public slots:
    ///������������� ���� ��� ��������� ������/�������/������ ������ ����������
    /// \details ����� ��������� ��������� ����� ����� ������������, ���������� � ������� ���������
    void initWidget(QString singleComponentName);

    ///������������� ���� ��� �������� ������ ����� ����� ������������ ������������
    void initWidget(QString firstComponentName, QString secondComponentName);

    ///������� ��������, ������� ����������
    void unInit();

    ///�������� ����� ��� ���������� ���� �����/����
    void createLink();

    ///���������� ���������� �����
    void breakLink();

    ///���������� ������ ��������
    void readSettings(QString file, QString group = "UComponentLinksWidget");
    ///������ ������ ��������
    void writeSettings(QString file, QString group);

signals:
    void updateScheme(bool forceUpdate);

private:
    Ui::UComponentLinksWidget *ui;

    QString settingsFileName;
    QString settingsGroupName;

    ///��� ���������� ���������
    QString firstComponentName;

    ///��� ���������� ���������
    QString secondComponentName;

    ///����������� ������� �������� ������ ��������� ����������� �� ������� ���������� �� �����
    void addComponentSons(QString componentName, QTreeWidgetItem *firstTreeWidgetItemFather, unsigned int firstTypeMask,
                          QTreeWidgetItem *secondTreeWidgetItemFather = NULL, unsigned int secondTypeMask = 0);

    ///������� ���������� ���������� �� �����
    void addParameters(QString componentName, QTreeWidgetItem *firstTreeWidgetItemFather, unsigned int firstTypeMask,
                       QTreeWidgetItem *secondTreeWidgetItemFather = NULL, unsigned int secondTypeMask = 0);

    ///������� ���������� ������ � ������� ������
    void addLinks(QString componentName);

/// ����� ������ �������
///
/// 0 - �� ����� �� ���� ���������;
/// 1 - ����������� ������/������/������ ������ ����������;
/// 2 - ����������� �������/������ ������� ������������, � ������ �������;
    int mode;
};

#endif // UCOMPONENTLINKSWIDGET_H
