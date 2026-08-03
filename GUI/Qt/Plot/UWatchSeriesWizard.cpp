#include "UWatchSeriesWizard.h"
#include "UWatchSourcePickerWidget.h"

#include "../UWatchChart.h"
#include "../UWatchSerie.h"
#include "../UStyleManager.h"
#include "../UEngineSelectionSync.h"

#include "rdk.h"
#include <rdk_application.h>
#include "../../Core/Engine/UContainerDescription.h"
#include "../../Core/Engine/UStorage.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QScrollArea>
#include <QFrame>
#include <QSpinBox>
#include <QToolButton>
#include <QTimer>
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

    int nextId() const override
    {
        if (!m_wizard)
            return QWizardPage::nextId();
        if (viz() == NMSDK::Plot::VizKind::TimeSeries)
            return m_wizard->ySourcePageId();
        return m_wizard->xSourcePageId();
    }

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
            m_hint->setText(QObject::tr(
                "XY accumulates (x, y) pairs from two properties each Watch update — not a time axis. "
                "Scalars are OK (parametric curve). Pick X then Y on the next pages; density on Style."));
        }
    }

    UWatchSeriesWizard* m_wizard = nullptr;
    QComboBox* m_viz = nullptr;
    QComboBox* m_form = nullptr;
    QLabel* m_hint = nullptr;
};

class YSourcePage : public QWizardPage
{
public:
    explicit YSourcePage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Y source"));
        setSubTitle(QObject::tr("Select the Y property (and matrix cell if needed)."));

        auto* root = new QVBoxLayout(this);
        m_picker = new UWatchSourcePickerWidget(this);
        root->addWidget(m_picker, 1);

        if (wizard)
            m_picker->configureForWatch(wizard->app(), true);

        connect(m_picker, &UWatchSourcePickerWidget::selectionChanged, this, [this]() {
            notifyComplete();
        });
    }

    void initializePage() override
    {
        const auto viz = m_wizard->selectedViz();
        const auto form = m_wizard->selectedForm();
        const bool xy = (viz != NMSDK::Plot::VizKind::TimeSeries);
        setTitle(xy ? QObject::tr("Y source") : QObject::tr("Y(t) source"));
        setSubTitle(xy ? QObject::tr("Select the Y property for the XY pair.")
                       : QObject::tr("Select the Y property vs time."));
        m_picker->setMatrixPickMode(xy ? MatrixPickMode::SingleCell : formToPickMode(form));
        notifyComplete();
        QTimer::singleShot(0, this, [this]() { notifyComplete(); });
    }

    bool isComplete() const override
    {
        return m_picker && m_picker->isComplete();
    }

    bool validatePage() override
    {
        m_wizard->captureYSourceIntoResult();
        return isComplete();
    }

    int nextId() const override
    {
        return m_wizard ? m_wizard->stylePageId() : QWizardPage::nextId();
    }

    UWatchSourcePickerWidget* picker() const { return m_picker; }

private:
    void notifyComplete()
    {
        emit completeChanged();
        if (QWizard* w = wizard())
        {
            if (QAbstractButton* next = w->button(QWizard::NextButton))
                next->setEnabled(isComplete());
        }
    }

    UWatchSeriesWizard* m_wizard = nullptr;
    UWatchSourcePickerWidget* m_picker = nullptr;
};

class XSourcePage : public QWizardPage
{
public:
    explicit XSourcePage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("X source"));
        setSubTitle(QObject::tr("Select the X property for the XY pair (not time)."));

        auto* root = new QVBoxLayout(this);
        m_picker = new UWatchSourcePickerWidget(this);
        root->addWidget(m_picker, 1);

        if (wizard)
            m_picker->configureForWatch(wizard->app(), false);
        m_picker->setMatrixPickMode(MatrixPickMode::SingleCell);

        connect(m_picker, &UWatchSourcePickerWidget::selectionChanged, this, [this]() {
            notifyComplete();
        });
    }

    void initializePage() override
    {
        notifyComplete();
        QTimer::singleShot(0, this, [this]() { notifyComplete(); });
    }

    bool isComplete() const override
    {
        return m_picker && m_picker->isComplete();
    }

    bool validatePage() override
    {
        m_wizard->captureXSourceIntoResult();
        return isComplete();
    }

    int nextId() const override
    {
        return m_wizard ? m_wizard->ySourcePageId() : QWizardPage::nextId();
    }

    UWatchSourcePickerWidget* picker() const { return m_picker; }

