#include "UProjectDescriptionWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QWidget>
#include <QCloseEvent>

UProjectDescriptionWindow::UProjectDescriptionWindow(QWidget *parent, RDK::UApplication* app)
    : UVisualControllerMainWidget(parent, app)
    , m_markdownViewer(nullptr)
    , m_markdownEditor(nullptr)
    , m_toggleEditButton(nullptr)
    , m_saveButton(nullptr)
    , m_toolBar(nullptr)
    , m_isEditMode(false)
    , m_readmeFilePath()
    , m_originalContent()
    , m_hasUnsavedChanges(false)
{
    setAccessibleName("UProjectDescriptionWindow");
    setWindowTitle("Project Description");

    // Создаем toolbar с кнопками
    m_toolBar = new QToolBar(this);
    m_toolBar->setWindowTitle(tr("Description Tools"));

    m_toggleEditButton = new QPushButton(tr("Edit"), this);
    m_toggleEditButton->setMaximumWidth(100);
    m_toolBar->addWidget(m_toggleEditButton);

    m_saveButton = new QPushButton(tr("Save"), this);
    m_saveButton->setMaximumWidth(100);
    m_saveButton->setEnabled(false); // По умолчанию отключена
    m_toolBar->addWidget(m_saveButton);

    addToolBar(m_toolBar);

    // Создаем виджет для отображения Markdown
    m_markdownViewer = new UMarkdownViewerWidget(this);

    // Создаем виджет для редактирования Markdown
    m_markdownEditor = new QPlainTextEdit(this);
    m_markdownEditor->setFont(QFont("Courier", 10));
    m_markdownEditor->setPlaceholderText(tr("Enter project description in Markdown format..."));
    m_markdownEditor->hide(); // По умолчанию скрыт

    // Создаем центральный виджет с layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_markdownViewer);
    mainLayout->addWidget(m_markdownEditor);

    setCentralWidget(centralWidget);

    // Подключаем сигналы
    connect(m_toggleEditButton, &QPushButton::clicked, this, &UProjectDescriptionWindow::toggleEditMode);
    connect(m_saveButton, &QPushButton::clicked, this, &UProjectDescriptionWindow::saveReadme);
    connect(m_markdownEditor, &QPlainTextEdit::textChanged, this, &UProjectDescriptionWindow::onTextChanged);

    // Загружаем описание проекта, если проект уже открыт
    if(application && application->GetProjectOpenFlag())
    {
        loadProjectDescription();
    }
}

UProjectDescriptionWindow::~UProjectDescriptionWindow()
{
    // Не сохраняем автоматически при закрытии - пользователь должен явно нажать Save
    // m_markdownViewer и другие виджеты удалятся автоматически как дочерние виджеты
}

void UProjectDescriptionWindow::AAfterLoadProject(void)
{
    loadProjectDescription();
}

void UProjectDescriptionWindow::ABeforeCloseProject(void)
{
    // Не сохраняем автоматически - пользователь должен явно нажать Save

    if(m_markdownViewer)
    {
        m_markdownViewer->clear();
    }
    if(m_markdownEditor)
    {
        m_markdownEditor->clear();
    }
    m_readmeFilePath.clear();
    m_originalContent.clear();
    m_hasUnsavedChanges = false;
    if(m_saveButton)
    {
        m_saveButton->setEnabled(false);
    }
}

void UProjectDescriptionWindow::AUpdateInterface(void)
{
    // Обновляем содержимое при обновлении интерфейса
    if(application && application->GetProjectOpenFlag())
    {
        loadProjectDescription();
    }
}

void UProjectDescriptionWindow::AClearInterface(void)
{
    if(m_markdownViewer)
    {
        m_markdownViewer->clear();
    }
    if(m_markdownEditor)
    {
        m_markdownEditor->clear();
    }
    m_readmeFilePath.clear();
    m_originalContent.clear();
    m_hasUnsavedChanges = false;
    if(m_saveButton)
    {
        m_saveButton->setEnabled(false);
    }
}

