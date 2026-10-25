#include "UWatchQuickAddDialog.h"
#include "UWatchSourcePickerWidget.h"
#include "../UWatchTab.h"
#include "../UWatchChart.h"

#include <QDialogButtonBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QSet>
#include <QSettings>
#include <QVBoxLayout>

namespace {

const QString FavoritesKey = QStringLiteral("Watch/QuickAdd/Favorites");
const QString RecentKey = QStringLiteral("Watch/QuickAdd/Recent");

QString signalKey(const UWatchQuickAddDialog::SignalRef& signal)
{
    return QStringLiteral("%1|%2|%3|%4|%5")
        .arg(signal.component, signal.property)
        .arg(signal.jx)
        .arg(signal.jy)
        .arg(signal.channel);
}

QJsonObject toJson(const UWatchQuickAddDialog::SignalRef& signal)
{
    QJsonObject object;
    object.insert(QStringLiteral("component"), signal.component);
    object.insert(QStringLiteral("property"), signal.property);
    object.insert(QStringLiteral("jx"), signal.jx);
    object.insert(QStringLiteral("jy"), signal.jy);
    object.insert(QStringLiteral("channel"), signal.channel);
    return object;
}

UWatchQuickAddDialog::SignalRef fromJson(const QJsonObject& object)
{
    UWatchQuickAddDialog::SignalRef signal;
    signal.component = object.value(QStringLiteral("component")).toString();
    signal.property = object.value(QStringLiteral("property")).toString();
    signal.jx = object.value(QStringLiteral("jx")).toInt();
    signal.jy = object.value(QStringLiteral("jy")).toInt();
    signal.channel = object.value(QStringLiteral("channel")).toInt();
    return signal;
}

QVector<UWatchQuickAddDialog::SignalRef> readSignals(const QString& key)
{
    QSettings settings;
    QVector<UWatchQuickAddDialog::SignalRef> entries;
    const QJsonArray array = settings.value(key).toJsonArray();
    for (const QJsonValue& value : array)
    {
        if (!value.isObject())
            continue;
        const auto signal = fromJson(value.toObject());
        if (signal.component.isEmpty() || signal.property.isEmpty())
            continue;
        bool duplicate = false;
        for (const auto& existing : entries)
            duplicate = duplicate || signalKey(existing) == signalKey(signal);
        if (!duplicate)
            entries.push_back(signal);
    }
    return entries;
}

void writeSignals(const QString& key,
                  const QVector<UWatchQuickAddDialog::SignalRef>& entries)
{
    QJsonArray array;
    for (const auto& signal : entries)
        array.push_back(toJson(signal));
    QSettings settings;
    settings.setValue(key, array);
}

bool containsSignal(const QVector<UWatchQuickAddDialog::SignalRef>& entries,
                    const UWatchQuickAddDialog::SignalRef& signal)
{
    const QString key = signalKey(signal);
    for (const auto& candidate : entries)
        if (signalKey(candidate) == key)
            return true;
    return false;
}

} // namespace

