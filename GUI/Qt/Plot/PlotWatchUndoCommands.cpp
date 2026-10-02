#include "PlotWatchUndoCommands.h"

#include "../UWatchTab.h"
#include "../UWatchChart.h"
#include "../UWatchSerie.h"

WatchDeletePanelCommand::WatchDeletePanelCommand(UWatchTab* tab, int chartIndex, const QString& titleText)
    : m_tab(tab)
    , m_index(chartIndex)
    , m_title(titleText)
{
    setText(QObject::tr("Delete panel \"%1\"").arg(titleText));
    if (m_tab && m_index >= 0 && m_index < m_tab->countGraphs())
    {
        if (UWatchChart* c = m_tab->getChart(m_index))
            m_panel = c->toPlotPanel();
    }
}

void WatchDeletePanelCommand::redo()
{
    if (!m_tab || m_index < 0)
        return;
    m_tab->deletePanelImpl(m_index);
}

void WatchDeletePanelCommand::undo()
{
    if (!m_tab)
        return;
    m_tab->restorePanelSnapshot(m_index, m_panel, true);
}

WatchPanelMoveCommand::WatchPanelMoveCommand(UWatchTab* tab, int fromIndex, int toIndex)
    : m_tab(tab)
    , m_from(fromIndex)
    , m_to(toIndex)
{
    setText(QObject::tr("Reorder Watch panel"));
}

void WatchPanelMoveCommand::undo()
{
    if (m_tab)
        m_tab->movePanel(m_to, m_from);
}

void WatchPanelMoveCommand::redo()
{
    if (m_tab)
        m_tab->movePanel(m_from, m_to);
}

WatchSerieEnabledCommand::WatchSerieEnabledCommand(UWatchTab* tab,
                                                   int chartIndex,
                                                   int serieIndex,
                                                   bool enabled)
    : m_tab(tab)
    , m_chartIndex(chartIndex)
    , m_serieIndex(serieIndex)
    , m_enabled(enabled)
{
    setText(enabled ? QObject::tr("Show series") : QObject::tr("Hide series"));
}

void WatchSerieEnabledCommand::undo()
{
    if (!m_tab)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        c->setSerieEnabled(m_serieIndex, !m_enabled);
        m_tab->syncDocumentFromCharts();
        m_tab->refreshInspectorIfOpen();
    }
}

void WatchSerieEnabledCommand::redo()
{
    if (!m_tab)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        c->setSerieEnabled(m_serieIndex, m_enabled);
        m_tab->syncDocumentFromCharts();
        m_tab->refreshInspectorIfOpen();
    }
}

WatchSerieDeleteCommand::WatchSerieDeleteCommand(UWatchTab* tab, int chartIndex, int serieIndex)
    : m_tab(tab)
    , m_chartIndex(chartIndex)
    , m_serieIndex(serieIndex)
{
    setText(QObject::tr("Delete series"));
    if (m_tab)
    {
        if (UWatchChart* c = m_tab->getChart(m_chartIndex))
        {
            if (UWatchSerie* s = c->getSerie(m_serieIndex))
            {
                m_serie = s->toPlotSeries();
                m_viz = s->vizKind;
            }
        }
    }
}

void WatchSerieDeleteCommand::redo()
{
    if (!m_tab)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        if (m_serieIndex >= 0 && m_serieIndex < c->countSeries())
        {
            c->deleteSerie(m_serieIndex);
            m_tab->syncDocumentFromCharts();
            m_tab->refreshInspectorIfOpen();
        }
    }
}

