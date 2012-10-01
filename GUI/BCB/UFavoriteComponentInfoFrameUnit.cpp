//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UFavoriteComponentInfoFrameUnit.h"
#include "UListInputFormUnit.h"
#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUFavoriteComponentInfoFrame *UFavoriteComponentInfoFrame;

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
TUFavoriteData::TUFavoriteData(void)
{
 Type=1;
 Mode=0;
 Time=0;
}

TUFavoriteData::TUFavoriteData(const TUFavoriteData &copy)
{
 Type=copy.Type;
 Mode=copy.Mode;
 Time=copy.Time;
 ComponentName=copy.ComponentName;
 DataName=copy.DataName;
 Data=copy.Data;
 Header=copy.Header;
}

TUFavoriteData::~TUFavoriteData(void)
{

}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Считывает значение в соответствии с типом и режимом и сохраняет его в переменной Data
void TUFavoriteData::ReadData(void)
{
 const char *value=0;
 int index=0;
 int data_size=0;
 int element_size=0;

 switch(Type)
 {
 case 1:
  value=Model_GetComponentParameterValue(ComponentName.c_str(), DataName.c_str());
  if(value)
   Data=value;
 break;

 case 2:
  value=Model_GetComponentStateValue(ComponentName.c_str(), DataName.c_str());
  if(value)
   Data=value;
 break;

 case 3:
  index=RDK::atoi(DataName);
  value=Model_GetComponentOutputData(ComponentName.c_str(), index);
  data_size=Model_GetComponentOutputDataSize(ComponentName.c_str(), index);
  element_size=Model_GetComponentOutputElementSize(ComponentName.c_str(), index);

  if(value && element_size==8 && data_size>0)
  {
   Data=RDK::sntoa(reinterpret_cast<const double*>(value)[0]);
  }
  else
  {
   Data="<Unrecognized>";
  }
 break;

 case 4:
  index=RDK::atoi(DataName);
  value=Model_GetComponentInputData(ComponentName.c_str(), index);
  data_size=Model_GetComponentOutputDataSize(ComponentName.c_str(), index);
  element_size=Model_GetComponentOutputElementSize(ComponentName.c_str(), index);

  if(value && element_size==8 && data_size>0)
  {
   Data=RDK::sntoa(reinterpret_cast<const double*>(value)[0]);
  }
  else
  {
   Data="<Unrecognized>";
  }
 break;

 default:
  Data="<Not parsed>";
  return;

 }

 if(Mode != 0)
 {
  pair<double,double> hist_val;
  double current_time=Model_GetTime();

  hist_val.first=current_time;
  hist_val.second=RDK::atof(Data);
  History.push_back(hist_val);
  while(!History.empty() && current_time-History[0].first>Time)
   History.erase(History.begin());
 }
 else
  History.clear();

 double sum_time=0,sum_values=0,min_values=0, max_values=0;

 switch(Mode)
 {
 case 0:
 break;

 case 1:
  for(size_t i=0;i<History.size();i++)
  {
   sum_time+=History[i].first;
   sum_values+=History[i].second;
  }
  if(sum_time > 1.e-7)
   Data=RDK::sntoa(sum_values/sum_time);
  else
   Data=RDK::sntoa(sum_values);
 break;

 case 2:
  sum_values=-1.e300;
  for(size_t i=0;i<History.size();i++)
  {
   if(sum_values<History[i].second)
	sum_values=History[i].second;
  }
  Data=RDK::sntoa(sum_values);
 break;

 case 3:
  sum_values=1.e300;
  for(size_t i=0;i<History.size();i++)
  {
   if(sum_values>History[i].second)
	sum_values=History[i].second;
  }
  Data=RDK::sntoa(sum_values);
 break;

 case 4:
  sum_values=0;
  for(size_t i=0;i<History.size();i++)
  {
   if(sum_values<fabs(History[i].second))
	sum_values=fabs(History[i].second);
  }
  Data=RDK::sntoa(sum_values);
 break;

 case 5:
  sum_values=1.e300;
  for(size_t i=0;i<History.size();i++)
  {
   if(sum_values>fabs(History[i].second))
	sum_values=fabs(History[i].second);
  }
  Data=RDK::sntoa(sum_values);
 break;

 case 6:
  min_values=1.e300;
  max_values=-1.e300;
  for(size_t i=0;i<History.size();i++)
  {
   if(min_values>History[i].second)
	min_values=History[i].second;
   if(max_values<History[i].second)
	max_values=History[i].second;
  }
  Data=RDK::sntoa(fabs(max_values-min_values));
 break;
 }
}
// --------------------------