private:
    void notifyComplete()
    {
        emit completeChanged();
        if (QWizard* w = wizard())
        {
            if (QAbstractButton* next = w->button(QWizard::NextButton))
                next->setEnabled(isComplete());
        }
    }

    UWatchSeriesWizard* m_wizard = nullptr;
    UWatchSourcePickerWidget* m_picker = nullptr;
};

class StylePage : public QWizardPage
{
public:
    explicit StylePage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Series style"));
        setSubTitle(QObject::tr("Style, axis limits (from ClDescr when available), XY sampling."));

        auto* pageLayout = new QVBoxLayout(this);
        pageLayout->setContentsMargins(0, 0, 0, 0);
        auto* scroll = new QScrollArea(this);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        auto* content = new QWidget(scroll);
        auto* root = new QVBoxLayout(content);
        root->setContentsMargins(8, 8, 8, 8);
        root->setSpacing(8);

        m_form = new QFormLayout();
        m_form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        m_form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_name = new QLineEdit(content);
        m_name->setPlaceholderText(QObject::tr("Auto"));
        m_name->setMinimumWidth(160);
        m_form->addRow(QObject::tr("Series name"), m_name);

        auto* colorRow = new QHBoxLayout();
        m_colorGroup = new QButtonGroup(this);
        m_colorGroup->setExclusive(true);

        auto* autoBtn = new QToolButton(content);
        autoBtn->setText(QObject::tr("Auto"));
        autoBtn->setCheckable(true);
        autoBtn->setChecked(true);
        autoBtn->setToolTip(QObject::tr("Next unused palette color"));
        m_colorGroup->addButton(autoBtn, -1);
        colorRow->addWidget(autoBtn);

        const int paletteCount = UStyleManager::instance()->getChartSeriesColorCount();
        const int n = qMin(12, qMax(1, paletteCount));
        for (int i = 0; i < n; ++i)
        {
            const QColor c = wizard && wizard->chart()
                                 ? wizard->chart()->getDefaultColor(i)
                                 : UStyleManager::instance()->getChartSeriesColor(i);
            QPixmap px(18, 18);
            px.fill(c);
            auto* btn = new QToolButton(content);
            btn->setIcon(QIcon(px));
            btn->setIconSize(QSize(18, 18));
            btn->setCheckable(true);
            btn->setToolTip(QObject::tr("Palette %1").arg(i + 1));
            m_colorGroup->addButton(btn, i);
            colorRow->addWidget(btn);
        }
        colorRow->addStretch(1);
        m_form->addRow(QObject::tr("Color"), colorRow);

        m_yShift = new QDoubleSpinBox(content);
        m_yShift->setRange(-1e9, 1e9);
        m_yShift->setDecimals(3);
        m_yShift->setMinimumWidth(120);
        m_form->addRow(QObject::tr("Y shift"), m_yShift);

        m_rangeHint = new QLabel(content);
        m_rangeHint->setWordWrap(true);
        m_rangeHint->setStyleSheet(QStringLiteral("color: palette(mid);"));
        m_form->addRow(m_rangeHint);

        auto makeSpin = [content]() {
            auto* s = new QDoubleSpinBox(content);
            s->setRange(-1e12, 1e12);
            s->setDecimals(6);
            s->setMinimumWidth(120);
            return s;
        };

        m_applyYRange = new QCheckBox(QObject::tr("Apply Y range"), content);
        m_yMin = makeSpin();
        m_yMax = makeSpin();
        m_form->addRow(m_applyYRange);
        m_form->addRow(QObject::tr("Y min"), m_yMin);
        m_form->addRow(QObject::tr("Y max"), m_yMax);

        m_applyXRange = new QCheckBox(QObject::tr("Apply X range"), content);
        m_xMin = makeSpin();
        m_xMax = makeSpin();
        m_form->addRow(m_applyXRange);
        m_form->addRow(QObject::tr("X min"), m_xMin);
        m_form->addRow(QObject::tr("X max"), m_xMax);

        root->addLayout(m_form);

        m_samplingBox = new QGroupBox(QObject::tr("XY sampling"), content);
        auto* samplingForm = new QFormLayout(m_samplingBox);
        samplingForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        m_windowSize = new QSpinBox(m_samplingBox);
        m_windowSize->setRange(10, 100000);
        m_windowSize->setValue(2000);
        m_windowSize->setMinimumWidth(120);
        samplingForm->addRow(QObject::tr("Max points"), m_windowSize);

