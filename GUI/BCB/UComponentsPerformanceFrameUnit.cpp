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
//---------------------------------------------------------------------------
__fastcall TUComponentsPerformanceFrame::TUComponentsPerformanceFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 for(int i=0;i<Chart->SeriesCount();i++)
  Chart->Series[i]->Clear();


 MyComponentsListForm=new TUComponentsListForm(this);
 UpdateInterval=100;
 AverageIterations=10;
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

 if(int(ComponentData.size())>=AverageIterations && AverageIterations>0)
  ComponentData.erase(ComponentData.begin());

 if(int(InterfaceData.size())>=AverageIterations && AverageIterations>0)
  InterfaceData.erase(InterfaceData.begin());

 unsigned long long model_time=Model_GetFullStepDuration("");
 unsigned long long sum=0;
 unsigned long long ext_gui=Model_GetInterstepsInterval("");
 unsigned long long gui_update=RDK::UIVisualControllerStorage::GetUpdateTime();

 std::vector<long long> comp_time;
 size_t last_comps_index=ComponentNames.size();

 comp_time.resize(ComponentNames.size()+5);
 for(size_t i=0;i<ComponentNames.size();i++)
 {
  try
  {
   sum+=comp_time[i]=Model_GetFullStepDuration(ComponentNames[i].c_str());
  }
  catch(RDK::UContainer::EComponentNameNotExist &exception)
  {
   ComponentNames.erase(ComponentNames.begin()+i);
   return;
  }
 }


 if(ComponentNames.size()>0)
  comp_time[last_comps_index]=model_time-sum;
 else
  comp_time[last_comps_index]=0;



 comp_time[last_comps_index+1]=model_time;
 comp_time[last_comps_index+2]=gui_update;
 comp_time[last_comps_index+3]=ext_gui;
 comp_time[last_comps_index+4]=ext_gui+model_time;

 ComponentData.push_back(comp_time);

 std::vector<long long> average;

 average.assign(ComponentNames.size()+5,0);
 for(size_t i=0;i<average.size();i++)
 {
  for(size_t j=0;j<ComponentData.size();j++)
  {
   if(ComponentData[j].size() > i)
	average[i]+=ComponentData[j][i];
  }
  if(ComponentData.size()>0)
   average[i]/=ComponentData.size();
 }

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

 if(int(ComponentData.size())<AverageIterations)
  return;

 for(size_t i=0;i<ComponentNames.size();i++)
 {
  std::string legend=ComponentNames[i];

  if(legend.size()>10)
  {
   size_t n=legend.find_last_of('.');
   if(n != std::string::npos)
    legend=legend.substr(n+1);

   size_t j=0;
   for(;j<legend.size();j++)
	if(j>0 && !(j%8) && legend.size()-j>3)
	{
	 legend.insert(j,"\r\n"); j+=2;
	}
  }
  Chart->Series[0]->AddY(average[i],legend.c_str());
  if(model_time && average[last_comps_index+1]>0)
   Chart->Series[1]->AddY((average[i]*100.0)/average[last_comps_index+1],legend.c_str());
  else
   Chart->Series[1]->AddY(0,legend.c_str());

 }

 if(ComponentNames.size()>0)
 {
  Chart->Series[0]->AddY(average[last_comps_index],"Others");
  if(model_time && average[last_comps_index+1]>0)
   Chart->Series[1]->AddY(((average[last_comps_index])*100.0)/average[last_comps_index+1],"Others");
  else
   Chart->Series[1]->AddY(0,"Others");
 }

  Chart->Series[0]->AddY(average[last_comps_index+1],"Model");
  Chart->Series[1]->AddY(100,"Model");

  Chart->Series[0]->AddY(average[last_comps_index+2],"GUI Upd");
  Chart->Series[0]->AddY(average[last_comps_index+3],"Ext. GUI");
  Chart->Series[0]->AddY(average[last_comps_index+4],"Full step");

 std::vector<long long> interface_time;
 long long sum_average(0);
 interface_time.resize(RDK::UIVisualControllerStorage::InterfaceUpdaters.size());
 std::vector<RDK::UIVisualController*> &interfaces=RDK::UIVisualControllerStorage::InterfaceUpdaters;
  for(size_t i=0;i<interfaces.size();i++)
  {
   interface_time[i]=interfaces[i]->GetUpdateTime();
  }
 InterfaceData.push_back(interface_time);
 average.assign(interfaces.size(),0);
 for(size_t i=0;i<average.size();i++)
 {
  for(size_t j=0;j<InterfaceData.size();j++)
  {
   if(InterfaceData[j].size() > i)
	average[i]+=InterfaceData[j][i];
  }
  if(InterfaceData.size()>0)
   average[i]/=InterfaceData.size();
  sum_average+=average[i];
 }

 if(PageControl1->ActivePage == TabSheet2)
 {
  InterfacesStringGrid->RowCount=int(RDK::UIVisualControllerStorage::InterfaceUpdaters.size())+1;
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

  for(size_t i=0;i<interfaces.size();i++)
  {
   InterfacesStringGrid->Cells[0][i+1]=IntToStr(int(i));
   InterfacesStringGrid->Cells[1][i+1]=interfaces[i]->GetName().c_str();
   InterfacesStringGrid->Cells[2][i+1]=interfaces[i]->GetClassName().c_str();
   InterfacesStringGrid->Cells[3][i+1]=IntToStr(int(interfaces[i]->GetUpdateInterval()));
   InterfacesStringGrid->Cells[4][i+1]=IntToStr(int(average[i]));//IntToStr(int(interfaces[i]->GetUpdateTime()));
   if(sum_average>0)
	InterfacesStringGrid->Cells[5][i+1]=FloatToStrF(average[i]*100.0/sum_average,ffFixed, 3,1);
   else
	InterfacesStringGrid->Cells[5][i+1]="";
  }
 }
}

