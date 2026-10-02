#include "UWatchQuickAddDialog.h"
#include "UWatchSourcePickerWidget.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

UWatchQuickAddDialog::UWatchQuickAddDialog(RDK::UApplication* app, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Quick add Y(t)"));
    setMinimumSize(520, 420);
    auto* root = new QVBoxLayout(this);
    m_picker = new UWatchSourcePickerWidget(this);
    m_picker->configureForWatch(app, true);
    m_picker->setTimeSeriesCompactMode(true);
    root->addWidget(m_picker, 1);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton* ok = buttons->button(QDialogButtonBox::Ok);
    ok->setText(tr("Add to chart"));
    connect(buttons, &QDialogButtonBox::accepted, this, [this, ok]() {
        if (!selectionComplete())
            return;
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_picker, &UWatchSourcePickerWidget::selectionChanged, this, [this, ok]() {
        if (ok)
            ok->setEnabled(selectionComplete());
    });
    if (ok)
        ok->setEnabled(false);
    root->addWidget(buttons);
}

QString UWatchQuickAddDialog::componentLongName() const
{
    return m_picker ? m_picker->componentLongName() : QString();
}

QString UWatchQuickAddDialog::propertyName() const
{
    return m_picker ? m_picker->propertyName() : QString();
}

int UWatchQuickAddDialog::matrixJx() const
{
    if (!m_picker || m_picker->cells().isEmpty())
        return 0;
    return m_picker->cells().first().jx;
}

int UWatchQuickAddDialog::matrixJy() const
{
    if (!m_picker || m_picker->cells().isEmpty())
        return 0;
    return m_picker->cells().first().jy;
}

int UWatchQuickAddDialog::channelIndex() const
{
    return m_picker ? m_picker->channelIndex() : 0;
}

bool UWatchQuickAddDialog::selectionComplete() const
{
    return m_picker && m_picker->isComplete();
}
