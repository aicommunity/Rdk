#include "UMarkdownViewerWidget.h"

#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

#ifdef RDK_USE_QT_WEBENGINE
#include <QWebEnginePage>
#include <QWebEngineSettings>
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

    QWebEngineSettings* settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);

    layout->addWidget(m_webView);
    initializeWebEngine();
    connect(m_webView, &QWebEngineView::loadFinished, this, &UMarkdownViewerWidget::onLoadFinished);
#else
    m_textEdit = new QTextEdit(this);
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
    m_currentMarkdown = markdown;

#ifdef RDK_USE_QT_WEBENGINE
    QString html = createHtmlFromMarkdown(markdown);
    // Базовый URL должен быть qrc:/markdown/, иначе скрипты (marked.min.js, mermaid.min.js)
    // не загружаются из-за политики происхождения (file:// vs qrc://).
    QUrl baseUrl = QUrl(QStringLiteral("qrc:/markdown/"));
    m_webView->setHtml(html, baseUrl);
#else
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    m_textEdit->setMarkdown(markdown);
#else
    m_textEdit->setPlainText(markdown);
#endif
#endif
}

bool UMarkdownViewerWidget::loadMarkdownFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open markdown file:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString markdown = in.readAll();
    file.close();

    QFileInfo fileInfo(filePath);
    m_baseUrl = QUrl::fromLocalFile(fileInfo.absolutePath() + "/");

    setMarkdown(markdown);
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
    if (!success)
        qWarning() << "Failed to load markdown content";
}

void UMarkdownViewerWidget::initializeWebEngine()
{
    m_webView->setHtml("<html><body></body></html>");
}

QString UMarkdownViewerWidget::createHtmlFromMarkdown(const QString& markdown) const
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
        const marked = {
            parse: function(md) {
                if (typeof window.marked !== 'undefined' && window.marked.parse)
                    return window.marked.parse(md);
                return this.simpleParse(md);
            },
            simpleParse: function(md) {
                let html = md;
                // Обрабатываем код блоки (до обработки inline кода и других элементов)
                html = html.replace(/```(\w+)?\n([\s\S]*?)```/g, function(match, lang, code) {
                    return '<pre><code>' + code + '</code></pre>';
                });
                // Обрабатываем заголовки (важно: от большего к меньшему)
                html = html.replace(/^#### (.*$)/gim, '<h4>$1</h4>');
                html = html.replace(/^### (.*$)/gim, '<h3>$1</h3>');
                html = html.replace(/^## (.*$)/gim, '<h2>$1</h2>');
                html = html.replace(/^# (.*$)/gim, '<h1>$1</h1>');
                // Обрабатываем списки (маркированные)
                html = html.replace(/^\- (.*$)/gim, '<li>$1</li>');
                html = html.replace(/(<li>.*?<\/li>)/gs, function(match) {
                    return '<ul>' + match + '</ul>';
                });
                // Обрабатываем жирный текст
                html = html.replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>');
                // Обрабатываем курсив
                html = html.replace(/\*(.*?)\*/g, '<em>$1</em>');
                // Обрабатываем inline код
                html = html.replace(/`([^`]+)`/g, '<code>$1</code>');
                // Обрабатываем ссылки
                html = html.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>');
                // Обрабатываем переводы строк
                html = html.replace(/\n\n/g, '</p><p>');
                html = html.replace(/\n/g, '<br>');
                return '<p>' + html + '</p>';
            },
            setOptions: function() {}
        };
    </script>
    <!-- Внешние библиотеки загружаются опционально, встроенный парсер используется по умолчанию -->
    <script src="marked.min.js" onerror="console.warn('marked.min.js failed to load, using built-in parser')"></script>
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
                        el.textContent = 'Error: Empty Base64 string in data attribute';
                        return;
                    }

                    // Декодируем Base64 обратно в байты, затем в UTF-8 строку
                    const binaryString = atob(markdownBase64);
                    // Преобразуем бинарную строку в Uint8Array
                    const bytes = new Uint8Array(binaryString.length);
                    for (let i = 0; i < binaryString.length; i++) {
                        bytes[i] = binaryString.charCodeAt(i);
                    }
                    // Декодируем UTF-8 используя TextDecoder
                    const decoder = new TextDecoder('utf-8');
                    const markdown = decoder.decode(bytes);

                    if (!markdown || markdown.length === 0) {
                        el.textContent = 'Error: Decoded markdown is empty';
                        return;
                    }

                    // Парсим и отображаем markdown используя встроенный парсер
                    // (встроенный парсер более надежен и не зависит от внешних библиотек)
                    if (typeof marked !== 'undefined' && marked.simpleParse) {
                        el.innerHTML = marked.simpleParse(markdown);
                    } else {
                        // Fallback: простая замена переводов строк
                        el.innerHTML = markdown.replace(/\n/g, '<br>');
                    }

                    // Пробуем обработать mermaid диаграммы (если библиотека доступна)
                    if (typeof mermaid !== 'undefined') {
                        try {
                            mermaid.initialize({ startOnLoad: true, theme: 'default', securityLevel: 'loose' });
                            var mels = document.querySelectorAll('code.language-mermaid, pre code.language-mermaid');
                            for (var i = 0; i < mels.length; i++) {
                                var mel = mels[i], parent = mel.parentElement;
                                if (!parent) continue;
                                var div = document.createElement('div');
                                div.className = 'mermaid';
                                div.textContent = mel.textContent;
                                var replaceTarget = (parent.tagName === 'PRE') ? parent : mel;
                                var replaceParent = (parent.tagName === 'PRE') ? parent.parentNode : mel.parentNode;
                                if (replaceParent && replaceTarget) {
                                    replaceParent.replaceChild(div, replaceTarget);
                                }
                            }
                            mermaid.run();
                        } catch (mermaidError) {
                            console.warn('Mermaid processing error (non-critical):', mermaidError);
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
