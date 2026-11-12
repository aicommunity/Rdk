#ifndef UCOMPONENTGUICONTEXT_H
#define UCOMPONENTGUICONTEXT_H

#include <QString>
#include <QMetaType>

struct UComponentGuiContext
{
    QString componentLongName;
    QString componentClassName;
    int channelIndex = -1;
};

Q_DECLARE_METATYPE(UComponentGuiContext)

#endif // UCOMPONENTGUICONTEXT_H
