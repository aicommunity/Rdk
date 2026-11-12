#include "UWatchMatrixSelector.h"

#include "rdk.h"
#include <rdk_application.h>

#include "../../Core/Math/UWatchablePropertyTypes.h"

#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QPoint>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <functional>
#include <typeinfo>

UWatchMatrixSelector::UWatchMatrixSelector(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    m_label = new QLabel(this);
    m_table = new QTableWidget(this);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    root->addWidget(m_label);
    root->addWidget(m_table, 1);

    m_timer = new QTimer(this);
    m_timer->setInterval(300);
    connect(m_timer, &QTimer::timeout, this, &UWatchMatrixSelector::refreshValues);
    connect(m_table, &QTableWidget::itemSelectionChanged,
            this, &UWatchMatrixSelector::onSelectionChanged);

    applySelectionPolicy();
    clearBinding();
}

void UWatchMatrixSelector::setPickMode(MatrixPickMode mode)
{
    m_mode = mode;
    applySelectionPolicy();
    expandSelectionToMode();
    emit selectionChanged();
}

void UWatchMatrixSelector::applySelectionPolicy()
{
    if (!m_table)
        return;
    switch (m_mode)
    {
    case MatrixPickMode::SingleCell:
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);
        m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
        break;
    case MatrixPickMode::MultiCells:
        m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
        m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
        break;
    case MatrixPickMode::WholeRow:
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        break;
    case MatrixPickMode::WholeColumn:
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);
        m_table->setSelectionBehavior(QAbstractItemView::SelectColumns);
        break;
    }
}

void UWatchMatrixSelector::clearBinding()
{
    m_component.clear();
    m_property.clear();
    m_propertyType = 0;
    m_scalar = true;
    m_label->setText(tr("Scalar / no matrix selection needed"));
    m_table->clear();
    m_table->setRowCount(0);
    m_table->setColumnCount(0);
    m_table->hide();
    m_label->show();
    m_timer->stop();
    emit selectionChanged();
}

bool UWatchMatrixSelector::bind(const QString& component, const QString& property)
{
    RDK::UELockPtr<RDK::UNet> model = RDK::GetModelLock<RDK::UNet>();
    if (!model)
        return false;

    RDK::UEPtr<RDK::UNet> comp;
    if (component.isEmpty())
    {
        comp = RDK::GetModel<RDK::UNet>();
    }
    else
    {
        comp = model->GetComponentL<RDK::UNet>(component.toStdString(), true);
        if (!comp)
            return false;
    }

    RDK::UIProperty* prop = comp->FindProperty(property.toStdString());
    if (!prop)
        return false;

    const std::type_info& ti = prop->GetLanguageType();
    if (RDK::isScalarWatchableLanguageType(ti))
    {
        // Avoid clearBinding() here: it emits selectionChanged with empty property and
        // briefly disables QWizard Next (clicks get swallowed).
        m_timer->stop();
        m_table->hide();
        m_table->clear();
        m_table->setRowCount(0);
        m_table->setColumnCount(0);
        m_scalar = true;
        m_propertyType = 0;
        m_component = component;
        m_property = property;
        m_label->setText(tr("%1.%2 (scalar)").arg(component, property));
        m_label->show();
        emit selectionChanged();
        return true;
    }

    int type = 0;
    if (ti == typeid(RDK::MDMatrix<double>))
        type = 1;
    else if (ti == typeid(RDK::MDMatrix<int>))
        type = 2;
    else if (ti == typeid(RDK::MDVector<double>))
        type = 3;
    else if (ti == typeid(RDK::MDVector<int>))
        type = 4;
    else
        return false;

    m_scalar = false;
    m_component = component;
    m_property = property;
    m_propertyType = type;
    m_label->setText(QStringLiteral("%1:%2").arg(component, property));
    m_table->show();
    applySelectionPolicy();
    refreshValues();
    if (m_table->rowCount() > 0 && m_table->columnCount() > 0)
    {
        if (m_mode == MatrixPickMode::WholeRow)
            m_table->selectRow(0);
        else if (m_mode == MatrixPickMode::WholeColumn)
            m_table->selectColumn(0);
        else
            m_table->setCurrentCell(0, 0);
    }
    if (!m_timer->isActive())
        m_timer->start();
    emit selectionChanged();
    return true;
}

bool UWatchMatrixSelector::isReady() const
{
    if (m_property.isEmpty())
        return false;
    if (m_scalar)
        return true;
    return !selectedCells().isEmpty();
}

QVector<UWatchMatrixSelector::CellRef> UWatchMatrixSelector::selectedCells() const
{
    QVector<CellRef> out;
    if (m_scalar)
    {
        if (!m_property.isEmpty())
            out.push_back(CellRef{0, 0});
        return out;
    }
    if (!m_table)
        return out;

    QItemSelectionModel* sel = m_table->selectionModel();
    if (!sel || !sel->hasSelection())
        return out;

    const QModelIndexList ids = sel->selectedIndexes();
    if (m_mode == MatrixPickMode::WholeRow && !ids.isEmpty())
    {
        const int row = ids.first().row();
        const int cols = m_table->columnCount();
        for (int c = 0; c < cols; ++c)
            out.push_back(CellRef{row, c});
        return out;
    }
    if (m_mode == MatrixPickMode::WholeColumn && !ids.isEmpty())
    {
        const int col = ids.first().column();
        const int rows = m_table->rowCount();
        for (int r = 0; r < rows; ++r)
            out.push_back(CellRef{r, col});
        return out;
    }

    for (const QModelIndex& idx : ids)
        out.push_back(CellRef{idx.row(), idx.column()});
    return out;
}

