#include "UHelpWindow.h"
#include "UStyleManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QLocale>
#include <QToolBar>
#include <QStatusBar>
#include <QUrl>
#include <QTextDocument>
#include <QDesktopServices>
#include <QStyle>

UHelpWindow::UHelpWindow(QWidget *parent, RDK::UApplication* app)
    : QMainWindow(parent)
    , m_textBrowser(nullptr)
    , m_searchEdit(nullptr)
    , m_homeButton(nullptr)
    , m_backButton(nullptr)
    , m_forwardButton(nullptr)
    , m_languageButton(nullptr)
    , m_currentTopic(QString())
    , m_application(app)
{
    setWindowTitle(tr("User Guide"));
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    setupUI();
    setupToolbar();

    // Inherit app stylesheet; refresh on theme switch without local QSS snapshot.
    connect(UStyleManager::instance(), &UStyleManager::themeChanged, this, [this]() {
        style()->unpolish(this);
        style()->polish(this);
        update();
    });
    
    // Load initial help content
    showHelp();
}

UHelpWindow::~UHelpWindow()
{
}

void UHelpWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create text browser
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(false);
    m_textBrowser->setOpenLinks(false);
    
    // Connect link clicked signal
    connect(m_textBrowser, &QTextBrowser::anchorClicked, this, &UHelpWindow::onLinkClicked);
    connect(m_textBrowser, &QTextBrowser::backwardAvailable, this, &UHelpWindow::updateNavigationButtons);
    connect(m_textBrowser, &QTextBrowser::forwardAvailable, this, &UHelpWindow::updateNavigationButtons);
    connect(m_textBrowser, &QTextBrowser::historyChanged, this, &UHelpWindow::updateNavigationButtons);
    
    mainLayout->addWidget(m_textBrowser);
    // Use application stylesheet (no local snapshot — see themeChanged handler).
}

void UHelpWindow::setupToolbar()
{
    QToolBar* toolbar = addToolBar(tr("Navigation"));
    toolbar->setMovable(false);
    
    // Home button
    m_homeButton = new QPushButton(tr("Home"), this);
    m_homeButton->setToolTip(tr("Go to main page"));
    connect(m_homeButton, &QPushButton::clicked, this, &UHelpWindow::onHomeClicked);
    toolbar->addWidget(m_homeButton);
    
    toolbar->addSeparator();
    
    // Back button
    m_backButton = new QPushButton(tr("Back"), this);
    m_backButton->setToolTip(tr("Go back"));
    m_backButton->setEnabled(false);
    connect(m_backButton, &QPushButton::clicked, this, &UHelpWindow::onBackClicked);
    toolbar->addWidget(m_backButton);
    
    // Forward button
    m_forwardButton = new QPushButton(tr("Forward"), this);
    m_forwardButton->setToolTip(tr("Go forward"));
    m_forwardButton->setEnabled(false);
    connect(m_forwardButton, &QPushButton::clicked, this, &UHelpWindow::onForwardClicked);
    toolbar->addWidget(m_forwardButton);
    
    toolbar->addSeparator();
    
    // Language switcher button
    m_languageButton = new QPushButton(this);
    updateLanguageButtonText();
    m_languageButton->setToolTip(tr("Switch language / Переключить язык"));
    connect(m_languageButton, &QPushButton::clicked, this, &UHelpWindow::onLanguageChanged);
    toolbar->addWidget(m_languageButton);
    
    toolbar->addSeparator();
    
    // Search (placeholder for future implementation)
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Search..."));
    m_searchEdit->setMaximumWidth(200);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &UHelpWindow::onSearchTextChanged);
    toolbar->addWidget(m_searchEdit);
    
    updateNavigationButtons();
}

void UHelpWindow::showHelp(const QString& topic)
{
    m_currentTopic = topic;
    loadHelpContent(topic);
}

void UHelpWindow::setLanguage(const QString& langCode)
{
    if(langCode == "en" || langCode == "ru")
    {
        m_currentLanguage = langCode;
        updateLanguageButtonText();
        // Reload current page with new language
        loadHelpContent(m_currentTopic);
    }
}

void UHelpWindow::updateLanguageButtonText()
{
    if(m_languageButton)
    {
        QString currentLang = getCurrentLanguage();
        if(currentLang == "ru")
        {
            m_languageButton->setText("EN");
            m_languageButton->setToolTip(tr("Switch to English / Переключить на английский"));
        }
        else
        {
            m_languageButton->setText("RU");
            m_languageButton->setToolTip(tr("Switch to Russian / Переключить на русский"));
        }
    }
}

void UHelpWindow::onLanguageChanged()
{
    QString currentLang = getCurrentLanguage();
    QString newLang = (currentLang == "ru") ? "en" : "ru";
    setLanguage(newLang);
}

