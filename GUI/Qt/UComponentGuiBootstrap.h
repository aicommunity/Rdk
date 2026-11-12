#ifndef UCOMPONENTGUIBOOTSTRAP_H
#define UCOMPONENTGUIBOOTSTRAP_H

#include <rdk_application.h>

struct UComponentGuiRegistrationOptions
{
    bool enableMotionControl = true;
    bool enablePulseLib = true;
    bool enableBasicLib = true;
    bool enableCvBasicLib = true;
    bool enableHardwareLib = true;
};

void RegisterComponentGuiForms(RDK::UApplication* app, const UComponentGuiRegistrationOptions& options);
void RegisterComponentGuiForms(RDK::UApplication* app);

#endif // UCOMPONENTGUIBOOTSTRAP_H
