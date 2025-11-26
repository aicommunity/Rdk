#include "UGuiTelemetry.h"

#include <mutex>

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

    // По умолчанию телеметрия не логируется, чтобы не засорять системный лог.
    // Пользователь может задать callback через UGuiTelemetrySink::SetCallback.
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

