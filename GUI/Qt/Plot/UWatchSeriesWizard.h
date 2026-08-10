#ifndef UWATCH_SERIES_WIZARD_H
#define UWATCH_SERIES_WIZARD_H

#include "PlotDocument.h"
#include "UWatchMatrixSelector.h"
#include "WatchPresetCatalog.h"

#include <QWizard>
#include <QVector>
#include <QString>

namespace RDK
{
class UApplication;
}

class UWatchChart;
class QWizardPage;

struct UWatchSeriesWizardResult
{
    enum class Mode
    {
        Manual = 0,
        Preset = 1
    } mode = Mode::Manual;

    NMSDK::Plot::VizKind viz = NMSDK::Plot::VizKind::TimeSeries;
    enum class Form
    {
        SingleCell = 0,
        MultiCells = 1,
        WholeRow = 2,
        WholeColumn = 3
    } form = Form::SingleCell;

    int channel = 0;
    QString yComponent;
    QString yProperty;
    QVector<UWatchMatrixSelector::CellRef> yCells;
    QString xComponent;
    QString xProperty;
    UWatchMatrixSelector::CellRef xCell{};

    /// Preset mode
    QString presetId;
    QString rootComponent;
    QString componentClassName;
    QVector<NMSDK::Plot::PropertyRef> presetBindings;

    QString seriesName;
    int colorIndex = -1;
    double yShift = 0.0;

    bool applyYRange = false;
    double yMin = -1.0;
    double yMax = 1.0;
    bool applyXRange = false;
    double xMin = 0.0;
    double xMax = 1.0;

    /// XY sampling (ignored for TimeSeries)
    int windowSize = 2000;
    int xyMinIntervalMs = 0;
    double xyMinDistance = 0.0;
};

class UWatchSeriesWizard : public QWizard
{
    Q_OBJECT
public:
    explicit UWatchSeriesWizard(UWatchChart* chart, RDK::UApplication* app, QWidget* parent = nullptr);

    UWatchSeriesWizardResult result() const { return m_result; }
    int applyToChart(UWatchChart* chart) const;

    NMSDK::Plot::VizKind selectedViz() const;
    UWatchSeriesWizardResult::Form selectedForm() const;
    bool isPresetMode() const;
    void captureYSourceIntoResult();
    void captureXSourceIntoResult();
    void captureSourcesIntoResult();
    void captureStyleIntoResult();
    void capturePresetIntoResult();

    UWatchChart* chart() const { return m_chart; }
    RDK::UApplication* app() const { return m_app; }

    int ySourcePageId() const { return m_ySourcePageId; }
    int xSourcePageId() const { return m_xSourcePageId; }
    int stylePageId() const { return m_stylePageId; }
    int presetComponentPageId() const { return m_presetComponentPageId; }
    int presetSelectPageId() const { return m_presetSelectPageId; }

public slots:
    void accept() override;

private:
    UWatchChart* m_chart = nullptr;
    RDK::UApplication* m_app = nullptr;
    UWatchSeriesWizardResult m_result;
    QWizardPage* m_typePage = nullptr;
    QWizardPage* m_ySourcePage = nullptr;
    QWizardPage* m_xSourcePage = nullptr;
    QWizardPage* m_stylePage = nullptr;
    QWizardPage* m_presetComponentPage = nullptr;
    QWizardPage* m_presetSelectPage = nullptr;
    int m_ySourcePageId = -1;
    int m_xSourcePageId = -1;
    int m_stylePageId = -1;
    int m_presetComponentPageId = -1;
    int m_presetSelectPageId = -1;
};

#endif // UWATCH_SERIES_WIZARD_H
