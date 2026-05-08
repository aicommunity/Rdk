#include <gtest/gtest.h>

#include <QApplication>
#include <QFrame>
#include <QMainWindow>
#include <QMdiArea>
#include <QString>

#include "../../Core/Serialize/USerStorageXML.h"
#include "../../GUI/Qt/UComponentFormRegistry.h"
#include "../../GUI/Qt/UComponentGuiService.h"
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

void SaveSessions(RDK::USerStorageXML& xml, const QList<UComponentGuiSessionSnapshot>& sessions)
{
    xml.Create("ComponentGuiLifecycle");
    xml.SelectNodeForce("ComponentGuiLayout");
    xml.WriteInteger("SessionCount", sessions.size());
    for(int i = 0; i < sessions.size(); ++i)
    {
        const UComponentGuiSessionSnapshot& s = sessions[i];
        xml.SelectNodeForce("Session_" + RDK::sntoa(i + 1));
        xml.WriteString("ComponentClassName", s.componentClassName.toStdString());
        xml.WriteString("ComponentLongName", s.componentLongName.toStdString());
        xml.WriteInteger("ChannelIndex", s.channelIndex);
        xml.WriteInteger("HostMode", static_cast<int>(s.hostMode));
        xml.WriteBool("IsActive", s.isActive);
        xml.SelectUp();
    }
    xml.SelectUp();
}

QList<UComponentGuiSessionSnapshot> LoadSessions(RDK::USerStorageXML& xml)
{
    QList<UComponentGuiSessionSnapshot> sessions;
    if(!xml.SelectNode("ComponentGuiLayout"))
        return sessions;
    const int count = xml.ReadInteger("SessionCount", 0);
    for(int i = 0; i < count; ++i)
    {
        if(!xml.SelectNode("Session_" + RDK::sntoa(i + 1)))
            continue;
        UComponentGuiSessionSnapshot s;
        s.componentClassName = QString::fromStdString(xml.ReadString("ComponentClassName", ""));
        s.componentLongName = QString::fromStdString(xml.ReadString("ComponentLongName", ""));
        s.channelIndex = xml.ReadInteger("ChannelIndex", -1);
        s.hostMode = static_cast<UComponentGuiHostMode>(xml.ReadInteger("HostMode", static_cast<int>(UComponentGuiHostMode::Mdi)));
        s.isActive = xml.ReadBool("IsActive", false);
        sessions.push_back(s);
        xml.SelectUp();
    }
    xml.SelectUp();
    return sessions;
}
}

