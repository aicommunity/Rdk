#include "UComponentGuiGridContainerWidget.h"
#include "UComponentGuiDndPayload.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QDrag>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

#include "UComponentGuiService.h"
#include "UVisualControllerWidget.h"

namespace
{
class UGridDropCellFrame final : public QFrame
{
public:
    UGridDropCellFrame(UComponentGuiGridContainerWidget* owner, int row, int col, QWidget* parent = nullptr)
        : QFrame(parent), m_owner(owner), m_row(row), m_col(col)
    {
        setAcceptDrops(true);
        setFrameShape(QFrame::StyledPanel);
        setContentsMargins(0, 0, 0, 0);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        if(event->button() == Qt::LeftButton)
            m_dragStartPos = event->pos();
        QFrame::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        if(!(event->buttons() & Qt::LeftButton) || !m_owner)
            return QFrame::mouseMoveEvent(event);
        if((event->pos() - m_dragStartPos).manhattanLength() < QApplication::startDragDistance())
            return QFrame::mouseMoveEvent(event);
        m_owner->startDragFromCell(m_row, m_col);
        QFrame::mouseMoveEvent(event);
    }

    void dragEnterEvent(QDragEnterEvent* event) override
    {
        if(event->mimeData() && event->mimeData()->hasFormat(UComponentGuiDndPayload::mimeType()))
        {
            qInfo() << "[ComponentGuiDnD][Grid] dragEnter cell=" << m_row << m_col;
            event->acceptProposedAction();
        }
        else
            event->ignore();
    }

    void dragMoveEvent(QDragMoveEvent* event) override
    {
        if(event->mimeData() && event->mimeData()->hasFormat(UComponentGuiDndPayload::mimeType()))
        {
            qInfo() << "[ComponentGuiDnD][Grid] dragMove cell=" << m_row << m_col;
            event->acceptProposedAction();
        }
        else
            event->ignore();
    }

    void dropEvent(QDropEvent* event) override
    {
        if(m_owner && m_owner->handleDropToCell(m_row, m_col, event->mimeData()))
        {
            qInfo() << "[ComponentGuiDnD][Grid] dropAccepted cell=" << m_row << m_col;
            event->acceptProposedAction();
        }
        else
        {
            qWarning() << "[ComponentGuiDnD][Grid] dropRejected cell=" << m_row << m_col;
            event->ignore();
        }
    }

private:
    UComponentGuiGridContainerWidget* m_owner = nullptr;
    int m_row = -1;
    int m_col = -1;
    QPoint m_dragStartPos;
};
}

UComponentGuiGridContainerWidget::UComponentGuiGridContainerWidget(const QString& gridId,
                                                                   UComponentGuiService* service,
                                                                   QWidget *parent,
                                                                   RDK::UApplication *app)
    : UVisualControllerWidget(parent, app),
      m_gridId(gridId),
      m_service(service)
{
    setAccessibleName(QStringLiteral("ComponentGuiGrid_%1").arg(gridId));
    setWindowTitle(QStringLiteral("Component GUI Grid [%1]").arg(gridId));
    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(0, 0, 0, 0);
    m_rootLayout->setSpacing(2);
    rebuildGrid();
}

QString UComponentGuiGridContainerWidget::gridId() const
{
    return m_gridId;
}

void UComponentGuiGridContainerWidget::setGridSize(int rows, int cols)
{
    m_rows = qMax(1, rows);
    m_cols = qMax(1, cols);
    rebuildGrid();
}

int UComponentGuiGridContainerWidget::rowCount() const
{
    return m_rows;
}

int UComponentGuiGridContainerWidget::colCount() const
{
    return m_cols;
}

bool UComponentGuiGridContainerWidget::assignCell(int row, int col, const UComponentGuiContext& context)
{
    if(!isValidCell(row, col))
        return false;

    QWidget* host = cellHostWidget(row, col);
    if(!host || !m_service)
        return false;

    UVisualControllerWidget* widget = m_service->createOrActivate(host, context);
    if(!widget)
        return false;

    if(widget->parentWidget() != host)
    {
        if(!m_service->moveToGridCell(context, m_gridId, row, col, host))
            return false;
    }
    else
    {
        widget->setGeometry(host->rect());
        widget->show();
    }

    m_cells[row][col].hasContext = true;
    m_cells[row][col].context = context;
    return true;
}

