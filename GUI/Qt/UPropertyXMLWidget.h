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

    /// флаг должен определять тип Property
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

    /// маска типа и группы свойства
    int propertyMask;

    /// флаг инициализации виджета
    bool isInit;

    ///имя компонента, свойство которого отображается
    QString componentName;
};

#endif // UPROPERTYXMLWIDGET_H
