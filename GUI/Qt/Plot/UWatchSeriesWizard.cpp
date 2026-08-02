#include "UWatchSeriesWizard.h"
#include "UWatchSourcePickerWidget.h"

#include "../UWatchChart.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWizardPage>

namespace watch_wizard {

MatrixPickMode formToPickMode(UWatchSeriesWizardResult::Form form)
{
    switch (form)
    {
    case UWatchSeriesWizardResult::Form::MultiCells:
        return MatrixPickMode::MultiCells;
    case UWatchSeriesWizardResult::Form::WholeRow:
        return MatrixPickMode::WholeRow;
    case UWatchSeriesWizardResult::Form::WholeColumn:
        return MatrixPickMode::WholeColumn;
    case UWatchSeriesWizardResult::Form::SingleCell:
    default:
        return MatrixPickMode::SingleCell;
    }
}

class TypeFormPage : public QWizardPage
{
public:
    explicit TypeFormPage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Chart type"));
        setSubTitle(QObject::tr("Choose visualization kind and how matrix sources are expanded into series."));

        auto* layout = new QFormLayout(this);
        m_viz = new QComboBox(this);
        m_viz->addItem(QObject::tr("Time series"), static_cast<int>(NMSDK::Plot::VizKind::TimeSeries));
        m_viz->addItem(QObject::tr("XY line"), static_cast<int>(NMSDK::Plot::VizKind::XYLine));
        m_viz->addItem(QObject::tr("XY scatter"), static_cast<int>(NMSDK::Plot::VizKind::XYScatter));
        layout->addRow(QObject::tr("Kind"), m_viz);

        m_form = new QComboBox(this);
        layout->addRow(QObject::tr("Source form"), m_form);

        m_hint = new QLabel(this);
        m_hint->setWordWrap(true);
        layout->addRow(m_hint);

        connect(m_viz, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
            rebuildFormItems();
        });

        if (wizard && wizard->chart())
        {
            const int viz = static_cast<int>(wizard->chart()->getVizKind());
            const int idx = m_viz->findData(viz);
            if (idx >= 0)
                m_viz->setCurrentIndex(idx);
        }
        rebuildFormItems();
    }

    NMSDK::Plot::VizKind viz() const
    {
        return static_cast<NMSDK::Plot::VizKind>(m_viz->currentData().toInt());
    }

    UWatchSeriesWizardResult::Form form() const
    {
        return static_cast<UWatchSeriesWizardResult::Form>(m_form->currentData().toInt());
    }

    bool isComplete() const override { return true; }

private:
    void rebuildFormItems()
    {
        m_form->clear();
        const auto v = viz();
        if (v == NMSDK::Plot::VizKind::TimeSeries)
        {
            m_form->addItem(QObject::tr("Single cell / scalar"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::SingleCell));
            m_form->addItem(QObject::tr("Multi cells → multiple series"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::MultiCells));
            m_form->addItem(QObject::tr("Whole row → multiple series"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::WholeRow));
            m_form->addItem(QObject::tr("Whole column → multiple series"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::WholeColumn));
            m_form->setEnabled(true);
            m_hint->setText(QObject::tr("Y(t): one property vs time. Matrix forms can create several series."));
        }
        else
        {
            m_form->addItem(QObject::tr("Single cell / scalar pair"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::SingleCell));
            m_form->setEnabled(false);
            m_hint->setText(QObject::tr("X–Y same-tick pair: pick Y and X sources on the next page."));
        }
    }

    UWatchSeriesWizard* m_wizard = nullptr;
    QComboBox* m_viz = nullptr;
    QComboBox* m_form = nullptr;
    QLabel* m_hint = nullptr;
};

class SourcesPage : public QWizardPage
{
public:
    explicit SourcesPage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Data sources"));
        setSubTitle(QObject::tr("Select component properties. Matrix cells are chosen inline."));

        auto* root = new QVBoxLayout(this);
        m_yLabel = new QLabel(QObject::tr("Y source"), this);
        m_yPicker = new UWatchSourcePickerWidget(this);
        m_xLabel = new QLabel(QObject::tr("X source"), this);
        m_xPicker = new UWatchSourcePickerWidget(this);

