#include "UVisualControllerWidget.h"

// Флаг, сообщающий что идет расчет
RDK::UELockVar<bool> UVisualControllerWidget::CalculationModeFlag(false);

UVisualControllerWidget::UVisualControllerWidget(QWidget *parent, RDK::UApplication *app):QWidget(parent)
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

UVisualControllerWidget::~UVisualControllerWidget()
{
    RDK::UIVisualControllerStorage::DelInterface(this);
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после загрузки проекта
void UVisualControllerWidget::AfterLoadProject(void)
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

void UVisualControllerWidget::AAfterLoadProject(void)
{
//  throw RDK::UException();
}

/// Метод, вызываемый перед закрытием проекта
/// \details Вызывает метод ABeforeCloseProject() в блоке обработки исключений
void UVisualControllerWidget::BeforeCloseProject(void)
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

void UVisualControllerWidget::ABeforeCloseProject(void)
{

}

// Метод, вызываемый перед сбросом модели
void UVisualControllerWidget::BeforeReset(void)
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

void UVisualControllerWidget::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void UVisualControllerWidget::AfterReset(void)
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

void UVisualControllerWidget::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void UVisualControllerWidget::BeforeCalculate(void)
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

void UVisualControllerWidget::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void UVisualControllerWidget::AfterCalculate(void)
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

void UVisualControllerWidget::AAfterCalculate(void)
{
}

// Обновление интерфейса
void UVisualControllerWidget::UpdateInterface(bool force_update)
{
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

void UVisualControllerWidget::AUpdateInterface(void)
{

}

// Возврат интерфейса в исходное состояние
void UVisualControllerWidget::ClearInterface(void)
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

void UVisualControllerWidget::AClearInterface(void)
{

}

// Возвращает уникальное имя интерфейса
std::string UVisualControllerWidget::GetName(void)
{
    return accessibleName().toLocal8Bit().constData();
}

// Возвращает имя класса интерфейса
std::string UVisualControllerWidget::GetClassName(void)
{
    return typeid(this).name();
}

std::string UVisualControllerWidget::CalcFullName(void)
{
 std::string full_name(GetName());
 if(full_name.empty())
  full_name="NoName";
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
long UVisualControllerWidget::GetUpdateInterval(void)
{
    return UpdateInterval;
}

// Задает интервал обновления интерфейса
bool UVisualControllerWidget::SetUpdateInterval(long value)
{
    if(value<0)
        return false;

    UpdateInterval=value;
    return true;
}


// Возвращает флаг разрешения обновления интерфейса даже если он не виден
bool UVisualControllerWidget::GetAlwaysUpdateFlag(void)
{
    return AlwaysUpdateFlag;
}

// Сохраняет параметры интерфейса в xml
void UVisualControllerWidget::SaveParameters(RDK::USerStorageXML &xml)
{
    try
    {
        //if(!Owner)
        //    return;
        //xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
        xml.SelectNodeForce(CalcFullName());
        ASaveParameters(xml);
        //xml.WriteInteger("UpdateInterval",UpdateInterval);
        //xml.WriteString("ComponentControlName",ComponentControlName);
        //xml.WriteBool("AlwaysUpdateFlag",AlwaysUpdateFlag);

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

void UVisualControllerWidget::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void UVisualControllerWidget::LoadParameters(RDK::USerStorageXML &xml)
{
    try
    {
        //if(!Owner)
        //    return;
        //xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
        xml.SelectNodeForce(CalcFullName());
        //ComponentControlName=xml.ReadString("ComponentControlName","");
        //UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
        //AlwaysUpdateFlag=xml.ReadBool("AlwaysUpdateFlag",false);*/
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

void UVisualControllerWidget::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
/*const std::string& UVisualControllerWidget::GetComponentControlName(void) const
{
    return ComponentControlName;
}*/

/*bool UVisualControllerWidget::SetComponentControlName(const std::string& name)
{
    if(ComponentControlName == name)
        return true;

    ComponentControlName=name;
    UpdateInterface();
    return true;
}*/

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void UVisualControllerWidget::ResetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=false;
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void UVisualControllerWidget::SetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=true;
}

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
bool UVisualControllerWidget::GetCalculationStepUpdatedFlag(void)
{
    return CalculationStepUpdatedFlag;
}

/// Возвращает время обновления интерфейса (мс)
unsigned long long UVisualControllerWidget::GetUpdateTime(void)
{
    return UpdateTime;
}

void UVisualControllerWidget::unInit()
{

}

// -----------------------------
