#ifndef U_GUI_MODEL_SNAPSHOT_H
#define U_GUI_MODEL_SNAPSHOT_H

#include <QDateTime>
#include <QHash>
#include <QReadWriteLock>
#include <QTimer>
#include <memory>
#include <vector>

#include "../Core/Engine/UEPtr.h"
#include "../Core/Engine/UELockPtr.h"
#include "../Core/Engine/UNet.h"
#include "../Core/Engine/UContainer.h"
#include "../Deploy/Include/rdk_cpp_init.h"

namespace NMSDK {

struct UGuiComponentSummary
{
    QString LongName;
    QString ShortName;
    QString ClassName;
    QString ParentName;
    int ChildCount = 0;
    quint64 Fingerprint = 0;
};

struct UGuiSnapshot
{
    quint64 Version = 0;
    QDateTime Timestamp;
    QHash<QString, UGuiComponentSummary> Components;
};

using UGuiSnapshotPtr = std::shared_ptr<const UGuiSnapshot>;

class UGuiModelSnapshot final : public QObject
{
    Q_OBJECT

public:
    static UGuiModelSnapshot& Instance();

    void Start(int intervalMs = 500);
    void Stop();

    UGuiSnapshotPtr CurrentSnapshot() const;

signals:
    void SnapshotUpdated(NMSDK::UGuiSnapshotPtr snapshot,
                         QStringList added,
                         QStringList removed,
                         QStringList changed);

private slots:
    void HandleRefresh();

private:
    void ensureRunning() const;
    explicit UGuiModelSnapshot(QObject* parent = nullptr);
    void RefreshSnapshot();
    void CollectComponent(const RDK::UEPtr<RDK::UContainer>& component,
                          const RDK::UEPtr<RDK::UContainer>& root,
                          const QString& parentName,
                          UGuiSnapshot& snapshot);

    mutable QReadWriteLock SnapshotLock;
    UGuiSnapshotPtr SnapshotValue;
    quint64 VersionCounter;
    QTimer* RefreshTimer;
    int RefreshIntervalMs;
    bool RefreshScheduled;
    mutable bool TimerAutoStarted;
};

} // namespace NMSDK

#endif // U_GUI_MODEL_SNAPSHOT_H

