#include "UComponentGuiDndPayload.h"

#include <QMimeData>

namespace
{
constexpr const char* kComponentGuiMime = "application/x-nmsdk-component-gui-context";
}

const char* UComponentGuiDndPayload::mimeType()
{
    return kComponentGuiMime;
}

QByteArray UComponentGuiDndPayload::encode(const UComponentGuiContext& context,
                                           const QString& sourceGridId,
                                           int sourceRow,
                                           int sourceCol)
{
    return (context.componentClassName + "\n"
            + context.componentLongName + "\n"
            + QString::number(context.channelIndex) + "\n"
            + sourceGridId + "\n"
            + QString::number(sourceRow) + "\n"
            + QString::number(sourceCol)).toUtf8();
}

bool UComponentGuiDndPayload::decode(const QMimeData* mimeData,
                                     UComponentGuiContext& context,
                                     QString& sourceGridId,
                                     int& sourceRow,
                                     int& sourceCol)
{
    if(!mimeData || !mimeData->hasFormat(kComponentGuiMime))
        return false;

    const QStringList parts = QString::fromUtf8(mimeData->data(kComponentGuiMime)).split('\n');
    if(parts.size() < 3)
        return false;

    context.componentClassName = parts[0];
    context.componentLongName = parts[1];
    context.channelIndex = parts[2].toInt();
    sourceGridId = parts.size() > 3 ? parts[3] : QString();
    sourceRow = parts.size() > 4 ? parts[4].toInt() : -1;
    sourceCol = parts.size() > 5 ? parts[5].toInt() : -1;

    return !context.componentClassName.trimmed().isEmpty();
}
