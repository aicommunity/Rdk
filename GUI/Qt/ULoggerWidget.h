#ifndef ULOGGERWIDGET_H
#define ULOGGERWIDGET_H

#include "UVisualControllerWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QTimer>
#include <QVector>
#include <QPair>

class ULoggerWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    ULoggerWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~ULoggerWidget();

    void AUpdateInterface();

private slots:
    void onUpdateTimer();
    void clearLog();
    void exportLog();

private:
    void AddString(int log_level, const QString &string);
    void appendVisibleString(int log_level, const QString& text,
                             bool countAsNew, bool scrollToEnd = true);
    void rebuildFilteredLog();
    bool passesFilters(int log_level, const QString& text) const;

    QVBoxLayout *layout = nullptr;
    QToolBar* toolBar = nullptr;
    QComboBox* levelFilter = nullptr;
    QLineEdit* searchEdit = nullptr;
    QCheckBox* pauseScroll = nullptr;
    QPushButton* clearBtn = nullptr;
    QPushButton* exportBtn = nullptr;
    QPlainTextEdit *textEdit = nullptr;
    QTimer *updateTimer = nullptr;
    int m_maxBlocks = 2000;
    int m_pendingWhilePaused = 0;
    QVector<QPair<int, QString>> m_history;
};

#endif // ULOGGERWIDGET_H