UWatchQuickAddDialog::UWatchQuickAddDialog(RDK::UApplication* app,
                                           UWatchTab* tab,
                                           int initialChartIndex,
                                           QWidget* parent)
    : QDialog(parent)
    , m_tab(tab)
    , m_favorites(readSignals(FavoritesKey))
    , m_recent(readSignals(RecentKey))
{
    setWindowTitle(tr("Quick add Y(t)"));
    setMinimumSize(580, 640);
    resize(700, 760);
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(6);

    auto* targetRow = new QHBoxLayout();
    targetRow->addWidget(new QLabel(tr("Add to panel"), this));
    m_targetChart = new QComboBox(this);
    m_targetChart->setAccessibleName(tr("Target Watch panel"));
    m_targetChart->setToolTip(tr("Choose which panel receives the selected signals"));
    if (m_tab)
    {
        for (int i = 0; i < m_tab->countGraphs(); ++i)
        {
            UWatchChart* chart = m_tab->getChart(i);
            const QString title = chart && !chart->getChartTitle().trimmed().isEmpty()
                                      ? chart->getChartTitle().trimmed()
                                      : tr("Chart %1").arg(i + 1);
            m_targetChart->addItem(tr("%1 · %2 series%3")
                                       .arg(title)
                                       .arg(chart ? chart->countSeries() : 0)
                                       .arg(chart && !chart->isPanelVisible() ? tr(" · hidden") : QString()),
                                   i);
        }
    }
    const int initialTarget = m_targetChart->findData(initialChartIndex);
    if (initialTarget >= 0)
        m_targetChart->setCurrentIndex(initialTarget);
    targetRow->addWidget(m_targetChart, 1);
    root->addLayout(targetRow);

    auto* matrixModeRow = new QHBoxLayout();
    matrixModeRow->addWidget(new QLabel(tr("Matrix selection"), this));
    m_matrixPickMode = new QComboBox(this);
    m_matrixPickMode->addItem(tr("One cell"), static_cast<int>(MatrixPickMode::SingleCell));
    m_matrixPickMode->addItem(tr("Several cells (Ctrl/Shift-click)"),
                              static_cast<int>(MatrixPickMode::MultiCells));
    m_matrixPickMode->setToolTip(tr("Choose one matrix cell or add several cells as separate series"));
    m_matrixPickMode->setAccessibleName(tr("Matrix cell selection mode"));
    matrixModeRow->addWidget(m_matrixPickMode, 1);
    root->addLayout(matrixModeRow);

    m_picker = new UWatchSourcePickerWidget(this);
    m_picker->configureForWatch(app, true);
    m_picker->setTimeSeriesCompactMode(true);
    root->addWidget(m_picker, 1);

    auto* quickBox = new QGroupBox(tr("Favorites and recent signals"), this);
    auto* quickLayout = new QVBoxLayout(quickBox);
    quickLayout->setContentsMargins(6, 6, 6, 6);
    m_quickPicks = new QListWidget(quickBox);
    m_quickPicks->setMaximumHeight(96);
    quickLayout->addWidget(m_quickPicks);
    auto* quickActions = new QHBoxLayout();
    m_addQuickPickBtn = new QPushButton(tr("Queue selected"), quickBox);
    m_removeFavoriteBtn = new QPushButton(tr("Remove favorite"), quickBox);
    quickActions->addWidget(m_addQuickPickBtn);
    quickActions->addWidget(m_removeFavoriteBtn);
    quickLayout->addLayout(quickActions);
    root->addWidget(quickBox);

    auto* queueBox = new QGroupBox(tr("Signals to add"), this);
    auto* queueLayout = new QVBoxLayout(queueBox);
    queueLayout->setContentsMargins(6, 6, 6, 6);
    m_queuedSignals = new QListWidget(queueBox);
    m_queuedSignals->setMaximumHeight(76);
    queueLayout->addWidget(m_queuedSignals);
    auto* queueActions = new QHBoxLayout();
    m_queueCurrentBtn = new QPushButton(tr("Queue current selection"), queueBox);
    m_favoriteCurrentBtn = new QPushButton(tr("☆ Add current to favorites"), queueBox);
    m_removeQueuedBtn = new QPushButton(tr("Remove"), queueBox);
    m_clearQueueBtn = new QPushButton(tr("Clear"), queueBox);
    queueActions->addWidget(m_queueCurrentBtn);
    queueActions->addWidget(m_favoriteCurrentBtn);
    queueActions->addWidget(m_removeQueuedBtn);
    queueActions->addWidget(m_clearQueueBtn);
    queueLayout->addLayout(queueActions);
    root->addWidget(queueBox);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_acceptButton = buttons->button(QDialogButtonBox::Ok);
    m_acceptButton->setText(tr("Add to chart"));
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        const QVector<SignalRef> selected = selectedSignals();
        if (selected.isEmpty())
            return;
        rememberRecentSignals(selected);
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);

    connect(m_picker, &UWatchSourcePickerWidget::selectionChanged,
            this, &UWatchQuickAddDialog::updateControls);
    connect(m_matrixPickMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (m_picker && index >= 0)
            m_picker->setMatrixPickMode(static_cast<MatrixPickMode>(
                m_matrixPickMode->itemData(index).toInt()));
    });
    connect(m_queueCurrentBtn, &QPushButton::clicked,
            this, &UWatchQuickAddDialog::queueCurrentSelection);
    connect(m_favoriteCurrentBtn, &QPushButton::clicked,
            this, &UWatchQuickAddDialog::addCurrentSelectionToFavorites);
    connect(m_addQuickPickBtn, &QPushButton::clicked, this, [this]() {
        if (!m_quickPicks || !m_quickPicks->currentItem())
            return;
        const QByteArray json = m_quickPicks->currentItem()->data(Qt::UserRole).toByteArray();
        const QJsonDocument document = QJsonDocument::fromJson(json);
        if (document.isObject())
            queueSignal(fromJson(document.object()));
    });
    connect(m_quickPicks, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem*) {
        if (m_addQuickPickBtn && m_addQuickPickBtn->isEnabled())
            m_addQuickPickBtn->click();
    });
    connect(m_removeFavoriteBtn, &QPushButton::clicked,
            this, &UWatchQuickAddDialog::removeSelectedFavorite);
    connect(m_removeQueuedBtn, &QPushButton::clicked, this, [this]() {
        const int row = m_queuedSignals ? m_queuedSignals->currentRow() : -1;
        if (row >= 0 && row < m_pending.size())
            m_pending.removeAt(row);
        refreshQueue();
        updateControls();
    });
    connect(m_clearQueueBtn, &QPushButton::clicked, this, [this]() {
        m_pending.clear();
        refreshQueue();
        updateControls();
    });
    connect(m_quickPicks, &QListWidget::currentRowChanged,
            this, &UWatchQuickAddDialog::updateControls);
    connect(m_queuedSignals, &QListWidget::currentRowChanged,
            this, &UWatchQuickAddDialog::updateControls);

    refreshQuickPicks();
    refreshQueue();
    updateControls();
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

