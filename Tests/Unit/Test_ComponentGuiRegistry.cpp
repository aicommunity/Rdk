#include <gtest/gtest.h>

#include <QString>
#include <QApplication>
#include <QWidget>
#include <QMdiArea>
#include <QFrame>
#include <QMimeData>
#include <QMainWindow>
#include <QDockWidget>

#include "../../GUI/Qt/UComponentFormRegistry.h"
#include "../../GUI/Qt/UComponentGuiService.h"
#include "../../GUI/Qt/UComponentGuiGridContainerWidget.h"
#include "../../GUI/Qt/UComponentGuiDndPayload.h"
#include "../../GUI/Qt/UVisualControllerWidget.h"
#include "../../GUI/Qt/UGenericComponentControllerWidget.h"

// Простейший smoke-тест: регистрация и canOpen/lookup по имени класса
TEST(ComponentGuiRegistry, RegisterAndLookupByClassName)
{
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();

    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form";
    descriptor.title = "Test Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        Q_UNUSED(app);
        return nullptr; // Для smoke-теста достаточно факта регистрации
    };

    const QString className = QStringLiteral("TestClass_ComponentGuiRegistry");

    registry.registerFormFactory(className, descriptor);

    // canOpen по строке
    EXPECT_TRUE(registry.canOpen(className));

    // canOpen по контексту
    UComponentGuiContext context;
    context.componentClassName = className;
    EXPECT_TRUE(registry.canOpen(context));

    // findDescriptor должен вернуть непустой дескриптор с ожидаемыми полями
    const UComponentFormDescriptor* found = registry.findDescriptor(className);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->formId, descriptor.formId);
    EXPECT_EQ(found->title, descriptor.title);
    EXPECT_TRUE(found->singleInstance);
}

// Smoke-тест для UComponentGuiService: singleInstance и проброс контекста
TEST(ComponentGuiService, SingleInstanceAndContextPropagation)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
    {
        new QApplication(argc, argv);
    }

    const QString className = QStringLiteral("TestClass_ComponentGuiService");

    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.single";
    descriptor.title = "Test Form Single";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.component.controller", "Test Controller", nullptr, app);
    };

    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QWidget owner;

    UComponentGuiContext context;
    context.componentClassName = className;
    context.componentLongName = QStringLiteral("Model.Component");
    context.channelIndex = 3;

    // Первый вызов должен создать новый контроллер
    UVisualControllerWidget* first = service.createOrActivate(&owner, context);
    ASSERT_NE(first, nullptr);

    // Повторный вызов с тем же контекстом должен вернуть тот же экземпляр (singleInstance)
    UVisualControllerWidget* second = service.createOrActivate(&owner, context);
    ASSERT_NE(second, nullptr);
    EXPECT_EQ(first, second);

    // Очистка
    service.clearAllInstances();
}

// Smoke-тест: сервис не может открыть форму, если класс не зарегистрирован
TEST(ComponentGuiService, CannotOpenWhenNotRegistered)
{
    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QWidget owner;

    UComponentGuiContext context;
    context.componentClassName = QStringLiteral("UnknownClass");
    context.componentLongName = QStringLiteral("Model.Unknown");
    context.channelIndex = -1;

    EXPECT_FALSE(service.canOpen(context));
    UVisualControllerWidget* widget = service.createOrActivate(&owner, context);
    EXPECT_EQ(widget, nullptr);
}

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
}

