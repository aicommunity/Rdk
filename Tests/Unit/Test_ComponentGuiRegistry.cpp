#include <gtest/gtest.h>

#include <QString>

#include "../../GUI/Qt/UComponentFormRegistry.h"

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

