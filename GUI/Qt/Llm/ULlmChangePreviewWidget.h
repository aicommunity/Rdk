#ifndef RDK_ULLM_CHANGE_PREVIEW_WIDGET_H
#define RDK_ULLM_CHANGE_PREVIEW_WIDGET_H

#include <QTextEdit>

#include "../UVisualControllerWidget.h"

class ULlmChangePreviewWidget : public UVisualControllerWidget {
    Q_OBJECT
public:
    ULlmChangePreviewWidget(QWidget* parent, RDK::UApplication* app);

    void showPlan(const QString& summary);

private:
    QTextEdit* m_view = nullptr;
};

#endif
