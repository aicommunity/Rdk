#ifndef UWATCH_SERIES_WIZARD_H
#define UWATCH_SERIES_WIZARD_H

#include "PlotDocument.h"
#include "UWatchMatrixSelector.h"

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

    QString seriesName;
    int colorIndex = -1;
    double yShift = 0.0;

    bool applyYRange = false;
    double yMin = -1.0;
    double yMax = 1.0;
    bool applyXRange = false;
    double xMin = 0.0;
    double xMax = 1.0;
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
    void captureSourcesIntoResult();
    void captureStyleIntoResult();

    UWatchChart* chart() const { return m_chart; }
    RDK::UApplication* app() const { return m_app; }

public slots:
    void accept() override;

private:
    UWatchChart* m_chart = nullptr;
    RDK::UApplication* m_app = nullptr;
    UWatchSeriesWizardResult m_result;
    QWizardPage* m_typePage = nullptr;
    QWizardPage* m_sourcesPage = nullptr;
    QWizardPage* m_stylePage = nullptr;
};

#endif // UWATCH_SERIES_WIZARD_H
