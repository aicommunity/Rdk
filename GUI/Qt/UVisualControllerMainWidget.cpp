#include "UVisualControllerMainWidget.h"

// Флаг, сообщающий что идет расчет
RDK::UELockVar<bool> UVisualControllerMainWidget::CalculationModeFlag(false);

UVisualControllerMainWidget::UVisualControllerMainWidget(QWidget *parent, RDK::UApplication *app): QMainWindow(parent)
{
    application = app;
    UpdateInterfaceFlag=false;
    AlwaysUpdateFlag=false;
    UpdateInterval=1000; // по умолчанию, интервал обновления виджета 1с.
    CalculationStepUpdatedFlag=false;
    CheckModelFlag=true;

    /// Время последнего обновления
    LastUpdateTime=0;

    /// Время, потраченное на обновление интерфейса
    UpdateTime=0;

    RDK::UIVisualControllerStorage::AddInterface(this);
}

UVisualControllerMainWidget::~UVisualControllerMainWidget()
{
    RDK::UIVisualControllerStorage::DelInterface(this);
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после загрузки проекта
void UVisualControllerMainWidget::AfterLoadProject(void)
{
    try
    {
        AAfterLoadProject();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-AfterLoadProject Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-AfterLoadProject Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-AfterLoadProject Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::AAfterLoadProject(void)
{
//  throw RDK::UException();
}

/// Метод, вызываемый перед закрытием проекта
/// \details Вызывает метод ABeforeCloseProject() в блоке обработки исключений
void UVisualControllerMainWidget::BeforeCloseProject(void)
{
 try
 {
     ABeforeCloseProject();
 }
 catch (RDK::UException &exception)
 {
     Log_LogMessage(exception.GetType(), (std::string("Core-BeforeCloseProject Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
 }
 catch (std::exception &exception)
 {
     Log_LogMessage(RDK_EX_ERROR, (std::string("Core-BeforeCloseProject Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
 }
 /*catch(Exception &exception)
 {
     Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeCloseProject Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
 }*/
}

void UVisualControllerMainWidget::ABeforeCloseProject(void)
{

}

// Метод, вызываемый перед сбросом модели
void UVisualControllerMainWidget::BeforeReset(void)
{
    try
    {
        ABeforeReset();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void UVisualControllerMainWidget::AfterReset(void)
{
    try
    {
        LastUpdateTime=0;
        UpdateTime=0;
        AAfterReset();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-AfterReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void UVisualControllerMainWidget::BeforeCalculate(void)
{
    try
    {
        ResetCalculationStepUpdatedFlag();
        ABeforeCalculate();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeCalculate Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void UVisualControllerMainWidget::AfterCalculate(void)
{
    try
    {
        AAfterCalculate();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-BeforeReset Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::AAfterCalculate(void)
{
}

// Обновление интерфейса
void UVisualControllerMainWidget::UpdateInterface(bool force_update)
{
    if(UpdateInterfaceFlag)
     return;
    if(UpdateInterval.Get() == 0 && !force_update) return;
    unsigned long long current_time=RDK::GetCurrentStartupTime();
    try
    {
        //  UpdateTime=RDK::GetCurrentStartupTime();
        if(!force_update)
        {
            //update();
            //UpdateControlState();
            //if(!Showing && !AlwaysUpdateFlag)
            if(!isVisible() && !AlwaysUpdateFlag)
            {
                //UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
                return;
            }
            //не обновляется если отец невидим и е проставлен AlwaysUpdateFlag
            if(!parentWidget() || (!AlwaysUpdateFlag && !(parentWidget()->isVisible())) || (UpdateInterval<0 && CalculationModeFlag))
            {
                //UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
                return;
            }
            if(UpdateInterval>0 && CalculationModeFlag)
            {
                unsigned long long curr_time=RDK::GetCurrentStartupTime();
                if(curr_time-LastUpdateTime<(unsigned long long)((UpdateTime>UpdateInterval)?UpdateTime:UpdateInterval))
                {
                    //UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
                    return;
                }

               /* if((curr_time-LastUpdateTime) > ((last_update_duration<10000)?last_update_duration:10000))
                {
                    return;
                }*/
                if(GetCalculationStepUpdatedFlag() == true)
                {
                   // UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
                    return;
                }
                else
                    SetCalculationStepUpdatedFlag();

                LastUpdateTime=curr_time;
            }
        }

        if(!Core_IsChannelInit())
        {
           // UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
            return;
        }

        if(CheckModelFlag && !Model_Check())
        {
           // UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
            return;
        }
        UpdateInterfaceFlag=true;
 //       current_time=RDK::GetCurrentStartupTime();
        AUpdateInterface();
    }
    catch (RDK::UException &exception)
    {
        UpdateInterfaceFlag=false;
        Log_LogMessage(exception.GetType(), (std::string("Core-UpdateInterface Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        UpdateInterfaceFlag=false;
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-UpdateInterface Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        UpdateInterfaceFlag=false;
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-UpdateInterface Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
    catch(...)
    {
        UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
        UpdateInterfaceFlag=false;
        throw;
    }
        UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
        UpdateInterfaceFlag=false;
}

void UVisualControllerMainWidget::AUpdateInterface(void)
{

}

// Возврат интерфейса в исходное состояние
void UVisualControllerMainWidget::ClearInterface(void)
{
    try
    {
        AClearInterface();
        //ComponentControlName.clear();
        //Длинное имя управляемого компонента модели (опционально)
        //std::string ComponentControlName; - удалено из .h
        UpdateInterface(true);
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-ClearInterface Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-ClearInterface Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-ClearInterface Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::AClearInterface(void)
{

}

// Возвращает уникальное имя интерфейса
std::string UVisualControllerMainWidget::GetName(void)
{
    return accessibleName().toLocal8Bit().constData();
}

// Возвращает имя класса интерфейса
std::string UVisualControllerMainWidget::GetClassName(void)
{
    return typeid(this).name();
}

std::string UVisualControllerMainWidget::CalcFullName(void)
{
    std::string full_name(GetName());
    QWidget* own=dynamic_cast<QWidget*>(parent());
    while(own != NULL)
    {
        std::string own_name=own->accessibleName().toLocal8Bit().constData();
        if(!own_name.empty())
         full_name=own_name+"."+full_name;
        own=dynamic_cast<QWidget*>(own->parent());
    }
    return full_name;
}


// Возвращает интервал обновления интерфейса
long UVisualControllerMainWidget::GetUpdateInterval(void)
{
    return UpdateInterval;
}

// Задает интервал обновления интерфейса
bool UVisualControllerMainWidget::SetUpdateInterval(long value)
{
    if(value<0)
        return false;

    UpdateInterval=value;
    return true;
}


// Возвращает флаг разрешения обновления интерфейса даже если он не виден
bool UVisualControllerMainWidget::GetAlwaysUpdateFlag(void)
{
    return AlwaysUpdateFlag;
}

// Сохраняет параметры интерфейса в xml
void UVisualControllerMainWidget::SaveParameters(RDK::USerStorageXML &xml)
{
    try
    {
        xml.SelectNodeForce(CalcFullName());
        ASaveParameters(xml);
        SaveFormPosition(xml);
        //xml.WriteString("ComponentControlName",ComponentControlName);
        xml.WriteInteger("UpdateInterval",UpdateInterval);
        xml.WriteBool("AlwaysUpdateFlag",AlwaysUpdateFlag);

        //xml.SelectUp();
        xml.SelectUp();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-SaveParameters Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-SaveParameters Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-SaveParameters Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::ASaveParameters(RDK::USerStorageXML &xml)
{

}

void UVisualControllerMainWidget::SaveFormPosition(RDK::USerStorageXML &xml)
{
    xml.SelectNodeForce("FormPosition");

    QPoint topLeft = mapToGlobal(rect().topLeft());
    QPoint bottomRight = mapToGlobal(rect().bottomRight());
    xml.WriteInteger("Left",topLeft.x());
    xml.WriteInteger("Top",topLeft.y());
    xml.WriteInteger("Width",bottomRight.x()-topLeft.x()+1);
    xml.WriteInteger("Height",bottomRight.y()-topLeft.y()+1);
    xml.WriteBool("Visible",isVisible());

    Qt::WindowStates win_state = windowState();
 //   Qt::WidgetStates widget_state = widgetState();
    xml.WriteInteger("WindowState",int(win_state));
    xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void UVisualControllerMainWidget::LoadParameters(RDK::USerStorageXML &xml)
{
    try
    {
        xml.SelectNodeForce(CalcFullName());
        UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
        AlwaysUpdateFlag=xml.ReadBool("AlwaysUpdateFlag",false);
        LoadFormPosition(xml);
        ALoadParameters(xml);
        //xml.SelectUp();
        xml.SelectUp();
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("Core-LoadParameters Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("Core-LoadParameters Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    /*catch(Exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-LoadParameters Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+AnsiString(exception.Message).c_str()).c_str());
    }*/
}

void UVisualControllerMainWidget::ALoadParameters(RDK::USerStorageXML &xml)
{
}

void UVisualControllerMainWidget::LoadFormPosition(RDK::USerStorageXML &xml)
{
    xml.SelectNodeForce("FormPosition");
 //   QRect Screen = QApplication::desktop()->availableGeometry();
    int num_screens = QApplication::desktop()->numScreens();
    std::vector<QRect> screen_sizes(num_screens);
    for(int i=0;i<int(screen_sizes.size());i++)
       screen_sizes[i] = QApplication::desktop()->availableGeometry(i);


    int value_x=0, value_y=0;

    value_x=xml.ReadInteger("Left",x());
    value_y=xml.ReadInteger("Top",y());

    int screen_index(0);
    for(int i=0;i<int(screen_sizes.size());i++)
    {
      if(value_x>=screen_sizes[i].left() && value_x <= screen_sizes[i].left()+screen_sizes[i].width() && value_y>=screen_sizes[i].top() && value_y <= screen_sizes[i].top()+screen_sizes[i].height())
      {
       screen_index = i;
       break;
      }
    }

    int width   = xml.ReadInteger("Width", this->width());
    int height  = xml.ReadInteger("Height", this->height());

    if(value_y<screen_sizes[screen_index].top() || value_y>= screen_sizes[screen_index].top()+screen_sizes[screen_index].height())
       value_x=screen_sizes[screen_index].left();

    if(value_y<screen_sizes[screen_index].top() || value_y>= screen_sizes[screen_index].top()+screen_sizes[screen_index].height())
       value_y=screen_sizes[screen_index].top();

    if(width>screen_sizes[screen_index].width())
       width = screen_sizes[screen_index].width();

    if(height>screen_sizes[screen_index].height())
       height = screen_sizes[screen_index].height();

    setGeometry(value_x, value_y, width, height);
    resize(width,height);

    // Если это не основная форма (основная форма всегда видна)
    if(!(accessibleName()=="UGEngineControllWidget"))
    {
        setVisible(xml.ReadBool("Visible", isVisible()));
        setWindowState((Qt::WindowState)xml.ReadInteger("WindowState",(int)windowState()));
    }

    xml.SelectUp();
}

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
/*const std::string& UVisualControllerMainWidget::GetComponentControlName(void) const
{
    return ComponentControlName;
}*/

/*bool UVisualControllerMainWidget::SetComponentControlName(const std::string& name)
{
    if(ComponentControlName == name)
        return true;

    ComponentControlName=name;
    UpdateInterface();
    return true;
}*/

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void UVisualControllerMainWidget::ResetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=false;
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void UVisualControllerMainWidget::SetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=true;
}

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
bool UVisualControllerMainWidget::GetCalculationStepUpdatedFlag(void)
{
    return CalculationStepUpdatedFlag;
}

/// Возвращает время обновления интерфейса (мс)
unsigned long long UVisualControllerMainWidget::GetUpdateTime(void)
{
    return UpdateTime;
}

void UVisualControllerMainWidget::unInit()
{

}

// -----------------------------
