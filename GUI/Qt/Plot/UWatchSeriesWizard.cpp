#include "UWatchSeriesWizard.h"
#include "UWatchSourcePickerWidget.h"
#include "WatchPresetCatalog.h"

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
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPixmap>
#include <QRadioButton>
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
        setSubTitle(QObject::tr("Choose manual setup or a component visualization preset."));

        auto* layout = new QFormLayout(this);

        m_modeManual = new QRadioButton(QObject::tr("Manual"), this);
        m_modePreset = new QRadioButton(QObject::tr("Preset"), this);
        m_modeManual->setChecked(true);
        auto* modeRow = new QHBoxLayout();
        modeRow->addWidget(m_modeManual);
        modeRow->addWidget(m_modePreset);
        modeRow->addStretch(1);
        layout->addRow(QObject::tr("Mode"), modeRow);

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

        auto syncModeUi = [this, layout]() {
            const bool preset = m_modePreset && m_modePreset->isChecked();
            m_viz->setVisible(!preset);
            m_form->setVisible(!preset);
            m_viz->setEnabled(!preset);
            m_form->setEnabled(!preset);
            if (QWidget* lab = layout->labelForField(m_viz))
                lab->setVisible(!preset);
            if (QWidget* lab = layout->labelForField(m_form))
                lab->setVisible(!preset);
            if (preset)
                m_hint->setText(QObject::tr(
                    "Preset: pick a component, then a visualization recipe for its class."));
            else
                rebuildFormItems();
        };
        connect(m_modeManual, &QRadioButton::toggled, this, [syncModeUi](bool) { syncModeUi(); });
        connect(m_modePreset, &QRadioButton::toggled, this, [syncModeUi](bool) { syncModeUi(); });

        connect(m_viz, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
            rebuildFormItems();
        });

        if (wizard && wizard->chart())
        {
            const int viz = static_cast<int>(wizard->chart()->getVizKind());
            const int idx = m_viz->findData(viz);
            if (idx >= 0)
                m_viz->setCurrentIndex(idx);

            // Lock family when chart already has series.
            const bool hasSeries = wizard->chart()->countSeries() > 0;
            if (hasSeries)
            {
                const bool xyOnly = NMSDK::Plot::isXYFamily(wizard->chart()->getVizKind());
                const auto keep = m_viz->currentData();
                m_viz->blockSignals(true);
                m_viz->clear();
                if (xyOnly)
                {
                    m_viz->addItem(QObject::tr("XY line"), static_cast<int>(NMSDK::Plot::VizKind::XYLine));
                    m_viz->addItem(QObject::tr("XY scatter"), static_cast<int>(NMSDK::Plot::VizKind::XYScatter));
                }
                else
                {
                    m_viz->addItem(QObject::tr("Time series"), static_cast<int>(NMSDK::Plot::VizKind::TimeSeries));
                }
                const int keepIdx = m_viz->findData(keep);
                if (keepIdx >= 0)
                    m_viz->setCurrentIndex(keepIdx);
                else if (m_viz->count() > 0)
                    m_viz->setCurrentIndex(0);
                m_viz->blockSignals(false);
            }
        }
        rebuildFormItems();
        syncModeUi();
    }

    bool isPresetMode() const
    {
        return m_modePreset && m_modePreset->isChecked();
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
        if (isPresetMode())
            return m_wizard->presetComponentPageId();
        if (viz() == NMSDK::Plot::VizKind::TimeSeries)
            return m_wizard->ySourcePageId();
        return m_wizard->xSourcePageId();
    }

