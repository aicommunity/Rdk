#ifndef UCLASSDESCRIPTIONDISPLAY_H
#define UCLASSDESCRIPTIONDISPLAY_H

#include <QDialog>
#include <rdk_application.h>
#include "../../Core/Engine/UContainerDescription.h"

namespace Ui {
class UClassDescriptionDisplay;
}

class UClassDescriptionDisplay : public QDialog
{
    Q_OBJECT
private:
    std::string ClassName;
    RDK::UEPtr<RDK::UContainerDescription> ClassDescription;

public:
    explicit UClassDescriptionDisplay(std::string class_name, QWidget *parent = nullptr);
    ~UClassDescriptionDisplay();

    void SaveDescription();

private:
    Ui::UClassDescriptionDisplay *ui;
};

#endif // UCLASSDESCRIPTIONDISPLAY_H