void UProjectDescriptionWindow::toggleEditMode()
{
    if(!m_markdownViewer || !m_markdownEditor)
        return;

    if(m_isEditMode)
    {
        // Переключаемся в режим просмотра
        // НЕ сохраняем автоматически - пользователь должен явно нажать Save

        m_markdownViewer->show();
        m_markdownEditor->hide();
        m_toggleEditButton->setText(tr("Edit"));
        m_isEditMode = false;
    }
    else
    {
        // Переключаемся в режим редактирования
        // Загружаем текущий markdown из файла в редактор, если редактор пуст или не синхронизирован
        QString currentMarkdown = m_markdownEditor->toPlainText();
        if((currentMarkdown.isEmpty() || m_originalContent.isEmpty()) && !m_readmeFilePath.isEmpty() && QFile::exists(m_readmeFilePath))
        {
            QFile file(m_readmeFilePath);
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                in.setCodec("UTF-8");
                QString content = in.readAll();
                m_markdownEditor->setPlainText(content);
                m_originalContent = content; // Сохраняем исходное содержимое
                m_hasUnsavedChanges = false;
                if(m_saveButton)
                {
                    m_saveButton->setEnabled(false);
                }
                file.close();
            }
        }
        else if(!currentMarkdown.isEmpty() && m_originalContent.isEmpty())
        {
            // Если содержимое есть, но исходное не сохранено, сохраняем его
            m_originalContent = currentMarkdown;
            m_hasUnsavedChanges = false;
            if(m_saveButton)
            {
                m_saveButton->setEnabled(false);
            }
        }

        m_markdownViewer->hide();
        m_markdownEditor->show();
        m_markdownEditor->setFocus();
        m_toggleEditButton->setText(tr("View"));
        m_isEditMode = true;
    }
}

void UProjectDescriptionWindow::onTextChanged()
{
    // Проверяем наличие несохраненных изменений
    if(m_markdownEditor && m_isEditMode)
    {
        QString currentContent = m_markdownEditor->toPlainText();
        m_hasUnsavedChanges = (currentContent != m_originalContent);

        if(m_saveButton)
        {
            m_saveButton->setEnabled(m_hasUnsavedChanges);
        }

        // Обновляем заголовок окна для индикации несохраненных изменений
        if(m_hasUnsavedChanges)
        {
            setWindowTitle(tr("Project Description *"));
        }
        else
        {
            setWindowTitle(tr("Project Description"));
        }
    }
}

bool UProjectDescriptionWindow::hasUnsavedChanges() const
{
    return m_hasUnsavedChanges && m_isEditMode;
}

void UProjectDescriptionWindow::closeEvent(QCloseEvent *event)
{
    // Проверяем наличие несохраненных изменений
    if(hasUnsavedChanges())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved changes. Do you want to save them before closing?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save
        );

        if(reply == QMessageBox::Save)
        {
            saveReadme();
            event->accept();
        }
        else if(reply == QMessageBox::Discard)
        {
            event->accept();
        }
        else // Cancel
        {
            event->ignore();
        }
    }
    else
    {
        event->accept();
    }
}

void UProjectDescriptionWindow::saveReadme()
{
    if(!m_markdownEditor || m_readmeFilePath.isEmpty())
        return;

    QString markdownContent = m_markdownEditor->toPlainText();

    // Сохраняем в файл README.md
    QFile file(m_readmeFilePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << markdownContent;
        file.close();

        // Обновляем исходное содержимое для отслеживания изменений
        m_originalContent = markdownContent;
        m_hasUnsavedChanges = false;
        if(m_saveButton)
        {
            m_saveButton->setEnabled(false);
        }

        // Обновляем отображение в viewer
        if(m_markdownViewer)
        {
            m_markdownViewer->setMarkdown(markdownContent);
        }

        // Обновляем ProjectDescriptionFileName в конфигурации проекта
        if(application && application->GetProjectOpenFlag())
        {
            RDK::TProjectConfig projectConfig = application->GetProjectConfig();

            // Добавляем ProjectDescriptionFileName в project.ini если его там еще нет
            if(projectConfig.DescriptionFileName != "README.md")
            {
                projectConfig.DescriptionFileName = "README.md";
                application->UpdateProject(projectConfig);
            }
        }

        // Обновляем заголовок окна для индикации сохранения
        setWindowTitle(tr("Project Description"));
    }
    else
    {
        QMessageBox::warning(this, tr("Error"),
            tr("Failed to save README.md:\n%1").arg(file.errorString()));
    }
}