int UWatchQuickAddDialog::targetChartIndex() const
{
    return m_targetChart ? m_targetChart->currentData().toInt() : -1;
}

QVector<UWatchQuickAddDialog::SignalRef> UWatchQuickAddDialog::selectedSignals() const
{
    QVector<SignalRef> result = m_pending;
    if (!m_picker || !m_picker->isComplete())
        return result;

    const auto cells = m_picker->cells();
    if (cells.isEmpty())
        return result;
    const QString component = m_picker->componentLongName();
    const QString property = m_picker->propertyName();
    const int channel = m_picker->channelIndex();
    for (const auto& cell : cells)
    {
        SignalRef signal{component, property, cell.jx, cell.jy, channel};
        if (!containsSignal(result, signal))
            result.push_back(signal);
    }
    return result;
}

bool UWatchQuickAddDialog::selectionComplete() const
{
    return !selectedSignals().isEmpty();
}

void UWatchQuickAddDialog::refreshQuickPicks()
{
    if (!m_quickPicks)
        return;
    m_quickPicks->clear();
    QSet<QString> seen;
    const auto addList = [this, &seen](const QVector<SignalRef>& entries, bool favorite) {
        for (const auto& signal : entries)
        {
            const QString key = signalKey(signal);
            if (seen.contains(key))
                continue;
            seen.insert(key);
            const QString prefix = favorite ? QStringLiteral("★ ") : QStringLiteral("↺ ");
            const QString label = tr("%1%2.%3[%4,%5] · ch %6")
                                      .arg(prefix, signal.component, signal.property)
                                      .arg(signal.jx)
                                      .arg(signal.jy)
                                      .arg(signal.channel);
            auto* item = new QListWidgetItem(label, m_quickPicks);
            item->setData(Qt::UserRole,
                          QJsonDocument(toJson(signal)).toJson(QJsonDocument::Compact));
            item->setData(Qt::UserRole + 1, favorite);
            item->setToolTip(signalKey(signal));
        }
    };
    addList(m_favorites, true);
    addList(m_recent, false);
}

void UWatchQuickAddDialog::refreshQueue()
{
    if (!m_queuedSignals)
        return;
    m_queuedSignals->clear();
    for (const auto& signal : m_pending)
    {
        auto* item = new QListWidgetItem(
            QStringLiteral("%1.%2[%3,%4] · ch %5")
                .arg(signal.component, signal.property)
                .arg(signal.jx)
                .arg(signal.jy)
                .arg(signal.channel),
            m_queuedSignals);
        item->setToolTip(signalKey(signal));
    }
}

