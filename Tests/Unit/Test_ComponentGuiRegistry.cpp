#include <gtest/gtest.h>

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
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

struct SecondaryTabHostFixture
{
    QMainWindow window;
    UComponentGuiTabHostWidget tabHost;

    SecondaryTabHostFixture(UComponentGuiService& service)
        : tabHost(QStringLiteral("Secondary"), &service, &window, reinterpret_cast<RDK::UApplication*>(0x1))
    {
        window.resize(640, 480);
        window.setCentralWidget(&tabHost);
        service.setSecondaryHostMainWindow(&window);
        service.setSecondaryTabHostWidget(&tabHost);
    }
};
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

    SecondaryTabHostFixture secondary(service);

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
    EXPECT_EQ(secondary.tabHost.tabCount(), 1);

    service.clearAllInstances();
}

TEST(ComponentGuiTabHost, CloseTabEndsSessionAndAllowsReopen)
{
    EnsureApp();
    const QString className = QStringLiteral("TestClass_TabHost_CloseReopen");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.tabhost.close";
    descriptor.title = "Tab Host Close";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.tabhost.close.controller", "Tab Host Close Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    SecondaryTabHostFixture secondary(service);
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.TabHost.Close"), 4);

    ASSERT_TRUE(secondary.tabHost.assignContext(context));
    EXPECT_EQ(secondary.tabHost.tabCount(), 1);

    ASSERT_TRUE(secondary.tabHost.removeContext(context));
    service.clearClosedInstances();
    EXPECT_EQ(secondary.tabHost.tabCount(), 0);

    UVisualControllerWidget* widget = nullptr;
    EXPECT_FALSE(service.tryGetWidgetByContext(context, widget));

    ASSERT_TRUE(secondary.tabHost.assignContext(context));
    EXPECT_EQ(secondary.tabHost.tabCount(), 1);
    ASSERT_TRUE(service.tryGetWidgetByContext(context, widget));
    ASSERT_NE(widget, nullptr);

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

TEST(ComponentGuiSecondaryTabHost, WidgetFillsTabCellAfterResize)
{
    EnsureApp();
    const QString className = QStringLiteral("TestClass_SecondaryTabHost_Fill");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.secondary.fill";
    descriptor.title = "Secondary Fill";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.secondary.fill.controller", "Secondary Fill Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);

    SecondaryTabHostFixture secondary(service);
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.SecondaryFill"), 3);
    UVisualControllerWidget* widget = service.createOrActivate(&mainWindow, context);
    ASSERT_NE(widget, nullptr);

    ASSERT_TRUE(service.attachToSecondaryDock(context));
    secondary.window.resize(800, 600);
    QApplication::processEvents();

    UVisualControllerWidget* embedded = nullptr;
    ASSERT_TRUE(service.tryGetWidgetByContext(context, embedded));
    ASSERT_NE(embedded, nullptr);
    QWidget* cell = embedded->parentWidget();
    ASSERT_NE(cell, nullptr);
    auto* layout = qobject_cast<QHBoxLayout*>(cell->layout());
    ASSERT_NE(layout, nullptr);
    EXPECT_EQ(layout->count(), 1);
    EXPECT_GE(embedded->width(), cell->width() * 9 / 10);

    service.clearAllInstances();
}

TEST(ComponentGuiSecondaryTabHost, TwoContextsGetTwoTabs)
{
    EnsureApp();
    const QString classA = QStringLiteral("TestClass_Secondary_TwoTabs_A");
    const QString classB = QStringLiteral("TestClass_Secondary_TwoTabs_B");

    UComponentFormDescriptor descriptorA;
    descriptorA.formId = "test.form.secondary.twotabs.a";
    descriptorA.title = "Two Tabs A";
    descriptorA.singleInstance = true;
    descriptorA.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.secondary.twotabs.a", "Two Tabs A", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classA, descriptorA);

    UComponentFormDescriptor descriptorB;
    descriptorB.formId = "test.form.secondary.twotabs.b";
    descriptorB.title = "Two Tabs B";
    descriptorB.singleInstance = true;
    descriptorB.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.secondary.twotabs.b", "Two Tabs B", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classB, descriptorB);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);

    SecondaryTabHostFixture secondary(service);
    const UComponentGuiContext contextA = MakeContext(classA, QStringLiteral("Model.TwoTabs.A"), 0);
    const UComponentGuiContext contextB = MakeContext(classB, QStringLiteral("Model.TwoTabs.B"), 1);

    ASSERT_NE(service.createOrActivate(&mainWindow, contextA), nullptr);
    ASSERT_NE(service.createOrActivate(&mainWindow, contextB), nullptr);
    ASSERT_TRUE(service.attachToSecondaryDock(contextA));
    ASSERT_TRUE(service.attachToSecondaryDock(contextB));

    EXPECT_EQ(secondary.tabHost.tabCount(), 2);
    EXPECT_TRUE(secondary.tabHost.hasContext(contextA));
    EXPECT_TRUE(secondary.tabHost.hasContext(contextB));

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

    SecondaryTabHostFixture secondary(service);

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

    SecondaryTabHostFixture secondary(service);

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
    EXPECT_EQ(secondary.tabHost.tabCount(), 1);

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