        m_xyMinInterval = new QSpinBox(m_samplingBox);
        m_xyMinInterval->setRange(0, 600000);
        m_xyMinInterval->setSuffix(QObject::tr(" ms"));
        m_xyMinInterval->setSpecialValueText(QObject::tr("Every update"));
        m_xyMinInterval->setValue(0);
        m_xyMinInterval->setMinimumWidth(120);
        m_xyMinInterval->setToolTip(QObject::tr("Min model-time gap between accepted points. Layout interval polls sources."));
        samplingForm->addRow(QObject::tr("Min interval"), m_xyMinInterval);

        m_xyMinDistance = new QDoubleSpinBox(m_samplingBox);
        m_xyMinDistance->setRange(0.0, 1e12);
        m_xyMinDistance->setDecimals(6);
        m_xyMinDistance->setSpecialValueText(QObject::tr("Off"));
        m_xyMinDistance->setValue(0.0);
        m_xyMinDistance->setMinimumWidth(120);
        m_xyMinDistance->setToolTip(QObject::tr(
            "Skip only if close to the last point on BOTH X and Y (|dx| and |dy| below this). "
            "Does not scan the whole cloud."));
        samplingForm->addRow(QObject::tr("Min distance"), m_xyMinDistance);

        root->addWidget(m_samplingBox);
        root->addStretch(1);
        scroll->setWidget(content);
        pageLayout->addWidget(scroll);

        setXyControlsVisible(false);
    }

    void initializePage() override
    {
        if (m_wizard)
            m_wizard->captureSourcesIntoResult();
        refreshSuggestedRanges();
    }

    bool isComplete() const override { return true; }

    bool validatePage() override
    {
        m_wizard->captureStyleIntoResult();
        return true;
    }

    QString seriesName() const { return m_name->text().trimmed(); }
    int colorIndex() const
    {
        return m_colorGroup ? m_colorGroup->checkedId() : -1;
    }
    double yShift() const { return m_yShift->value(); }
    bool applyYRange() const { return m_applyYRange->isChecked(); }
    double yMin() const { return m_yMin->value(); }
    double yMax() const { return m_yMax->value(); }
    bool applyXRange() const { return m_applyXRange->isVisible() && m_applyXRange->isChecked(); }
    double xMin() const { return m_xMin->value(); }
    double xMax() const { return m_xMax->value(); }
    int windowSize() const { return m_windowSize->value(); }
    int xyMinIntervalMs() const { return m_xyMinInterval->value(); }
    double xyMinDistance() const { return m_xyMinDistance->value(); }

