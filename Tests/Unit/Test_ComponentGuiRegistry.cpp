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

TEST(ComponentGuiTransfer, TabHostSecondaryRoundTripViaServiceRoute)
{
    EnsureApp();
    const QString className = QStringLiteral("TestClass_TabHost_Secondary_RoundTrip");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.tabhost.secondary.roundtrip";
    descriptor.title = "TabHost Secondary RoundTrip";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.tabhost.secondary.roundtrip.controller",
                                                     "TabHost Secondary RoundTrip Controller",
                                                     nullptr,
                                                     app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);

    QMainWindow secondaryWindow;
    service.setSecondaryHostMainWindow(&secondaryWindow);

    UComponentGuiTabHostWidget tabHost(QStringLiteral("MainTabHost"),
                                       &service,
                                       nullptr,
                                       reinterpret_cast<RDK::UApplication*>(0x1));
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.RoundTrip"), 2);

    ASSERT_NE(service.createOrActivate(&mainWindow, context), nullptr);
    UComponentGuiHostMode mode = UComponentGuiHostMode::Floating;
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    ASSERT_TRUE(tabHost.assignContext(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::TabHost);
    EXPECT_TRUE(tabHost.hasContext(context));
    EXPECT_EQ(tabHost.contexts().size(), 1);

    ASSERT_TRUE(service.attachToSecondaryDock(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::SecondaryDock);

    ASSERT_TRUE(tabHost.assignContext(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::TabHost);
    EXPECT_TRUE(tabHost.hasContext(context));

    ASSERT_TRUE(service.attachToMdi(context, &mdiArea));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    const QList<UComponentGuiSessionSnapshot> sessions = service.snapshotOpenSessions();
    ASSERT_EQ(sessions.size(), 1);
    EXPECT_EQ(sessions.first().hostMode, UComponentGuiHostMode::Mdi);

    service.clearAllInstances();
}

TEST(ComponentGuiTransfer, TwoContextsCrossSwapBetweenTabHostAndSecondary)
{
    EnsureApp();
    const QString classA = QStringLiteral("TestClass_TabHost_Secondary_Cross_A");
    const QString classB = QStringLiteral("TestClass_TabHost_Secondary_Cross_B");

    UComponentFormDescriptor descriptorA;
    descriptorA.formId = "test.form.tabhost.secondary.cross.a";
    descriptorA.title = "Cross A";
    descriptorA.singleInstance = true;
    descriptorA.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.tabhost.secondary.cross.controller.a",
                                                     "Cross Controller A",
                                                     nullptr,
                                                     app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classA, descriptorA);

    UComponentFormDescriptor descriptorB;
    descriptorB.formId = "test.form.tabhost.secondary.cross.b";
    descriptorB.title = "Cross B";
    descriptorB.singleInstance = true;
    descriptorB.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.tabhost.secondary.cross.controller.b",
                                                     "Cross Controller B",
                                                     nullptr,
                                                     app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classB, descriptorB);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);

    QMainWindow secondaryWindow;
    service.setSecondaryHostMainWindow(&secondaryWindow);

    UComponentGuiTabHostWidget tabHost(QStringLiteral("MainTabHost"),
                                       &service,
                                       nullptr,
                                       reinterpret_cast<RDK::UApplication*>(0x1));
    const UComponentGuiContext contextA = MakeContext(classA, QStringLiteral("Model.Cross.A"), 10);
    const UComponentGuiContext contextB = MakeContext(classB, QStringLiteral("Model.Cross.B"), 11);

    ASSERT_NE(service.createOrActivate(&mainWindow, contextA), nullptr);
    ASSERT_NE(service.createOrActivate(&mainWindow, contextB), nullptr);

    ASSERT_TRUE(tabHost.assignContext(contextA));
    ASSERT_TRUE(service.attachToSecondaryDock(contextB));

    UComponentGuiHostMode modeA = UComponentGuiHostMode::Floating;
    UComponentGuiHostMode modeB = UComponentGuiHostMode::Floating;
    ASSERT_TRUE(service.tryGetHostModeByContext(contextA, modeA));
    ASSERT_TRUE(service.tryGetHostModeByContext(contextB, modeB));
    EXPECT_EQ(modeA, UComponentGuiHostMode::TabHost);
    EXPECT_EQ(modeB, UComponentGuiHostMode::SecondaryDock);

    // Cross swap: A -> Secondary, B -> TabHost.
    ASSERT_TRUE(service.attachToSecondaryDock(contextA));
    ASSERT_TRUE(tabHost.assignContext(contextB));

    ASSERT_TRUE(service.tryGetHostModeByContext(contextA, modeA));
    ASSERT_TRUE(service.tryGetHostModeByContext(contextB, modeB));
    EXPECT_EQ(modeA, UComponentGuiHostMode::SecondaryDock);
    EXPECT_EQ(modeB, UComponentGuiHostMode::TabHost);
    EXPECT_TRUE(tabHost.hasContext(contextB));

    const QList<UComponentGuiSessionSnapshot> sessions = service.snapshotOpenSessions();
    ASSERT_EQ(sessions.size(), 2);
    int tabHostCount = 0;
    int secondaryCount = 0;
    for(const UComponentGuiSessionSnapshot& s : sessions)
    {
        if(s.hostMode == UComponentGuiHostMode::TabHost)
            ++tabHostCount;
        if(s.hostMode == UComponentGuiHostMode::SecondaryDock)
            ++secondaryCount;
    }
    EXPECT_EQ(tabHostCount, 1);
    EXPECT_EQ(secondaryCount, 1);

    service.clearAllInstances();
}
