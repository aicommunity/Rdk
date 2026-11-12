#ifndef ICOMPONENTCONTROLLERWIDGET_H
#define ICOMPONENTCONTROLLERWIDGET_H

#include <QString>

struct UComponentGuiContext;

class IComponentControllerWidget
{
public:
    virtual ~IComponentControllerWidget() = default;

    virtual void setComponentContext(const UComponentGuiContext& context) = 0;
    virtual void refreshFromModel(bool force) = 0;
    virtual QString componentGuiId() const = 0;
};

#endif // ICOMPONENTCONTROLLERWIDGET_H
