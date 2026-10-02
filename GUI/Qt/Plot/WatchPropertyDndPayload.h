#ifndef WATCH_PROPERTY_DND_PAYLOAD_H
#define WATCH_PROPERTY_DND_PAYLOAD_H

#include <QByteArray>
#include <QString>

class QMimeData;

namespace NMSDK
{
namespace Plot
{

struct WatchPropertyDragRef
{
    QString component;
    QString property;
    int jx = 0;
    int jy = 0;
    int channel = 0;
};

class WatchPropertyDndPayload
{
public:
    static const char* mimeType();

    static QByteArray encode(const WatchPropertyDragRef& ref);
    static bool decode(const QMimeData* mime, WatchPropertyDragRef& out);
};

} // namespace Plot
} // namespace NMSDK

#endif // WATCH_PROPERTY_DND_PAYLOAD_H
