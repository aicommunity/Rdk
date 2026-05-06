#include <gtest/gtest.h>

#include <QApplication>
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
    QMdiArea mdiArea;

    const UComponentGuiContext contextMdi = MakeContext(classA, QStringLiteral("Model.Component.MDI"), 0);
    const UComponentGuiContext contextFloating = MakeContext(classB, QStringLiteral("Model.Component.Floating"), 1);

    ASSERT_NE(service.createOrActivate(&mdiArea, contextMdi), nullptr);
    ASSERT_NE(service.createOrActivate(&mdiArea, contextFloating), nullptr);
    EXPECT_TRUE(service.detachToFloating(contextFloating));

    QList<UComponentGuiSessionSnapshot> beforeSave = service.snapshotOpenSessions();
    ASSERT_EQ(beforeSave.size(), 2);

    RDK::USerStorageXML xml;
    SaveSessions(xml, beforeSave);

    service.clearAllInstances();

    UComponentGuiService reopened(reinterpret_cast<RDK::UApplication*>(0x1));
    QList<UComponentGuiSessionSnapshot> restored = LoadSessions(xml);
    ASSERT_EQ(restored.size(), 2);

    for(const UComponentGuiSessionSnapshot& s : restored)
    {
        UComponentGuiContext context = MakeContext(s.componentClassName, s.componentLongName, s.channelIndex);
        ASSERT_NE(reopened.createOrActivate(&mdiArea, context), nullptr);
        if(s.hostMode == UComponentGuiHostMode::Floating)
            EXPECT_TRUE(reopened.detachToFloating(context));
    }

    QList<UComponentGuiSessionSnapshot> afterOpen = reopened.snapshotOpenSessions();
    ASSERT_EQ(afterOpen.size(), 2);

    int mdiCount = 0;
    int floatingCount = 0;
    for(const UComponentGuiSessionSnapshot& s : afterOpen)
    {
        if(s.hostMode == UComponentGuiHostMode::Mdi)
            ++mdiCount;
        if(s.hostMode == UComponentGuiHostMode::Floating)
            ++floatingCount;
    }
    EXPECT_EQ(mdiCount, 1);
    EXPECT_EQ(floatingCount, 1);

    reopened.clearAllInstances();
}