QString UHelpWindow::getHelpPath() const
{
    QString helpPath;
    
    // Primary: use work directory from application (relative path from work dir)
    // Work directory is typically Bin/Platform/Linux, so ../../Help gives Bin/Help
    if(m_application)
    {
        QString workDir = QString::fromStdString(m_application->GetWorkDirectory());
        if(!workDir.isEmpty())
        {
            QDir workDirObj(workDir);
            
            // From Bin/Platform/Linux, ../../Help gives Bin/Help
            helpPath = workDirObj.absoluteFilePath("../../Help");
            QDir helpDir(helpPath);
            QString canonical = helpDir.canonicalPath();
            if(!canonical.isEmpty() && helpDir.exists())
            {
                return canonical;
            }
            
            // Try ../Bin/Help (if workDir is Bin/Platform/Linux)
            helpPath = workDirObj.absoluteFilePath("../Bin/Help");
            helpDir = QDir(helpPath);
            canonical = helpDir.canonicalPath();
            if(!canonical.isEmpty() && helpDir.exists())
            {
                return canonical;
            }
            
            // Try Bin/Help relative to work directory (if workDir is project root)
            helpPath = workDirObj.absoluteFilePath("Bin/Help");
            helpDir = QDir(helpPath);
            canonical = helpDir.canonicalPath();
            if(!canonical.isEmpty() && helpDir.exists())
            {
                return canonical;
            }
        }
    }
    
    // Fallback: relative to application directory (executable location)
    QString appDir = QCoreApplication::applicationDirPath();
    QDir appDirObj(appDir);
    
    // Try ../../Help (from Bin/Platform/Linux to Bin/Help)
    helpPath = appDirObj.absoluteFilePath("../../Help");
    QDir helpDir(helpPath);
    QString canonicalPath = helpDir.canonicalPath();
    if(!canonicalPath.isEmpty() && helpDir.exists())
    {
        return canonicalPath;
    }
    
    // Try ../Bin/Help (if app is in Bin/Platform/Linux)
    helpPath = appDirObj.absoluteFilePath("../Bin/Help");
    helpDir = QDir(helpPath);
    canonicalPath = helpDir.canonicalPath();
    if(!canonicalPath.isEmpty() && helpDir.exists())
    {
        return canonicalPath;
    }
    
    // Try ../Help (if app is in Bin subdirectory)
    helpPath = appDirObj.absoluteFilePath("../Help");
    helpDir = QDir(helpPath);
    canonicalPath = helpDir.canonicalPath();
    if(!canonicalPath.isEmpty() && helpDir.exists())
    {
        return canonicalPath;
    }
    
    // Try ../Bin/Help (if app is in Bin subdirectory)
    helpPath = appDirObj.absoluteFilePath("../Bin/Help");
    helpDir = QDir(helpPath);
    canonicalPath = helpDir.canonicalPath();
    if(!canonicalPath.isEmpty() && helpDir.exists())
    {
        return canonicalPath;
    }
    
    // Try Bin/Help (if app is in project root)
    helpPath = appDirObj.absoluteFilePath("Bin/Help");
    helpDir = QDir(helpPath);
    canonicalPath = helpDir.canonicalPath();
    if(!canonicalPath.isEmpty() && helpDir.exists())
    {
        return canonicalPath;
    }
    
    // Last fallback: try Docs/Help
    if(m_application)
    {
        QString workDir = QString::fromStdString(m_application->GetWorkDirectory());
        if(!workDir.isEmpty())
        {
            QDir workDirObj(workDir);
            helpPath = workDirObj.absoluteFilePath("../Docs/Help");
            helpDir = QDir(helpPath);
            canonicalPath = helpDir.canonicalPath();
            if(!canonicalPath.isEmpty() && helpDir.exists())
            {
                return canonicalPath;
            }
        }
    }
    
    // Return normalized path even if it doesn't exist (for error message)
    helpPath = appDirObj.absoluteFilePath("../../Help");
    helpDir = QDir(helpPath);
    canonicalPath = helpDir.canonicalPath();
    return canonicalPath.isEmpty() ? helpDir.absolutePath() : canonicalPath;
}

QString UHelpWindow::getCurrentLanguage() const
{
    if(!m_currentLanguage.isEmpty())
    {
        return m_currentLanguage;
    }
    
    QLocale locale = QLocale::system();
    QString lang = (locale.language() == QLocale::Russian) ? "ru" : "en";
    
    // Check if help files exist for this language
    QString helpPath = getHelpPath();
    QString indexPath = helpPath + "/" + lang + "/index.html";
    
    if(!QFile::exists(indexPath))
    {
        lang = "en"; // Fallback to English
    }
    
    return lang;
}

