#include "UEngineSelectionSync.h"

#include <rdk_init.h>

void syncEngineCurrentComponent(const QString& component_long_name)
{
    const QByteArray utf8 = component_long_name.trimmed().toUtf8();
    if(utf8.isEmpty())
    {
        Env_ResetCurrentComponent(nullptr);
        return;
    }
    Env_SelectCurrentComponent(utf8.constData());
}
