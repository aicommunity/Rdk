#ifndef RDK_LLM_GUI_TYPES_H
#define RDK_LLM_GUI_TYPES_H

#include <QRectF>
#include <QString>

namespace RDK {
class UApplication;
}

struct LLMGuiContext {
    RDK::UApplication* application = nullptr;
    int channel_index = 0;
    QString project_xml_path;
    QString focused_component_long_name;
    QString focused_class_name;
    QRectF diagram_viewport_scene_rect;
    qint64 snapshot_fingerprint = 0;
};

#endif