void UWatchQuickAddDialog::updateControls()
{
    const bool currentComplete = m_picker && m_picker->isComplete();
    const SignalRef current{componentLongName(), propertyName(), matrixJx(), matrixJy(), channelIndex()};
    const bool isFavorite = currentComplete && containsSignal(m_favorites, current);
    if (m_queueCurrentBtn)
        m_queueCurrentBtn->setEnabled(currentComplete);
    if (m_favoriteCurrentBtn)
    {
        m_favoriteCurrentBtn->setEnabled(currentComplete);
        m_favoriteCurrentBtn->setText(isFavorite ? tr("★ Remove current favorite")
                                                 : tr("☆ Add current to favorites"));
    }
    if (m_addQuickPickBtn)
        m_addQuickPickBtn->setEnabled(m_quickPicks && m_quickPicks->currentRow() >= 0);
    if (m_removeFavoriteBtn)
        m_removeFavoriteBtn->setEnabled(m_quickPicks && m_quickPicks->currentItem()
                                        && m_quickPicks->currentItem()->data(Qt::UserRole + 1).toBool());
    if (m_removeQueuedBtn)
        m_removeQueuedBtn->setEnabled(m_queuedSignals && m_queuedSignals->currentRow() >= 0);
    if (m_clearQueueBtn)
        m_clearQueueBtn->setEnabled(!m_pending.isEmpty());
    if (m_matrixPickMode && m_picker && m_picker->matrixSelector())
        m_matrixPickMode->setEnabled(!m_picker->matrixSelector()->isScalarProperty());
    if (m_acceptButton)
        m_acceptButton->setEnabled(selectionComplete());
}

void UWatchQuickAddDialog::queueCurrentSelection()
{
    if (!m_picker || !m_picker->isComplete())
        return;
    const QString component = m_picker->componentLongName();
    const QString property = m_picker->propertyName();
    const int channel = m_picker->channelIndex();
    for (const auto& cell : m_picker->cells())
        queueSignal(SignalRef{component, property, cell.jx, cell.jy, channel});
}

void UWatchQuickAddDialog::queueSignal(const SignalRef& signal)
{
    if (signal.component.isEmpty() || signal.property.isEmpty()
        || containsSignal(m_pending, signal))
        return;
    m_pending.push_back(signal);
    refreshQueue();
    updateControls();
}

void UWatchQuickAddDialog::addCurrentSelectionToFavorites()
{
    if (!m_picker || !m_picker->isComplete())
        return;
    const QString component = m_picker->componentLongName();
    const QString property = m_picker->propertyName();
    const int channel = m_picker->channelIndex();
    const auto cells = m_picker->cells();
    for (const auto& cell : cells)
    {
        const SignalRef signal{component, property, cell.jx, cell.jy, channel};
        if (containsSignal(m_favorites, signal))
        {
            for (int i = m_favorites.size() - 1; i >= 0; --i)
                if (signalKey(m_favorites[i]) == signalKey(signal))
                    m_favorites.removeAt(i);
        }
        else
        {
            m_favorites.prepend(signal);
        }
    }
    writeSignals(FavoritesKey, m_favorites);
    refreshQuickPicks();
    updateControls();
}

void UWatchQuickAddDialog::removeSelectedFavorite()
{
    if (!m_quickPicks || !m_quickPicks->currentItem()
        || !m_quickPicks->currentItem()->data(Qt::UserRole + 1).toBool())
        return;
    const QJsonDocument document = QJsonDocument::fromJson(
        m_quickPicks->currentItem()->data(Qt::UserRole).toByteArray());
    if (!document.isObject())
        return;
    const QString key = signalKey(fromJson(document.object()));
    for (int i = m_favorites.size() - 1; i >= 0; --i)
        if (signalKey(m_favorites[i]) == key)
            m_favorites.removeAt(i);
    writeSignals(FavoritesKey, m_favorites);
    refreshQuickPicks();
    updateControls();
}

void UWatchQuickAddDialog::rememberRecentSignals(const QVector<SignalRef>& entries)
{
    for (auto it = entries.crbegin(); it != entries.crend(); ++it)
    {
        for (int i = m_recent.size() - 1; i >= 0; --i)
            if (signalKey(m_recent[i]) == signalKey(*it))
                m_recent.removeAt(i);
        m_recent.prepend(*it);
    }
    while (m_recent.size() > 12)
        m_recent.removeLast();
    writeSignals(RecentKey, m_recent);
}
