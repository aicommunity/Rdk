#ifndef UCLASSDESCRIPTIONDISPLAY_H
#define UCLASSDESCRIPTIONDISPLAY_H

#include <QWidget>
#include <rdk_application.h>
#include "../../Core/Engine/UContainerDescription.h"
#include <UVisualControllerWidget.h>

namespace Ui {
class UClassDescriptionDisplay;
}

class UClassDescriptionDisplay : public UVisualControllerWidget
{
    Q_OBJECT
private:
    std::string ClassName;
    RDK::UEPtr<RDK::UContainerDescription> ClassDescription;

public:
    explicit UClassDescriptionDisplay(std::string class_name, QWidget *parent = nullptr, RDK::UApplication *app = NULL);
    ~UClassDescriptionDisplay();

    const Ui::UClassDescriptionDisplay* GetUi() const;
public slots:
    void SaveDescription();
    void ChangeClassDescription(const std::string& class_name);

private:
    Ui::UClassDescriptionDisplay *ui;
};

#endif // UCLASSDESCRIPTIONDISPLAY_H