private:
    void rebuildFormItems()
    {
        if (isPresetMode())
            return;
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
            m_form->addItem(QObject::tr("Whole row → curve snapshot"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::WholeRow));
            m_form->addItem(QObject::tr("Whole column → curve snapshot"),
                            static_cast<int>(UWatchSeriesWizardResult::Form::WholeColumn));
            m_form->setEnabled(true);
            m_hint->setText(QObject::tr(
                "Y(x): pick X then Y. Scalar pair accumulates over time. "
                "Row/column snapshots replace the curve each update (zip by min length; size may change)."));
        }
    }

    UWatchSeriesWizard* m_wizard = nullptr;
    QRadioButton* m_modeManual = nullptr;
    QRadioButton* m_modePreset = nullptr;
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
        m_picker->setMatrixPickMode(formToPickMode(form));
        m_picker->setTimeSeriesCompactMode(!xy);
        if (xy && (form == UWatchSeriesWizardResult::Form::WholeRow
                   || form == UWatchSeriesWizardResult::Form::WholeColumn))
        {
            setSubTitle(QObject::tr(
                "Select the matrix row/column used as the Y vector (snapshot each update)."));
        }
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
        const auto form = m_wizard->selectedForm();
        m_picker->setMatrixPickMode(formToPickMode(form));
        if (form == UWatchSeriesWizardResult::Form::WholeRow
            || form == UWatchSeriesWizardResult::Form::WholeColumn)
        {
            setSubTitle(QObject::tr(
                "Select the matrix row/column used as the X vector (snapshot each update)."));
        }
        else
        {
            setSubTitle(QObject::tr("Select the X property for the XY pair (not time)."));
        }
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
        m_yShift->setLocale(QLocale::c());
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
            s->setLocale(QLocale::c());
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
        m_xyMinDistance->setLocale(QLocale::c());
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
        {
            if (m_wizard->isPresetMode())
            {
                const UWatchSeriesWizardResult r = m_wizard->result();
                if (m_name->text().trimmed().isEmpty() && !r.seriesName.isEmpty())
                    m_name->setText(r.seriesName);
            }
            else
            {
                m_wizard->captureSourcesIntoResult();
            }
        }
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

class PresetComponentPage : public QWizardPage
{
public:
    explicit PresetComponentPage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Component"));
        setSubTitle(QObject::tr("Select a component instance. Presets are filtered by its class."));

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(4);

        m_picker = new UWatchSourcePickerWidget(this);
        root->addWidget(m_picker, 1);
        if (wizard)
        {
            m_picker->configureForWatch(wizard->app(), true);
            m_picker->setComponentOnlyMode(true);
        }

        m_classLabel = new QLabel(this);
        m_classLabel->setWordWrap(true);
        m_classLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        m_classLabel->setStyleSheet(QStringLiteral("color: palette(mid);"));
        root->addWidget(m_classLabel, 0);

        connect(m_picker, &UWatchSourcePickerWidget::selectionChanged, this, [this]() {
            refreshClass();
            notifyComplete();
        });
    }

    void initializePage() override
    {
        refreshClass();
        notifyComplete();
    }

    QString componentLongName() const
    {
        return m_picker ? m_picker->componentLongName() : QString();
    }

    QString className() const { return m_className; }

    int channelIndex() const
    {
        return m_picker ? m_picker->channelIndex() : 0;
    }

    bool isComplete() const override
    {
        return !componentLongName().isEmpty() && !m_className.isEmpty();
    }

    int nextId() const override
    {
        return m_wizard ? m_wizard->presetSelectPageId() : QWizardPage::nextId();
    }

private:
    void refreshClass()
    {
        m_className.clear();
        const QString longName = componentLongName();
        if (longName.isEmpty() || !m_wizard)
        {
            m_classLabel->setText(QObject::tr("No component selected."));
            return;
        }
        m_className = componentClassNameFromModelScope(channelIndex(), longName);
        const int n = WatchPresetCatalog::instance().presetsForClass(m_className).size();
        m_classLabel->setText(
            QObject::tr("Class: %1 · presets: %2")
                .arg(m_className.isEmpty() ? QObject::tr("(unknown)") : m_className)
                .arg(n));
    }

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
    QLabel* m_classLabel = nullptr;
    QString m_className;
};

