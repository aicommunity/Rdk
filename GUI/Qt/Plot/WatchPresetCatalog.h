#ifndef WATCHPRESETCATALOG_H
#define WATCHPRESETCATALOG_H

#include "PlotDocument.h"

#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>

namespace RDK
{
class UApplication;
}

struct WatchPresetSeriesRef
{
    QString path;      ///< relative to selected root; empty = root itself
    QString property;
    int jx = 0;
    int jy = 0;
};

struct WatchPreset
{
    QString id;
    QString title;
    QString description;
    NMSDK::Plot::VizKind vizKind = NMSDK::Plot::VizKind::TimeSeries;
    QVector<WatchPresetSeriesRef> series;
    QString className;
    QString library;
};

/// Loads human-editable JSON watch presets from Bin/WatchPresets/<Lib>/<Class>.json.
class WatchPresetCatalog
{
public:
    static WatchPresetCatalog& instance();

    void setRootPath(const QString& absoluteOrRelativePath);
    QString rootPath() const { return m_rootPath; }

    /// Reload from disk (skips meta/ and draft/).
    bool reload();

    QVector<WatchPreset> presetsForClass(const QString& className) const;
    WatchPreset presetById(const QString& className, const QString& presetId) const;

    /// Resolve series to absolute PropertyRefs; empty list on failure (see errorOut).
    QVector<NMSDK::Plot::PropertyRef> resolve(const WatchPreset& preset,
                                              const QString& rootLongName,
                                              int channel,
                                              QString* errorOut = nullptr) const;

    int classCount() const { return m_byClass.size(); }

private:
    WatchPresetCatalog() = default;
    bool loadFile(const QString& filePath);
    static NMSDK::Plot::VizKind vizFromString(const QString& s);

    QString m_rootPath;
    QHash<QString, QVector<WatchPreset>> m_byClass;
};

QString defaultWatchPresetsPath(RDK::UApplication* app);

#endif // WATCHPRESETCATALOG_H
