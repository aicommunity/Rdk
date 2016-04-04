#include "../../../Rtv/Deploy/Include/rtv_initdll.h"

#include <QCloseEvent>
#include <QVBoxLayout>
#include <QSettings>
#include <QStringList>

#include <qdebug.h>

#include "ModelEditorWindow.h"

ModelEditorWindow::ModelEditorWindow(QWidget *parent)
 : QWidget(parent)
{
 QVBoxLayout *layout = new QVBoxLayout(this);
 splitter = new QSplitter(this);
 scrollArea = new QScrollArea(this);
 area = new PictureArea(this);
 scrollArea->setWidget(area);
 QStringList classesNameList = QString(Storage_GetClassesNameList()).split(",");
 list = new ComponentList(classesNameList, this);
 splitter->addWidget(scrollArea);
 splitter->addWidget(list);

 setLayout(layout);
 layout->addWidget(splitter);

 setWindowTitle("Model Editor");
 // В MainWindow::DrawModelTriggered() кривовато используется этот window title

 QSettings settings("Settings.ini", QSettings::IniFormat);
 settings.beginGroup("ModelEditorWindow");
 splitter->restoreState(settings.value("splitter_state").toByteArray());
 splitter->restoreGeometry(settings.value("splitter_geometry").toByteArray());
 settings.endGroup();
}

ModelEditorWindow::~ModelEditorWindow()
{
 WriteSettings();
}

QSize ModelEditorWindow::sizeHint() const
{
 return QSize(400, 200);
}

void ModelEditorWindow::WriteSettings()
{
 QSettings settings("Settings.ini", QSettings::IniFormat);
 settings.beginGroup("ModelEditorWindow");
 settings.setValue("splitter_state", splitter->saveState());
 settings.setValue("splitter_geometry", splitter->saveGeometry());
 settings.endGroup();
}
