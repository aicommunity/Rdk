#include "ULoggerWidget.h"

ULoggerWidget::ULoggerWidget(QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent)
{
    setAccessibleName("ULoggerWidget");
    UpdateInterval = 200;
    application = app;
    CheckModelFlag = false; // for updating logger when model not loaded yet

    layout = new QVBoxLayout(this);
    textEdit = new QPlainTextEdit(this);
    textEdit->clear();
    layout->addWidget(textEdit);
    layout->setMargin(0);

    UpdateInterface(true);
}

ULoggerWidget::~ULoggerWidget()
{
    delete textEdit;
}

void ULoggerWidget::AUpdateInterface()
{
    if(!application) return;
    std::list<std::string> log = application->GetEngineControl()->GetEngineStateThread()->ReadGuiUnsentLog();
    for(std::list<std::string>::iterator i = log.begin(); i != log.end(); ++i)
        textEdit->appendPlainText(QString::fromLocal8Bit(i->c_str()));
}
