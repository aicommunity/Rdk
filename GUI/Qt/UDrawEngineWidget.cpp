#include "UDrawEngineWidget.h"
#include "ui_UDrawEngineWidget.h"
#include <QDebug>
#include <QThread>

UDrawEngineWidget::UDrawEngineWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UDrawEngineWidget)
{
    CheckModelFlag=false;
    ui->setupUi(this);

    classesList = new UClassesListWidget(this,app);

    ui->verticalLayoutForClassesList->addWidget(classesList);
 //   QRect rect= classesList->geometry();
 //   rect.setWidth(100);
 //   classesList->setGeometry(rect);

 //   QRect rect=ui->verticalLayoutForClassesList->geometry();
 //   rect.setWidth(100);
 //   ui->verticalLayoutForClassesList->setGeometry(rect);


    modelScheme = new UDrawEngineImageWidget(ui->scrollArea);
    classesList->SetModelScheme(modelScheme);
    modelScheme->SetApplication(app);
    connect(modelScheme, SIGNAL(componentSelected(QString)), this, SIGNAL(componentSelectedFromScheme(QString)));
    connect(modelScheme, SIGNAL(componentDoubleClick(QString)), this, SIGNAL(componentDoubleClickFromScheme(QString)));
    connect(modelScheme, SIGNAL(componentStapBack()), this, SIGNAL(componentStapBackFromScheme()));
    connect(modelScheme, SIGNAL(updateComponentsList()), this, SIGNAL(updateComponentsListFromScheme()));
    connect(modelScheme, SIGNAL(viewLinks(QString)), this, SIGNAL(viewLinksFromScheme(QString)));
    connect(modelScheme, SIGNAL(createLinks(QString,QString)), this, SIGNAL(createLinksFromScheme(QString,QString)));
    connect(modelScheme, SIGNAL(switchLinks(QString,QString)), this, SIGNAL(switchLinksFromScheme(QString,QString)));
    ui->scrollArea->setWidget(modelScheme);
    ui->scrollArea->setWidgetResizable(true);
    modelScheme->setFixedSize(ui->scrollArea->width(),ui->scrollArea->height());

    // Современная диаграмма
    modernScheme = new UModernDiagramWidget(ui->modernContainer);
    modernScheme->SetApplication(app);
    modernScheme->SetComponentName("");
    auto layoutModern = new QVBoxLayout(ui->modernContainer);
    layoutModern->setContentsMargins(0,0,0,0);
    layoutModern->addWidget(modernScheme);
    
    // Подключение сигналов от современной диаграммы
    connect(modernScheme, SIGNAL(componentSelected(QString)), this, SIGNAL(componentSelectedFromScheme(QString)));
    connect(modernScheme, SIGNAL(componentDoubleClicked(QString)), this, SIGNAL(componentDoubleClickFromScheme(QString)));
    connect(modernScheme, SIGNAL(componentStapBack()), this, SIGNAL(componentStapBackFromScheme()));
    connect(modernScheme, SIGNAL(updateComponentsList()), this, SIGNAL(updateComponentsListFromScheme()));

    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);

    UpdateInterval = 0; // don't update by core ticks
    setAccessibleName("UDrawEngineWidget"); // имя класса для сериализации
    ALoadParameters();

    UpdateInterface(true);
}

UDrawEngineWidget::~UDrawEngineWidget()
{
    delete ui;
}

void UDrawEngineWidget::AUpdateInterface()
{
  modelScheme->reDrawScheme(true);
  modernScheme->Reload();
}

void UDrawEngineWidget::ASaveParameters()
{
  if(!application) return;

  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt",
                     QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  settings.setValue("splitterState", ui->splitter->saveState());
  settings.setValue("labelModelScheme_w", QVariant(modelScheme->width()));
  settings.setValue("labelModelScheme_h", QVariant(modelScheme->height()));
  settings.endGroup();

}

void UDrawEngineWidget::ALoadParameters()
{
  if(!application) return;

  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt",
                     QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  ui->splitter->restoreState(settings.value("splitterState").toByteArray());
//  modelScheme->setFixedSize(settings.value("labelModelScheme_w").toInt(),
//                               settings.value("labelModelScheme_h").toInt());
  settings.endGroup();
}

//расширение схемы при ресайзе
void UDrawEngineWidget::resizeEvent(QResizeEvent*)
{
 if(modelScheme->width()<width() || modelScheme->height()<height())
 {
  modelScheme->setFixedSize(width(),height());
 }
}

void UDrawEngineWidget::componentDoubleClick(QString name)
{
    modelScheme->setComponentName(name);
    modernScheme->SetComponentName(name);
    modernScheme->Reload();
}

void UDrawEngineWidget::componentSingleClick(QString name)
{
    modelScheme->selectComponent(name);
    modernScheme->componentSingleClick(name);
}

void UDrawEngineWidget::updateScheme(bool reloadXml)
{
 modelScheme->reDrawScheme(reloadXml);
 modernScheme->updateScheme(reloadXml);
}

void UDrawEngineWidget::updateClassesList()
{
 classesList->on_tabWidget_currentChanged(0);
}