void UHelpWindow::loadHelpContent(const QString& topic)
{
    QString lang = getCurrentLanguage();
    QString helpPath = getHelpPath();
    QString fileName = topic.isEmpty() ? "index.html" : topic;
    
    // Ensure topic doesn't contain path separators for security
    fileName = QFileInfo(fileName).fileName();
    
    // Store current topic for language switching (always store, even if empty means index.html)
    m_currentTopic = fileName;
    
    QString filePath = helpPath + "/" + lang + "/" + fileName;
    
    if(QFile::exists(filePath))
    {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            // Set base URL for resolving relative links BEFORE loading content
            QDir helpDir(helpPath);
            QDir langDir(helpDir.absoluteFilePath(lang));
            QUrl baseUrl = QUrl::fromLocalFile(langDir.absolutePath() + "/");
            m_textBrowser->document()->setBaseUrl(baseUrl);
            
            // Load content
            QString content = QString::fromUtf8(file.readAll());
            m_textBrowser->setHtml(content);
            
            // Update language button text after loading content
            updateLanguageButtonText();
        }
        else
        {
            m_textBrowser->setHtml(tr("<h1>Error</h1><p>Could not open help file: %1</p>").arg(filePath));
        }
    }
    else
    {
        QString errorMsg = tr(
            "<h1>Help not found</h1>"
            "<p>Help file not found: %1</p>"
            "<p>Please ensure help files are installed in: %2</p>"
        ).arg(filePath, helpPath);
        m_textBrowser->setHtml(errorMsg);
    }
}

void UHelpWindow::onHomeClicked()
{
    showHelp();
}

void UHelpWindow::onBackClicked()
{
    m_textBrowser->backward();
}

void UHelpWindow::onForwardClicked()
{
    m_textBrowser->forward();
}

void UHelpWindow::onSearchTextChanged(const QString& text)
{
    // Placeholder for future search implementation
    Q_UNUSED(text);
}

void UHelpWindow::onLinkClicked(const QUrl& url)
{
    // Handle external URLs
    if(url.scheme() == "http" || url.scheme() == "https")
    {
        QDesktopServices::openUrl(url);
        return;
    }
    
    QString helpPath = getHelpPath();
    QString lang = getCurrentLanguage();
    QString langPath = helpPath + "/" + lang;
    
    QString fileName;
    QString localPath;
    
    // Try to get local file path
    if(url.isLocalFile())
    {
        localPath = url.toLocalFile();
    }
    else
    {
        // For relative links, QTextBrowser might resolve them to file:// URLs
        // Try to extract path from URL
        QString path = url.path();
        if(!path.isEmpty())
        {
            // Remove leading slash
            while(path.startsWith("/"))
            {
                path = path.mid(1);
            }
            
            // Check if this looks like a relative path (just filename)
            if(path.contains("/") == false && path.endsWith(".html"))
            {
                // It's a simple filename, use it directly
                fileName = path.split("#").first(); // Remove anchor
                QFileInfo fileInfo(fileName);
                fileName = fileInfo.fileName();
                if(!fileName.isEmpty())
                {
                    loadHelpContent(fileName);
                    return;
                }
            }
            
            // Try to construct full path
            QDir langDir(langPath);
            QString fullPath = langDir.absoluteFilePath(path.split("#").first());
            if(QFile::exists(fullPath))
            {
                localPath = fullPath;
            }
        }
    }
    
    // If we have a local path, check if it's in our help directory
    if(!localPath.isEmpty())
    {
        // Check if it's a help file in the current language directory
        if(localPath.startsWith(langPath))
        {
            // Extract relative path from language directory
            QDir langDir(langPath);
            QString relativePath = langDir.relativeFilePath(localPath);
            // Extract just the filename
            QFileInfo fileInfo(relativePath);
            fileName = fileInfo.fileName();
            if(!fileName.isEmpty())
            {
                loadHelpContent(fileName);
                return;
            }
        }
        else if(localPath.startsWith(helpPath))
        {
            // Help file but in different language or styles directory
            // Try to extract filename and load in current language
            QFileInfo fileInfo(localPath);
            fileName = fileInfo.fileName();
            if(!fileName.isEmpty())
            {
                loadHelpContent(fileName);
                return;
            }
        }
        else
        {
            // External file - open in default application
            QDesktopServices::openUrl(url);
            return;
        }
    }
    
    // Try to extract filename from URL path (for relative links)
    QString path = url.path();
    if(path.isEmpty())
    {
        path = url.toString();
        // Remove scheme if present
        if(path.contains("://"))
        {
            int schemeEnd = path.indexOf("://") + 3;
            path = path.mid(schemeEnd);
        }
    }
    
    // Remove leading slash
    while(path.startsWith("/"))
    {
        path = path.mid(1);
    }
    
    // Extract filename (handle anchors like "file.html#section")
    if(!path.isEmpty())
    {
        fileName = path.split("#").first();
        // Remove any directory components, keep only filename
        QFileInfo fileInfo(fileName);
        fileName = fileInfo.fileName();
        
        // If it's an HTML file, load it
        if(!fileName.isEmpty() && fileName.endsWith(".html"))
        {
            loadHelpContent(fileName);
            return;
        }
    }
    
    // Last resort: let QTextBrowser try to handle it (for anchors, etc.)
    m_textBrowser->setSource(url);
}

void UHelpWindow::updateNavigationButtons()
{
    if(m_backButton)
    {
        m_backButton->setEnabled(m_textBrowser->isBackwardAvailable());
    }
    if(m_forwardButton)
    {
        m_forwardButton->setEnabled(m_textBrowser->isForwardAvailable());
    }
}

