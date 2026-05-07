#ifndef UCOMPONENTGUIFLOATINGFRAME_H
#define UCOMPONENTGUIFLOATINGFRAME_H

#include <QWidget>

#include "UComponentGuiContext.h"

class UVisualControllerWidget;
class QWidget;
class UComponentGuiService;

class UComponentGuiFloatingFrame : public QWidget
{
    Q_OBJECT
public:
    UComponentGuiFloatingFrame(const UComponentGuiContext& context,
                                UVisualControllerWidget* contentWidget,
                                UComponentGuiService* service,
                                QWidget* parent = nullptr);

    QWidget* contentContainer() const;

private:
    Q_DISABLE_COPY(UComponentGuiFloatingFrame)

    class UDockDragBar;

private:
    UComponentGuiContext m_context;
    UComponentGuiService* m_service = nullptr;
    QWidget* m_topBar = nullptr;
    QWidget* m_contentContainer = nullptr;
    UDockDragBar* m_dragBar = nullptr;

private:
    void startDrag();
};

#endif // UCOMPONENTGUIFLOATINGFRAME_H

