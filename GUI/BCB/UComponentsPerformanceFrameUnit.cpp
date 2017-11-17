//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UComponentsPerformanceFrameUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentsPerformanceFrame *UComponentsPerformanceFrame;

/// Экзепляр класса приложения
extern RDK::UApplication RdkApplication;

//---------------------------------------------------------------------------
__fastcall TUComponentsPerformanceFrame::TUComponentsPerformanceFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 for(int i=0;i<Chart->SeriesCount();i++)
  Chart->Series[i]->Clear();


 MyComponentsListForm=new TUComponentsListForm(this);
 UpdateInterval=1000;
}

__fastcall TUComponentsPerformanceFrame::~TUComponentsPerformanceFrame(void)
{
 if(MyComponentsListForm)
  delete MyComponentsListForm;
}

//---------------------------------------------------------------------------
void TUComponentsPerformanceFrame::ABeforeCalculate(void)
{

}

void TUComponentsPerformanceFrame::AAfterCalculate(void)
{

}

void TUComponentsPerformanceFrame::AUpdateInterface(void)
{
 if(!Model_Check())
  return;

 int sel_index=Core_GetSelectedChannelIndex();
 RDK::UChannelProfiler* profiler=RdkApplication.GetEngineControl()->GetChannelProfiler(sel_index);
 if(!profiler)
  return;

 std::list<std::pair<std::string, RDK::UPerfomanceResults> > comp_perfomance=profiler->GetComponentsProfilerOutputData();
 std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator cI=comp_perfomance.begin(),cJ=comp_perfomance.end();

 Chart->Series[0]->Clear();
 Chart->Series[1]->Clear();

 if(ShowModeRadioGroup->ItemIndex == 0)
 {
  Chart->Series[0]->Visible=true;
  Chart->Series[1]->Visible=false;
 }
 else
 {
  Chart->Series[0]->Visible=false;
  Chart->Series[1]->Visible=true;
 }

 for(;cI != cJ;++cI)
 {
  Chart->Series[0]->AddY(cI->second.AvgDuration*1000,cI->first.c_str());
  Chart->Series[1]->AddY(cI->second.Percentage,cI->first.c_str());
 }

 if(PageControl1->ActivePage == TabSheet2)
 {
  std::list<std::pair<std::string, RDK::UPerfomanceResults> > gui_perfomance=profiler->GetGuiProfilerOutputData();
  std::list<std::pair<std::string, RDK::UPerfomanceResults> >::iterator gI=gui_perfomance.begin(),gJ=gui_perfomance.end();

  std::vector<RDK::UIVisualController*> &interfaces=RDK::UIVisualControllerStorage::InterfaceUpdaters;
  InterfacesStringGrid->RowCount=int(interfaces.size())+1;
  InterfacesStringGrid->ColCount=6;
  InterfacesStringGrid->Cells[0][0]="#";
  InterfacesStringGrid->Cells[1][0]="GUI Name";
  InterfacesStringGrid->Cells[2][0]="GUI Class Name";
  InterfacesStringGrid->Cells[3][0]="Int., ms";
  InterfacesStringGrid->Cells[4][0]="Time, ms";
  InterfacesStringGrid->Cells[5][0]="Shared, %";
  InterfacesStringGrid->ColWidths[0]=20;
  InterfacesStringGrid->ColWidths[3]=90;
  InterfacesStringGrid->ColWidths[4]=90;
  InterfacesStringGrid->ColWidths[5]=90;
  int width=InterfacesStringGrid->ClientWidth-(InterfacesStringGrid->ColWidths[0]+InterfacesStringGrid->ColWidths[3]+InterfacesStringGrid->ColWidths[4]+InterfacesStringGrid->ColWidths[5]+20);
  InterfacesStringGrid->ColWidths[1]=width/2;
  InterfacesStringGrid->ColWidths[2]=width/2;

  for(size_t i=0;i<interfaces.size();i++,++gI)
  {
   InterfacesStringGrid->Cells[0][i+1]=IntToStr(int(i));
   InterfacesStringGrid->Cells[1][i+1]=interfaces[i]->GetName().c_str();
   InterfacesStringGrid->Cells[2][i+1]=interfaces[i]->GetClassName().c_str();
   InterfacesStringGrid->Cells[3][i+1]=IntToStr(int(interfaces[i]->GetUpdateInterval()));
   InterfacesStringGrid->Cells[4][i+1]=IntToStr(int(gI->second.AvgDuration*1000));//IntToStr(int(interfaces[i]->GetUpdateTime()));
   InterfacesStringGrid->Cells[5][i+1]=FloatToStrF(gI->second.Percentage*100,ffFixed, 3,1);
  }
 }
}

// Возврат интерфейса в исходное состояние
void TUComponentsPerformanceFrame::AClearInterface(void)
{
// ClearComponents();
}

// Сохраняет параметры интерфейса в xml
void TUComponentsPerformanceFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("ShowModeRadioGroup",ShowModeRadioGroup->ItemIndex);
}

// Загружает параметры интерфейса из xml
void TUComponentsPerformanceFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 ShowModeRadioGroup->ItemIndex=xml.ReadInteger("ShowModeRadioGroup",ShowModeRadioGroup->ItemIndex);
}

// Добавляет компонент для мониторинга
void TUComponentsPerformanceFrame::AddComponent(const std::string &componentname)
{
 RDK::UChannelProfiler* pefromance=RdkApplication.GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
 pefromance->AddComponent(componentname);

 UpdateInterface();
}

// Добавляет все компоненты, содержащиеся непосредственно в выбранном компоненте
void TUComponentsPerformanceFrame::AddAllComponents(const std::string &componentname)
{
 RDK::UChannelProfiler* pefromance=RdkApplication.GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
 pefromance->AddAllComponents(componentname);

 UpdateInterface();
}

// Удаляет наблюдаемый компонент
void TUComponentsPerformanceFrame::DelComponent(const std::string &componentname)
{
 RDK::UChannelProfiler* pefromance=RdkApplication.GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
 pefromance->DelComponent(componentname);
 UpdateInterface();
}

void TUComponentsPerformanceFrame::DelComponent(std::size_t index)
{
 RDK::UChannelProfiler* pefromance=RdkApplication.GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
 pefromance->DelComponent(index);
 UpdateInterface();
}

// Удаляет все наблюдаемые компоненты
void TUComponentsPerformanceFrame::ClearComponents(void)
{
 RDK::UChannelProfiler* pefromance=RdkApplication.GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
 pefromance->DelAllComponents();
 UpdateInterface();
}

void __fastcall TUComponentsPerformanceFrame::SelectSource1Click(TObject *Sender)

{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 AddComponent(MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName());
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsPerformanceFrame::ClearAll1Click(TObject *Sender)
{
 ClearComponents();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsPerformanceFrame::AverageIntervalSpinEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 RDK::UChannelProfiler* perfomance=RdkApplication.GetEngineControl()->GetChannelProfiler(Core_GetSelectedChannelIndex());
 perfomance->SetAverageIterations(AverageIntervalSpinEdit->Value);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsPerformanceFrame::ShowModeRadioGroupClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsPerformanceFrame::SelectAll1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 ClearAll1Click(Sender);
 AddAllComponents(MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName());

// ((ScrollPager*)Chart->Tools[0])->SubChartTool->Charts[0].ITool = ((ScrollPager)Chart->Tools[0])->SubChartTool;
}
//---------------------------------------------------------------------------