TEST(ComponentGuiPipeline, DiagramEntrypointContextOpenAndReopen)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
    {
        new QApplication(argc, argv);
    }

    const QString className = QStringLiteral("TestClass_DiagramEntrypoint");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.diagram";
    descriptor.title = "Diagram Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.diagram.controller", "Diagram Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QWidget owner;
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.FromDiagram"), 0);

    UVisualControllerWidget* first = service.createOrActivate(&owner, context);
    ASSERT_NE(first, nullptr);
    UVisualControllerWidget* second = service.createOrActivate(&owner, context);
    ASSERT_NE(second, nullptr);
    EXPECT_EQ(first, second);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, ComponentsListEntrypointContextOpenAndReopen)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
    {
        new QApplication(argc, argv);
    }

    const QString className = QStringLiteral("TestClass_ComponentsListEntrypoint");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.list";
    descriptor.title = "List Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.list.controller", "List Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QWidget owner;
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.FromList"), 2);

    UVisualControllerWidget* first = service.createOrActivate(&owner, context);
    ASSERT_NE(first, nullptr);
    UVisualControllerWidget* second = service.createOrActivate(&owner, context);
    ASSERT_NE(second, nullptr);
    EXPECT_EQ(first, second);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, DrawEngineEntrypointContextOpenAndFallback)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
    {
        new QApplication(argc, argv);
    }

    const QString registeredClass = QStringLiteral("TestClass_DrawEngineEntrypoint");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.drawengine";
    descriptor.title = "DrawEngine Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.drawengine.controller", "DrawEngine Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(registeredClass, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QWidget owner;

    const UComponentGuiContext registeredContext =
        MakeContext(registeredClass, QStringLiteral("Model.FromDrawEngine"), 1);
    UVisualControllerWidget* opened = service.createOrActivate(&owner, registeredContext);
    ASSERT_NE(opened, nullptr);

    const UComponentGuiContext unknownContext =
        MakeContext(QStringLiteral("UnknownClass_DrawEngine"), QStringLiteral("Model.Unknown"), 1);
    EXPECT_FALSE(service.canOpen(unknownContext));
    UVisualControllerWidget* fallback = service.createOrActivate(&owner, unknownContext);
    EXPECT_EQ(fallback, nullptr);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, CloseAndReopenCreatesNewInstance)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
    {
        new QApplication(argc, argv);
    }

    const QString className = QStringLiteral("TestClass_CloseReopen");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.closereopen";
    descriptor.title = "Close Reopen Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.close.reopen.controller", "Close Reopen Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QWidget owner;
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.CloseReopen"), 4);

    UVisualControllerWidget* first = service.createOrActivate(&owner, context);
    ASSERT_NE(first, nullptr);

    // Simulate "close all forms" and ensure the next open creates a new instance.
    service.clearAllInstances();
    UVisualControllerWidget* reopened = service.createOrActivate(&owner, context);
    ASSERT_NE(reopened, nullptr);
    EXPECT_NE(first, reopened);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, DetachAttachAndSnapshotHostMode)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString className = QStringLiteral("TestClass_DetachAttachSnapshot");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.detach";
    descriptor.title = "Detach Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.detach.controller", "Detach Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMdiArea mdiArea;
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.DetachAttach"), 1);
    ASSERT_NE(service.createOrActivate(&mdiArea, context), nullptr);
    EXPECT_TRUE(service.detachToFloating(context));

    QList<UComponentGuiSessionSnapshot> snapshots = service.snapshotOpenSessions();
    ASSERT_FALSE(snapshots.isEmpty());
    EXPECT_EQ(snapshots.first().hostMode, UComponentGuiHostMode::Floating);

    EXPECT_TRUE(service.attachToMdi(context, &mdiArea));
    snapshots = service.snapshotOpenSessions();
    ASSERT_FALSE(snapshots.isEmpty());
    EXPECT_EQ(snapshots.first().hostMode, UComponentGuiHostMode::Mdi);

    UVisualControllerWidget* resolvedWidget = nullptr;
    EXPECT_TRUE(service.tryGetWidgetByContext(context, resolvedWidget));
    EXPECT_NE(resolvedWidget, nullptr);
    UComponentGuiHostMode hostMode = UComponentGuiHostMode::Floating;
    EXPECT_TRUE(service.tryGetHostModeByContext(context, hostMode));
    EXPECT_EQ(hostMode, UComponentGuiHostMode::Mdi);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, MoveToGridAndBackToMdiUpdatesHostMode)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString className = QStringLiteral("TestClass_MoveGridBack");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.move.grid";
    descriptor.title = "Move Grid Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.move.grid.controller", "Move Grid Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMdiArea mdiArea;
    QFrame gridCellHost;
    gridCellHost.resize(400, 300);

    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.MoveGridBack"), 2);
    ASSERT_NE(service.createOrActivate(&mdiArea, context), nullptr);

    EXPECT_TRUE(service.moveToGridCell(context, QStringLiteral("MainGrid"), 0, 0, &gridCellHost));
    UComponentGuiHostMode hostMode = UComponentGuiHostMode::Mdi;
    EXPECT_TRUE(service.tryGetHostModeByContext(context, hostMode));
    EXPECT_EQ(hostMode, UComponentGuiHostMode::Grid);

    EXPECT_TRUE(service.attachToMdi(context, &mdiArea));
    EXPECT_TRUE(service.tryGetHostModeByContext(context, hostMode));
    EXPECT_EQ(hostMode, UComponentGuiHostMode::Mdi);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, GridDropFromExternalPayloadAssignsWidgetToCell)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString className = QStringLiteral("TestClass_GridExternalDrop");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.grid.external.drop";
    descriptor.title = "Grid External Drop Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.grid.external.drop.controller", "Grid External Drop Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    UComponentGuiGridContainerWidget grid(QStringLiteral("MainGrid"), &service, nullptr, reinterpret_cast<RDK::UApplication*>(0x1));
    grid.setGridSize(1, 2);

    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.GridExternalDrop"), 3);
    auto* mime = new QMimeData();
    mime->setData(UComponentGuiDndPayload::mimeType(), UComponentGuiDndPayload::encode(context));

    ASSERT_TRUE(grid.handleDropToCell(0, 0, mime));
    EXPECT_TRUE(grid.hasContext(0, 0));

    QWidget* host = grid.cellHostWidget(0, 0);
    ASSERT_NE(host, nullptr);
    bool foundControllerChild = false;
    for(QWidget* child : host->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
    {
        if(qobject_cast<UVisualControllerWidget*>(child))
        {
            foundControllerChild = true;
            break;
        }
    }
    EXPECT_TRUE(foundControllerChild);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, GridToGridDropClearsSourceAndSetsTarget)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString className = QStringLiteral("TestClass_GridToGridDrop");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.grid.to.grid";
    descriptor.title = "Grid To Grid Form";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.grid.to.grid.controller", "Grid To Grid Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    UComponentGuiGridContainerWidget grid(QStringLiteral("MainGrid"), &service, nullptr, reinterpret_cast<RDK::UApplication*>(0x1));
    grid.setGridSize(1, 2);

    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.GridToGrid"), 4);
    ASSERT_TRUE(grid.assignCell(0, 0, context));
    ASSERT_TRUE(grid.hasContext(0, 0));
    ASSERT_FALSE(grid.hasContext(0, 1));

    auto* mime = new QMimeData();
    mime->setData(UComponentGuiDndPayload::mimeType(),
                  UComponentGuiDndPayload::encode(context, QStringLiteral("MainGrid"), 0, 0));
    ASSERT_TRUE(grid.handleDropToCell(0, 1, mime));

    EXPECT_FALSE(grid.hasContext(0, 0));
    EXPECT_TRUE(grid.hasContext(0, 1));
    EXPECT_EQ(grid.contextAt(0, 1).componentLongName, context.componentLongName);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, SwapCellsPreservesHostPointers)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString classNameA = QStringLiteral("TestClass_SwapA");
    const QString classNameB = QStringLiteral("TestClass_SwapB");

    UComponentFormDescriptor descriptorA;
    descriptorA.formId = "test.form.swap.a";
    descriptorA.title = "Swap A";
    descriptorA.singleInstance = true;
    descriptorA.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.swap.a.controller", "Swap A Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classNameA, descriptorA);

    UComponentFormDescriptor descriptorB;
    descriptorB.formId = "test.form.swap.b";
    descriptorB.title = "Swap B";
    descriptorB.singleInstance = true;
    descriptorB.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.swap.b.controller", "Swap B Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(classNameB, descriptorB);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    UComponentGuiGridContainerWidget grid(QStringLiteral("MainGrid"), &service, nullptr, reinterpret_cast<RDK::UApplication*>(0x1));
    grid.setGridSize(1, 2);

    const UComponentGuiContext contextA = MakeContext(classNameA, QStringLiteral("Model.SwapA"), 0);
    const UComponentGuiContext contextB = MakeContext(classNameB, QStringLiteral("Model.SwapB"), 1);

    QWidget* host00 = grid.cellHostWidget(0, 0);
    QWidget* host01 = grid.cellHostWidget(0, 1);
    ASSERT_NE(host00, nullptr);
    ASSERT_NE(host01, nullptr);

    ASSERT_TRUE(grid.assignCell(0, 0, contextA));
    ASSERT_TRUE(grid.assignCell(0, 1, contextB));

    ASSERT_TRUE(grid.swapCells(0, 0, 0, 1));

    EXPECT_EQ(grid.cellHostWidget(0, 0), host00);
    EXPECT_EQ(grid.cellHostWidget(0, 1), host01);
    EXPECT_EQ(grid.contextAt(0, 0).componentLongName, contextB.componentLongName);
    EXPECT_EQ(grid.contextAt(0, 1).componentLongName, contextA.componentLongName);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, FullHostCycleMdiFloatingGridMdi)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString className = QStringLiteral("TestClass_FullHostCycle");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.full.host.cycle";
    descriptor.title = "Full Host Cycle";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.full.host.cycle.controller", "Full Host Cycle Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMdiArea mdiArea;
    QFrame gridHost;
    gridHost.resize(320, 240);
    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.FullHostCycle"), 5);

    ASSERT_NE(service.createOrActivate(&mdiArea, context), nullptr);
    UComponentGuiHostMode mode = UComponentGuiHostMode::Grid;
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    ASSERT_TRUE(service.detachToFloating(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Floating);

    ASSERT_TRUE(service.moveToGridCell(context, QStringLiteral("MainGrid"), 0, 0, &gridHost));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Grid);

    ASSERT_TRUE(service.attachToMdi(context, &mdiArea));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    service.clearAllInstances();
}