void WatchSerieDeleteCommand::undo()
{
    if (!m_tab)
        return;
    UWatchChart* c = m_tab->getChart(m_chartIndex);
    if (!c)
        return;

    const bool serieIsXY = m_serie.binding.x.kind == NMSDK::Plot::DataRoleKind::Property;
    if (serieIsXY)
    {
        c->createSerieXY(m_serie.binding.channel,
                         m_serie.binding.x.prop.component, m_serie.binding.x.prop.property,
                         m_serie.binding.x.prop.jx, m_serie.binding.x.prop.jy,
                         m_serie.binding.y.prop.component, m_serie.binding.y.prop.property,
                         m_serie.binding.y.prop.jx, m_serie.binding.y.prop.jy,
                         m_serie.yOffset, m_viz,
                         m_serie.binding.x.prop.slice, m_serie.binding.y.prop.slice);
    }
    else
    {
        c->createSerie(m_serie.binding.channel,
                       m_serie.binding.y.prop.component,
                       m_serie.binding.y.prop.property, QString(),
                       m_serie.binding.y.prop.jx < 0 ? 0 : m_serie.binding.y.prop.jx,
                       m_serie.binding.y.prop.jy < 0 ? 0 : m_serie.binding.y.prop.jy,
                       c->getAxisXrange(), m_serie.yOffset);
    }
    const int idx = c->countSeries() - 1;
    if (idx < 0)
        return;
    if (c->getSerie(idx))
        c->getSerie(idx)->setPlotSeriesId(m_serie.id);
    if (!m_serie.visual.displayName.isEmpty())
        c->setSerieName(idx, m_serie.visual.displayName);
    c->setSerieWidth(idx, m_serie.visual.width);
    c->setSerieLineType(idx, static_cast<Qt::PenStyle>(m_serie.visual.penStyle));
    if (c->getSerie(idx))
        c->getSerie(idx)->setColor(m_serie.visual.color);
    c->setSerieEnabled(idx, m_serie.enabled);
    if (m_serieIndex >= 0 && m_serieIndex < idx)
        c->moveSerie(idx, m_serieIndex);
    m_tab->syncDocumentFromCharts();
    m_tab->refreshInspectorIfOpen();
}

WatchSerieMoveCommand::WatchSerieMoveCommand(UWatchTab* tab, int chartIndex, int fromIndex, int toIndex)
    : m_tab(tab)
    , m_chartIndex(chartIndex)
    , m_from(fromIndex)
    , m_to(toIndex)
{
    setText(QObject::tr("Reorder series"));
}

void WatchSerieMoveCommand::undo()
{
    if (!m_tab)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        c->moveSerie(m_to, m_from);
        m_tab->syncDocumentFromCharts();
        m_tab->refreshInspectorIfOpen();
    }
}

void WatchSerieMoveCommand::redo()
{
    if (!m_tab)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        c->moveSerie(m_from, m_to);
        m_tab->syncDocumentFromCharts();
        m_tab->refreshInspectorIfOpen();
    }
}

WatchSerieDuplicateCommand::WatchSerieDuplicateCommand(UWatchTab* tab, int chartIndex, int serieIndex)
    : m_tab(tab)
    , m_chartIndex(chartIndex)
    , m_serieIndex(serieIndex)
{
    setText(QObject::tr("Duplicate series"));
}

void WatchSerieDuplicateCommand::redo()
{
    if (!m_tab)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        m_createdIndex = c->duplicateSerie(m_serieIndex);
        if (UWatchSerie* created = c->getSerie(m_createdIndex))
        {
            if (m_createdSerieId.isEmpty())
                m_createdSerieId = created->plotSeriesId();
            else
                created->setPlotSeriesId(m_createdSerieId);
        }
        m_tab->syncDocumentFromCharts();
        m_tab->refreshInspectorIfOpen();
    }
}

void WatchSerieDuplicateCommand::undo()
{
    if (!m_tab || m_createdIndex < 0)
        return;
    if (UWatchChart* c = m_tab->getChart(m_chartIndex))
    {
        if (m_createdIndex < c->countSeries())
            c->deleteSerie(m_createdIndex);
        m_createdIndex = -1;
        m_tab->syncDocumentFromCharts();
        m_tab->refreshInspectorIfOpen();
    }
}
