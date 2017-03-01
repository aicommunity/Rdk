#include "UPropertyXMLWidget.h"
#include "ui_UPropertyXMLWidget.h"

UPropertyXMLWidget::UPropertyXMLWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UPropertyXMLWidget)
{
    ui->setupUi(this);
    isInit = false;
    connect(ui->pushButtonSetProperty, SIGNAL(pressed()), this, SLOT(setProperty()));
    connect(ui->pushButtonReloadProperty, SIGNAL(pressed()), this, SLOT(reloadProperty()));
    connect(ui->pushButtonDefaultAllProperty, SIGNAL(pressed()), this, SLOT(defaultAllProperty()));
    connect(ui->pushButtonCloseWindow, SIGNAL(pressed()), this, SLOT(reject()));
}

UPropertyXMLWidget::~UPropertyXMLWidget()
{
    delete ui;
}

void UPropertyXMLWidget::initWidget(QString longName, int mask)
{
    isInit = true;
    componentName = longName;
    propertyMask = mask;

    QString propertyName;
    switch(propertyMask)
    {
    case(ptPubState):
        propertyName = "state XML";
        break;

    case(ptPubParameter):
        propertyName = "parameters XML";
        break;

    case(ptPubInput):
        propertyName = "inputs XML";
        break;

    case(ptPubOutput):
        propertyName = "outputs XML";
        break;

    default:
        propertyName = "undefined property XML";
    }
    setWindowTitle(componentName + " " + propertyName);

    reloadProperty();
}

void UPropertyXMLWidget::setProperty()
{
    if(!isInit) return;
    Model_SetComponentProperties(componentName.toLocal8Bit(),
                                 ui->plainTextEditPropertyXml->toPlainText().toLocal8Bit());
}

void UPropertyXMLWidget::reloadProperty()
{
    if(!isInit) return;

    const char *stringBuff = Model_GetComponentProperties(componentName.toLocal8Bit(), propertyMask);
    ui->plainTextEditPropertyXml->clear();
    ui->plainTextEditPropertyXml->insertPlainText(stringBuff);
    Engine_FreeBufString(stringBuff);
}

void UPropertyXMLWidget::defaultAllProperty()
{
    if(!isInit) return;
    Env_Default(componentName.toLocal8Bit(), false);
    //Env_Reset(selectedComponentLongName.toLocal8Bit()); //тут вопрос о сбросе вложенных компонент (как в билдере)
    reloadProperty();
}
