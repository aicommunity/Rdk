#include "UMarkdownWebEnginePage.h"

#ifdef RDK_USE_QT_WEBENGINE
#include <QDesktopServices>

UMarkdownWebEnginePage::UMarkdownWebEnginePage(QObject* parent)
    : QWebEnginePage(parent)
{
}

bool UMarkdownWebEnginePage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    if (!isMainFrame)
        return true;
    if (url.isLocalFile())
    {
        QString path = url.toLocalFile();
        if (path.endsWith(QLatin1String(".md"), Qt::CaseInsensitive))
        {
            emit openLocalMarkdownRequested(path);
            return false;
        }
    }
    if (url.scheme() == QLatin1String("http") || url.scheme() == QLatin1String("https"))
    {
        QDesktopServices::openUrl(url);
        return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}
#endif
