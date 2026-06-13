#ifndef UMARKDOWNVIEWERWIDGET_H
#define UMARKDOWNVIEWERWIDGET_H

#include <QWidget>
#include <QUrl>

#ifdef RDK_USE_QT_WEBENGINE
#include <QWebEngineView>
class QWebEnginePage;
#else
class QTextBrowser;
#endif

/// Виджет для отображения Markdown (с поддержкой mermaid при наличии Qt WebEngine)
class UMarkdownViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UMarkdownViewerWidget(QWidget *parent = nullptr);
    ~UMarkdownViewerWidget();

    /// Установить markdown текст для отображения
    void setMarkdown(const QString& markdown);

    /// Загрузить markdown из файла. При emitDocumentLoaded == true эмитит documentLoaded(filePath) после загрузки.
    bool loadMarkdownFromFile(const QString& filePath, bool emitDocumentLoaded = true);

    /// Установить базовый URL для разрешения относительных ссылок
    void setBaseUrl(const QUrl& url);

    /// Очистить содержимое
    void clear();

    /// Создать HTML из markdown (публично, чтобы использовать в автотестах). basePath — путь каталога или file URL для разрешения относительных ссылок.
#ifdef RDK_USE_QT_WEBENGINE
    QString createHtmlFromMarkdown(const QString& markdown, const QString& basePath = QString()) const;
#endif

signals:
    /// Эмитируется после загрузки документа по ссылке или loadMarkdownFromFile(..., true)
    void documentLoaded(const QString& filePath);

private slots:
#ifdef RDK_USE_QT_WEBENGINE
    void onLoadFinished(bool success);
#endif

private:
#ifdef RDK_USE_QT_WEBENGINE
    QWebEngineView* m_webView;
#else
    QTextBrowser* m_textEdit;
#endif
    QUrl m_baseUrl;
    QString m_currentMarkdown;

#ifdef RDK_USE_QT_WEBENGINE
    /// Инициализация WebEngine
    void initializeWebEngine();
#endif
};

#endif // UMARKDOWNVIEWERWIDGET_H
