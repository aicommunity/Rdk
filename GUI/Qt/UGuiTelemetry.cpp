#include "UGuiTelemetry.h"

#include <QByteArray>
#include <QThread>
#include <mutex>

#include <rdk_logging.h>

namespace {
std::mutex gTelemetryMutex;
NMSDK::UGuiTelemetrySink::Callback gTelemetryCallback;
}

namespace NMSDK {

void UGuiTelemetrySink::SetCallback(Callback callback)
{
    std::lock_guard<std::mutex> lock(gTelemetryMutex);
    gTelemetryCallback = std::move(callback);
}

void UGuiTelemetrySink::Emit(const QString& category, const QString& name, qint64 durationMs)
{
    Callback callbackCopy;
    {
        std::lock_guard<std::mutex> lock(gTelemetryMutex);
        callbackCopy = gTelemetryCallback;
    }

    if (callbackCopy) {
        callbackCopy(category, name, durationMs);
        return;
    }

    const QByteArray buffer = QStringLiteral("GUI|%1|%2|%3ms|thread=%4")
                                  .arg(category, name)
                                  .arg(durationMs)
                                  .arg(reinterpret_cast<qulonglong>(QThread::currentThreadId()), 0, 16)
                                  .toLocal8Bit();

    RDK::Logging::SystemLog(RDK_EX_DEBUG, buffer.constData());
}

UGuiTelemetryScope::UGuiTelemetryScope(const QString& category, const QString& name)
    : Category(category)
    , Name(name)
    , Active(true)
{
    Timer.start();
}

UGuiTelemetryScope::~UGuiTelemetryScope()
{
    if (Active) {
        UGuiTelemetrySink::Emit(Category, Name, Timer.elapsed());
    }
}

qint64 UGuiTelemetryScope::Elapsed() const
{
    return Timer.elapsed();
}

void UGuiTelemetryScope::Stop()
{
    if (!Active)
        return;

    Active = false;
    UGuiTelemetrySink::Emit(Category, Name, Timer.elapsed());
}

} // namespace NMSDK

