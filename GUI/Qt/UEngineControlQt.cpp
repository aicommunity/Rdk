#include "UEngineControlQt.h"
#ifdef QT_GUI_LIB
#include <UVisualControllerWidget.h>
#endif
#include "../../Deploy/Include/rdk_logging.h"

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
        #ifdef QT_GUI_LIB
        UVisualControllerWidget::CalculationModeFlag=true;
        #endif
        // Оптимизация: увеличен минимальный интервал с 1 мс до 16 мс для снижения нагрузки на UI поток
        // 16 мс соответствует ~60 FPS, что достаточно для плавного обновления интерфейса
        timer->start(16, this);
        break;

    case 1:
        #ifdef QT_GUI_LIB
        UVisualControllerWidget::CalculationModeFlag=true;
        #endif
        timer->start(GetApplication()->GetProjectConfig().MTUpdateInterfaceInterval, this);
        break;
    }
}

void UEngineControlQt::PauseChannel(int channel_index)
{
    RDK::UEngineControl::PauseChannel(channel_index);
    switch(GetThreadMode())
    {
    case 0:
        #ifdef QT_GUI_LIB
        UVisualControllerWidget::CalculationModeFlag=false;
        #endif
        timer->stop();
        break;

    case 1:
        #ifdef QT_GUI_LIB
        UVisualControllerWidget::CalculationModeFlag=false;
        #endif
        timer->stop();
        break;
    }
}

void UEngineControlQt::timerEvent(QTimerEvent *)
{
 try
 {
  TimerExecute();
 }
 catch(RDK::UException &ex)
 {
  RDK::Logging::ChannelLog(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControlQt::TimerTimer - ")+ex.what()).c_str());
 }
 catch(std::exception &ex)
 {
  RDK::Logging::ChannelLog(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControlQt::TimerTimer - ")+ex.what()).c_str());
 }
 catch(...)
 {
  RDK::Logging::ChannelLog(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "UEngineControlQt::TimerTimer - unhandled exception");
 }
}
