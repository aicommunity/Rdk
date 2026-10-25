#include "WatchTemplateStore.h"

#include "../../Core/Serialize/USerStorageXML.h"
#include "../../Core/Application/UApplication.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace NMSDK
{
namespace Plot
{

namespace
{
constexpr char kTemplateRoot[] = "WatchTemplate";
}

QString watchTemplatesRoot(RDK::UApplication* app)
{
    if (!app)
        return {};
    QString path = QString::fromStdString(app->GetProjectPath());
    if (path.isEmpty())
        return {};
    if (!path.endsWith(QLatin1Char('/')) && !path.endsWith(QLatin1Char('\\')))
        path += QLatin1Char('/');
    path += QStringLiteral("WatchTemplates/");
    return path;
}

QString sharedWatchTemplatesRoot()
{
    // Prefer Bin/WatchTemplates next to Platform/<OS>/ binaries.
    const QString appDir = QCoreApplication::applicationDirPath();
    QDir d(appDir);
    // .../Bin/Platform/Linux -> .../Bin/WatchTemplates
    if (d.cdUp() && d.cdUp())
    {
        const QString candidate = d.absoluteFilePath(QStringLiteral("WatchTemplates"));
        if (QDir(candidate).exists() || QDir().mkpath(candidate))
            return candidate + QLatin1Char('/');
    }
    return {};
}

QStringList listWatchTemplateNames(const QString& dir)
{
    QStringList out;
    if (dir.isEmpty())
        return out;
    QDir d(dir);
    if (!d.exists())
        return out;
    const QStringList files = d.entryList(QStringList() << QStringLiteral("*.watch.xml"),
                                          QDir::Files, QDir::Name);
    for (const QString& f : files)
    {
        QString name = f;
        if (name.endsWith(QStringLiteral(".watch.xml"), Qt::CaseInsensitive))
            name.chop(10);
        out.push_back(name);
    }
    return out;
}

bool saveWatchTemplateFile(const QString& filePath, const PlotDocument& doc, QString* errorOut)
{
    if (filePath.isEmpty())
    {
        if (errorOut)
            *errorOut = QStringLiteral("Empty path");
        return false;
    }
    QDir().mkpath(QFileInfo(filePath).absolutePath());
    RDK::USerStorageXML xml;
    if (!xml.Create(kTemplateRoot))
    {
        if (errorOut)
            *errorOut = QStringLiteral("Failed to create XML root");
        return false;
    }
    xml.SelectRoot();
    savePlotDocument(xml, doc);
    if (!xml.SaveToFile(filePath.toStdString()))
    {
        if (errorOut)
            *errorOut = QStringLiteral("Failed to write %1").arg(filePath);
        return false;
    }
    return true;
}

bool loadWatchTemplateFile(const QString& filePath, PlotDocument& doc, QString* errorOut)
{
    if (filePath.isEmpty() || !QFileInfo::exists(filePath))
    {
        if (errorOut)
            *errorOut = QStringLiteral("File not found");
        return false;
    }
    RDK::USerStorageXML xml;
    if (!xml.LoadFromFile(filePath.toStdString(), kTemplateRoot))
    {
        // Fallback: try empty root / first node for hand-edited files.
        if (!xml.LoadFromFile(filePath.toStdString(), ""))
        {
            if (errorOut)
                *errorOut = QStringLiteral("Failed to parse %1").arg(filePath);
            return false;
        }
    }
    xml.SelectRoot();
    if (!loadPlotDocument(xml, doc))
    {
        if (errorOut)
            *errorOut = QStringLiteral("Invalid Watch template content");
        return false;
    }
    return true;
}

void reassignPlotObjectIds(PlotDocument& doc)
{
    for (PlotPanel& panel : doc.panels)
    {
        panel.id = makePlotObjectId(QStringLiteral("panel"));
        for (PlotSeries& serie : panel.series)
            serie.id = makePlotObjectId(QStringLiteral("serie"));
    }
}

} // namespace Plot
} // namespace NMSDK
