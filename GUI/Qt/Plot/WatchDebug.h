#ifndef NMSDK_WATCH_DEBUG_H
#define NMSDK_WATCH_DEBUG_H

#include <QByteArray>
#include <cstdlib>

namespace NMSDK {
namespace WatchDebug {

inline bool& runtimeFlag()
{
    static bool enabled = false;
    return enabled;
}

inline void setEnabled(bool on)
{
    runtimeFlag() = on;
}

inline bool enabled()
{
    if (runtimeFlag())
        return true;
    const char* env = std::getenv("NMSDK_WATCH_DEBUG");
    return env && env[0] != '\0' && env[0] != '0';
}

} // namespace WatchDebug
} // namespace NMSDK

#endif
