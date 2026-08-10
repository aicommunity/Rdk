#include "NmsdkQtCompat.h"
#include "UMarkdownViewerWidget.h"

#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QByteArray>
#ifndef RDK_USE_QT_WEBENGINE
#include <QTextBrowser>
#endif
#ifdef RDK_USE_GLOG
#include "../../Deploy/Include/rdk_logging.h"
#endif

#ifdef RDK_USE_QT_WEBENGINE
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include "UMarkdownWebEnginePage.h"
#endif

UMarkdownViewerWidget::UMarkdownViewerWidget(QWidget *parent)
    : QWidget(parent)
#ifdef RDK_USE_QT_WEBENGINE
    , m_webView(nullptr)
#else
    , m_textEdit(nullptr)
#endif
    , m_baseUrl()
    , m_currentMarkdown()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

#ifdef RDK_USE_QT_WEBENGINE
    m_webView = new QWebEngineView(this);
    m_webView->setContextMenuPolicy(Qt::NoContextMenu);

    UMarkdownWebEnginePage* page = new UMarkdownWebEnginePage(this);
    m_webView->setPage(page);
    connect(page, &UMarkdownWebEnginePage::openLocalMarkdownRequested, this, [this](const QString& path) {
        loadMarkdownFromFile(path, true);
    });

    QWebEngineSettings* settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);

    // Примечание: подключение к javaScriptConsoleMessage невозможно напрямую,
    // так как это protected сигнал в QWebEnginePage. JS-логирование остаётся
    // только во встроенной консоли разработчика QtWebEngine (если доступна).

    layout->addWidget(m_webView);
    initializeWebEngine();
    connect(m_webView, &QWebEngineView::loadFinished, this, &UMarkdownViewerWidget::onLoadFinished);
#else
    m_textEdit = new QTextBrowser(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setOpenExternalLinks(true);
    layout->addWidget(m_textEdit);
#endif
}

UMarkdownViewerWidget::~UMarkdownViewerWidget()
{
}

void UMarkdownViewerWidget::setMarkdown(const QString& markdown)
{
    // Empty content: show a clear placeholder instead of an empty Base64 payload.
    const QString content = markdown.trimmed().isEmpty()
        ? QStringLiteral("No project description yet.")
        : markdown;
    m_currentMarkdown = content;

#ifdef RDK_USE_QT_WEBENGINE
    QString basePath = m_baseUrl.toString();
    QString html = createHtmlFromMarkdown(content, basePath);
    // Базовый URL должен быть qrc:/markdown/, иначе скрипты (marked.min.js, mermaid.min.js)
    // не загружаются из-за политики происхождения (file:// vs qrc://).
    QUrl baseUrl = QUrl(QStringLiteral("qrc:/markdown/"));
    m_webView->setHtml(html, baseUrl);
#else
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    m_textEdit->setMarkdown(content);
#else
    m_textEdit->setPlainText(content);
#endif
#endif
}

bool UMarkdownViewerWidget::loadMarkdownFromFile(const QString& filePath, bool emitDocumentLoaded)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
#ifdef RDK_USE_GLOG
        std::string logMsg = "UMarkdownViewerWidget: Failed to open markdown file: " + filePath.toStdString();
        RDK::Logging::GlobalLog(RDK_EX_WARNING, logMsg.c_str());
#endif
        return false;
    }

    QTextStream in(&file);
    NMSDK_QT_TEXT_STREAM_SET_CODEC(in, "UTF-8");
    QString markdown = in.readAll();
    file.close();

    QFileInfo fileInfo(filePath);
    m_baseUrl = QUrl::fromLocalFile(fileInfo.absolutePath() + "/");

    setMarkdown(markdown);
    if (emitDocumentLoaded)
        emit documentLoaded(filePath);
    return true;
}

void UMarkdownViewerWidget::setBaseUrl(const QUrl& url)
{
    m_baseUrl = url;
}