class PresetSelectPage : public QWizardPage
{
public:
    explicit PresetSelectPage(UWatchSeriesWizard* wizard, QWidget* parent = nullptr)
        : QWizardPage(parent)
        , m_wizard(wizard)
    {
        setTitle(QObject::tr("Visualization preset"));
        setSubTitle(QObject::tr(
            "Presets for this class, including nested children and similar types."));

        auto* root = new QVBoxLayout(this);
        m_list = new QListWidget(this);
        root->addWidget(m_list, 1);
        m_preview = new QLabel(this);
        m_preview->setWordWrap(true);
        root->addWidget(m_preview);

        connect(m_list, &QListWidget::currentRowChanged, this, [this](int) {
            updatePreview();
            notifyComplete();
        });
    }

    void initializePage() override
    {
        m_list->clear();
        m_presets.clear();
        auto* compPage = m_wizard
            ? static_cast<PresetComponentPage*>(m_wizard->page(m_wizard->presetComponentPageId()))
            : nullptr;
        const QString cls = compPage ? compPage->className() : QString();
        m_presets = WatchPresetCatalog::instance().presetsForClass(cls);
        for (const WatchPreset& p : m_presets)
        {
            QString label = p.title;
            if (!p.viaSlot.isEmpty())
                label += QObject::tr(" (via %1)").arg(p.viaSlot);
            else if (p.viaFamily && !p.sourceClass.isEmpty())
                label += QObject::tr(" (like %1)").arg(p.sourceClass);
            auto* item = new QListWidgetItem(label, m_list);
            item->setToolTip(p.description);
            item->setData(Qt::UserRole, p.id);
        }
        if (m_list->count() > 0)
            m_list->setCurrentRow(0);
        updatePreview();
        notifyComplete();
    }

    WatchPreset selectedPreset() const
    {
        const int row = m_list ? m_list->currentRow() : -1;
        if (row < 0 || row >= m_presets.size())
            return {};
        return m_presets[row];
    }

    bool isComplete() const override
    {
        return m_list && m_list->currentRow() >= 0 && !m_presets.isEmpty();
    }

    bool validatePage() override
    {
        if (!m_wizard || !isComplete())
            return false;
        m_wizard->capturePresetIntoResult();
        if (m_wizard->result().presetBindings.isEmpty())
        {
            QMessageBox::warning(this, QObject::tr("Preset"),
                                 QObject::tr("Could not resolve preset series for the selected component."));
            return false;
        }
        if (m_wizard->chart()
            && !m_wizard->chart()->canAddVizKind(m_wizard->result().viz))
        {
            QMessageBox::warning(this, QObject::tr("Preset"),
                                 QObject::tr("This chart already uses a different visualization family."));
            return false;
        }
        return true;
    }

    int nextId() const override
    {
        return m_wizard ? m_wizard->stylePageId() : QWizardPage::nextId();
    }

private:
    void updatePreview()
    {
        const WatchPreset p = selectedPreset();
        if (p.id.isEmpty())
        {
            m_preview->setText(QObject::tr("No presets for this class."));
            return;
        }
        QStringList lines;
        lines << p.description;
        if (!p.sourceClass.isEmpty())
        {
            if (!p.viaSlot.isEmpty())
                lines << QObject::tr("Source: %1 via %2").arg(p.sourceClass, p.viaSlot);
            else if (p.viaFamily)
                lines << QObject::tr("Similar class: %1").arg(p.sourceClass);
        }
        for (const WatchPresetSeriesRef& s : p.series)
        {
            const QString path = s.path.isEmpty() ? QStringLiteral(".") : s.path;
            lines << QStringLiteral("• %1.%2 (%3,%4)").arg(path, s.property).arg(s.jx).arg(s.jy);
        }
        m_preview->setText(lines.join(QLatin1Char('\n')));
    }

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
    QListWidget* m_list = nullptr;
    QLabel* m_preview = nullptr;
    QVector<WatchPreset> m_presets;
};

} // namespace watch_wizard

