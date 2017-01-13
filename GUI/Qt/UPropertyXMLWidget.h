#ifndef UPROPERTYXMLWIDGET_H
#define UPROPERTYXMLWIDGET_H


#include "rdk_application.h"

#include <QDialog>

namespace Ui {
class UPropertyXMLWidget;
}

class UPropertyXMLWidget : public QDialog
{
    Q_OBJECT

public:
    explicit UPropertyXMLWidget(QWidget *parent = 0);
    virtual ~UPropertyXMLWidget();

    /// ���� ������ ���������� ��� Property
    ///
    /// true - parameter
    /// false -  state
    void initWidget(QString longName, int mask);

public slots:
    void setProperty();
    void reloadProperty();
    void defaultAllProperty();

private:
    Ui::UPropertyXMLWidget *ui;

    /// ����� ���� � ������ ��������
    int propertyMask;

    /// ���� ������������� �������
    bool isInit;

    ///��� ����������, �������� �������� ������������
    QString componentName;
};

#endif // UPROPERTYXMLWIDGET_H
