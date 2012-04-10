//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UComponentsPerformanceFrameUnit.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUComponentsPerformanceFrame *UComponentsPerformanceFrame;
//---------------------------------------------------------------------------
__fastcall TUComponentsPerformanceFrame::TUComponentsPerformanceFrame(TComponent* Owner)
	: TFrame(Owner)
{
 for(int i=0;i<Chart->SeriesCount();i++)
  Chart->Series[i]->Clear();

 UpdateInterfaceFlag=false;

 MyComponentsListForm=new TUComponentsListForm(this);
}

__fastcall TUComponentsPerformanceFrame::~TUComponentsPerformanceFrame(void)
{
 if(MyComponentsListForm)
  delete MyComponentsListForm;
}

//---------------------------------------------------------------------------
void TUComponentsPerformanceFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 long long model_time=Model_GetFullStepDuration("");
 long long sum=0;

 std::vector<long long> comp_time;

 comp_time.resize(ComponentNames.size());
 for(size_t i=0;i<ComponentNames.size();i++)
  sum+=comp_time[i]=Model_GetFullStepDuration(ComponentNames[i].c_str());

 Chart->Series[0]->Clear();
 Chart->Series[1]->Clear();

 if(!model_time)
 {
  UpdateInterfaceFlag=false;
  return;
 }

 for(size_t i=0;i<ComponentNames.size();i++)
 {
  std::string legend=ComponentNames[i];

  if(legend.size()>10)
  {
   size_t n=legend.find_last_of('.');
   if(n != std::string::npos)
    legend=legend.substr(n+1);

   int j=0;
   for(;j<legend.size();j++)
	if(j>0 && !(j%8) && legend.size()-j>3)
	{
	 legend.insert(j,"\r\n"); j+=2;
	}
//  if(legend.Length()>15)
//   legend=AnsiString("...")+legend.SubString(legend.Length()-15,15);
  }
  Chart->Series[0]->AddY(comp_time[i],legend.c_str());
  Chart->Series[1]->AddY((comp_time[i]*100.0)/model_time);
 }

 if(ComponentNames.size()>0)
 {
  Chart->Series[0]->AddY(model_time-sum,"Others");
  Chart->Series[1]->AddY(((model_time-sum)*100.0)/model_time);
 }

  Chart->Series[0]->AddY(model_time,"Model");
  Chart->Series[1]->AddY(100);

 UpdateInterfaceFlag=false;
}


// Сохраняет информацию об источниках данных в заданный ini файл
void TUComponentsPerformanceFrame::SaveToIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 ini->EraseSection(section);
 for(size_t i=0;i<ComponentNames.size();i++)
 {
  ini->WriteString(section,ComponentNames[i].c_str(),"");
 }
}

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
}

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
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsPerformanceFrame::ClearAll1Click(TObject *Sender)
{
 ClearComponents();
}
//---------------------------------------------------------------------------