TEST(ComponentGuiLifecycle, SaveCloseOpenRestoresSessionsAndHostModes)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString classA = QStringLiteral("TestClass_Lifecycle_A");
    const QString classB = QStringLiteral("TestClass_Lifecycle_B");

    UComponentFormDescriptor descriptorA;
    descriptorA.formId = "test.lifecycle.a";
    descriptorA.title = "Lifecycle A";
    descriptorA.singleInstance = true;
    descriptorA.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.lifecycle.controller.a", "Lifecycle A Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classA, descriptorA);

    UComponentFormDescriptor descriptorB;
    descriptorB.formId = "test.lifecycle.b";
    descriptorB.title = "Lifecycle B";
    descriptorB.singleInstance = true;
    descriptorB.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.lifecycle.controller.b", "Lifecycle B Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classB, descriptorB);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);
    QMainWindow secondaryWindow;
    service.setSecondaryHostMainWindow(&secondaryWindow);

    const UComponentGuiContext contextMdi = MakeContext(classA, QStringLiteral("Model.Component.MDI"), 0);
    const UComponentGuiContext contextSecondary = MakeContext(classB, QStringLiteral("Model.Component.Secondary"), 1);

    ASSERT_NE(service.createOrActivate(&mainWindow, contextMdi), nullptr);
    ASSERT_NE(service.createOrActivate(&mainWindow, contextSecondary), nullptr);
    EXPECT_TRUE(service.detachToFloating(contextMdi));
    QFrame tabHostCell;
    tabHostCell.resize(320, 240);
    EXPECT_TRUE(service.moveToTabHost(contextMdi, QStringLiteral("MainTabHost"), &tabHostCell));
    EXPECT_TRUE(service.attachToSecondaryDock(contextSecondary));

    QList<UComponentGuiSessionSnapshot> beforeSave = service.snapshotOpenSessions();
    ASSERT_EQ(beforeSave.size(), 2);

    RDK::USerStorageXML xml;
    SaveSessions(xml, beforeSave);

    service.clearAllInstances();

    UComponentGuiService reopened(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow reopenedMainWindow;
    QMdiArea reopenedMdiArea;
    reopenedMainWindow.setCentralWidget(&reopenedMdiArea);
    reopened.setHostMainWindow(&reopenedMainWindow);
    QMainWindow reopenedSecondaryWindow;
    reopened.setSecondaryHostMainWindow(&reopenedSecondaryWindow);
    QList<UComponentGuiSessionSnapshot> restored = LoadSessions(xml);
    ASSERT_EQ(restored.size(), 2);
    QFrame reopenedTabHostCell;
    reopenedTabHostCell.resize(320, 240);

    for(const UComponentGuiSessionSnapshot& s : restored)
    {
        UComponentGuiContext context = MakeContext(s.componentClassName, s.componentLongName, s.channelIndex);
        ASSERT_NE(reopened.createOrActivate(&reopenedMainWindow, context), nullptr);
        if(s.hostMode == UComponentGuiHostMode::Floating)
            EXPECT_TRUE(reopened.detachToFloating(context));
        if(s.hostMode == UComponentGuiHostMode::TabHost)
        {
            EXPECT_TRUE(reopened.moveToTabHost(context, QStringLiteral("MainTabHost"), &reopenedTabHostCell));
        }
        if(s.hostMode == UComponentGuiHostMode::SecondaryDock)
            EXPECT_TRUE(reopened.attachToSecondaryDock(context));
    }

    QList<UComponentGuiSessionSnapshot> afterOpen = reopened.snapshotOpenSessions();
    ASSERT_EQ(afterOpen.size(), 2);

    int tabHostCount = 0;
    int secondaryCount = 0;
    for(const UComponentGuiSessionSnapshot& s : afterOpen)
    {
        if(s.hostMode == UComponentGuiHostMode::TabHost)
            ++tabHostCount;
        if(s.hostMode == UComponentGuiHostMode::SecondaryDock)
            ++secondaryCount;
    }
    EXPECT_EQ(tabHostCount, 1);
    EXPECT_EQ(secondaryCount, 1);

    reopened.clearAllInstances();
}

TEST(ComponentGuiLifecycle, LegacyGridHostModeRestoresToTabHost)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString classA = QStringLiteral("TestClass_Lifecycle_Legacy");
    UComponentFormDescriptor descriptorA;
    descriptorA.formId = "test.lifecycle.legacy";
    descriptorA.title = "Lifecycle Legacy";
    descriptorA.singleInstance = true;
    descriptorA.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.lifecycle.controller.legacy", "Lifecycle Legacy Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classA, descriptorA);

    RDK::USerStorageXML xml;
    xml.Create("ComponentGuiLifecycleLegacy");
    xml.SelectNodeForce("ComponentGuiLayout");
    xml.WriteInteger("SessionCount", 1);
    xml.SelectNodeForce("Session_1");
    xml.WriteString("ComponentClassName", classA.toStdString());
    xml.WriteString("ComponentLongName", QStringLiteral("Model.Component.Legacy").toStdString());
    xml.WriteInteger("ChannelIndex", 0);
    xml.WriteInteger("HostMode", static_cast<int>(UComponentGuiHostMode::Grid));
    xml.WriteBool("IsActive", true);
    xml.SelectUp();
    xml.SelectUp();

    QList<UComponentGuiSessionSnapshot> restored = LoadSessions(xml);
    ASSERT_EQ(restored.size(), 1);
    EXPECT_EQ(restored.first().hostMode, UComponentGuiHostMode::Grid);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow mainWindow;
    QMdiArea mdiArea;
    mainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&mainWindow);
    const UComponentGuiContext context = MakeContext(restored.first().componentClassName,
                                                     restored.first().componentLongName,
                                                     restored.first().channelIndex);
    ASSERT_NE(service.createOrActivate(&mainWindow, context), nullptr);
    QFrame tabHostCell;
    tabHostCell.resize(320, 240);
    ASSERT_TRUE(service.moveToTabHost(context, QStringLiteral("MainTabHost"), &tabHostCell));

    UComponentGuiHostMode mode = UComponentGuiHostMode::Mdi;
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::TabHost);

    service.clearAllInstances();
}