bool UComponentGuiGridContainerWidget::clearCell(int row, int col)
{
    if(!isValidCell(row, col))
        return false;

    QWidget* host = cellHostWidget(row, col);
    const UComponentGuiContext oldContext = m_cells[row][col].context;
    const bool hadContext = m_cells[row][col].hasContext;

    // If the cell currently owns a context, move it out through service transition.
    if(hadContext && m_service)
    {
        UComponentGuiHostMode mode = UComponentGuiHostMode::Mdi;
        UVisualControllerWidget* widget = nullptr;
        const bool hasLiveWidget = m_service->tryGetWidgetByContext(oldContext, widget) && widget;
        const bool hasMode = m_service->tryGetHostModeByContext(oldContext, mode);
        if(hasLiveWidget && hasMode && mode == UComponentGuiHostMode::Grid && widget->parentWidget() == host)
            m_service->detachToFloating(oldContext);
    }

    if(host)
    {
        const QList<QWidget*> children = host->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        for(QWidget* child : children)
            child->hide();
    }
    m_cells[row][col].hasContext = false;
    m_cells[row][col].context = UComponentGuiContext();
    return true;
}

bool UComponentGuiGridContainerWidget::swapCells(int r1, int c1, int r2, int c2)
{
    if(!isValidCell(r1, c1) || !isValidCell(r2, c2))
        return false;

    const bool firstHas = m_cells[r1][c1].hasContext;
    const bool secondHas = m_cells[r2][c2].hasContext;
    const UComponentGuiContext firstContext = m_cells[r1][c1].context;
    const UComponentGuiContext secondContext = m_cells[r2][c2].context;

    // Important: never swap host widgets, only context occupancy metadata.
    m_cells[r1][c1].hasContext = secondHas;
    m_cells[r1][c1].context = secondContext;
    m_cells[r2][c2].hasContext = firstHas;
    m_cells[r2][c2].context = firstContext;

    if(secondHas)
    {
        if(!assignCell(r1, c1, secondContext))
            return false;
    }
    else if(QWidget* host = cellHostWidget(r1, c1))
    {
        const QList<QWidget*> children = host->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        for(QWidget* child : children)
            child->hide();
    }

    if(firstHas)
    {
        if(!assignCell(r2, c2, firstContext))
            return false;
    }
    else if(QWidget* host = cellHostWidget(r2, c2))
    {
        const QList<QWidget*> children = host->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        for(QWidget* child : children)
            child->hide();
    }

    return true;
}

bool UComponentGuiGridContainerWidget::hasContext(int row, int col) const
{
    return isValidCell(row, col) && m_cells[row][col].hasContext;
}

UComponentGuiContext UComponentGuiGridContainerWidget::contextAt(int row, int col) const
{
    if(!isValidCell(row, col))
        return UComponentGuiContext();
    return m_cells[row][col].context;
}

QByteArray UComponentGuiGridContainerWidget::saveGridState() const
{
    return m_rootSplitter ? m_rootSplitter->saveState() : QByteArray();
}

void UComponentGuiGridContainerWidget::restoreGridState(const QByteArray& state)
{
    if(m_rootSplitter && !state.isEmpty())
        m_rootSplitter->restoreState(state);
}

QByteArray UComponentGuiGridContainerWidget::saveRowState(int row) const
{
    if(row < 0 || row >= m_rowSplitters.size() || !m_rowSplitters[row])
        return QByteArray();
    return m_rowSplitters[row]->saveState();
}

void UComponentGuiGridContainerWidget::restoreRowState(int row, const QByteArray& state)
{
    if(row < 0 || row >= m_rowSplitters.size() || !m_rowSplitters[row] || state.isEmpty())
        return;
    m_rowSplitters[row]->restoreState(state);
}

QWidget* UComponentGuiGridContainerWidget::cellHostWidget(int row, int col) const
{
    if(!isValidCell(row, col))
        return nullptr;
    return m_cells[row][col].host.data();
}

bool UComponentGuiGridContainerWidget::isValidCell(int row, int col) const
{
    return row >= 0 && row < m_cells.size() && col >= 0 && col < m_cells[row].size();
}