        root->addWidget(m_yLabel);
        root->addWidget(m_yPicker, 1);
        root->addWidget(m_xLabel);
        root->addWidget(m_xPicker, 1);

        if (wizard)
        {
            m_yPicker->configureForWatch(wizard->app(), true);
            m_xPicker->configureForWatch(wizard->app(), false);
        }

        connect(m_yPicker, &UWatchSourcePickerWidget::selectionChanged, this, [this]() {
            emit completeChanged();
        });
        connect(m_xPicker, &UWatchSourcePickerWidget::selectionChanged, this, [this]() {
            emit completeChanged();
        });
    }

    void initializePage() override
    {
        const auto viz = m_wizard->selectedViz();
        const auto form = m_wizard->selectedForm();
        const bool xy = (viz != NMSDK::Plot::VizKind::TimeSeries);
        m_xLabel->setVisible(xy);
        m_xPicker->setVisible(xy);
        m_yLabel->setText(xy ? QObject::tr("Y source") : QObject::tr("Y(t) source"));

        m_yPicker->setMatrixPickMode(formToPickMode(form));
        m_xPicker->setMatrixPickMode(MatrixPickMode::SingleCell);
        emit completeChanged();
    }

    bool isComplete() const override
    {
        if (!m_yPicker || !m_yPicker->isComplete())
            return false;
        const auto viz = m_wizard->selectedViz();
        if (viz == NMSDK::Plot::VizKind::TimeSeries)
            return true;
        if (!m_xPicker || !m_xPicker->isComplete())
            return false;
        return m_yPicker->cells().size() == 1;
    }

    bool validatePage() override
    {
        m_wizard->captureSourcesIntoResult();
        return true;
    }

    UWatchSourcePickerWidget* yPicker() const { return m_yPicker; }
    UWatchSourcePickerWidget* xPicker() const { return m_xPicker; }

private:
    UWatchSeriesWizard* m_wizard = nullptr;
    QLabel* m_yLabel = nullptr;
    QLabel* m_xLabel = nullptr;
    UWatchSourcePickerWidget* m_yPicker = nullptr;
    UWatchSourcePickerWidget* m_xPicker = nullptr;
};

class StylePage : public QWizardPage
{
public:
    explicit StylePage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Series style"));
        setSubTitle(QObject::tr("Optional. Leave name empty to use the automatic series title."));

        auto* form = new QFormLayout(this);
        m_name = new QLineEdit(this);
        m_name->setPlaceholderText(QObject::tr("Auto"));
        m_color = new QComboBox(this);
        m_color->addItem(QObject::tr("Auto (next palette color)"), -1);
        if (wizard && wizard->chart())
        {
            for (int i = 0; i < 12; ++i)
            {
                const QColor c = wizard->chart()->getDefaultColor(i);
                m_color->addItem(QObject::tr("Palette %1").arg(i + 1), i);
                m_color->setItemData(m_color->count() - 1, c, Qt::DecorationRole);
            }
        }
        m_yShift = new QDoubleSpinBox(this);
        m_yShift->setRange(-1e9, 1e9);
        m_yShift->setDecimals(3);
        form->addRow(QObject::tr("Series name"), m_name);
        form->addRow(QObject::tr("Color"), m_color);
        form->addRow(QObject::tr("Y shift"), m_yShift);
    }

    bool isComplete() const override { return true; }

    bool validatePage() override
    {
        m_wizard->captureStyleIntoResult();
        return true;
    }

    QString seriesName() const { return m_name->text().trimmed(); }
    int colorIndex() const { return m_color->currentData().toInt(); }
    double yShift() const { return m_yShift->value(); }

private:
    UWatchSeriesWizard* m_wizard = nullptr;
    QLineEdit* m_name = nullptr;
    QComboBox* m_color = nullptr;
    QDoubleSpinBox* m_yShift = nullptr;
};

} // namespace watch_wizard

UWatchSeriesWizard::UWatchSeriesWizard(UWatchChart* chart, RDK::UApplication* app, QWidget* parent)
    : QWizard(parent)
    , m_chart(chart)
    , m_app(app)
{
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::HaveFinishButtonOnEarlyPages, true);
    setMinimumSize(900, 640);

    m_typePage = new watch_wizard::TypeFormPage(this);
    m_sourcesPage = new watch_wizard::SourcesPage(this);
    m_stylePage = new watch_wizard::StylePage(this);
    addPage(m_typePage);
    addPage(m_sourcesPage);
    addPage(m_stylePage);
}

