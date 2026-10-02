#include "WatchPropertyDndPayload.h"

#include <QMimeData>

namespace NMSDK
{
namespace Plot
{

namespace
{
constexpr char kMime[] = "application/x-nmsdk-watch-property";
}

const char* WatchPropertyDndPayload::mimeType()
{
    return kMime;
}

QByteArray WatchPropertyDndPayload::encode(const WatchPropertyDragRef& ref)
{
    return QStringLiteral("%1\n%2\n%3\n%4\n%5")
        .arg(ref.component, ref.property)
        .arg(ref.jx)
        .arg(ref.jy)
        .arg(ref.channel)
        .toUtf8();
}

bool WatchPropertyDndPayload::decode(const QMimeData* mime, WatchPropertyDragRef& out)
{
    if (!mime || !mime->hasFormat(kMime))
        return false;
    const QStringList parts = QString::fromUtf8(mime->data(kMime)).split(QLatin1Char('\n'));
    if (parts.size() < 2)
        return false;
    out.component = parts.at(0).trimmed();
    out.property = parts.at(1).trimmed();
    out.jx = parts.size() > 2 ? parts.at(2).toInt() : 0;
    out.jy = parts.size() > 3 ? parts.at(3).toInt() : 0;
    out.channel = parts.size() > 4 ? parts.at(4).toInt() : 0;
    return !out.component.isEmpty() && !out.property.isEmpty();
}

} // namespace Plot
} // namespace NMSDK