TEST(ComponentGuiPipeline, NativeDockHostFloatAndReattachCycle)
{
    static int argc = 1;
    static char arg0[] = "test";
    static char* argv[] = {arg0, nullptr};
    if(!qApp)
        new QApplication(argc, argv);

    const QString className = QStringLiteral("TestClass_NativeDockCycle");
    UComponentFormDescriptor descriptor;
    descriptor.formId = "test.form.native.dock.cycle";
    descriptor.title = "Native Dock Cycle";
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget* {
        return new UGenericComponentControllerWidget("test.native.dock.cycle.controller", "Native Dock Cycle Controller", nullptr, app);
    };
    UComponentFormRegistry::instance().registerFormFactory(className, descriptor);

    UComponentGuiService service(reinterpret_cast<RDK::UApplication*>(0x1));
    QMainWindow hostMainWindow;
    QMdiArea mdiArea;
    hostMainWindow.setCentralWidget(&mdiArea);
    service.setHostMainWindow(&hostMainWindow);

    const UComponentGuiContext context = MakeContext(className, QStringLiteral("Model.NativeDockCycle"), 6);
    UVisualControllerWidget* widget = service.createOrActivate(&hostMainWindow, context);
    ASSERT_NE(widget, nullptr);

    UComponentGuiHostMode mode = UComponentGuiHostMode::Floating;
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    ASSERT_TRUE(service.detachToFloating(context));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Floating);

    ASSERT_TRUE(service.attachToMdi(context, &mdiArea));
    ASSERT_TRUE(service.tryGetHostModeByContext(context, mode));
    EXPECT_EQ(mode, UComponentGuiHostMode::Mdi);

    QDockWidget* dockHost = nullptr;
    QObject* current = widget->parent();
    while(current)
    {
        dockHost = qobject_cast<QDockWidget*>(current);
        if(dockHost)
            break;
        current = current->parent();
    }
    ASSERT_NE(dockHost, nullptr);
    EXPECT_FALSE(dockHost->isFloating());
    EXPECT_TRUE(dockHost->features().testFlag(QDockWidget::DockWidgetMovable));
    EXPECT_TRUE(dockHost->features().testFlag(QDockWidget::DockWidgetFloatable));

    service.clearAllInstances();
}
