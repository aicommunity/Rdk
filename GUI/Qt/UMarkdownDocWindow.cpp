#include "UMarkdownDocWindow.h"

#include "UMarkdownViewerWidget.h"

#include <QFileInfo>
#include <QVBoxLayout>
#include <QWidget>

UMarkdownDocWindow::UMarkdownDocWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowTitle(tr("Documentation"));
    resize(900, 700);
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    m_viewer = new UMarkdownViewerWidget(central);
    layout->addWidget(m_viewer);
    setCentralWidget(central);
}

bool UMarkdownDocWindow::openFile(const QString& absPath, const QString& title)
{
    if(absPath.isEmpty())
        return false;
    if(!m_viewer->loadMarkdownFromFile(absPath, true))
        return false;
    if(!title.isEmpty())
        setWindowTitle(title);
    else
        setWindowTitle(QFileInfo(absPath).fileName());
    show();
    raise();
    activateWindow();
    return true;
}
