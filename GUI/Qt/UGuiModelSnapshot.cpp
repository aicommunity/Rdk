#include "UGuiModelSnapshot.h"

#include <QCoreApplication>
#include <QHash>
#include <QSet>
#include <QMetaObject>
#include <QtConcurrent/QtConcurrent>
#include <vector>

#include <rdk_init.h>

#include "UGuiTelemetry.h"

namespace NMSDK {

namespace {

QString ExtractShortName(const QString& longName)
{
    const int separator = longName.lastIndexOf('.');
    if (separator < 0)
        return longName;
    return longName.mid(separator + 1);
}

quint64 BuildFingerprint(const UGuiComponentSummary& summary)
{
    quint64 value = qHash(summary.LongName);
    value ^= qHash(summary.ClassName);
    value ^= quint64(summary.ChildCount + 1) << 32;
    return value;
}

} // namespace

UGuiModelSnapshot& UGuiModelSnapshot::Instance()
{
    static UGuiModelSnapshot instance;
    return instance;
}

UGuiModelSnapshot::UGuiModelSnapshot(QObject* parent)
    : QObject(parent)
    , SnapshotValue(std::make_shared<UGuiSnapshot>())
    , VersionCounter(0)
    , RefreshTimer(new QTimer(this))
    , RefreshIntervalMs(500)
    , RefreshScheduled(false)
    , TimerAutoStarted(false)
{
    RefreshTimer->setTimerType(Qt::CoarseTimer);
    RefreshTimer->setInterval(RefreshIntervalMs);
    connect(RefreshTimer, &QTimer::timeout, this, &UGuiModelSnapshot::HandleRefresh);
}

void UGuiModelSnapshot::Start(int intervalMs)
{
    RefreshIntervalMs = qMax(50, intervalMs);
    RefreshTimer->setInterval(RefreshIntervalMs);
    if (!RefreshTimer->isActive()) {
        RefreshTimer->start();
    }
    HandleRefresh();
    TimerAutoStarted = true;
}

void UGuiModelSnapshot::Stop()
{
    RefreshTimer->stop();
}

UGuiSnapshotPtr UGuiModelSnapshot::CurrentSnapshot() const
{
    ensureRunning();
    QReadLocker locker(&SnapshotLock);
    return SnapshotValue;
}

void UGuiModelSnapshot::ensureRunning() const
{
    if (!RefreshTimer->isActive()) {
        TimerAutoStarted = true;
        const_cast<UGuiModelSnapshot*>(this)->Start(RefreshIntervalMs);
    }
}

void UGuiModelSnapshot::HandleRefresh()
{
    if (RefreshScheduled)
        return;
    RefreshScheduled = true;

    QtConcurrent::run([this]() {
        NMSDK::UGuiTelemetryScope scope(QStringLiteral("Snapshot"), QStringLiteral("Refresh"));
        RefreshSnapshot();
        scope.Stop();
        QMetaObject::invokeMethod(this, [this]() { RefreshScheduled = false; }, Qt::QueuedConnection);
    });
}

void UGuiModelSnapshot::RefreshSnapshot()
{
    auto nextSnapshot = std::make_shared<UGuiSnapshot>();
    nextSnapshot->Timestamp = QDateTime::currentDateTimeUtc();

    QStringList added;
    QStringList removed;
    QStringList changed;

    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>();
    if (model) {
        RDK::UEPtr<RDK::UNet> net(model.Get());
        RDK::UEPtr<RDK::UContainer> root(net);
        CollectComponent(root, root, QString(), *nextSnapshot);
    }

    nextSnapshot->Version = ++VersionCounter;

    {
        QWriteLocker locker(&SnapshotLock);
        if (SnapshotValue && !SnapshotValue->Components.isEmpty()) {
            const auto oldList = SnapshotValue->Components.keys();
            const auto newList = nextSnapshot->Components.keys();
            const QSet<QString> oldKeys(oldList.cbegin(), oldList.cend());
            const QSet<QString> newKeys(newList.cbegin(), newList.cend());

            for (const QString& key : newKeys) {
                if (!oldKeys.contains(key)) {
                    added << key;
                } else {
                    const auto& oldSummary = SnapshotValue->Components[key];
                    const auto& newSummary = nextSnapshot->Components[key];
                    if (oldSummary.Fingerprint != newSummary.Fingerprint) {
                        changed << key;
                    }
                }
            }

            for (const QString& key : oldKeys) {
                if (!newKeys.contains(key)) {
                    removed << key;
                }
            }
        } else {
            added = nextSnapshot->Components.keys();
        }

        SnapshotValue = nextSnapshot;
    }

    emit SnapshotUpdated(nextSnapshot, added, removed, changed);
}

void UGuiModelSnapshot::CollectComponent(const RDK::UEPtr<RDK::UContainer>& component,
                                         const RDK::UEPtr<RDK::UContainer>& root,
                                         const QString& parentName,
                                         UGuiSnapshot& snapshot)
{
    if (!component)
        return;

    std::string buffer;
    QString longName = QString::fromLocal8Bit(component->GetLongName(root, buffer).c_str());
    if (longName.isEmpty())
        longName = QStringLiteral("UnnamedComponent");

    UGuiComponentSummary summary;
    summary.LongName = longName;
    summary.ShortName = ExtractShortName(longName);
    summary.ClassName = QString::fromLocal8Bit(component->GetCompClassName().c_str());
    summary.ParentName = parentName;

    std::vector<RDK::UId> children;
    component->GetComponentsList(children);
    summary.ChildCount = static_cast<int>(children.size());
    summary.Fingerprint = BuildFingerprint(summary);

    snapshot.Components.insert(summary.LongName, summary);

    for (const RDK::UId& childId : children) {
        RDK::UEPtr<RDK::UContainer> child = component->GetComponent(childId, true);
        CollectComponent(child, root, summary.LongName, snapshot);
    }
}

} // namespace NMSDK

