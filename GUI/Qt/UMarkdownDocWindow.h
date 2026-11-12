#ifndef UMARKDOWNDOCWINDOW_H
#define UMARKDOWNDOCWINDOW_H

#include <QMainWindow>
#include <QString>

class UMarkdownViewerWidget;

/// Lightweight markdown documentation viewer for LLM / chat navigation (DD-DOC-001).
class UMarkdownDocWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit UMarkdownDocWindow(QWidget* parent = nullptr);

    /// Load file; returns false if missing or unreadable.
    bool openFile(const QString& absPath, const QString& title = QString());

private:
    UMarkdownViewerWidget* m_viewer = nullptr;
};

#endif
