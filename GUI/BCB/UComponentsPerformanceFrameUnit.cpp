//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UComponentsPerformanceFrameUnit.h"
#include "rdk_cpp_initdll.h"

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

 long long model_time=Model_GetFullStepDuration("");
 long long sum=0;
 long long ext_gui=Model_GetInterstepsInterval("");

 std::vector<long long> comp_time;
 size_t last_comps_index=ComponentNames.size();

 comp_time.resize(ComponentNames.size()+4);
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
 comp_time[last_comps_index+2]=ext_gui;
 comp_time[last_comps_index+3]=ext_gui+model_time;

 ComponentData.push_back(comp_time);

 std::vector<long long> average;

 average.assign(ComponentNames.size()+4,0);
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
  if(model_time && average[last_comps_index+1]>10e-8)
   Chart->Series[1]->AddY((average[i]*100.0)/average[last_comps_index+1],legend.c_str());
  else
   Chart->Series[1]->AddY(0,legend.c_str());

 }

 if(ComponentNames.size()>0)
 {
  Chart->Series[0]->AddY(average[last_comps_index],"Others");
  if(model_time && average[last_comps_index+1]>10e-8)
   Chart->Series[1]->AddY(((average[last_comps_index])*100.0)/average[last_comps_index+1],"Others");
  else
   Chart->Series[1]->AddY(0,"Others");
 }

  Chart->Series[0]->AddY(average[last_comps_index+1],"Model");
  Chart->Series[1]->AddY(100,"Model");

  Chart->Series[0]->AddY(average[last_comps_index+2],"Ext. GUI");
  Chart->Series[0]->AddY(average[last_comps_index+3],"Full step");
}

// Возврат интерфейса в исходное состояние
void TUComponentsPerformanceFrame::AClearInterface(void)
{
 ClearComponents();
}

// Сохраняет параметры интерфейса в xml
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

// Загружает параметры интерфейса из xml
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
// Загружает информацию об источниках данных из заданного ini файла
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

// Возвращает число наблюдаемых компонент
std::size_t TUComponentsPerformanceFrame::GetNumComponents(void) const
{
 return ComponentNames.size();
}

// Добавляет компонент для мониторинга
void TUComponentsPerformanceFrame::AddComponent(const std::string &componentname)
{
 ComponentNames.push_back(componentname);
 UpdateInterface();
}

// Добавляет все компоненты, содержащиеся непосредственно в выбранном компоненте
void TUComponentsPerformanceFrame::AddAllComponents(const std::string &componentname)
{
 int numcomponents=Model_GetNumComponents(componentname.c_str());

 std::vector<int> ids;
 ids.resize(numcomponents);
 Model_GetComponentsList(componentname.c_str(), &ids[0]);
 std::vector<std::string> names;
 names.resize(ids.size());

 std::string componentid=Model_GetComponentLongId(componentname.c_str());

 for(size_t i=0;i<ids.size();i++)
  names[i]=Model_GetComponentLongName((componentid+std::string(".")+AnsiString(ids[i]).c_str()).c_str());

 AnsiString str;
 for(size_t i=0;i<names.size();i++)
 {
  ComponentNames.push_back(names[i]);
 }

 UpdateInterface();
}

// Удаляет наблюдаемый компонент
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

// Удаляет все наблюдаемые компоненты
void TUComponentsPerformanceFrame::ClearComponents(void)
{
 ComponentNames.clear();
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