NMSDK::Plot::VizKind UWatchSeriesWizard::selectedViz() const
{
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->viz();
}

UWatchSeriesWizardResult::Form UWatchSeriesWizard::selectedForm() const
{
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->form();
}

void UWatchSeriesWizard::captureSourcesIntoResult()
{
    m_result.viz = selectedViz();
    m_result.form = selectedForm();

    auto* sources = static_cast<watch_wizard::SourcesPage*>(m_sourcesPage);
    auto* y = sources->yPicker();
    auto* x = sources->xPicker();
    if (!y)
        return;

    m_result.channel = y->channelIndex();
    m_result.yComponent = y->componentLongName();
    m_result.yProperty = y->propertyName();
    m_result.yCells = y->cells();
    if (m_result.yCells.isEmpty())
        m_result.yCells.push_back(UWatchMatrixSelector::CellRef{0, 0});

    if (m_result.viz != NMSDK::Plot::VizKind::TimeSeries && x)
    {
        m_result.xComponent = x->componentLongName();
        m_result.xProperty = x->propertyName();
        const auto xc = x->cells();
        m_result.xCell = xc.isEmpty() ? UWatchMatrixSelector::CellRef{0, 0} : xc.front();
    }
}

void UWatchSeriesWizard::captureStyleIntoResult()
{
    if (m_result.yComponent.isEmpty())
        captureSourcesIntoResult();

    auto* style = static_cast<watch_wizard::StylePage*>(m_stylePage);
    m_result.seriesName = style->seriesName();
    m_result.colorIndex = style->colorIndex();
    m_result.yShift = style->yShift();
}

int UWatchSeriesWizard::applyToChart(UWatchChart* chart) const
{
    if (!chart)
        return 0;

    auto* self = const_cast<UWatchSeriesWizard*>(this);
    if (m_result.yComponent.isEmpty())
        self->captureSourcesIntoResult();

    const UWatchSeriesWizardResult& r = m_result;
    if (r.yComponent.isEmpty() || r.yProperty.isEmpty())
        return 0;

    const double t = chart->getAxisXrange();
    const int before = chart->countSeries();

    if (r.viz == NMSDK::Plot::VizKind::TimeSeries)
    {
        for (const auto& c : r.yCells)
        {
            chart->createSerie(r.channel, r.yComponent, r.yProperty, QString(),
                               c.jx, c.jy, t, r.yShift);
            const int idx = chart->countSeries() - 1;
            if (!r.seriesName.isEmpty() && r.yCells.size() == 1)
                chart->setSerieName(idx, r.seriesName);
            if (r.colorIndex >= 0)
                chart->setSerieColor(idx, r.colorIndex);
        }
    }
    else
    {
        if (r.xComponent.isEmpty() || r.xProperty.isEmpty())
            return 0;
        const auto yc = r.yCells.isEmpty() ? UWatchMatrixSelector::CellRef{} : r.yCells.front();
        chart->createSerieXY(r.channel,
                             r.xComponent, r.xProperty, r.xCell.jx, r.xCell.jy,
                             r.yComponent, r.yProperty, yc.jx, yc.jy,
                             r.yShift, r.viz);
        const int idx = chart->countSeries() - 1;
        if (!r.seriesName.isEmpty())
            chart->setSerieName(idx, r.seriesName);
        if (r.colorIndex >= 0)
            chart->setSerieColor(idx, r.colorIndex);
    }
    return chart->countSeries() - before;
}

void UWatchSeriesWizard::accept()
{
    if (currentPage() == m_typePage)
        return; // must pick sources first

    if (currentPage() == m_sourcesPage)
        captureSourcesIntoResult();
    else if (currentPage() == m_stylePage)
        captureStyleIntoResult();
    else
        captureSourcesIntoResult();

    if (m_result.yComponent.isEmpty() || m_result.yProperty.isEmpty())
        return;
    if (m_result.viz != NMSDK::Plot::VizKind::TimeSeries
        && (m_result.xComponent.isEmpty() || m_result.xProperty.isEmpty()))
        return;

    QWizard::accept();
}