// ������� ���������� � �������� ���������
void TUComponentsPerformanceFrame::AClearInterface(void)
{
 ClearComponents();
}

// ��������� ��������� ���������� � xml
void TUComponentsPerformanceFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("AverageIterations",AverageIterations);
 xml.WriteInteger("ShowModeRadioGroup",ShowModeRadioGroup->ItemIndex);
 xml.WriteInteger("AverageIntervalSpinEdit",AverageIntervalSpinEdit->Value);

 xml.SelectNodeForce("Components");
 xml.DelNodeInternalContent();
 for(size_t i=0;i<ComponentNames.size();i++)
 {
  xml.WriteString(RDK::sntoa(i),ComponentNames[i]);
 }
 xml.SelectUp();
}

// ��������� ��������� ���������� �� xml
void TUComponentsPerformanceFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 ClearComponents();
 ComponentData.clear();

 xml.SelectNodeForce("Components");
 for(int i=0;i<xml.GetNumNodes();i++)
 {
  std::string name=xml.ReadString(i,"");
  ComponentNames.push_back(name);
 }
 xml.SelectUp();

 AverageIterations=xml.ReadInteger("AverageIterations",AverageIterations);
 ShowModeRadioGroup->ItemIndex=xml.ReadInteger("ShowModeRadioGroup",ShowModeRadioGroup->ItemIndex);
 AverageIntervalSpinEdit->Value=xml.ReadInteger("AverageIntervalSpinEdit",AverageIntervalSpinEdit->Value);
}
/*
// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void TUComponentsPerformanceFrame::LoadFromIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 TStringList* Strings=new TStringList;
 ini->ReadSection(section, Strings);
 ClearComponents();

 for(int i=0;i<Strings->Count;i++)
 {
  AnsiString str=(*Strings)[i];
  ComponentNames.push_back(str.c_str());
 }

 delete Strings;
} */

// ���������� ����� ����������� ���������
std::size_t TUComponentsPerformanceFrame::GetNumComponents(void) const
{
 return ComponentNames.size();
}

// ��������� ��������� ��� �����������
void TUComponentsPerformanceFrame::AddComponent(const std::string &componentname)
{
 ComponentNames.push_back(componentname);
 UpdateInterface();
}

// ��������� ��� ����������, ������������ ��������������� � ��������� ����������
void TUComponentsPerformanceFrame::AddAllComponents(const std::string &componentname)
{
 int numcomponents=Model_GetNumComponents(componentname.c_str());

 std::vector<int> ids;
 ids.resize(numcomponents);
 Model_GetComponentsList(componentname.c_str(), &ids[0]);
 std::vector<std::string> names;
 names.resize(ids.size());

 const char* pcomponentid=Model_GetComponentLongId(componentname.c_str());
 std::string componentid;
 if(pcomponentid)
  componentid=pcomponentid;
 Engine_FreeBufString(pcomponentid);

 for(size_t i=0;i<ids.size();i++)
 {
  const char *pname=0;
  if(!componentid.empty())
   pname=Model_GetComponentLongName((componentid+std::string(".")+AnsiString(ids[i]).c_str()).c_str());
  else
   pname=Model_GetComponentLongName(AnsiString(ids[i]).c_str());

  if(pname)
   names[i]=pname;
  Engine_FreeBufString(pname);
 }

 AnsiString str;
 for(size_t i=0;i<names.size();i++)
 {
  ComponentNames.push_back(names[i]);
 }

 UpdateInterface();
}

// ������� ����������� ���������
void TUComponentsPerformanceFrame::DelComponent(const std::string &componentname)
{
 std::vector<std::string>::iterator I=find(ComponentNames.begin(),ComponentNames.end(),componentname);
 if(I != ComponentNames.end())
  ComponentNames.erase(I);
 UpdateInterface();
}

void TUComponentsPerformanceFrame::DelComponent(std::size_t index)
{
 if(index<ComponentNames.size())
  ComponentNames.erase(ComponentNames.begin()+index);
 UpdateInterface();
}

// ������� ��� ����������� ����������
void TUComponentsPerformanceFrame::ClearComponents(void)
{
 ComponentNames.clear();
 ComponentData.clear();
 UpdateInterface();
}

void __fastcall TUComponentsPerformanceFrame::SelectSource1Click(TObject *Sender)

{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 AddComponent(MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName());
 ComponentData.clear();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsPerformanceFrame::ClearAll1Click(TObject *Sender)
{
 ClearComponents();
 ComponentData.clear();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsPerformanceFrame::AverageIntervalSpinEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;

 AverageIterations=AverageIntervalSpinEdit->Value;
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
 ComponentData.clear();

// ((ScrollPager*)Chart->Tools[0])->SubChartTool->Charts[0].ITool = ((ScrollPager)Chart->Tools[0])->SubChartTool;
}
//---------------------------------------------------------------------------

