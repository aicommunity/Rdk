#include "UVisualControllerMainWidget.h"

// ����, ���������� ��� ���� ������
RDK::UELockVar<bool> UVisualControllerMainWidget::CalculationModeFlag(false);

UVisualControllerMainWidget::UVisualControllerMainWidget(QWidget *parent, RDK::UApplication *app): QMainWindow(parent)
{
    application = app;
    UpdateInterfaceFlag=false;
    AlwaysUpdateFlag=false;
    UpdateInterval=1000; // �� ���������, �������� ���������� ������� 1�.
    CalculationStepUpdatedFlag=false;
    CheckModelFlag=true;

    /// ����� ���������� ����������
    LastUpdateTime=0;

    /// �����, ����������� �� ���������� ����������
    UpdateTime=0;

    RDK::UIVisualControllerStorage::AddInterface(this);
}

UVisualControllerMainWidget::~UVisualControllerMainWidget()
{
    RDK::UIVisualControllerStorage::DelInterface(this);
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� �������� �������
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

/// �����, ���������� ����� ��������� �������
/// \details �������� ����� ABeforeCloseProject() � ����� ��������� ����������
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

// �����, ���������� ����� ������� ������
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

// �����, ���������� ����� ������ ������
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

// �����, ���������� ����� ����� �������
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

// �����, ���������� ����� ���� �������
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

// ���������� ����������
void UVisualControllerMainWidget::UpdateInterface(bool force_update)
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
            //�� ����������� ���� ���� ������� � � ���������� AlwaysUpdateFlag
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

// ������� ���������� � �������� ���������
void UVisualControllerMainWidget::ClearInterface(void)
{
    try
    {
        AClearInterface();
        //ComponentControlName.clear();
        //������� ��� ������������ ���������� ������ (�����������)
        //std::string ComponentControlName; - ������� �� .h
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

// ���������� ���������� ��� ����������
std::string UVisualControllerMainWidget::GetName(void)
{
    return accessibleName().toLocal8Bit().constData();
}

// ���������� ��� ������ ����������
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


// ���������� �������� ���������� ����������
long UVisualControllerMainWidget::GetUpdateInterval(void)
{
    return UpdateInterval;
}

// ������ �������� ���������� ����������
bool UVisualControllerMainWidget::SetUpdateInterval(long value)
{
    if(value<0)
        return false;

    UpdateInterval=value;
    return true;
}


// ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
bool UVisualControllerMainWidget::GetAlwaysUpdateFlag(void)
{
    return AlwaysUpdateFlag;
}

// ��������� ��������� ���������� � xml
void UVisualControllerMainWidget::SaveParameters(RDK::USerStorageXML &xml)
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

void UVisualControllerMainWidget::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void UVisualControllerMainWidget::LoadParameters(RDK::USerStorageXML &xml)
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

void UVisualControllerMainWidget::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
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

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
void UVisualControllerMainWidget::ResetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=false;
}

/// ���������� ���� ��������� ����������� � ���� �������� �����
void UVisualControllerMainWidget::SetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=true;
}

/// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
bool UVisualControllerMainWidget::GetCalculationStepUpdatedFlag(void)
{
    return CalculationStepUpdatedFlag;
}

/// ���������� ����� ���������� ���������� (��)
unsigned long long UVisualControllerMainWidget::GetUpdateTime(void)
{
    return UpdateTime;
}

void UVisualControllerMainWidget::unInit()
{

}

// -----------------------------