void UComponentGuiGridContainerWidget::startDragFromCell(int row, int col)
{
    if(!isValidCell(row, col) || !m_cells[row][col].hasContext)
        return;
    qInfo() << "[ComponentGuiDnD][Grid] startDragFromCell source="
            << m_gridId << row << col
            << "context=" << m_cells[row][col].context.componentClassName
            << m_cells[row][col].context.componentLongName
            << "channel=" << m_cells[row][col].context.channelIndex;
    QDrag* drag = new QDrag(this);
    QMimeData* mime = new QMimeData();
    mime->setData(UComponentGuiDndPayload::mimeType(),
                  UComponentGuiDndPayload::encode(m_cells[row][col].context, m_gridId, row, col));
    drag->setMimeData(mime);
    const Qt::DropAction result = drag->exec(Qt::MoveAction);
    qInfo() << "[ComponentGuiDnD][Grid] dragFinished source=" << m_gridId << row << col << "result=" << result;
}

bool UComponentGuiGridContainerWidget::handleDropToCell(int row, int col, const QMimeData* mimeData)
{
    if(!isValidCell(row, col))
        return false;
    UComponentGuiContext context;
    QString sourceGridId;
    int sourceRow = -1;
    int sourceCol = -1;
    if(!UComponentGuiDndPayload::decode(mimeData, context, sourceGridId, sourceRow, sourceCol))
    {
        qWarning() << "[ComponentGuiDnD][Grid] decodeFailed targetCell=" << row << col;
        return false;
    }

    qInfo() << "[ComponentGuiDnD][Grid] decoded targetCell=" << row << col
            << "sourceGrid=" << sourceGridId << "sourceCell=" << sourceRow << sourceCol
            << "context=" << context.componentClassName << context.componentLongName
            << "channel=" << context.channelIndex;

    if(sourceGridId == m_gridId && sourceRow >= 0 && sourceCol >= 0 && isValidCell(sourceRow, sourceCol))
    {
        if(sourceRow == row && sourceCol == col)
        {
            qInfo() << "[ComponentGuiDnD][Grid] noOp sameSourceAndTarget cell=" << row << col;
            return true;
        }
        if(m_cells[row][col].hasContext)
        {
            const bool swapped = swapCells(sourceRow, sourceCol, row, col);
            qInfo() << "[ComponentGuiDnD][Grid] swapCells result=" << swapped
                    << "from=" << sourceRow << sourceCol << "to=" << row << col;
            return swapped;
        }
        const bool assigned = assignCell(row, col, context);
        if(assigned)
            clearCell(sourceRow, sourceCol);
        qInfo() << "[ComponentGuiDnD][Grid] moveWithinGrid assigned=" << assigned
                << "from=" << sourceRow << sourceCol << "to=" << row << col;
        return assigned;
    }

    if(m_cells[row][col].hasContext)
        clearCell(row, col);
    const bool assigned = assignCell(row, col, context);
    qInfo() << "[ComponentGuiDnD][Grid] externalDrop assigned=" << assigned
            << "targetCell=" << row << col;
    return assigned;
}

void UComponentGuiGridContainerWidget::rebuildGrid()
{
    clearGridWidgets();
    m_cells = QVector<QVector<UGridCell>>(m_rows, QVector<UGridCell>(m_cols));
    m_rowSplitters.clear();

    m_rootSplitter = new QSplitter(Qt::Vertical, this);
    for(int r = 0; r < m_rows; ++r)
    {
        QSplitter* rowSplitter = new QSplitter(Qt::Horizontal, m_rootSplitter);
        m_rowSplitters.push_back(rowSplitter);
        for(int c = 0; c < m_cols; ++c)
        {
            QFrame* cellFrame = new UGridDropCellFrame(this, r, c, rowSplitter);
            m_cells[r][c].host = cellFrame;
        }
    }
    m_rootLayout->addWidget(m_rootSplitter);
}

void UComponentGuiGridContainerWidget::clearGridWidgets()
{
    if(!m_rootLayout)
        return;
    while(QLayoutItem* item = m_rootLayout->takeAt(0))
    {
        if(item->widget())
            item->widget()->deleteLater();
        delete item;
    }
    m_rootSplitter = nullptr;
}