void UProjectDescriptionWindow::loadProjectDescription()
{
    if(!m_markdownViewer || !application || !application->GetProjectOpenFlag())
    {
        qDebug() << "UProjectDescriptionWindow: Cannot load - viewer:" << (m_markdownViewer != nullptr)
                 << "application:" << (application != nullptr)
                 << "projectOpen:" << (application ? application->GetProjectOpenFlag() : false);
        return;
    }

    QString projectPath = QString::fromStdString(application->GetProjectPath());
    if(projectPath.isEmpty())
    {
        qDebug() << "UProjectDescriptionWindow: Project path is empty";
        return;
    }

    // Используем QDir для правильного формирования путей
    QDir projectDir(projectPath);
    if(!projectDir.exists())
    {
        qDebug() << "UProjectDescriptionWindow: Project directory does not exist:" << projectPath;
        return;
    }

    // Определяем имя файла описания из конфигурации проекта
    RDK::TProjectConfig projectConfig = application->GetProjectConfig();
    QString descriptionFileName = QString::fromStdString(projectConfig.DescriptionFileName);
    if(descriptionFileName.isEmpty())
    {
        descriptionFileName = "README.md"; // По умолчанию
    }

    QString readmePath = projectDir.absoluteFilePath(descriptionFileName);
    QString rtfPath = projectDir.absoluteFilePath("Description.rtf");

    // Сохраняем путь к файлу README.md для последующего сохранения
    if(descriptionFileName == "README.md" || descriptionFileName.endsWith(".md"))
    {
        m_readmeFilePath = readmePath;
    }
    else
    {
        m_readmeFilePath = projectDir.absoluteFilePath("README.md"); // Используем README.md по умолчанию
    }

    qDebug() << "UProjectDescriptionWindow: Loading from projectPath:" << projectPath;
    qDebug() << "UProjectDescriptionWindow: README path:" << readmePath;
    qDebug() << "UProjectDescriptionWindow: README exists:" << QFile::exists(readmePath);

    // Пытаемся загрузить указанный файл описания
    bool loaded = false;
    if(descriptionFileName.endsWith(".md"))
    {
        loaded = m_markdownViewer->loadMarkdownFromFile(readmePath);
        if(loaded && m_markdownEditor)
        {
            // Загружаем содержимое в редактор для возможности редактирования
            QFile file(readmePath);
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                in.setCodec("UTF-8");
                QString content = in.readAll();
                m_markdownEditor->setPlainText(content);
                m_originalContent = content; // Сохраняем исходное содержимое
                file.close();
            }
        }
    }

    if(!loaded)
    {
        qDebug() << "UProjectDescriptionWindow: Failed to load" << descriptionFileName << ", trying Description.rtf";
        // Fallback: загружаем Description.rtf как plain text
        QFile rtfFile(rtfPath);
        if(rtfFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&rtfFile);
            in.setCodec("Windows-1251");
            QString rtfContent = in.readAll();
            rtfFile.close();
            // Отображаем RTF как plain text (упрощенное отображение)
            QString markdownContent = "# Description\n\n" + rtfContent;
            m_markdownViewer->setMarkdown(markdownContent);
            if(m_markdownEditor)
            {
                m_markdownEditor->setPlainText(markdownContent);
                m_originalContent = markdownContent; // Сохраняем исходное содержимое
            }
            qDebug() << "UProjectDescriptionWindow: Loaded Description.rtf, content length:" << rtfContent.length();
        }
        else
        {
            qDebug() << "UProjectDescriptionWindow: Failed to load Description.rtf from:" << rtfPath;
            m_markdownViewer->clear();
            if(m_markdownEditor)
            {
                m_markdownEditor->clear();
            }
        }
    }
    else
    {
        qDebug() << "UProjectDescriptionWindow: Successfully loaded" << descriptionFileName;
    }

    // Сбрасываем флаг несохраненных изменений после загрузки
    m_hasUnsavedChanges = false;
    if(m_saveButton)
    {
        m_saveButton->setEnabled(false);
    }
    setWindowTitle(tr("Project Description"));
}