void UMarkdownViewerWidget::clear()
{
    m_currentMarkdown.clear();
#ifdef RDK_USE_QT_WEBENGINE
    m_webView->setHtml("<html><body></body></html>");
#else
    m_textEdit->clear();
#endif
}

#ifdef RDK_USE_QT_WEBENGINE
void UMarkdownViewerWidget::onLoadFinished(bool success)
{
#ifdef RDK_USE_GLOG
    if (!success)
    {
        RDK::Logging::GlobalLog(RDK_EX_WARNING, "UMarkdownViewerWidget: Failed to load markdown content");
    }
#else
    (void)success; // Suppress unused parameter warning when RDK_USE_GLOG is not defined
#endif
}

void UMarkdownViewerWidget::initializeWebEngine()
{
    m_webView->setHtml("<html><body></body></html>");
}

QString UMarkdownViewerWidget::createHtmlFromMarkdown(const QString& markdown, const QString& basePath) const
{
    // Используем разделитель HTML, чтобы )" внутри JS-шаблона не завершало raw string
    QString html = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Markdown Viewer</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
            line-height: 1.6;
            color: #333;
            max-width: 100%;
            padding: 20px;
            margin: 0;
            background-color: #fff;
        }
        h1, h2, h3, h4, h5, h6 {
            margin-top: 24px;
            margin-bottom: 16px;
            font-weight: 600;
            line-height: 1.25;
        }
        h1 { font-size: 2em; border-bottom: 1px solid #eaecef; padding-bottom: 0.3em; }
        h2 { font-size: 1.5em; border-bottom: 1px solid #eaecef; padding-bottom: 0.3em; }
        h3 { font-size: 1.25em; }
        p { margin-bottom: 16px; }
        code {
            background-color: rgba(27, 31, 35, 0.05);
            border-radius: 3px;
            font-size: 85%;
            margin: 0;
            padding: 0.2em 0.4em;
            font-family: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, monospace;
        }
        pre {
            background-color: #f6f8fa;
            border-radius: 3px;
            font-size: 85%;
            line-height: 1.45;
            overflow: auto;
            padding: 16px;
            margin-bottom: 16px;
        }
        pre code {
            background-color: transparent;
            border: 0;
            display: inline;
            line-height: inherit;
            margin: 0;
            overflow: visible;
            padding: 0;
            word-wrap: normal;
        }
        blockquote {
            border-left: 4px solid #dfe2e5;
            color: #6a737d;
            padding: 0 1em;
            margin: 0 0 16px 0;
        }
        ul, ol {
            margin-bottom: 16px;
            padding-left: 2em;
        }
        li { margin-bottom: 0.25em; }
        table {
            border-collapse: collapse;
            border-spacing: 0;
            margin-bottom: 16px;
            width: 100%;
        }
        table th, table td {
            border: 1px solid #dfe2e5;
            padding: 6px 13px;
        }
        table th {
            background-color: #f6f8fa;
            font-weight: 600;
        }
        a {
            color: #0366d6;
            text-decoration: none;
        }
        a:hover { text-decoration: underline; }
        img { max-width: 100%; height: auto; }
        .mermaid { text-align: center; margin: 20px 0; }
    </style>
    <script>
        // Polyfill для Array.prototype.at и String.prototype.at
        // Необходим для работы современных версий marked/mermaid в Qt WebEngine (Chromium < 92)
        (function() {
            if (!Array.prototype.at) {
                Array.prototype.at = function(n) {
                    n = Math.trunc(n) || 0;
                    if (n < 0) n += this.length;
                    if (n < 0 || n >= this.length) return undefined;
                    return this[n];
                };
            }
            if (!String.prototype.at) {
                String.prototype.at = function(n) {
                    n = Math.trunc(n) || 0;
                    if (n < 0) n += this.length;
                    if (n < 0 || n >= this.length) return "";
                    return this.charAt(n);
                };
            }
        })();

        const marked = {
            // Единая точка входа: всегда используем встроенный парсер
            parse: function(md) {
                return this.simpleParse(md);
            },
            simpleParse: function(md) {
                let html = md;

                // 0) Сохраняем mermaid-блоки во временный массив, чтобы остальная обработка
                // (жирный, списки, переносы строк и т.п.) не ломала синтаксис диаграмм.
                const mermaidPlaceholders = [];
                html = html.replace(/```mermaid\s*[\r\n]+([\s\S]*?)```/g, function(match, code) {
                    const id = mermaidPlaceholders.length;
                    mermaidPlaceholders.push(code);
                    return '[[[MERMAID_BLOCK_' + id + ']]]';
                });

                // 1) Обычные кодовые блоки (до обработки inline-кода и других элементов)
                html = html.replace(/```(\w+)?\s*[\r\n]+([\s\S]*?)```/g, function(match, lang, code) {
                    return '<pre><code>' + code + '</code></pre>';
                });

                // 2) Заголовки (от большего к меньшему)
                html = html.replace(/^#### (.*$)/gim, '<h4>$1</h4>');
                html = html.replace(/^### (.*$)/gim, '<h3>$1</h3>');
                html = html.replace(/^## (.*$)/gim, '<h2>$1</h2>');
                html = html.replace(/^# (.*$)/gim, '<h1>$1</h1>');

                // 3) Маркированные списки
                html = html.replace(/^\- (.*$)/gim, '<li>$1</li>');
                html = html.replace(/(<li>[\s\S]*?<\/li>)/g, function(match) {
                    return '<ul>' + match + '</ul>';
                });

                // 4) Жирный и курсив
                html = html.replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>');
                html = html.replace(/\*(.*?)\*/g, '<em>$1</em>');

                // 5) Встроенный код
                html = html.replace(/`([^`]+)`/g, '<code>$1</code>');

                // 6) Ссылки
                html = html.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>');

                // 7) Переводы строк
                html = html.replace(/\n\n/g, '</p><p>');
                html = html.replace(/\n/g, '<br>');

                // 8) Восстанавливаем mermaid-блоки без дальнейших преобразований
                html = html.replace(/\[\[\[MERMAID_BLOCK_(\d+)\]\]\]/g, function(match, id) {
                    const idx = parseInt(id, 10);
                    const code = mermaidPlaceholders[idx] || '';
                    return '<div class="mermaid">' + code + '</div>';
                });

                return '<p>' + html + '</p>';
            },
            setOptions: function() {}
        };
    </script>
    <!-- Внешняя библиотека marked.min.js не используется, оставляем только mermaid -->
    <script src="mermaid.min.js" onerror="console.warn('mermaid.min.js failed to load, mermaid diagrams will not be rendered')"></script>
</head>
<body>
    <div id="content" data-markdown-base64=")HTML";
    // Кодируем markdown в Base64 для безопасной передачи через data-атрибут
    // Base64 содержит только безопасные символы (A-Z, a-z, 0-9, +, /, =)
    // которые не требуют экранирования в HTML атрибутах
    QByteArray markdownUtf8 = markdown.toUtf8();
    QByteArray base64 = markdownUtf8.toBase64();
    QString base64String = QString::fromLatin1(base64);

    html += base64String;
    if (!basePath.isEmpty())
    {
        html += QStringLiteral("\" data-base-path=\"");
        html += basePath.toHtmlEscaped();
    }
    html += R"HTML("></div>
    <script>
        (function() {
            function renderMarkdown() {
                var el = document.getElementById('content');
                if (!el) {
                    console.error('Content element not found');
                    return;
                }

                try {
                    // Читаем Base64 из data-атрибута
                    const markdownBase64 = el.getAttribute('data-markdown-base64');
                    if (!markdownBase64 || markdownBase64.length === 0) {
                        el.textContent = 'No project description yet.';
                        return;
                    }

                    // Декодируем Base64 обратно в байты, затем в UTF-8 строку.
                    // На новых движках используем TextDecoder, на старых (Qt 5.15 WebEngine)
                    // fallback через decodeURIComponent(escape(...)).
                    console.log('Decoding Base64 markdown, length:', markdownBase64.length);
                    const binaryString = atob(markdownBase64);
                    let markdown = "";
                    if (typeof TextDecoder !== 'undefined') {
                        console.log('Using TextDecoder for UTF-8 decoding');
                        const bytes = new Uint8Array(binaryString.length);
                        for (let i = 0; i < binaryString.length; i++) {
                            bytes[i] = binaryString.charCodeAt(i);
                        }
                        const decoder = new TextDecoder('utf-8');
                        markdown = decoder.decode(bytes);
                    } else {
                        console.log('TextDecoder not available, using fallback decodeURIComponent(escape(...))');
                        // Fallback для старых Chromium без TextDecoder
                        markdown = decodeURIComponent(escape(binaryString));
                    }

                    console.log('Decoded markdown length:', markdown ? markdown.length : 0);
                    if (!markdown || markdown.length === 0) {
                        el.textContent = 'No project description yet.';
                        return;
                    }

                    // Парсим и отображаем markdown, используя встроенный парсер
                    if (typeof marked !== 'undefined' && marked.simpleParse) {
                        console.log('Using marked.simpleParse for markdown rendering');
                        el.innerHTML = marked.simpleParse(markdown);
                    } else {
                        console.warn('marked parser not available, using fallback');
                        // Fallback: простая замена переводов строк
                        el.innerHTML = markdown.replace(/\n/g, '<br>');
                    }

                    // Рендерим mermaid-диаграммы (если библиотека доступна)
                    if (typeof mermaid !== 'undefined') {
                        try {
                            // Инициализация mermaid (однократно безопасна)
                            mermaid.initialize({ startOnLoad: false, theme: 'default', securityLevel: 'loose' });
                            // Ищем все div.mermaid внутри контента
                            var mermaidBlocks = document.querySelectorAll('#content .mermaid');
                            if (mermaidBlocks && mermaidBlocks.length > 0) {
                                if (typeof mermaid.init === 'function') {
                                    mermaid.init(undefined, mermaidBlocks);
                                } else if (typeof mermaid.run === 'function') {
                                    mermaid.run();
                                }
                            }
                        } catch (mermaidError) {
                            console.warn('Mermaid processing error (non-critical):', mermaidError);
                        }
                    }

                    // Переписываем относительные ссылки на .md в file:// URL для навигации внутри виджета
                    var basePath = el.getAttribute('data-base-path');
                    if (basePath) {
                        var links = el.querySelectorAll('a[href]');
                        for (var i = 0; i < links.length; i++) {
                            var href = links[i].getAttribute('href');
                            if (href && !href.match(/^(https?:|mailto:|#|file:)/)) {
                                var baseNorm = basePath.replace(/\/+$/, '');
                                var hrefNorm = href.replace(/^\/+/, '');
                                links[i].setAttribute('href', baseNorm + '/' + hrefNorm);
                            }
                        }
                    }

                } catch (e) {
                    el.innerHTML = '<p style="color: red;">Error: ' + e.message + '</p>';
                    console.error('Markdown decode error:', e);
                    const base64Attr = el ? el.getAttribute('data-markdown-base64') : 'null';
                    console.error('Base64 preview:', base64Attr ? base64Attr.substring(0, 50) + '...' : 'null');
                }
            }

            // Ждем загрузки DOM и скриптов
            if (document.readyState === 'loading') {
                document.addEventListener('DOMContentLoaded', renderMarkdown);
            } else {
                // DOM уже загружен, но скрипты могут еще загружаться
                // Используем небольшую задержку для загрузки внешних скриптов
                setTimeout(renderMarkdown, 100);
            }
        })();
    </script>
</body>
</html>
)HTML";
    return html;
}
#endif
