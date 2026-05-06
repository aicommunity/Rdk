#include <gtest/gtest.h>

#include <QString>
#include <QApplication>
#include <QPointer>

#include "../../GUI/Qt/UComponentFormRegistry.h"
#include "../../GUI/Qt/UComponentGuiService.h"

// Локальный тестовый контроллер для проверки UComponentGuiService
class TestControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget
{
    Q_OBJECT
public:
    explicit TestControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr)
        : UVisualControllerWidget(parent, app)
    {
    }

    void setComponentContext(const UComponentGuiContext& context) override
    {
        m_lastContext = context;
    }

    void refreshFromModel(bool force) override
    {
        Q_UNUSED(force);
        ++m_refreshCalls;
    }

    QString componentGuiId() const override
    {
        return "test.component.controller";
    }

    UComponentGuiContext lastContext() const { return m_lastContext; }
    int refreshCalls() const { return m_refreshCalls; }

private:
    UComponentGuiContext m_lastContext;
    int m_refreshCalls = 0;
};

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
        return new TestControllerWidget(nullptr, app);
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

    auto* typedFirst = dynamic_cast<TestControllerWidget*>(first);
    ASSERT_NE(typedFirst, nullptr);
    EXPECT_EQ(typedFirst->lastContext().componentLongName, context.componentLongName);
    EXPECT_EQ(typedFirst->lastContext().componentClassName, context.componentClassName);
    EXPECT_EQ(typedFirst->lastContext().channelIndex, context.channelIndex);

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

#include "Test_ComponentGuiRegistry.moc"
