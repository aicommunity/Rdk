#include "NmsdkQtCompat.h"
#include "ULoggerWidget.h"

#include "../../Core/Engine/UGlogGuiSink.h"
#include "../../Deploy/Include/rdk_error_codes.h"

#include <QFileDialog>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QTextStream>

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
#endif

ULoggerWidget::ULoggerWidget(QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app)
{
    setAccessibleName("ULoggerWidget");
    UpdateInterval = 200;
    CheckModelFlag = false;

    layout = new QVBoxLayout(this);
    NMSDK_QT_LAYOUT_SET_MARGIN(layout, 0);

    toolBar = new QToolBar(tr("Log tools"), this);
    toolBar->setIconSize(QSize(16, 16));
    levelFilter = new QComboBox(toolBar);
    levelFilter->addItem(tr("All levels"), -1);
    levelFilter->addItem(tr("App"), RDK_EX_APP);
    levelFilter->addItem(tr("Info"), RDK_EX_INFO);
    levelFilter->addItem(tr("Debug"), RDK_EX_DEBUG);
    levelFilter->addItem(tr("Warning"), RDK_EX_WARNING);
    levelFilter->addItem(tr("Error"), RDK_EX_ERROR);
    levelFilter->addItem(tr("Fatal"), RDK_EX_FATAL);
    toolBar->addWidget(new QLabel(tr(" Level "), toolBar));
    toolBar->addWidget(levelFilter);
    searchEdit = new QLineEdit(toolBar);
    searchEdit->setPlaceholderText(tr("Search…"));
    searchEdit->setClearButtonEnabled(true);
    toolBar->addWidget(searchEdit);
    pauseScroll = new QCheckBox(tr("Pause scroll"), toolBar);
    pauseScroll->setAccessibleName(tr("Pause automatic scrolling"));
    toolBar->addWidget(pauseScroll);
    clearBtn = new QPushButton(tr("Clear"), toolBar);
    exportBtn = new QPushButton(tr("Export…"), toolBar);
    toolBar->addWidget(clearBtn);
    toolBar->addWidget(exportBtn);
    layout->addWidget(toolBar);

    textEdit = new QPlainTextEdit(this);
    textEdit->clear();
    textEdit->setReadOnly(true);
    textEdit->setMaximumBlockCount(m_maxBlocks);
    layout->addWidget(textEdit);

    connect(clearBtn, &QPushButton::clicked, this, &ULoggerWidget::clearLog);
    connect(exportBtn, &QPushButton::clicked, this, &ULoggerWidget::exportLog);
    connect(levelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ULoggerWidget::rebuildFilteredLog);
    connect(searchEdit, &QLineEdit::textChanged,
            this, &ULoggerWidget::rebuildFilteredLog);
    connect(pauseScroll, &QCheckBox::toggled, this, [this](bool paused) {
        if (paused)
            return;
        m_pendingWhilePaused = 0;
        pauseScroll->setText(tr("Pause scroll"));
        if (textEdit) {
            QTextCursor cursor = textEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            textEdit->setTextCursor(cursor);
        }
    });
    RDK::UGlogGuiSink::Instance().SetMaxMessages(m_maxBlocks);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(200);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimer()));
    updateTimer->start();

    UpdateInterface(true);
}

ULoggerWidget::~ULoggerWidget()
{
    if(updateTimer)
        updateTimer->stop();
}

namespace
{
int MapLogSeverity(const RDK::UGlogGuiMessage& message)
{
 int level=RDK_EX_INFO;
#ifdef RDK_USE_GLOG
 switch(message.Severity)
 {
 case google::GLOG_FATAL:
  level=RDK_EX_FATAL;
  break;
 case google::GLOG_ERROR:
  level=RDK_EX_ERROR;
  break;
 case google::GLOG_WARNING:
  level=RDK_EX_WARNING;
  break;
 case google::GLOG_INFO:
 default:
  level=RDK_EX_INFO;
  break;
 }
#endif

 if(message.Text.find("[APP]") != std::string::npos)
  level=RDK_EX_APP;
 else
 if(message.Text.find("[DEBUG]") != std::string::npos)
  level=RDK_EX_DEBUG;

 return level;
}
}

