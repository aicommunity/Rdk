#ifndef UWATCH_SOURCE_PICKER_WIDGET_H
#define UWATCH_SOURCE_PICKER_WIDGET_H

#include "UWatchMatrixSelector.h"

#include <QWidget>
#include <QVector>
#include <QString>

namespace RDK
{
class UApplication;
}

class UComponentsListWidgetModern;
class QLabel;
class QSplitter;
class QVBoxLayout;

class UWatchSourcePickerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UWatchSourcePickerWidget(QWidget* parent = nullptr);

    void configureForWatch(RDK::UApplication* app, bool showChannels);

    /// Hide property binding readout and matrix — component tree only (Preset wizard page).
    void setComponentOnlyMode(bool enabled);

    /// Manual TimeSeries: compact bottom readout, matrix only when non-scalar.
    void setTimeSeriesCompactMode(bool enabled);

    int channelIndex() const;
    QString componentLongName() const;
    QString propertyName() const;
    QVector<UWatchMatrixSelector::CellRef> cells() const;
    bool isComplete() const;

    void setMatrixPickMode(MatrixPickMode mode);
    UWatchMatrixSelector* matrixSelector() const { return m_matrix; }

signals:
    void selectionChanged();

private slots:
    void onListSelectionChanged();

private:
    void updateBindingFromList();
    void updateReadout();
    void applyLayoutModes();

    UComponentsListWidgetModern* m_list = nullptr;
    UWatchMatrixSelector* m_matrix = nullptr;
    QLabel* m_bindingReadout = nullptr;
    QSplitter* m_split = nullptr;
    QVBoxLayout* m_rootLayout = nullptr;
    bool m_componentOnly = false;
    bool m_timeSeriesCompact = false;
};

#endif // UWATCH_SOURCE_PICKER_WIDGET_H
