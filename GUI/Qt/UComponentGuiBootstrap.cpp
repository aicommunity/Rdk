#include "UComponentGuiBootstrap.h"

#include "UComponentFormRegistry.h"
#include "UGenericComponentControllerWidget.h"

namespace
{
UComponentFormDescriptor MakeDescriptor(const QString& id, const QString& title)
{
    UComponentFormDescriptor descriptor;
    descriptor.formId = id;
    descriptor.title = title;
    descriptor.singleInstance = true;
    descriptor.factory = [id, title](RDK::UApplication* app) -> UVisualControllerWidget*
    {
        return new UGenericComponentControllerWidget(id, title, nullptr, app);
    };
    return descriptor;
}
}

// registration units from library subrepositories
void RegisterMotionControlComponentGuiForms();
void RegisterPulseLibComponentGuiForms();
void RegisterBasicLibComponentGuiForms();
void RegisterCvBasicLibComponentGuiForms();
void RegisterHardwareLibComponentGuiForms();

void RegisterComponentGuiForms(RDK::UApplication* app, const UComponentGuiRegistrationOptions& options)
{
    Q_UNUSED(app);
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();

    // Reference forms proving core registration flow.
    registry.registerFormFactory("NModel", MakeDescriptor("core.model.inspector", "Model Inspector"));
    registry.registerFormFactory("UNet", MakeDescriptor("core.net.inspector", "Network Inspector"));

    if(options.enableMotionControl)
        RegisterMotionControlComponentGuiForms();
    if(options.enablePulseLib)
        RegisterPulseLibComponentGuiForms();
    if(options.enableBasicLib)
        RegisterBasicLibComponentGuiForms();
    if(options.enableCvBasicLib)
        RegisterCvBasicLibComponentGuiForms();
    if(options.enableHardwareLib)
        RegisterHardwareLibComponentGuiForms();
}

void RegisterComponentGuiForms(RDK::UApplication* app)
{
    RegisterComponentGuiForms(app, UComponentGuiRegistrationOptions{});
}