UWatchSeriesWizard::UWatchSeriesWizard(UWatchChart* chart, RDK::UApplication* app, QWidget* parent)
    : QWizard(parent)
    , m_chart(chart)
    , m_app(app)
{
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::HaveFinishButtonOnEarlyPages, false);

    auto& catalog = WatchPresetCatalog::instance();
    if (catalog.rootPath().isEmpty())
    {
        catalog.setRootPath(defaultWatchPresetsPath(app));
        catalog.reload();
    }

    m_typePage = new watch_wizard::TypeFormPage(this);
    m_presetComponentPage = new watch_wizard::PresetComponentPage(this);
    m_presetSelectPage = new watch_wizard::PresetSelectPage(this);
    m_ySourcePage = new watch_wizard::YSourcePage(this);
    m_xSourcePage = new watch_wizard::XSourcePage(this);
    m_stylePage = new watch_wizard::StylePage(this);
    addPage(m_typePage);
    m_presetComponentPageId = addPage(m_presetComponentPage);
    m_presetSelectPageId = addPage(m_presetSelectPage);
    m_xSourcePageId = addPage(m_xSourcePage);
    m_ySourcePageId = addPage(m_ySourcePage);
    m_stylePageId = addPage(m_stylePage);
    setMinimumSize(720, 480);
}

NMSDK::Plot::VizKind UWatchSeriesWizard::selectedViz() const
{
    if (isPresetMode() && !m_result.presetId.isEmpty())
        return m_result.viz;
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->viz();
}

UWatchSeriesWizardResult::Form UWatchSeriesWizard::selectedForm() const
{
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->form();
}

bool UWatchSeriesWizard::isPresetMode() const
{
    return static_cast<watch_wizard::TypeFormPage*>(m_typePage)->isPresetMode();
}

void UWatchSeriesWizard::capturePresetIntoResult()
{
    m_result = UWatchSeriesWizardResult{};
    m_result.mode = UWatchSeriesWizardResult::Mode::Preset;
    auto* compPage = static_cast<watch_wizard::PresetComponentPage*>(m_presetComponentPage);
    auto* selPage = static_cast<watch_wizard::PresetSelectPage*>(m_presetSelectPage);
    if (!compPage || !selPage)
        return;

    m_result.channel = compPage->channelIndex();
    m_result.rootComponent = compPage->componentLongName();
    m_result.componentClassName = compPage->className();
    const WatchPreset preset = selPage->selectedPreset();
    m_result.presetId = preset.id;
    m_result.viz = preset.vizKind;
    m_result.form = UWatchSeriesWizardResult::Form::SingleCell;
    m_result.seriesName = preset.title;

    QString err;
    m_result.presetBindings = WatchPresetCatalog::instance().resolve(
        preset, m_result.rootComponent, m_result.channel, &err);
    if (!m_result.presetBindings.isEmpty())
    {
        // Mirror first binding into y* for style page / accept guards
        m_result.yComponent = m_result.presetBindings.front().component;
        m_result.yProperty = m_result.presetBindings.front().property;
        m_result.yCells = {UWatchMatrixSelector::CellRef{
            m_result.presetBindings.front().jx,
            m_result.presetBindings.front().jy}};
    }
    else if (!err.isEmpty())
    {
        qWarning() << "Watch preset resolve:" << err;
    }
}

