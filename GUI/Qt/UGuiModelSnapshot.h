#ifndef U_GUI_MODEL_SNAPSHOT_H
#define U_GUI_MODEL_SNAPSHOT_H

#include <QDateTime>
#include <QHash>
#include <QReadWriteLock>
#include <QSet>
#include <QTimer>
#include <QVariant>
#include <QMutex>
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

/// Key for property subscription (component + property + matrix indices + channel)
struct UGuiPropertyKey
{
    QString ComponentName;
    QString PropertyName;
    int ChannelIndex = 0;
    int Jx = 0;
    int Jy = 0;

    bool operator==(const UGuiPropertyKey& other) const
    {
        return ComponentName == other.ComponentName &&
               PropertyName == other.PropertyName &&
               ChannelIndex == other.ChannelIndex &&
               Jx == other.Jx && Jy == other.Jy;
    }
};

inline uint qHash(const UGuiPropertyKey& key, uint seed = 0)
{
    return ::qHash(key.ComponentName, seed) ^
           ::qHash(key.PropertyName, seed) ^
           ::qHash(key.ChannelIndex, seed) ^
           ::qHash(key.Jx, seed) ^
           ::qHash(key.Jy, seed);
}

/// Cached property value
struct UGuiPropertyValue
{
    double NumericValue = 0.0;
    bool Available = false;
    quint64 UpdateTime = 0;
};

struct UGuiSnapshot
{
    quint64 Version = 0;
    QDateTime Timestamp;
    QHash<QString, UGuiComponentSummary> Components;
    QHash<UGuiPropertyKey, UGuiPropertyValue> PropertyValues;
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

    /// Subscribe to property value updates (thread-safe)
    void SubscribeProperty(const UGuiPropertyKey& key);

    /// Unsubscribe from property value updates (thread-safe)
    void UnsubscribeProperty(const UGuiPropertyKey& key);

    /// Clear all property subscriptions (thread-safe)
    void ClearPropertySubscriptions();

    /// Get current property value from latest snapshot (lock-free read)
    /// Returns false if property is not subscribed or unavailable
    bool GetPropertyValue(const UGuiPropertyKey& key, double& outValue) const;

    /// Check if property value is available in current snapshot
    bool IsPropertyAvailable(const UGuiPropertyKey& key) const;

signals:
    void SnapshotUpdated(NMSDK::UGuiSnapshotPtr snapshot,
                         QStringList added,
                         QStringList removed,
                         QStringList changed);

    /// Emitted when subscribed property values are updated
    void PropertyValuesUpdated(NMSDK::UGuiSnapshotPtr snapshot);

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

    /// Collect values for all subscribed properties
    void CollectPropertyValues(UGuiSnapshot& snapshot);

    /// Read single property value from kernel (must be called under model lock)
    bool ReadPropertyValue(const UGuiPropertyKey& key, double& outValue);

    mutable QReadWriteLock SnapshotLock;
    UGuiSnapshotPtr SnapshotValue;
    quint64 VersionCounter;
    QTimer* RefreshTimer;
    int RefreshIntervalMs;
    bool RefreshScheduled;
    mutable bool TimerAutoStarted;

    /// Property subscriptions (protected by SubscriptionsMutex)
    mutable QMutex SubscriptionsMutex;
    QSet<UGuiPropertyKey> PropertySubscriptions;
};

} // namespace NMSDK

#endif // U_GUI_MODEL_SNAPSHOT_H

