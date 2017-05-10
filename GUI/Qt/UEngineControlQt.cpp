#include "UEngineControlQt.h"
#include <UVisualControllerWidget.h>

UEngineControlQt::UEngineControlQt(QObject *parent):
    QObject(parent)
{
    timer = new QBasicTimer();
    //connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
    UpdateInterval = 30;
}

UEngineControlQt::~UEngineControlQt()
{
    timer->stop();
    delete timer;
}

void UEngineControlQt::StartChannel(int channel_index)
{
    RDK::UEngineControl::StartChannel(channel_index);
    switch(GetThreadMode())
    {
    case 0:
        UVisualControllerWidget::CalculationModeFlag=true;
        timer->start(UpdateInterval, this);
        break;

    case 1:
        UVisualControllerWidget::CalculationModeFlag=true;
        timer->start(UpdateInterval, this);
        break;
    }
}

void UEngineControlQt::PauseChannel(int channel_index)
{
    RDK::UEngineControl::PauseChannel(channel_index);
    switch(GetThreadMode())
    {
    case 0:
        UVisualControllerWidget::CalculationModeFlag=false;
        timer->stop();
        break;

    case 1:
        UVisualControllerWidget::CalculationModeFlag=false;
        timer->stop();
        break;
    }
}

void UEngineControlQt::timerEvent(QTimerEvent *)
{
    Application->GetEngineControl()->TimerExecute();
}