#include "UComponentGuiGridDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QVBoxLayout>

UComponentGuiGridDialog::UComponentGuiGridDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Component GUI Grid"));

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    m_gridIdCombo = new QComboBox(this);
    m_gridIdCombo->setEditable(true);
    m_gridIdCombo->setInsertPolicy(QComboBox::NoInsert);

    m_rowsSpin = new QSpinBox(this);
    m_rowsSpin->setRange(1, 16);
    m_rowsSpin->setValue(2);

    m_colsSpin = new QSpinBox(this);
    m_colsSpin->setRange(1, 16);
    m_colsSpin->setValue(2);

    m_rowSpin = new QSpinBox(this);
    m_rowSpin->setRange(0, 15);

    m_colSpin = new QSpinBox(this);
    m_colSpin->setRange(0, 15);

    m_rowLabel = new QLabel(tr("Row (0-based):"), this);
    m_colLabel = new QLabel(tr("Column (0-based):"), this);

    formLayout->addRow(tr("Grid ID:"), m_gridIdCombo);
    formLayout->addRow(tr("Rows (new grid):"), m_rowsSpin);
    formLayout->addRow(tr("Columns (new grid):"), m_colsSpin);
    formLayout->addRow(m_rowLabel, m_rowSpin);
    formLayout->addRow(m_colLabel, m_colSpin);

    rootLayout->addLayout(formLayout);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    rootLayout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &UComponentGuiGridDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &UComponentGuiGridDialog::reject);
    connect(m_gridIdCombo->lineEdit(), &QLineEdit::textChanged, this, &UComponentGuiGridDialog::updateUiState);

    updateUiState();
}

void UComponentGuiGridDialog::setExistingGridIds(const QStringList& gridIds)
{
    m_existingGridIds = gridIds;
    m_gridIdCombo->clear();
    m_gridIdCombo->addItems(m_existingGridIds);
    updateUiState();
}

void UComponentGuiGridDialog::setInitialGridId(const QString& gridId)
{
    if(gridId.isEmpty())
        return;
    m_gridIdCombo->setEditText(gridId);
    updateUiState();
}

void UComponentGuiGridDialog::setMoveModeEnabled(bool enabled)
{
    m_moveModeEnabled = enabled;
    m_rowLabel->setVisible(enabled);
    m_colLabel->setVisible(enabled);
    m_rowSpin->setVisible(enabled);
    m_colSpin->setVisible(enabled);
    updateUiState();
}

void UComponentGuiGridDialog::setRowColLimits(int maxRow, int maxCol)
{
    m_rowSpin->setRange(0, qMax(0, maxRow));
    m_colSpin->setRange(0, qMax(0, maxCol));
}

void UComponentGuiGridDialog::setDefaultGridSize(int rows, int cols)
{
    m_rowsSpin->setValue(qMax(1, rows));
    m_colsSpin->setValue(qMax(1, cols));
}

QString UComponentGuiGridDialog::selectedGridId() const
{
    return m_gridIdCombo->currentText().trimmed();
}

bool UComponentGuiGridDialog::createNewGrid() const
{
    return !m_existingGridIds.contains(selectedGridId());
}

int UComponentGuiGridDialog::rows() const
{
    return m_rowsSpin->value();
}

int UComponentGuiGridDialog::cols() const
{
    return m_colsSpin->value();
}

int UComponentGuiGridDialog::row() const
{
    return m_rowSpin->value();
}

int UComponentGuiGridDialog::col() const
{
    return m_colSpin->value();
}

void UComponentGuiGridDialog::updateUiState()
{
    const bool createNew = createNewGrid();
    m_rowsSpin->setEnabled(createNew);
    m_colsSpin->setEnabled(createNew);
}

void UComponentGuiGridDialog::accept()
{
    if(selectedGridId().isEmpty())
    {
        QMessageBox::warning(this, tr("Component GUI Grid"), tr("Grid ID must not be empty."));
        return;
    }
    QDialog::accept();
}