private:
    static bool lookupClDescrRange(const QString& component, const QString& property,
                                   int channel, double& outMin, double& outMax, QString& note)
    {
        note.clear();
        if (component.isEmpty() || property.isEmpty())
            return false;

        const QString classNameStr = componentClassNameFromModelScope(channel, component);
        if (classNameStr.isEmpty())
        {
            note = QObject::tr("Class name unavailable for %1").arg(component);
            return false;
        }

        RDK::UEPtr<RDK::UContainerDescription> desc =
            RDK::GetStorageLock()->GetClassDescription(classNameStr.toLocal8Bit().constData(), true);
        if (!desc)
            return false;

        const RDK::UPropertyDescription& pd = desc->GetPropertyDescription(property.toStdString());
        if (pd.DataSelectionType != 2 && pd.DataSelectionType != 4)
        {
            note = QObject::tr("No range in class description for %1.%2")
                       .arg(component, property);
            return false;
        }
        if (pd.ValueList.size() < 2)
            return false;

        bool okMin = false;
        bool okMax = false;
        const double mn = QString::fromStdString(pd.ValueList[0]).toDouble(&okMin);
        const double mx = QString::fromStdString(pd.ValueList[1]).toDouble(&okMax);
        if (!okMin || !okMax || !(mn < mx))
            return false;

        outMin = mn;
        outMax = mx;
        note = QObject::tr("From ClDescr %1::%2 → [%3, %4]")
                   .arg(classNameStr, property)
                   .arg(mn)
                   .arg(mx);
        return true;
    }

    void setXyControlsVisible(bool xy)
    {
        m_applyXRange->setVisible(xy);
        m_xMin->setVisible(xy);
        m_xMax->setVisible(xy);
        if (m_form)
        {
            if (QWidget* lab = m_form->labelForField(m_xMin))
                lab->setVisible(xy);
            if (QWidget* lab = m_form->labelForField(m_xMax))
                lab->setVisible(xy);
        }
        if (m_samplingBox)
            m_samplingBox->setVisible(xy);
    }

    void refreshSuggestedRanges()
    {
        if (!m_wizard)
            return;
        const auto& r = m_wizard->result();
        const bool xy = (r.viz != NMSDK::Plot::VizKind::TimeSeries);
        setXyControlsVisible(xy);

        double ymin = -1.0;
        double ymax = 1.0;
        QString yNote;
        const bool hasY = lookupClDescrRange(r.yComponent, r.yProperty, r.channel, ymin, ymax, yNote);
        if (hasY)
        {
            m_yMin->setValue(ymin);
            m_yMax->setValue(ymax);
            m_applyYRange->setChecked(true);
            m_applyYRange->setEnabled(true);
            m_yMin->setEnabled(true);
            m_yMax->setEnabled(true);
        }
        else
        {
            if (m_wizard->chart())
            {
                m_yMin->setValue(m_wizard->chart()->getAxisYmin());
                m_yMax->setValue(m_wizard->chart()->getAxisYmax());
            }
            m_applyYRange->setChecked(false);
        }

        QString hint = hasY ? yNote
                            : (yNote.isEmpty()
                                   ? QObject::tr("Y range: no ClDescr range for selected Y property.")
                                   : yNote);

        if (xy)
        {
            double xmin = 0.0;
            double xmax = 1.0;
            QString xNote;
            const bool hasX = lookupClDescrRange(r.xComponent, r.xProperty, r.channel, xmin, xmax, xNote);
            if (hasX)
            {
                m_xMin->setValue(xmin);
                m_xMax->setValue(xmax);
                m_applyXRange->setChecked(true);
                hint += QStringLiteral("\n") + xNote;
            }
            else
            {
                m_applyXRange->setChecked(false);
                if (!xNote.isEmpty())
                    hint += QStringLiteral("\n") + xNote;
                else
                    hint += QStringLiteral("\n")
                            + QObject::tr("X range: no ClDescr range for selected X property.");
            }
        }

        m_rangeHint->setText(hint);
    }

    UWatchSeriesWizard* m_wizard = nullptr;
    QFormLayout* m_form = nullptr;
    QLineEdit* m_name = nullptr;
    QButtonGroup* m_colorGroup = nullptr;
    QDoubleSpinBox* m_yShift = nullptr;
    QLabel* m_rangeHint = nullptr;
    QCheckBox* m_applyYRange = nullptr;
    QDoubleSpinBox* m_yMin = nullptr;
    QDoubleSpinBox* m_yMax = nullptr;
    QCheckBox* m_applyXRange = nullptr;
    QDoubleSpinBox* m_xMin = nullptr;
    QDoubleSpinBox* m_xMax = nullptr;
    QGroupBox* m_samplingBox = nullptr;
    QSpinBox* m_windowSize = nullptr;
    QSpinBox* m_xyMinInterval = nullptr;
    QDoubleSpinBox* m_xyMinDistance = nullptr;
};

} // namespace watch_wizard

UWatchSeriesWizard::UWatchSeriesWizard(UWatchChart* chart, RDK::UApplication* app, QWidget* parent)
    : QWizard(parent)
    , m_chart(chart)
    , m_app(app)
{
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::HaveFinishButtonOnEarlyPages, false);

    m_typePage = new watch_wizard::TypeFormPage(this);
    m_ySourcePage = new watch_wizard::YSourcePage(this);
    m_xSourcePage = new watch_wizard::XSourcePage(this);
    m_stylePage = new watch_wizard::StylePage(this);
    addPage(m_typePage);
    m_xSourcePageId = addPage(m_xSourcePage);
    m_ySourcePageId = addPage(m_ySourcePage);
    m_stylePageId = addPage(m_stylePage);
    setMinimumSize(720, 480);
}

NMSDK::Plot::VizKind UWatchSeriesWizard::selectedViz() const
{
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->viz();
}

UWatchSeriesWizardResult::Form UWatchSeriesWizard::selectedForm() const
{
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->form();
}