bool ULoggerWidget::passesFilters(int log_level, const QString& text) const
{
    if (levelFilter)
    {
        const int want = levelFilter->currentData().toInt();
        if (want >= 0 && log_level != want)
            return false;
    }
    if (searchEdit && !searchEdit->text().trimmed().isEmpty())
    {
        if (!text.contains(searchEdit->text().trimmed(), Qt::CaseInsensitive))
            return false;
    }
    return true;
}

void ULoggerWidget::AUpdateInterface()
{
 if(!application)
  return;

 const std::vector<RDK::UGlogGuiMessage> messages = RDK::UGlogGuiSink::Instance().ReadMessages(512);
 for(const RDK::UGlogGuiMessage& message : messages)
 {
  const int severity = MapLogSeverity(message);
  AddString(severity, QString::fromLocal8Bit(message.Text.c_str()));
 }
}

void ULoggerWidget::onUpdateTimer()
{
    AUpdateInterface();
}

void ULoggerWidget::clearLog()
{
    m_history.clear();
    if (textEdit)
        textEdit->clear();
    m_pendingWhilePaused = 0;
}

void ULoggerWidget::exportLog()
{
    if (!textEdit)
        return;
    const QString path = QFileDialog::getSaveFileName(this, tr("Export log"), QString(),
                                                      tr("Text (*.txt)"));
    if (path.isEmpty())
        return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&f);
    out << textEdit->toPlainText();
}

void ULoggerWidget::AddString(int log_level, const QString &string)
{
 m_history.push_back(qMakePair(log_level, string));
 if (m_history.size() > m_maxBlocks)
  m_history.remove(0, m_history.size() - m_maxBlocks);
 if (!passesFilters(log_level, string))
  return;

 appendVisibleString(log_level, string, true);
}

void ULoggerWidget::rebuildFilteredLog()
{
    if (!textEdit)
        return;

    QScrollBar* scrollBar = textEdit->verticalScrollBar();
    const int oldScrollValue = scrollBar ? scrollBar->value() : 0;
    const int pendingCount = m_pendingWhilePaused;
    const QSignalBlocker blocker(textEdit);
    textEdit->clear();
    for (const auto& entry : m_history)
        if (passesFilters(entry.first, entry.second))
            appendVisibleString(entry.first, entry.second, false, false);

    m_pendingWhilePaused = pendingCount;
    if (pauseScroll && pauseScroll->isChecked())
    {
        if (m_pendingWhilePaused > 0)
            pauseScroll->setText(tr("Pause scroll (%1 new)").arg(m_pendingWhilePaused));
        if (scrollBar)
            scrollBar->setValue(qMin(oldScrollValue, scrollBar->maximum()));
    }
    else if (scrollBar)
    {
        scrollBar->setValue(scrollBar->maximum());
    }
}

void ULoggerWidget::appendVisibleString(int log_level, const QString& string,
                                       bool countAsNew, bool scrollToEnd)
{
 if (pauseScroll && pauseScroll->isChecked() && countAsNew)
 {
  ++m_pendingWhilePaused;
  pauseScroll->setText(tr("Pause scroll (%1 new)").arg(m_pendingWhilePaused));
 }

 Qt::GlobalColor color;
 switch(log_level)
 {
 case RDK_EX_APP:
  color=Qt::blue;
 break;
 case RDK_EX_INFO:
  color=Qt::darkGreen;
 break;
 case RDK_EX_DEBUG:
  color=Qt::darkBlue;
 break;
 case RDK_EX_WARNING:
  color=Qt::darkYellow;
 break;
 case RDK_EX_ERROR:
  color=Qt::darkRed;
 break;
 case RDK_EX_FATAL:
  color=Qt::red;
 break;
 case RDK_EX_UNKNOWN:
  color=Qt::magenta;
 break;
 default:
  color=Qt::black;
 }
 QTextCharFormat tf=textEdit->currentCharFormat();
 tf.setForeground(QBrush(color));
 textEdit->setCurrentCharFormat(tf);
 textEdit->appendPlainText(string);

 if (pauseScroll && !pauseScroll->isChecked() && scrollToEnd)
 {
  m_pendingWhilePaused = 0;
  pauseScroll->setText(tr("Pause scroll"));
  QTextCursor c = textEdit->textCursor();
  c.movePosition(QTextCursor::End);
  textEdit->setTextCursor(c);
 }
}