//---------------------------------------------------------------------------
__fastcall TUFavoriteComponentInfoFrame::TUFavoriteComponentInfoFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
}

// Обновляет интерфейс
void TUFavoriteComponentInfoFrame::AUpdateInterface(void)
{
 StringGrid->ColWidths[1]=StringGrid->ClientWidth-StringGrid->ColWidths[0];
 StringGrid->Cells[0][0]="Name";
 StringGrid->Cells[1][0]="Value";
 StringGrid->RowCount=Info.size()+1;
 if(StringGrid->RowCount>1)
  StringGrid->FixedRows=1;

 for(size_t i=0;i<Info.size();i++)
 {
  Info[i].ReadData();
  StringGrid->Cells[0][i+1]=Info[i].Header.c_str();
  StringGrid->Cells[1][i+1]=Info[i].Data.c_str();
 }
}

// Сохраняет параметры интерфейса в xml
void TUFavoriteComponentInfoFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.WriteInteger("ColWidth",StringGrid->ColWidths[0]);

 xml.WriteInteger("InfoSize",Info.size());
 xml.SelectNodeForce("Info");
 for(size_t i=0;i<Info.size();i++)
 {
  xml.WriteInteger(std::string("Type_")+RDK::sntoa(i+1),Info[i].Type);
  xml.WriteInteger(std::string("Mode_")+RDK::sntoa(i+1),Info[i].Mode);
  xml.WriteFloat(std::string("Time_")+RDK::sntoa(i+1),Info[i].Time);
  xml.WriteString(std::string("ComponentName_")+RDK::sntoa(i+1),Info[i].ComponentName);
  xml.WriteString(std::string("DataName_")+RDK::sntoa(i+1),Info[i].DataName);
  xml.WriteString(std::string("Data_")+RDK::sntoa(i+1),Info[i].Data);
  xml.WriteString(std::string("Header_")+RDK::sntoa(i+1),Info[i].Header);
 }
 xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void TUFavoriteComponentInfoFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 StringGrid->ColWidths[0]=xml.ReadInteger("ColWidth",StringGrid->ColWidths[0]);
 int size=xml.ReadInteger("InfoSize",0);
 Info.resize(size);
 xml.SelectNodeForce("Info");
 for(size_t i=0;i<Info.size();i++)
 {
  Info[i].Type=xml.ReadInteger(std::string("Type_")+RDK::sntoa(i+1),1);
  Info[i].Mode=xml.ReadInteger(std::string("Mode_")+RDK::sntoa(i+1),0);
  Info[i].Time=xml.ReadFloat(std::string("Time_")+RDK::sntoa(i+1),1);
  Info[i].ComponentName=xml.ReadString(std::string("ComponentName_")+RDK::sntoa(i+1),"");
  Info[i].Data=xml.ReadString(std::string("Data_")+RDK::sntoa(i+1),"");
  Info[i].DataName=xml.ReadString(std::string("DataName_")+RDK::sntoa(i+1),"");
  Info[i].Header=xml.ReadString(std::string("Header_")+RDK::sntoa(i+1),"");
 }
 xml.SelectUp();
}
//---------------------------------------------------------------------------
void __fastcall TUFavoriteComponentInfoFrame::Add1Click(TObject *Sender)
{
 TUFavoriteData data;
 vector<string> vals;
 vals.push_back("Parameter");
 vals.push_back("State");
 vals.push_back("Output");
 vals.push_back("Input");

 UListInputForm->PresentSelect=true;
 UListInputForm->Sort=false;
 UListInputForm->MustInput=true;
 UListInputForm->Init("Select line type",vals,"Parameter");
 if(UListInputForm->ShowModal() != mrOk)
  return;
 data.Type=UListInputForm->List->Row+1;

 switch(data.Type)
 {
 case 0:
  return;
 break;

 case 1:
  if(UComponentsListForm->ShowParameterSelect() != mrOk)
   return;
  data.DataName=data.Header=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentParameterName();
 break;

 case 2:
  if(UComponentsListForm->ShowStateSelect() != mrOk)
   return;
  data.DataName=data.Header=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
 break;

 case 3:
  if(UComponentsListForm->ShowIOSelect() != mrOk)
   return;
  data.DataName=RDK::sntoa(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentInput());
  data.Header=std::string("Inputs[")+data.DataName+"]";
 break;

 case 4:
  if(UComponentsListForm->ShowIOSelect() != mrOk)
   return;
  data.DataName=RDK::sntoa(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput());
  data.Header=std::string("Outputs[")+data.DataName+"]";
 break;

 default:
  return;
 }
 data.ComponentName=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();

 vals.clear();
 UListInputForm->PresentSelect=false;
 UListInputForm->Sort=false;
 UListInputForm->MustInput=true;
 UListInputForm->Init("Input line header",vals,data.Header);
 if(UListInputForm->ShowModal() != mrOk)
  return;
 data.Header=AnsiString(UListInputForm->Edit->Text).c_str();

 vals.clear();
 vals.push_back("Last value");
 vals.push_back("The average over the interval");
 vals.push_back("The maximum over the interval");
 vals.push_back("The minimum over the interval");
 vals.push_back("The absolute maximum over the interval");
 vals.push_back("The absolute minimum over the interval");
 vals.push_back("The amplitude over the interval");

 UListInputForm->PresentSelect=true;
 UListInputForm->Sort=false;
 UListInputForm->MustInput=true;
 UListInputForm->Init("Select accumulation mode",vals,"Last value");
 if(UListInputForm->ShowModal() != mrOk)
  return;
 data.Mode=UListInputForm->List->Row;

 Info.push_back(data);

 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUFavoriteComponentInfoFrame::Delete1Click(TObject *Sender)
{
 if(StringGrid->Row>0 && StringGrid->Row<int(Info.size()+1))
 {
  Info.erase(Info.begin()+StringGrid->Row-1);
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------


void __fastcall TUFavoriteComponentInfoFrame::MoveDown1Click(TObject *Sender)
{
 if(StringGrid->Row>0 && (StringGrid->Row<int(Info.size())))
 {
  TUFavoriteData data=Info[StringGrid->Row-1];
  Info[StringGrid->Row-1]=Info[StringGrid->Row];
  Info[StringGrid->Row]=data;
  StringGrid->Row=StringGrid->Row+1;
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUFavoriteComponentInfoFrame::Moveup1Click(TObject *Sender)
{
 if(StringGrid->Row>1 && StringGrid->Row<int(Info.size()+1))
 {
  TUFavoriteData data=Info[StringGrid->Row-1];
  Info[StringGrid->Row-1]=Info[StringGrid->Row-2];
  Info[StringGrid->Row-2]=data;
  StringGrid->Row=StringGrid->Row-1;
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------

void __fastcall TUFavoriteComponentInfoFrame::Rename1Click(TObject *Sender)
{
 if(StringGrid->Row>0 && StringGrid->Row<int(Info.size()+1))
 {
  std::vector<std::string> vals;
  UListInputForm->PresentSelect=false;
  UListInputForm->Sort=false;
  UListInputForm->MustInput=true;
  UListInputForm->Init("Input line header",vals,Info[StringGrid->Row-1].Header);
  if(UListInputForm->ShowModal() != mrOk)
   return;
  Info[StringGrid->Row-1].Header=AnsiString(UListInputForm->Edit->Text).c_str();
  UpdateInterface();
 }
}
//---------------------------------------------------------------------------

