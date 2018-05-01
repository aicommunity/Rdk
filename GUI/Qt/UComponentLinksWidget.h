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
    explicit UComponentLinksWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UComponentLinksWidget();
    void AUpdateInterface();    

    /// ������ ����� ��������
    virtual void ASaveParameters();
    /// ���������� ����� ��������
    virtual void ALoadParameters();

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

signals:
    void updateScheme(bool forceUpdate);

private:
    Ui::UComponentLinksWidget *ui;

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
