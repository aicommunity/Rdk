#include "UComponentGuiFloatingFrame.h"

#include <QDrag>
#include <QMimeData>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>

#include "UComponentGuiDndPayload.h"
#include "UComponentGuiService.h"

// Forward declared in header
#include "UVisualControllerWidget.h"

class UComponentGuiFloatingFrame::UDockDragBar : public QWidget
{
public:
    explicit UDockDragBar(UComponentGuiFloatingFrame* owner)
        : QWidget(owner)
        , m_owner(owner)
    {
        setCursor(Qt::OpenHandCursor);
        setMinimumHeight(24);
        setMaximumHeight(24);
        setStyleSheet(QStringLiteral("background: palette(window); border-bottom: 1px solid rgba(128,128,128,0.4);"));
    }

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        if(event->button() == Qt::LeftButton)
        {
            m_pressPos = event->pos();
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        if(!(event->buttons() & Qt::LeftButton))
            return;
        if(m_contextualDragStarted)
            return;

        constexpr int dragThresholdPx = 4;
        if((event->pos() - m_pressPos).manhattanLength() < dragThresholdPx)
            return;

        m_contextualDragStarted = true;
        m_owner->startDrag();
        m_contextualDragStarted = false;
    }

private:
    UComponentGuiFloatingFrame* m_owner = nullptr;
    QPoint m_pressPos;
    bool m_contextualDragStarted = false;
};

UComponentGuiFloatingFrame::UComponentGuiFloatingFrame(const UComponentGuiContext& context,
                                                         UVisualControllerWidget* contentWidget,
                                                         UComponentGuiService* service,
                                                         QWidget* parent)
    : QWidget(parent)
    , m_context(context)
    , m_service(service)
{
    // Reliable Qt DnD requires app-controlled drag surface instead of native title-bar move.
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setObjectName(QStringLiteral("UComponentGuiFloatingFrame"));
    setStyleSheet(QStringLiteral("#UComponentGuiFloatingFrame { border: 1px solid rgba(255,255,255,0.15); }"));

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    m_topBar = new QWidget(this);
    auto* topBarLayout = new QHBoxLayout(m_topBar);
    topBarLayout->setContentsMargins(6, 2, 6, 2);
    topBarLayout->setSpacing(6);

    m_dragBar = new UDockDragBar(this);
    auto* titleLabel = new QLabel(tr("Drag to MDI/Grid"), m_dragBar);
    titleLabel->setStyleSheet(QStringLiteral("background: transparent;"));
    auto* dragBarLayout = new QHBoxLayout(m_dragBar);
    dragBarLayout->setContentsMargins(8, 0, 8, 0);
    dragBarLayout->addWidget(titleLabel);
    dragBarLayout->addStretch(1);
    topBarLayout->addWidget(m_dragBar, 1);

    auto* closeButton = new QPushButton(QStringLiteral("x"), m_topBar);
    closeButton->setFixedSize(18, 18);
    closeButton->setStyleSheet(QStringLiteral("QPushButton{border:1px solid rgba(128,128,128,0.6);border-radius:2px;}"));
    QObject::connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    topBarLayout->addWidget(closeButton, 0, Qt::AlignRight);

    rootLayout->addWidget(m_topBar, 0);

    m_contentContainer = new QWidget(this);
    auto* contentLayout = new QVBoxLayout(m_contentContainer);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    rootLayout->addWidget(m_contentContainer, 1);

    if(contentWidget)
    {
        contentLayout->addWidget(contentWidget);
        contentWidget->setWindowFlags(Qt::Widget);
        contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        contentWidget->show();
    }
}

QWidget* UComponentGuiFloatingFrame::contentContainer() const
{
    return m_contentContainer;
}

void UComponentGuiFloatingFrame::startDrag()
{
    qInfo() << "[ComponentGuiDnD][Floating] startDrag context="
            << m_context.componentClassName
            << m_context.componentLongName
            << "channel=" << m_context.channelIndex;

    auto* drag = new QDrag(m_dragBar ? static_cast<QWidget*>(m_dragBar) : this);
    auto* mime = new QMimeData();
    mime->setData(UComponentGuiDndPayload::mimeType(), UComponentGuiDndPayload::encode(m_context));
    drag->setMimeData(mime);

    const Qt::DropAction result = drag->exec(Qt::MoveAction);
    qInfo() << "[ComponentGuiDnD][Floating] dragFinished result=" << result;
}

