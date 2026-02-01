#ifndef UMARKDOWNWEBENGINEPAGE_H
#define UMARKDOWNWEBENGINEPAGE_H

#ifdef RDK_USE_QT_WEBENGINE
#include <QWebEnginePage>

/// Страница WebEngine, перехватывающая переходы по локальным .md и эмитирующая openLocalMarkdownRequested
class UMarkdownWebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit UMarkdownWebEnginePage(QObject* parent = nullptr);

signals:
    void openLocalMarkdownRequested(const QString& path);

protected:
    bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override;
};

#endif // RDK_USE_QT_WEBENGINE

#endif // UMARKDOWNWEBENGINEPAGE_H
