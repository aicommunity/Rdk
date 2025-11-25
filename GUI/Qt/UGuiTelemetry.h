#ifndef U_GUI_TELEMETRY_H
#define U_GUI_TELEMETRY_H

#include <QElapsedTimer>
#include <QString>
#include <functional>

namespace NMSDK {

class UGuiTelemetrySink
{
public:
    using Callback = std::function<void(const QString&, const QString&, qint64)>;

    static void SetCallback(Callback callback);
    static void Emit(const QString& category, const QString& name, qint64 durationMs);
};

class UGuiTelemetryScope
{
public:
    UGuiTelemetryScope(const QString& category, const QString& name);
    ~UGuiTelemetryScope();

    qint64 Elapsed() const;
    void Stop();

private:
    QString Category;
    QString Name;
    QElapsedTimer Timer;
    bool Active;
};

} // namespace NMSDK

#endif // U_GUI_TELEMETRY_H

