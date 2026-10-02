#ifndef PLOT_WATCH_UNDO_COMMANDS_H
#define PLOT_WATCH_UNDO_COMMANDS_H

#include "PlotDocument.h"

#include <QUndoCommand>

class UWatchTab;

class WatchDeletePanelCommand : public QUndoCommand
{
public:
    WatchDeletePanelCommand(UWatchTab* tab, int chartIndex, const QString& titleText);

    void undo() override;
    void redo() override;

private:
    UWatchTab* m_tab = nullptr;
    int m_index = -1;
    NMSDK::Plot::PlotPanel m_panel;
    QString m_title;
};

class WatchPanelMoveCommand : public QUndoCommand
{
public:
    WatchPanelMoveCommand(UWatchTab* tab, int fromIndex, int toIndex);

    void undo() override;
    void redo() override;

private:
    UWatchTab* m_tab = nullptr;
    int m_from = -1;
    int m_to = -1;
};

class WatchSerieEnabledCommand : public QUndoCommand
{
public:
    WatchSerieEnabledCommand(UWatchTab* tab, int chartIndex, int serieIndex, bool enabled);

    void undo() override;
    void redo() override;

private:
    UWatchTab* m_tab = nullptr;
    int m_chartIndex = -1;
    int m_serieIndex = -1;
    bool m_enabled = true;
};

class WatchSerieDeleteCommand : public QUndoCommand
{
public:
    WatchSerieDeleteCommand(UWatchTab* tab, int chartIndex, int serieIndex);

    void undo() override;
    void redo() override;

private:
    UWatchTab* m_tab = nullptr;
    int m_chartIndex = -1;
    int m_serieIndex = -1;
    NMSDK::Plot::PlotSeries m_serie;
    NMSDK::Plot::VizKind m_viz = NMSDK::Plot::VizKind::TimeSeries;
};

class WatchSerieMoveCommand : public QUndoCommand
{
public:
    WatchSerieMoveCommand(UWatchTab* tab, int chartIndex, int fromIndex, int toIndex);

    void undo() override;
    void redo() override;

private:
    UWatchTab* m_tab = nullptr;
    int m_chartIndex = -1;
    int m_from = -1;
    int m_to = -1;
};

class WatchSerieDuplicateCommand : public QUndoCommand
{
public:
    WatchSerieDuplicateCommand(UWatchTab* tab, int chartIndex, int serieIndex);

    void undo() override;
    void redo() override;

private:
    UWatchTab* m_tab = nullptr;
    int m_chartIndex = -1;
    int m_serieIndex = -1;
    int m_createdIndex = -1;
    QString m_createdSerieId;
};

#endif // PLOT_WATCH_UNDO_COMMANDS_H
