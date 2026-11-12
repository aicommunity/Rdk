#ifndef UCOMPONENTFORMREGISTRY_H
#define UCOMPONENTFORMREGISTRY_H

#include <functional>
#include <vector>

#include <QString>
#include <QHash>

#include <rdk_application.h>

#include "UComponentGuiContext.h"

class UVisualControllerWidget;

struct UComponentFormDescriptor
{
    QString formId;
    QString title;
    bool singleInstance = true;
    std::function<UVisualControllerWidget*(RDK::UApplication*)> factory;
};

class UComponentFormRegistry
{
public:
    static UComponentFormRegistry& instance();

    void registerFormFactory(const QString& componentClassName, const UComponentFormDescriptor& descriptor);
    bool canOpen(const QString& componentClassName) const;
    bool canOpen(const UComponentGuiContext& context) const;
    const UComponentFormDescriptor* findDescriptor(const QString& componentClassName) const;

    std::vector<QString> registeredClasses() const;

private:
    QHash<QString, UComponentFormDescriptor> m_formsByClass;
};

#endif // UCOMPONENTFORMREGISTRY_H
