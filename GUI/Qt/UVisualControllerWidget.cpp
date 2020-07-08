#include "UVisualControllerWidget.h"

// ����, ���������� ��� ���� ������
RDK::UELockVar<bool> UVisualControllerWidget::CalculationModeFlag(false);

UVisualControllerWidget::UVisualControllerWidget(QWidget *parent, RDK::UApplication *app):QWidget(parent)
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

UVisualControllerWidget::~UVisualControllerWidget()
{
    RDK::UIVisualControllerStorage::DelInterface(this);
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� �������� �������
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

/// �����, ���������� ����� ��������� �������
/// \details �������� ����� ABeforeCloseProject() � ����� ��������� ����������
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

// �����, ���������� ����� ������� ������
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

// �����, ���������� ����� ������ ������
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

// �����, ���������� ����� ����� �������
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

// �����, ���������� ����� ���� �������
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

// ���������� ����������
void UVisualControllerWidget::UpdateInterface(bool force_update)
{
    if(UpdateInterval.Get() == 0 && !force_update) return;
    unsigned long long current_time=RDK::GetCurrentStartupTime();
    try
    {
        //  UpdateTime=RDK::GetCurrentStartupTime();
        if(!force_update)
        {
            UpdateTime=0;
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
                if(curr_time-LastUpdateTime<(unsigned long long)(UpdateInterval))
                {
                    //UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),current_time);
                    return;
                }

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

// ������� ���������� � �������� ���������
void UVisualControllerWidget::ClearInterface(void)
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

void UVisualControllerWidget::AClearInterface(void)
{

}

// ���������� ���������� ��� ����������
std::string UVisualControllerWidget::GetName(void)
{
    return accessibleName().toLocal8Bit().constData();
}

// ���������� ��� ������ ����������
std::string UVisualControllerWidget::GetClassName(void)
{
    return typeid(this).name();
}

std::string UVisualControllerWidget::CalcFullName(void)
{
 return GetName(); // TODO: ��� ���� ��������� ��� �� ����� �����������
}


// ���������� �������� ���������� ����������
long UVisualControllerWidget::GetUpdateInterval(void)
{
    return UpdateInterval;
}

// ������ �������� ���������� ����������
bool UVisualControllerWidget::SetUpdateInterval(long value)
{
    if(value<0)
        return false;

    UpdateInterval=value;
    return true;
}


// ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
bool UVisualControllerWidget::GetAlwaysUpdateFlag(void)
{
    return AlwaysUpdateFlag;
}

// ��������� ��������� ���������� � xml
void UVisualControllerWidget::SaveParameters(RDK::USerStorageXML &)
{
    try
    {
        /*if(!Owner)
            return;
        xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
        xml.SelectNodeForce(GetName());*/
        ASaveParameters();
        /*xml.WriteInteger("UpdateInterval",UpdateInterval);
        //xml.WriteString("ComponentControlName",ComponentControlName);
        xml.WriteBool("AlwaysUpdateFlag",AlwaysUpdateFlag);

        xml.SelectUp();
        xml.SelectUp();*/
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

void UVisualControllerWidget::ASaveParameters()
{

}

// ��������� ��������� ���������� �� xml
void UVisualControllerWidget::LoadParameters(RDK::USerStorageXML &)
{
    try
    {
        /*if(!Owner)
            return;
        xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
        xml.SelectNodeForce(GetName());
        ComponentControlName=xml.ReadString("ComponentControlName","");
        UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
        AlwaysUpdateFlag=xml.ReadBool("AlwaysUpdateFlag",false);*/
        ALoadParameters();
        /*xml.SelectUp();
        xml.SelectUp();*/
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

void UVisualControllerWidget::ALoadParameters()
{
}

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
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

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
void UVisualControllerWidget::ResetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=false;
}

/// ���������� ���� ��������� ����������� � ���� �������� �����
void UVisualControllerWidget::SetCalculationStepUpdatedFlag(void)
{
    CalculationStepUpdatedFlag=true;
}

/// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
bool UVisualControllerWidget::GetCalculationStepUpdatedFlag(void)
{
    return CalculationStepUpdatedFlag;
}

/// ���������� ����� ���������� ���������� (��)
unsigned long long UVisualControllerWidget::GetUpdateTime(void)
{
    return UpdateTime;
}

void UVisualControllerWidget::unInit()
{

}

// -----------------------------
