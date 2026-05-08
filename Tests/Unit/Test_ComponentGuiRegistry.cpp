#include <gtest/gtest.h>

#include <QApplication>
#include <QFrame>
#include <QMainWindow>
#include <QMdiArea>

#include "../../GUI/Qt/UComponentFormRegistry.h"
#include "../../GUI/Qt/UComponentGuiService.h"
#include "../../GUI/Qt/UComponentGuiTabHostWidget.h"
#include "../../GUI/Qt/UGenericComponentControllerWidget.h"

namespace
{
UComponentGuiContext MakeContext(const QString& className, const QString& longName, int channel)
{
    UComponentGuiContext context;
    context.componentClassName = className;
    context.componentLongName = longName;
    context.channelIndex = channel;
    return context;
}

void EnsureApp()
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);
}
}

TEST(ComponentGuiRegistry, RegisterAndLookupByClassName)
{
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form";
    descriptor.title = "Test Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication*) -> UVisualControllerWidget* { return nullptr; };

    const QString className = QStringLiteral("TestClass_ComponentGuiRegistry");
    registry.registerFormFactory(className, descriptor);
    EXPECT_TRUE(registry.canOpen(className));
    EXPECT_TRUE(registry.findDescriptor(className) != nullptr);
}

TEST(ComponentGuiService, DockFloatTabHostSecondaryCycle)
{
    EnsureApp();
    const QString className = QStringLiteral("TestClass_HostCycle");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.host.cycle";
    descriptor.title = "Host Cycle";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.host.cycle.controller", "Host Cycle Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);

    QMainWindow secondaryWindow;
    service.setSecondaryHostMainWindow(&secondaryWindow);

    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.HostCycle"), 1);
    ASSERT_NE(service.createOrActivate(&mainWindow, context), nullptr);

    UComponentGuiHostMode mode = UComponentGuiHostMode::Floating;
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    ASSERT_TRUE(service.detachToFloating(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Floating);

    ASSERT_TRUE(service.attachToMdi(context, &mdiArea));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    QFrame tabHostCell;
    tabHostCell.resize(300, 240);
    ASSERT_TRUE(service.moveToTabHost(context, QStringLiteral("MainTabHost"), &tabHostCell));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::TabHost);

    ASSERT_TRUE(service.attachToSecondaryDock(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::SecondaryDock);

    service.clearAllInstances();
}

TEST(ComponentGuiTabHost, AssignContextAddsTab)
{
    EnsureApp();
    const QString className = QStringLiteral("TestClass_TabHost");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.tabhost";
    descriptor.title = "Tab Host";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.tabhost.controller", "Tab Host Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    UComponentGuiTabHostWidget host(QStringLiteral("MainTabHost"), &service, nullptr, reinterpret_cast<RDK::UApplication*>(0x1));
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.TabHost"), 0);
    ASSERT_TRUE(host.assignContext(context));
    EXPECT_TRUE(host.hasContext(context));
    EXPECT_EQ(host.contexts().size(), 1);
    service.clearAllInstances();
}