void UWatchSeriesWizard::captureYSourceIntoResult()
{
    m_result.mode = UWatchSeriesWizardResult::Mode::Manual;
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
    if (m_result.mode != UWatchSeriesWizardResult::Mode::Preset
        && m_result.yComponent.isEmpty())
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
    if (m_result.mode == UWatchSeriesWizardResult::Mode::Preset)
    {
        if (m_result.presetBindings.isEmpty())
            self->capturePresetIntoResult();
    }
    else if (m_result.yComponent.isEmpty())
    {
        self->captureSourcesIntoResult();
    }

    const UWatchSeriesWizardResult& r = m_result;
    const double t = chart->getAxisXrange();
    const int before = chart->countSeries();

    auto applyColor = [&](int idx) {
        if (r.colorIndex >= 0)
            chart->setSerieColor(idx, r.colorIndex);
        else
            chart->setSerieColor(idx, chart->suggestAutoColorIndex(idx));
    };

    if (r.mode == UWatchSeriesWizardResult::Mode::Preset)
    {
        if (r.presetBindings.isEmpty())
            return 0;
        if (!chart->canAddVizKind(r.viz))
            return 0;
        int created = 0;
        for (const NMSDK::Plot::PropertyRef& ref : r.presetBindings)
        {
            if (r.viz != NMSDK::Plot::VizKind::TimeSeries)
            {
                // v1 catalog is TimeSeries-only; XY role bindings need explicit X series refs
                qWarning() << "Watch preset: XY vizKind not applied in v1 apply path";
                break;
            }
            chart->createSerie(r.channel, ref.component, ref.property, QString(),
                               ref.jx, ref.jy, t, r.yShift);
            const int idx = chart->countSeries() - 1;
            if (created == 0 && !r.seriesName.isEmpty() && r.presetBindings.size() == 1)
                chart->setSerieName(idx, r.seriesName);
            else if (!r.seriesName.isEmpty())
                chart->setSerieName(idx, r.seriesName + QStringLiteral(" / %1").arg(ref.property));
            applyColor(idx);
            ++created;
        }
        if (r.applyYRange && r.yMin < r.yMax)
        {
            chart->setAxisYmin(r.yMin);
            chart->setAxisYmax(r.yMax);
            chart->fixInitialAxesState();
        }
        return chart->countSeries() - before;
    }

    if (r.yComponent.isEmpty() || r.yProperty.isEmpty())
        return 0;

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
        if (!chart->canAddVizKind(r.viz))
            return 0;

        NMSDK::Plot::SliceKind slice = NMSDK::Plot::SliceKind::Cell;
        if (r.form == UWatchSeriesWizardResult::Form::WholeRow)
            slice = NMSDK::Plot::SliceKind::Row;
        else if (r.form == UWatchSeriesWizardResult::Form::WholeColumn)
            slice = NMSDK::Plot::SliceKind::Column;

        const auto yc = r.yCells.isEmpty() ? UWatchMatrixSelector::CellRef{} : r.yCells.front();
        int xJx = r.xCell.jx;
        int xJy = r.xCell.jy;
        int yJx = yc.jx;
        int yJy = yc.jy;
        if (slice == NMSDK::Plot::SliceKind::Row)
        {
            xJy = -1;
            yJy = -1;
        }
        else if (slice == NMSDK::Plot::SliceKind::Column)
        {
            xJx = -1;
            yJx = -1;
        }

        chart->createSerieXY(r.channel,
                             r.xComponent, r.xProperty, xJx, xJy,
                             r.yComponent, r.yProperty, yJx, yJy,
                             r.yShift, r.viz, slice, slice);
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

    if (currentPage() == m_presetSelectPage)
        capturePresetIntoResult();
    else if (currentPage() == m_ySourcePage)
        captureYSourceIntoResult();
    else if (currentPage() == m_xSourcePage)
        captureXSourceIntoResult();
    else if (currentPage() == m_stylePage)
        captureStyleIntoResult();
    else if (m_result.mode != UWatchSeriesWizardResult::Mode::Preset)
        captureSourcesIntoResult();

    if (m_result.mode == UWatchSeriesWizardResult::Mode::Preset)
    {
        if (m_result.presetBindings.isEmpty())
            return;
        QWizard::accept();
        return;
    }

    if (m_result.yComponent.isEmpty() || m_result.yProperty.isEmpty())
        return;
    if (m_result.viz != NMSDK::Plot::VizKind::TimeSeries
        && (m_result.xComponent.isEmpty() || m_result.xProperty.isEmpty()))
        return;

    QWizard::accept();
}
