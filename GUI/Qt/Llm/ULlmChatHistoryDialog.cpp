#include "ULlmChatHistoryDialog.h"

#include "ULlmChatHistoryArchive.h"

#include <filesystem>

#include <QAbstractItemView>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace fs = std::filesystem;

namespace {

constexpr int kRoleFilePath = Qt::UserRole;
constexpr int kRoleSessionId = Qt::UserRole + 1;

QString shortSessionLabel(const QString& session_id)
{
    if(session_id.size() <= 20)
        return session_id;
    return session_id.left(8) + QStringLiteral("…") + session_id.right(6);
}

} // namespace

ULlmChatHistoryDialog::ULlmChatHistoryDialog(QWidget* parent, ULlmChatHistoryArchive& archive,
                                             const QString& sessions_dir)
    : QDialog(parent)
    , m_archive(archive)
    , m_sessions_dir(sessions_dir)
{
    setWindowTitle(tr("Chat history"));
    resize(720, 420);

    auto* layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({tr("When"), tr("Preview"), tr("Session")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    auto* buttons = new QDialogButtonBox(this);
    auto* open_btn = buttons->addButton(tr("Open"), QDialogButtonBox::ActionRole);
    auto* continue_btn = buttons->addButton(tr("Continue"), QDialogButtonBox::ActionRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(open_btn, &QPushButton::clicked, this, [this]() {
        if(auto r = selectedResult(Action::Open))
        {
            m_result = *r;
            accept();
        }
    });
    connect(continue_btn, &QPushButton::clicked, this, [this]() {
        if(auto r = selectedResult(Action::Continue))
        {
            m_result = *r;
            accept();
        }
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [open_btn]() { open_btn->click(); });

    auto update_continue = [this, continue_btn]() {
        continue_btn->setEnabled(selectedResult(Action::Continue).has_value());
    };
    connect(m_table, &QTableWidget::itemSelectionChanged, this, update_continue);

    populateTable();
    update_continue();
}

void ULlmChatHistoryDialog::populateTable()
{
    const auto entries = m_archive.listChats();
    m_table->setRowCount(static_cast<int>(entries.size()));
    int row = 0;
    for(const ChatListEntry& entry : entries)
    {
        const QString when = entry.created_at_iso.empty()
                                 ? QString::number(entry.mtime_unix)
                                 : QString::fromStdString(entry.created_at_iso);
        auto* when_item = new QTableWidgetItem(when);
        auto* preview_item = new QTableWidgetItem(QString::fromStdString(entry.preview));
        const QString session_q = QString::fromStdString(entry.session_id);
        auto* session_item = new QTableWidgetItem(shortSessionLabel(session_q));

        const QString file_q = QString::fromStdString(entry.file_path.string());
        when_item->setData(kRoleFilePath, file_q);
        when_item->setData(kRoleSessionId, session_q);
        preview_item->setData(kRoleFilePath, file_q);
        preview_item->setData(kRoleSessionId, session_q);
        session_item->setData(kRoleFilePath, file_q);
        session_item->setData(kRoleSessionId, session_q);

        const fs::path sessions_path(m_sessions_dir.toStdString());
        const bool json_exists =
            fs::exists(sessions_path / (entry.session_id + ".json"));
        if(!json_exists)
            session_item->setForeground(Qt::gray);

        m_table->setItem(row, 0, when_item);
        m_table->setItem(row, 1, preview_item);
        m_table->setItem(row, 2, session_item);
        ++row;
    }
    if(!entries.empty())
        m_table->selectRow(0);
}

std::optional<ULlmChatHistoryDialog::Result> ULlmChatHistoryDialog::selectedResult(
    Action action) const
{
    const auto items = m_table->selectedItems();
    if(items.isEmpty())
        return std::nullopt;

    const QString file_q = items.first()->data(kRoleFilePath).toString();
    const QString session_q = items.first()->data(kRoleSessionId).toString();
    if(file_q.isEmpty() || session_q.isEmpty())
        return std::nullopt;

    Result r;
    r.action = action;
    r.chat_file = file_q;
    r.session_id = session_q;

    if(action == Action::Continue)
    {
        const fs::path sessions_path(m_sessions_dir.toStdString());
        if(!fs::exists(sessions_path / (r.session_id.toStdString() + ".json")))
            return std::nullopt;
    }
    return r;
}

std::optional<ULlmChatHistoryDialog::Result> ULlmChatHistoryDialog::run(
    QWidget* parent, ULlmChatHistoryArchive& archive, const QString& sessions_dir)
{
    ULlmChatHistoryDialog dialog(parent, archive, sessions_dir);
    if(dialog.exec() != QDialog::Accepted)
        return std::nullopt;
    return dialog.m_result;
}