void UWatchSeriesWizard::captureYSourceIntoResult()
{
    m_result.viz = selectedViz();
    m_result.form = selectedForm();

    auto* yPage = static_cast<watch_wizard::YSourcePage*>(m_ySourcePage);
    auto* y = yPage ? yPage->picker() : nullptr;
    if (!y)
        return;

    m_result.channel = y->channelIndex();
    m_result.yComponent = y->componentLongName();
    m_result.yProperty = y->propertyName();
    m_result.yCells = y->cells();
    if (m_result.yCells.isEmpty())
        m_result.yCells.push_back(UWatchMatrixSelector::CellRef{0, 0});
}

void UWatchSeriesWizard::captureXSourceIntoResult()
{
    m_result.viz = selectedViz();
    auto* xPage = static_cast<watch_wizard::XSourcePage*>(m_xSourcePage);
    auto* x = xPage ? xPage->picker() : nullptr;
    if (!x || m_result.viz == NMSDK::Plot::VizKind::TimeSeries)
        return;

    m_result.xComponent = x->componentLongName();
    m_result.xProperty = x->propertyName();
    const auto xc = x->cells();
    m_result.xCell = xc.isEmpty() ? UWatchMatrixSelector::CellRef{0, 0} : xc.front();
}

void UWatchSeriesWizard::captureSourcesIntoResult()
{
    captureYSourceIntoResult();
    if (m_result.viz != NMSDK::Plot::VizKind::TimeSeries)
        captureXSourceIntoResult();
}

void UWatchSeriesWizard::captureStyleIntoResult()
{
    if (m_result.yComponent.isEmpty())
        captureSourcesIntoResult();

    auto* style = static_cast<watch_wizard::StylePage*>(m_stylePage);
    m_result.seriesName = style->seriesName();
    m_result.colorIndex = style->colorIndex();
    m_result.yShift = style->yShift();
    m_result.applyYRange = style->applyYRange();
    m_result.yMin = style->yMin();
    m_result.yMax = style->yMax();
    m_result.applyXRange = style->applyXRange();
    m_result.xMin = style->xMin();
    m_result.xMax = style->xMax();
    m_result.windowSize = style->windowSize();
    m_result.xyMinIntervalMs = style->xyMinIntervalMs();
    m_result.xyMinDistance = style->xyMinDistance();
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

    auto applyColor = [&](int idx) {
        if (r.colorIndex >= 0)
            chart->setSerieColor(idx, r.colorIndex);
        else
            chart->setSerieColor(idx, chart->suggestAutoColorIndex(idx));
    };

    if (r.viz == NMSDK::Plot::VizKind::TimeSeries)
    {
        for (const auto& c : r.yCells)
        {
            chart->createSerie(r.channel, r.yComponent, r.yProperty, QString(),
                               c.jx, c.jy, t, r.yShift);
            const int idx = chart->countSeries() - 1;
            if (!r.seriesName.isEmpty() && r.yCells.size() == 1)
                chart->setSerieName(idx, r.seriesName);
            applyColor(idx);
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
        if (idx >= 0)
        {
            if (UWatchSerie* s = chart->getSerie(idx))
            {
                s->windowSize = r.windowSize > 0 ? r.windowSize : 2000;
                s->xyMinIntervalMs = qMax(0, r.xyMinIntervalMs);
                s->xyMinDistance = r.xyMinDistance > 0.0 ? r.xyMinDistance : 0.0;
            }
            if (!r.seriesName.isEmpty())
                chart->setSerieName(idx, r.seriesName);
            applyColor(idx);
        }
    }

    if (r.applyYRange && r.yMin < r.yMax)
    {
        chart->setAxisYmin(r.yMin);
        chart->setAxisYmax(r.yMax);
    }
    if (r.applyXRange && r.xMin < r.xMax
        && r.viz != NMSDK::Plot::VizKind::TimeSeries)
    {
        chart->setAxisXmin(r.xMin);
        chart->setAxisXmax(r.xMax);
        chart->isAxisXtrackable = false;
    }
    if ((r.applyYRange && r.yMin < r.yMax)
        || (r.applyXRange && r.xMin < r.xMax && r.viz != NMSDK::Plot::VizKind::TimeSeries))
    {
        chart->fixInitialAxesState();
    }

    return chart->countSeries() - before;
}

void UWatchSeriesWizard::accept()
{
    if (currentPage() == m_typePage)
        return;

    if (currentPage() == m_ySourcePage)
        captureYSourceIntoResult();
    else if (currentPage() == m_xSourcePage)
        captureXSourceIntoResult();
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
