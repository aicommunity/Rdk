#include "WatchPresetCatalog.h"

#include "../UEngineSelectionSync.h"

#include "rdk.h"
#include <rdk_application.h>
#include "../../Core/Engine/UContainer.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QDebug>

WatchPresetCatalog& WatchPresetCatalog::instance()
{
    static WatchPresetCatalog cat;
    return cat;
}

void WatchPresetCatalog::setRootPath(const QString& absoluteOrRelativePath)
{
    m_rootPath = absoluteOrRelativePath;
}

NMSDK::Plot::VizKind WatchPresetCatalog::vizFromString(const QString& s)
{
    if (s.compare(QStringLiteral("XYLine"), Qt::CaseInsensitive) == 0)
        return NMSDK::Plot::VizKind::XYLine;
    if (s.compare(QStringLiteral("XYScatter"), Qt::CaseInsensitive) == 0)
        return NMSDK::Plot::VizKind::XYScatter;
    return NMSDK::Plot::VizKind::TimeSeries;
}

bool WatchPresetCatalog::loadFile(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
    {
        qWarning() << "WatchPresetCatalog: invalid JSON" << filePath << err.errorString();
        return false;
    }
    const QJsonObject root = doc.object();
    const int schema = root.value(QStringLiteral("schemaVersion")).toInt(0);
    if (schema != 1)
    {
        qWarning() << "WatchPresetCatalog: unsupported schemaVersion" << schema << filePath;
        return false;
    }
    const QString className = root.value(QStringLiteral("className")).toString();
    if (className.isEmpty())
        return false;
    const QString library = root.value(QStringLiteral("library")).toString();
    QVector<WatchPreset> list;
    const QJsonArray presets = root.value(QStringLiteral("presets")).toArray();
    for (const QJsonValue& pv : presets)
    {
        if (!pv.isObject())
            continue;
        const QJsonObject po = pv.toObject();
        WatchPreset p;
        p.id = po.value(QStringLiteral("id")).toString();
        p.title = po.value(QStringLiteral("title")).toString();
        p.description = po.value(QStringLiteral("description")).toString();
        p.vizKind = vizFromString(po.value(QStringLiteral("vizKind")).toString());
        p.className = className;
        p.library = library;
        if (p.id.isEmpty())
            continue;
        const QJsonArray series = po.value(QStringLiteral("series")).toArray();
        for (const QJsonValue& sv : series)
        {
            if (!sv.isObject())
                continue;
            const QJsonObject so = sv.toObject();
            WatchPresetSeriesRef ref;
            ref.path = so.value(QStringLiteral("path")).toString();
            ref.property = so.value(QStringLiteral("property")).toString();
            ref.jx = so.value(QStringLiteral("jx")).toInt(0);
            ref.jy = so.value(QStringLiteral("jy")).toInt(0);
            if (ref.property.isEmpty())
                continue;
            p.series.push_back(ref);
        }
        if (p.series.isEmpty())
            continue;
        list.push_back(p);
    }
    if (list.isEmpty())
        return false;
    m_byClass[className] = list;
    return true;
}

bool WatchPresetCatalog::reload()
{
    m_byClass.clear();
    if (m_rootPath.isEmpty())
        return false;
    QDir root(m_rootPath);
    if (!root.exists())
    {
        qWarning() << "WatchPresetCatalog: root missing" << m_rootPath;
        return false;
    }

    static const QStringList kSkip = {
        QStringLiteral("meta"),
        QStringLiteral("draft"),
    };

    int loaded = 0;
    for (const QFileInfo& libInfo : root.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        if (kSkip.contains(libInfo.fileName()))
            continue;
        QDir libDir(libInfo.absoluteFilePath());
        for (const QFileInfo& fi : libDir.entryInfoList({QStringLiteral("*.json")}, QDir::Files))
        {
            if (loadFile(fi.absoluteFilePath()))
                ++loaded;
        }
    }
    return loaded > 0;
}

QVector<WatchPreset> WatchPresetCatalog::presetsForClass(const QString& className) const
{
    return m_byClass.value(className);
}

WatchPreset WatchPresetCatalog::presetById(const QString& className, const QString& presetId) const
{
    for (const WatchPreset& p : m_byClass.value(className))
    {
        if (p.id == presetId)
            return p;
    }
    return {};
}

QVector<NMSDK::Plot::PropertyRef> WatchPresetCatalog::resolve(const WatchPreset& preset,
                                                              const QString& rootLongName,
                                                              int channel,
                                                              QString* errorOut) const
{
    QVector<NMSDK::Plot::PropertyRef> out;
    if (rootLongName.isEmpty() || preset.series.isEmpty())
    {
        if (errorOut)
            *errorOut = QObject::tr("Missing component or empty preset.");
        return {};
    }

    RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock<RDK::UContainer>(channel);
    if (!model)
    {
        if (errorOut)
            *errorOut = QObject::tr("Model not available for channel %1.").arg(channel);
        return {};
    }

    QStringList failures;
    for (const WatchPresetSeriesRef& s : preset.series)
    {
        QString full = rootLongName;
        if (!s.path.isEmpty())
            full = rootLongName + QLatin1Char('.') + s.path;

        RDK::UEPtr<RDK::UContainer> cont = model->GetComponentL(full.toStdString(), true);
        if (!cont)
        {
            failures << full;
            continue;
        }
        NMSDK::Plot::PropertyRef ref;
        ref.component = full;
        ref.property = s.property;
        ref.jx = s.jx;
        ref.jy = s.jy;
        ref.slice = NMSDK::Plot::SliceKind::Cell;
        out.push_back(ref);
    }

    if (!failures.isEmpty())
    {
        if (errorOut)
            *errorOut = QObject::tr("Components not found:\n%1").arg(failures.join(QLatin1Char('\n')));
        return {};
    }
    if (out.size() != preset.series.size())
    {
        if (errorOut)
            *errorOut = QObject::tr("Failed to resolve all preset series.");
        return {};
    }
    return out;
}

QString defaultWatchPresetsPath(RDK::UApplication* app)
{
    if (app && !app->GetWatchPresetsPath().empty())
        return QString::fromStdString(app->GetWatchPresetsPath());
    // Sibling of typical ClDescPath ../../ClDesc/ → ../../WatchPresets/
    if (app && !app->GetClDescPath().empty())
    {
        QString cl = QString::fromStdString(app->GetClDescPath());
        if (cl.contains(QStringLiteral("ClDesc")))
            return cl.replace(QStringLiteral("ClDesc"), QStringLiteral("WatchPresets"));
    }
    return QStringLiteral("../../WatchPresets/");
}