void UWatchMatrixSelector::onSelectionChanged()
{
    expandSelectionToMode();
    emit selectionChanged();
}

void UWatchMatrixSelector::expandSelectionToMode()
{
}

void UWatchMatrixSelector::refreshValues()
{
    if (m_scalar || m_component.isEmpty() || m_property.isEmpty())
        return;
    if (m_propertyType < 1 || m_propertyType > 4)
        return;
    if (!m_table)
        return;

    // Preserve selection across refresh. Clearing the table (even briefly) makes
    // isReady()/wizard isComplete() flicker to false and QWizard Next swallows clicks.
    QVector<QPoint> savedCells;
    if (QItemSelectionModel* sel_m = m_table->selectionModel())
    {
        const QModelIndexList ids = sel_m->selectedIndexes();
        savedCells.reserve(ids.size());
        for (const QModelIndex& idx : ids)
            savedCells.push_back(QPoint(idx.column(), idx.row()));
    }

    const std::string comp = m_component.toStdString();
    const std::string prop = m_property.toStdString();

    int rows = 0;
    int cols = 0;
    std::function<QString(int, int)> at;

    switch (m_propertyType)
    {
    case 1:
    {
        RDK::MDMatrix<double> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine = RDK::GetEngineLock();
            m = engine->Model_GetComponentPropertyData<RDK::MDMatrix<double>>(comp.c_str(), prop.c_str());
        }
        rows = m.GetRows();
        cols = m.GetCols();
        at = [m](int i, int j) { return QString::number(m(i, j)); };
        break;
    }
    case 2:
    {
        RDK::MDMatrix<int> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine = RDK::GetEngineLock();
            m = engine->Model_GetComponentPropertyData<RDK::MDMatrix<int>>(comp.c_str(), prop.c_str());
        }
        rows = m.GetRows();
        cols = m.GetCols();
        at = [m](int i, int j) { return QString::number(m(i, j)); };
        break;
    }
    case 3:
    {
        RDK::MDVector<double> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine = RDK::GetEngineLock();
            m = engine->Model_GetComponentPropertyData<RDK::MDVector<double>>(comp.c_str(), prop.c_str());
        }
        rows = m.GetRows();
        cols = m.GetCols();
        at = [m](int i, int) { return QString::number(m(i)); };
        break;
    }
    case 4:
    {
        RDK::MDVector<int> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine = RDK::GetEngineLock();
            m = engine->Model_GetComponentPropertyData<RDK::MDVector<int>>(comp.c_str(), prop.c_str());
        }
        rows = m.GetRows();
        cols = m.GetCols();
        at = [m](int i, int) { return QString::number(m(i)); };
        break;
    }
    default:
        return;
    }

    const bool sameShape = (m_table->rowCount() == rows && m_table->columnCount() == cols);
    if (sameShape)
    {
        // In-place text update: selection and isReady() stay stable.
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
            {
                QTableWidgetItem* item = m_table->item(i, j);
                if (!item)
                {
                    item = new QTableWidgetItem;
                    m_table->setItem(i, j, item);
                }
                const QString text = at(i, j);
                if (item->text() != text)
                    item->setText(text);
            }
        return;
    }

    {
        QSignalBlocker tableBlocker(m_table);
        QSignalBlocker selBlocker(m_table->selectionModel());

        m_table->clearContents();
        m_table->setRowCount(rows);
        m_table->setColumnCount(cols);
        QStringList labels;
        for (int k = 0; k < rows; ++k)
            labels << QString::number(k);
        m_table->setVerticalHeaderLabels(labels);
        labels.clear();
        for (int k = 0; k < cols; ++k)
            labels << QString::number(k);
        m_table->setHorizontalHeaderLabels(labels);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                m_table->setItem(i, j, new QTableWidgetItem(at(i, j)));

        if (QItemSelectionModel* sel_m = m_table->selectionModel())
        {
            for (const QPoint& p : savedCells)
            {
                if (p.y() < rows && p.x() < cols)
                {
                    const QModelIndex temp = m_table->model()->index(p.y(), p.x());
                    sel_m->select(temp, QItemSelectionModel::Select);
                }
            }
            if (!sel_m->hasSelection() && rows > 0 && cols > 0)
            {
                if (m_mode == MatrixPickMode::WholeRow)
                    m_table->selectRow(0);
                else if (m_mode == MatrixPickMode::WholeColumn)
                    m_table->selectColumn(0);
                else
                    m_table->setCurrentCell(0, 0);
            }
        }
    }
}
