#include <gtest/gtest.h>

#include <QString>
#include <QApplication>
#include <QWidget>

#include "../../GUI/Qt/UComponentFormRegistry.h"
#include "../../GUI/Qt/UComponentGuiService.h"
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
