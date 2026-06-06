#ifndef UENGINE_SELECTION_SYNC_H
#define UENGINE_SELECTION_SYNC_H

#include <QByteArray>
#include <QString>
#include <QStringList>

#include <string>

namespace RDK {
class UEngine;
class UContainer;
class UStorage;
}

/// Resolves a UI selection path to a model-root long name (e.g. PNeuron -> Model.PNeuron).
QString resolveComponentLongNameFromModelRoot(int channel_index, const QString& path);

/// Keeps RDK engine CurrentComponent aligned with NeuroModeler UI selection.
void syncEngineCurrentComponent(const QString& component_long_name);

/// Same as syncEngineCurrentComponent but uses an already-locked engine (no extra mutex acquire).
void syncEngineCurrentComponentWithEngine(RDK::UEngine* engine, const QString& component_long_name);

/// Child short names under a model-root dotted scope (e.g. `Model`, `Model.Firmata`). Does not use CurrentComponent.
QStringList childComponentShortNamesFromModelScope(int channel_index, const QString& scope_long_name);

/// Class name for a component path from model root (not CurrentComponent). Empty if not found.
QString componentClassNameFromModelScope(int channel_index, const QString& component_long_name);

/// Internal links XML for a schematic scope from model root. Empty if none or not a net.
std::string internalLinksXmlFromModelScope(int channel_index, const QString& scope_long_name);

struct DiagramAddComponentResult {
    bool ok = false;
    QString short_name;
    QString long_name;
};

/// Add component under a model-root parent scope with engine+storage locks held through Init/Reset.
DiagramAddComponentResult addComponentUnderModelScope(int channel_index,
                                                      const QString& parent_scope_long_name,
                                                      const QByteArray& class_name_utf8);

/// Same as addComponentUnderModelScope when engine+storage locks are already held on this thread.
DiagramAddComponentResult addComponentUnderModelScopeWithLocks(RDK::UEngine* engine,
                                                               RDK::UStorage* storage,
                                                               const QString& parent_scope_long_name,
                                                               const QByteArray& class_name_utf8);

/// Property access from model root (not FindComponent / Current). Caller should hold channel lock.
bool propertyValueFromModelScope(RDK::UContainer* model_root,
                                 const QString& component_long_name,
                                 const QByteArray& param_name,
                                 std::string& out_value);
bool setPropertyValueFromModelScope(RDK::UContainer* model_root,
                                    const QString& component_long_name,
                                    const QByteArray& param_name,
                                    const std::string& value);

/// Comma-separated "name:index" list like Model_GetComponentPropertiesLookupList.
QString propertiesLookupListFromModelScope(RDK::UContainer* model_root,
                                           const QString& component_long_name,
                                           unsigned int type_mask);

#endif
