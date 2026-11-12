#include "UWatchLayoutDialog.h"

#include "../UWatchTab.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {

struct GridPreset
{
    const char* label;
    int rows;
    int cols;
    bool custom;
};

const GridPreset kPresets[] = {
    {"1×1", 1, 1, false},
    {"1×2", 1, 2, false},
    {"2×1", 2, 1, false},
    {"2×2", 2, 2, false},
    {"2×3", 2, 3, false},
    {"Custom", 0, 0, true},
};

} // namespace

UWatchLayoutDialog::UWatchLayoutDialog(UWatchTab* tab, QWidget* parent)
    : QDialog(parent)
    , m_tab(tab)
{
    setWindowTitle(tr("Tab layout"));
    setModal(true);
    setMinimumWidth(320);
    buildUi();
    loadFromTab();
}

bool UWatchLayoutDialog::execForTab(UWatchTab* tab, QWidget* parent)
{
    if (!tab)
        return false;
    UWatchLayoutDialog dlg(tab, parent);
    return dlg.exec() == QDialog::Accepted;
}

void UWatchLayoutDialog::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    auto* form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    m_presetCombo = new QComboBox(this);
    for (const auto& p : kPresets)
        m_presetCombo->addItem(tr(p.label));
    form->addRow(tr("Grid"), m_presetCombo);

    auto* customRow = new QHBoxLayout();
    m_rowsSpin = new QSpinBox(this);
    m_colsSpin = new QSpinBox(this);
    m_rowsSpin->setRange(1, 8);
    m_colsSpin->setRange(1, 8);
    customRow->addWidget(new QLabel(tr("Rows"), this));
    customRow->addWidget(m_rowsSpin);
    customRow->addWidget(new QLabel(tr("Cols"), this));
    customRow->addWidget(m_colsSpin);
    customRow->addStretch(1);
    form->addRow(tr("Custom size"), customRow);

    m_intervalSpin = new QSpinBox(this);
    m_intervalSpin->setRange(16, 10000);
    m_intervalSpin->setSuffix(QStringLiteral(" ms"));
    form->addRow(tr("Update interval"), m_intervalSpin);

    root->addLayout(form);

    auto* previewLabel = new QLabel(tr("Preview"), this);
    root->addWidget(previewLabel);

    m_previewFrame = new QFrame(this);
    m_previewFrame->setFrameShape(QFrame::StyledPanel);
    m_previewFrame->setMinimumHeight(100);
    m_previewGrid = new QGridLayout(m_previewFrame);
    m_previewGrid->setContentsMargins(6, 6, 6, 6);
    m_previewGrid->setSpacing(4);
    root->addWidget(m_previewFrame);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    root->addWidget(buttons);

    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UWatchLayoutDialog::onPresetChanged);
    connect(m_rowsSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &UWatchLayoutDialog::onCustomSpinChanged);
    connect(m_colsSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &UWatchLayoutDialog::onCustomSpinChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &UWatchLayoutDialog::acceptAndApply);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void UWatchLayoutDialog::loadFromTab()
{
    if (!m_tab)
        return;

    const int rows = qMax(1, m_tab->getRowNumber());
    const int cols = qMax(1, m_tab->getColNumber());
    m_rowsSpin->setValue(rows);
    m_colsSpin->setValue(cols);
    m_intervalSpin->setValue(m_tab->UpdateIntervalMs);

    int match = -1;
    for (int i = 0; i < int(sizeof(kPresets) / sizeof(kPresets[0])); ++i)
    {
        if (!kPresets[i].custom && kPresets[i].rows == rows && kPresets[i].cols == cols)
        {
            match = i;
            break;
        }
    }
    m_updatingPreset = true;
    m_presetCombo->setCurrentIndex(match >= 0 ? match : int(sizeof(kPresets) / sizeof(kPresets[0])) - 1);
    m_updatingPreset = false;
    syncCustomSpinsEnabled();
    updatePreview(rows, cols);
}

QPair<int, int> UWatchLayoutDialog::currentGrid() const
{
    const int idx = m_presetCombo->currentIndex();
    if (idx >= 0 && idx < int(sizeof(kPresets) / sizeof(kPresets[0])) && !kPresets[idx].custom)
        return {kPresets[idx].rows, kPresets[idx].cols};
    return {m_rowsSpin->value(), m_colsSpin->value()};
}

void UWatchLayoutDialog::syncCustomSpinsEnabled()
{
    const int idx = m_presetCombo->currentIndex();
    const bool custom = idx >= 0 && idx < int(sizeof(kPresets) / sizeof(kPresets[0]))
                        && kPresets[idx].custom;
    m_rowsSpin->setEnabled(custom);
    m_colsSpin->setEnabled(custom);
}

void UWatchLayoutDialog::onPresetChanged(int)
{
    if (m_updatingPreset)
        return;
    syncCustomSpinsEnabled();
    const auto grid = currentGrid();
    if (!kPresets[m_presetCombo->currentIndex()].custom)
    {
        m_rowsSpin->blockSignals(true);
        m_colsSpin->blockSignals(true);
        m_rowsSpin->setValue(grid.first);
        m_colsSpin->setValue(grid.second);
        m_rowsSpin->blockSignals(false);
        m_colsSpin->blockSignals(false);
    }
    updatePreview(grid.first, grid.second);
}

void UWatchLayoutDialog::onCustomSpinChanged()
{
    if (m_updatingPreset)
        return;
    // Switch to Custom when user edits spins while a preset is selected
    const int customIdx = int(sizeof(kPresets) / sizeof(kPresets[0])) - 1;
    if (m_presetCombo->currentIndex() != customIdx && m_rowsSpin->isEnabled())
    {
        // already custom
    }
    const auto grid = currentGrid();
    updatePreview(grid.first, grid.second);
}

void UWatchLayoutDialog::updatePreview(int rows, int cols)
{
    while (QLayoutItem* item = m_previewGrid->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
    rows = qBound(1, rows, 8);
    cols = qBound(1, cols, 8);
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            auto* cell = new QFrame(m_previewFrame);
            cell->setFrameShape(QFrame::Box);
            cell->setLineWidth(1);
            cell->setMinimumSize(24, 20);
            cell->setStyleSheet(QStringLiteral("background: palette(base);"));
            m_previewGrid->addWidget(cell, r, c);
        }
    }
}

void UWatchLayoutDialog::acceptAndApply()
{
    if (!m_tab)
    {
        reject();
        return;
    }
    const auto grid = currentGrid();
    m_tab->saveUpdateInterval(m_intervalSpin->value());
    m_tab->createGridLayout(grid.first, grid.second);
    m_tab->syncDocumentFromCharts();
    accept();
}
