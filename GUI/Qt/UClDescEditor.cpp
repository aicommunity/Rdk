#include "UClDescEditor.h"
#include "ui_UClassDescriptionDisplay.h"
#include "ui_UClDescEditor.h"


UClDescEditor::UClDescEditor(QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ui(new Ui::UClDescEditor),
    ClassDescriptionDisplay(NULL),
    ClassesListWidget(NULL)
{
    ui->setupUi(this);

    ClassDescriptionDisplay = new UClassDescriptionDisplay("",this);
    disconnect(ClassDescriptionDisplay->GetUi()->pushButtonCancel, &QPushButton::clicked, ClassDescriptionDisplay, &QWidget::close);
    connect(ClassDescriptionDisplay->GetUi()->pushButtonCancel, &QPushButton::clicked, this, &QWidget::close);

    ClassesListWidget = new UClassesListWidget(this,app);
    // Отключение popup меню и соответсвующего action
    ClassesListWidget->disable_cl_desc_popup_menu();
    // Убираем вкладку "Libs Control" т.к она не нужна
    ClassesListWidget->removeTab(2);
    ui->horizontalLayout_2->addWidget(ClassDescriptionDisplay);
    ui->horizontalLayout_2->addWidget(ClassesListWidget);

    connect(ClassesListWidget, SIGNAL(classSelectionChanged()), this, SLOT(clListItemChanged()));
}

UClDescEditor::~UClDescEditor()
{
    delete ui;
}

void UClDescEditor::clListItemChanged()
{
    ClassDescriptionDisplay->ChangeClassDescription(ClassesListWidget->selctedClass().toStdString());
}

void UClDescEditor::closeEvent(QCloseEvent *event)
{
    //int k = 9;
}


