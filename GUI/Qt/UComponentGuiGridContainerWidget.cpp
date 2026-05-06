#include "UComponentGuiGridContainerWidget.h"

#include <QVBoxLayout>
#include <QFrame>

#include "UComponentGuiService.h"

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
    qSwap(m_cells[r1][c1], m_cells[r2][c2]);

    if(m_cells[r1][c1].hasContext)
        assignCell(r1, c1, m_cells[r1][c1].context);
    else
        clearCell(r1, c1);

    if(m_cells[r2][c2].hasContext)
        assignCell(r2, c2, m_cells[r2][c2].context);
    else
        clearCell(r2, c2);
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
            QFrame* cellFrame = new QFrame(rowSplitter);
            cellFrame->setFrameShape(QFrame::StyledPanel);
            cellFrame->setContentsMargins(0, 0, 0, 0);
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
