#include "UProjectDescriptionWindow.h"

#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

UProjectDescriptionWindow::UProjectDescriptionWindow(QWidget *parent, RDK::UApplication* app)
    : UVisualControllerMainWidget(parent, app)
    , m_markdownViewer(nullptr)
{
    setAccessibleName("UProjectDescriptionWindow");
    setWindowTitle("Project Description");

    // Создаем виджет для отображения Markdown
    m_markdownViewer = new UMarkdownViewerWidget(this);

    // Устанавливаем как центральный виджет
    setCentralWidget(m_markdownViewer);

    // Загружаем описание проекта, если проект уже открыт
    if(application && application->GetProjectOpenFlag())
    {
        loadProjectDescription();
    }
}

UProjectDescriptionWindow::~UProjectDescriptionWindow()
{
    // m_markdownViewer удалится автоматически как дочерний виджет
}

void UProjectDescriptionWindow::AAfterLoadProject(void)
{
    loadProjectDescription();
}

void UProjectDescriptionWindow::ABeforeCloseProject(void)
{
    if(m_markdownViewer)
    {
        m_markdownViewer->clear();
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

    QString readmePath = projectDir.absoluteFilePath("README.md");
    QString rtfPath = projectDir.absoluteFilePath("Description.rtf");

    qDebug() << "UProjectDescriptionWindow: Loading from projectPath:" << projectPath;
    qDebug() << "UProjectDescriptionWindow: README path:" << readmePath;
    qDebug() << "UProjectDescriptionWindow: README exists:" << QFile::exists(readmePath);

    // Пытаемся загрузить README.md, затем Description.rtf
    if(!m_markdownViewer->loadMarkdownFromFile(readmePath))
    {
        qDebug() << "UProjectDescriptionWindow: Failed to load README.md, trying Description.rtf";
        // Fallback: загружаем Description.rtf как plain text
        QFile rtfFile(rtfPath);
        if(rtfFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&rtfFile);
            in.setCodec("Windows-1251");
            QString rtfContent = in.readAll();
            rtfFile.close();
            // Отображаем RTF как plain text (упрощенное отображение)
            m_markdownViewer->setMarkdown("# Description\n\n" + rtfContent);
            qDebug() << "UProjectDescriptionWindow: Loaded Description.rtf, content length:" << rtfContent.length();
        }
        else
        {
            qDebug() << "UProjectDescriptionWindow: Failed to load Description.rtf from:" << rtfPath;
            m_markdownViewer->clear();
        }
    }
    else
    {
        qDebug() << "UProjectDescriptionWindow: Successfully loaded README.md";
    }
}
