#ifndef WATCH_TEMPLATE_STORE_H
#define WATCH_TEMPLATE_STORE_H

#include "PlotDocument.h"

#include <QString>
#include <QStringList>

namespace RDK
{
class UApplication;
}

namespace NMSDK
{
namespace Plot
{

/// Root for user Watch layout templates: `<ProjectPath>/WatchTemplates/` or empty.
QString watchTemplatesRoot(RDK::UApplication* app);

/// Shared/fallback catalog: `Bin/WatchTemplates/` next to the executable tree.
QString sharedWatchTemplatesRoot();

/// List `*.watch.xml` basenames (without extension) under `dir`.
QStringList listWatchTemplateNames(const QString& dir);

/// Persist a full tab layout (grid + panels + series) as standalone XML.
bool saveWatchTemplateFile(const QString& filePath, const PlotDocument& doc, QString* errorOut = nullptr);

/// Load a Watch template XML into `doc`.
bool loadWatchTemplateFile(const QString& filePath, PlotDocument& doc, QString* errorOut = nullptr);

/// Assign fresh PanelId/SerieId values (safe when applying a template onto a live tab).
void reassignPlotObjectIds(PlotDocument& doc);

} // namespace Plot
} // namespace NMSDK

#endif // WATCH_TEMPLATE_STORE_H
