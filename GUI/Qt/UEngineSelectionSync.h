#ifndef UENGINE_SELECTION_SYNC_H
#define UENGINE_SELECTION_SYNC_H

#include <QString>

/// Keeps RDK engine CurrentComponent aligned with NeuroModeler UI selection.
void syncEngineCurrentComponent(const QString& component_long_name);

#endif
