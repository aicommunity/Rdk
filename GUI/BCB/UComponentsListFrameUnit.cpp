//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <vector>
#include "UComponentsListFrameUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include "UCRPerseptronFormUnit.h"
//#include "UCRSampleFormUnit.h"
//#include "UCRTeacherPerseptronDLFormUnit.h"
//#include "UCRTeacherPerseptronBPFormUnit.h"
#include "UComponentsListFormUnit.h"
#include "UListInputFormUnit.h"
#include "UDrawEngineFrameUnit.h"
#include <ClipBrd.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "TEnchancedStringGrid"
#pragma resource "*.dfm"
TUComponentsListFrame *UComponentsListFrame;

// Собственно список форм
std::map<std::string, TUVisualControllerForm*> TUComponentsListFrame::ComponentControllers;

using namespace RDK;
//---------------------------------------------------------------------------
__fastcall TUComponentsListFrame::TUComponentsListFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 ShowXMLComponentParameters=true;
 UpdateInterval=-1;
 DrawEngineFrame=0;
 TreeReadOnlyFlag=false;
 ParamReadOnlyFlag=false;
 RegistryModified=false;
 SelectedId=0;
}
//---------------------------------------------------------------------------

// Перерисовывает путь до текущего узла
void __fastcall TUComponentsListFrame::UpdatePath(void)
{
 UpdateInterfaceFlag=true;

 HeaderControl->Sections->Clear();

 CurrentPath.resize(0);

 RDK::separatestring(CurrentComponentName,CurrentPath,'.');

 for(int i=0;i<int(CurrentPath.size());i++)
 {
  THeaderSection *section=HeaderControl->Sections->Add();
  section->Text=CurrentPath[i].c_str();
  section->Width=HeaderControl->Canvas->TextWidth(section->Text)+20;
 }

 UpdateInterfaceFlag=false;
}

// Отрисовка фрейма
void TUComponentsListFrame::AUpdateInterface(void)
{
 if(ComponentControllers.empty())
 {
//  ComponentControllers["UCRPerseptron"]=UCRPerseptronForm;
//  ComponentControllers["UCRSample"]=UCRSampleForm;
//  ComponentControllers["UCRTeacherPerseptronDL"]=UCRTeacherPerseptronDLForm;
//  ComponentControllers["UCRTeacherPerseptronBP"]=UCRTeacherPerseptronBPForm;
 }


 int row=StringGrid->Row;

 if(SelectedId == 0)
  row=1;


 int numcomponents=Model_GetNumComponents(CurrentComponentName.c_str());
 StringGrid->ColCount=2;
 StringGrid->RowCount=numcomponents+2;
 StringGrid->Cells[0][1]="..";

 if(numcomponents<0)
 {
  UpdatePath();
  UpdateSelectedComponentInfo();
  UpdateParameters();
  UpdateInterfaceFlag=false;
  return;
 }

 std::vector<std::string> ids;
// ids.resize(numcomponents);
 const char* p=Model_GetComponentsNameList(CurrentComponentName.c_str());
 std::string names;
 if(p)
  names=p;
 Engine_FreeBufString(p);

 RDK::separatestring(names,ids,',');
 for(int i=0;i<numcomponents;i++)
 {
  StringGrid->Cells[1][i+2]=ids[i].c_str();
  std::string longid=CurrentComponentName;
  if(longid.size()>0)
   longid+=".";

  const char *psid=Model_GetComponentLongId((longid+ids[i]).c_str(),CurrentComponentName.c_str());
  std::string sid;
  if(psid)
   sid=psid;
  Engine_FreeBufString(psid);
  int id=RDK::atoi(sid);
  StringGrid->Cells[0][i+2]=IntToStr(id);
  if(id == SelectedId)
   row=i+2;
 }

 StringGrid->ColWidths[0]=30;
 StringGrid->ColWidths[1]=Width-StringGrid->ColWidths[0];

 StringGrid->Cells[0][0]="Id";
 StringGrid->Cells[1][0]="Component name";

 if(row<StringGrid->RowCount)
  StringGrid->Row=row;

 StringGrid->FixedRows=1;
 UpdateInterfaceFlag=false;
 UpdatePath();
 UpdateSelectedComponentInfo();

 if(PageControl1->ActivePage == TabSheet1)
  UpdateParameters();
 if(PageControl1->ActivePage == TabSheet2)
  UpdateState();
 if(PageControl1->ActivePage == TabSheet3)
 UpdateIO();
// if(PageControl1->ActivePage == TabSheet4)
//  UpdateParametersList();
// if(PageControl1->ActivePage == TabSheet5)
//  UpdateStatesList();
 if(PageControl1->ActivePage == TabSheet6)
  UpdateNiceParamsList(EnchancedSG1);
 if(PageControl1->ActivePage == TabSheet7)
  UpdateNiceStatesList(EnchancedSG2);
}

// Возврат интерфейса в исходное состояние
void TUComponentsListFrame::AClearInterface(void)
{
 SelectedComponentName.clear();
 SelectedComponentId.clear();
 CurrentComponentName.clear();
 CurrentComponentId.clear();
 SelectedComponentParameterName.clear();
 SelectedComponentStateName.clear();
 CurrentPath.clear();
 SelectedComponentOutput.clear();
 SelectedComponentInput.clear();

 SelectedId=-1;

 LastParams="";

 TempParams="";

 LastStates="";

 TempStates="";


 StringGrid->RowCount=0;
// ParametersListStringGrid->RowCount=0;
// StatesListStringGrid->RowCount=0;
 OutputsStringGrid->RowCount=0;
 InputsStringGrid->RowCount=0;
 ParametersRichEdit->Lines->Clear();
 StateRichEdit->Lines->Clear();
// ParameterValueRichEdit->Lines->Clear();
 NiceParamValRichEdit->Lines->Clear();
 NiceStateValRichEdit->Lines->Clear();
}

// Сохраняет параметры интерфейса в xml
void TUComponentsListFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUComponentsListFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 SelectedComponentName.clear();
 SelectedComponentId.clear();
 CurrentComponentName.clear();
 CurrentComponentId.clear();
 SelectedComponentParameterName.clear();
 SelectedComponentStateName.clear();
 CurrentPath.clear();

}

// Обновляет параметры компонента
void TUComponentsListFrame::UpdateParameters(void)
{
 if(PageControl1->ActivePage != TabSheet1)
  return;
 UpdateInterfaceFlag=true;

 if(ShowXMLComponentParameters)
 {
  Panel1->Visible=true;
  const char *p=Model_GetComponentParametersEx(GetSelectedComponentLongName().c_str());
  if(p)
   TempParams=p;
  Engine_FreeBufString(p);
  if(TempParams.Length() != LastParams.Length() || TempParams != LastParams)
   ParametersRichEdit->Text=LastParams=TempParams;
 }
 else
 {
  Panel1->Visible=false;
 }

 std::string name;
 const char *p=Model_GetComponentClassName(SelectedComponentName.c_str());
 if(p)
  name=p;
 Engine_FreeBufString(p);

 std::map<std::string, TUVisualControllerForm*>::iterator I=ComponentControllers.find(name);
 if(I != ComponentControllers.end() && I->second)
  GUI1->Enabled=true;
 else
  GUI1->Enabled=false;

// ParameterValueRichEdit->Text=ParametersListStringGrid->Cells[2][ParametersListStringGrid->Row];

 RegistryModified=false;
 UpdateInterfaceFlag=false;
}


// Обновляет состояние компонента
void TUComponentsListFrame::UpdateState(void)
{
 if(PageControl1->ActivePage == TabSheet2)
 {
	 UpdateInterfaceFlag=true;

	 if(ShowXMLComponentParameters)
	 {
	  Panel1->Visible=true;

	  const char* p=Model_GetComponentState(GetSelectedComponentLongName().c_str());
	  if(p)
	   TempStates=p;
	  Engine_FreeBufString(p);
	  if(TempStates.Length() != LastStates.Length() || TempStates != LastStates)
	   StateRichEdit->Text=LastStates=TempStates;
	 }
	 else
	 {
	  Panel1->Visible=false;
	 }

//	 StateValueRichEdit->Text=StatesListStringGrid->Cells[2][StatesListStringGrid->Row];
	 RegistryModified=false;
	 UpdateInterfaceFlag=false;
 }
 else if(PageControl1->ActivePage == TabSheet7)
 {
  UpdateInterfaceFlag=true;
  if(ShowXMLComponentParameters)
  {
   Panel1->Visible=true;
   const char* p=Model_GetComponentState(GetSelectedComponentLongName().c_str());
   if(p)
	TempStates=p;
   Engine_FreeBufString(p);
   if(TempStates.Length() != LastStates.Length() || TempStates != LastStates)
   NiceStateValRichEdit->Text=LastStates=TempStates;
  }
  else
  {
   Panel1->Visible=false;
  }

  NiceStateValRichEdit->Text=EnchancedSG2->BasicStringGrid->Cells[2][EnchancedSG2->BasicStringGrid->Row];

  RegistryModified=false;
  UpdateInterfaceFlag=false;
 }
}

// Обновляет данные ввода-вывода
void TUComponentsListFrame::UpdateIO(void)
{
 if(PageControl1->ActivePage != TabSheet3)
  return;
 UpdateInterfaceFlag=true;

 std::string output_names;

 const char *p_out=Model_GetComponentPropertiesList(GetSelectedComponentLongName().c_str(), ptOutput | pgPublic);
 if(p_out)
  output_names=p_out;
 Engine_FreeBufString(p_out);

 std::string input_names;

 const char *p_in=Model_GetComponentPropertiesList(GetSelectedComponentLongName().c_str(), ptInput | pgPublic);
 if(p_in)
  input_names=p_in;
 Engine_FreeBufString(p_in);

 std::vector<std::string> outputs,inputs;
 RDK::separatestring(output_names,outputs,',');
 RDK::separatestring(input_names,inputs,',');

 InputsStringGrid->Perform(WM_SETREDRAW, 0, (NativeInt)0);
 OutputsStringGrid->Perform(WM_SETREDRAW, 0, (NativeInt)0);
try
{

 if(outputs.size()>0)
 {
// int num=Model_GetComponentNumOutputs(GetSelectedComponentLongName().c_str());
  OutputsStringGrid->RowCount=1+outputs.size();
  OutputsStringGrid->ColCount=2;

  OutputsStringGrid->Cells[0][0]="Name";
  OutputsStringGrid->Cells[1][0]="Type";

  for(int i=0;i<int(outputs.size());i++)
  {
   OutputsStringGrid->Cells[0][i+1]=outputs[i].c_str();
   OutputsStringGrid->Cells[1][i+1]="";
  }

  OutputsStringGrid->ColWidths[0]=OutputsStringGrid->Width-OutputsStringGrid->ColWidths[0]-25;
 }
 else
 {
  int num=Model_GetComponentNumOutputs(GetSelectedComponentLongName().c_str());
  if(num>0)
  {
   OutputsStringGrid->RowCount=1+num;
   OutputsStringGrid->ColCount=2;

   OutputsStringGrid->Cells[0][0]="Name";
   OutputsStringGrid->Cells[1][0]="Type";

   for(int i=0;i<num;i++)
   {
	OutputsStringGrid->Cells[0][i+1]=IntToStr(i);
	OutputsStringGrid->Cells[1][i+1]="";
   }

   OutputsStringGrid->ColWidths[0]=OutputsStringGrid->Width-OutputsStringGrid->ColWidths[0]-25;
  }
 }

 if(OutputsStringGrid->RowCount>1)
  OutputsStringGrid->FixedRows=1;
// num=Model_GetComponentNumInputs(GetSelectedComponentLongName().c_str());
 InputsStringGrid->RowCount=1+inputs.size();
 InputsStringGrid->ColCount=2;

 InputsStringGrid->Cells[0][0]="Name";
 InputsStringGrid->Cells[1][0]="Type";

 for(int i=0;i<int(inputs.size());i++)
 {
  InputsStringGrid->Cells[0][i+1]=inputs[i].c_str();
  InputsStringGrid->Cells[1][i+1]="";
 }

 InputsStringGrid->ColWidths[0]=InputsStringGrid->Width-InputsStringGrid->ColWidths[0]-25;

 if(InputsStringGrid->RowCount>1)
  InputsStringGrid->FixedRows=1;

}
__finally
{
  InputsStringGrid->Perform(WM_SETREDRAW, 1, (NativeInt)0);
  InputsStringGrid->Invalidate();
  OutputsStringGrid->Perform(WM_SETREDRAW, 1, (NativeInt)0);
  OutputsStringGrid->Invalidate();
}
 UpdateInterfaceFlag=false;
}


// Обновляет данные списка параметров
void TUComponentsListFrame::UpdateParametersList(void)
{
/* if(PageControl1->ActivePage != TabSheet4)
  return;
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentParametersEx(GetSelectedComponentLongName().c_str());
 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("Parameters");
 int num=xml.GetNumNodes();


 ParametersListStringGrid->RowCount=1+num;
 ParametersListStringGrid->ColCount=1+3;

 if(ParametersListStringGrid->RowCount>1)
  ParametersListStringGrid->FixedRows=1;

 ParametersListStringGrid->Cells[0][0]="#";
 ParametersListStringGrid->Cells[1][0]="Name";
 ParametersListStringGrid->Cells[2][0]="Value";
 ParametersListStringGrid->Cells[3][0]="Description";

 for(int i=1;i<ParametersListStringGrid->ColCount;i++)
  ParametersListStringGrid->ColWidths[i]=10;

 for(int i=0;i<num;i++)
 {
  ParametersListStringGrid->Cells[0][i+1]=IntToStr(i);
  xml.SelectNode(i);
  ParametersListStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  if(xml.GetNumNodes() == 0)
   ParametersListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   ParametersListStringGrid->Cells[2][i+1]=value.c_str();
  }
  int global_width=ParametersListStringGrid->ColWidths[0];
  ParametersListStringGrid->Cells[3][i+1]=xml.GetNodeAttribute("Header").c_str();
  int width=ParametersListStringGrid->Canvas->TextWidth(ParametersListStringGrid->Cells[1][i+1])+10;
  if(width>ParametersListStringGrid->Width/2)
   width=ParametersListStringGrid->Width/2;
  if(ParametersListStringGrid->ColWidths[1]<width)
   ParametersListStringGrid->ColWidths[1]=width;
  global_width+=ParametersListStringGrid->ColWidths[1];

  width=ParametersListStringGrid->Canvas->TextWidth(ParametersListStringGrid->Cells[2][i+1])+10;
  if(width>ParametersListStringGrid->Width/2)
   width=ParametersListStringGrid->Width/2;
  if(ParametersListStringGrid->ColWidths[2]<width)
   ParametersListStringGrid->ColWidths[2]=width;
  global_width+=ParametersListStringGrid->ColWidths[2];

  width=ParametersListStringGrid->Canvas->TextWidth(ParametersListStringGrid->Cells[3][i+1])+10;
  if(width < ParametersListStringGrid->Width-global_width)
   width=ParametersListStringGrid->Width-global_width;

  if(ParametersListStringGrid->ColWidths[3]<width)
   ParametersListStringGrid->ColWidths[3]=width;
  xml.SelectUp();
 }

 SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][1]).c_str();
 ParameterValueRichEdit->Text=ParametersListStringGrid->Cells[2][ParametersListStringGrid->Row];
 UpdateInterfaceFlag=false;  */
}

// Обновляет данные измененного списка параметров
void TUComponentsListFrame::UpdateNiceParamsList(TEnchancedSG *frame)
{
 if(PageControl1->ActivePage != TabSheet6)
  return;
 UpdateInterfaceFlag=true;

 if(!Model_Check())
  return;

 std::string xml_data;

 const char* p_data=Model_GetComponentParametersEx(GetSelectedComponentLongName().c_str());
 if(p_data)
  xml_data=p_data;
 Engine_FreeBufString(p_data);
 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("Parameters");
 int num=xml.GetNumNodes();

 frame->BasicStringGrid->Perform(WM_SETREDRAW, 0, (NativeInt)0);
try
{
 if(num>0)
	frame->BasicStringGrid->RowCount=1+num;
 else
	frame->BasicStringGrid->RowCount=2;

 frame->BasicStringGrid->ColCount=1+2;

 if(frame->BasicStringGrid->RowCount>1)
  frame->BasicStringGrid->FixedRows=1;

 if(frame->BasicStringGrid->ColCount>1)
  frame->BasicStringGrid->FixedCols=1;

 for(int i=0;i<num;i++)
 {
	frame->BasicStringGrid->Cells[2][i+1]="";
 }
 frame->txtBorderedIntEdit->Visible = false;
 frame->udBorderedInt->Visible = false;
 frame->txtIntEdit->Visible = false;
 frame->txtStringEdit->Visible = false;
 frame->txtDoubleEdit->Visible = false;
 frame->chbBoolEdit->Visible = false;
 frame->udBorderedInt->Visible = false;

// if(ParametersListStringGrid->RowCount>1)
//  ParametersListStringGrid->FixedRows=1;

 frame->BasicStringGrid->Cells[0][0]="#";
 frame->BasicStringGrid->Cells[1][0]="Name";
 frame->BasicStringGrid->Cells[2][0]="Value";
// frame->BasicStringGrid->Cells[3][0]="Description";

 for(int i=1;i<frame->BasicStringGrid->ColCount;i++)
  frame->BasicStringGrid->ColWidths[i]=10;

 frame->m_storage.Clear();
 for(int i=0;i<num;i++)
 {

  xml.SelectNode(i);
  //------------------------------------
  frame->BasicStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  TProperty p;
  p.SetName(xml.GetNodeName().c_str());
  p.SetNumber(i);
  if(xml.GetNumNodes() == 0)
  {
   //ParametersListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
   String type = xml.GetNodeAttribute("Type").c_str();
   PropertyType t;
   if(type=="bool")
   {
	t = BoolP;
   }
   else if(type=="int")
   {
	t = IntP;
   }
   else if(type=="double")
   {
	t = DoubleP;
   }
   else if(type=="unsigned int")
   {
	t = IntP;
   }
   else
   {
	t = StrP;
   }
   p.SetValue(xml.GetNodeText().c_str(), t);
  }
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   //ParametersListStringGrid->Cells[2][i+1]=value.c_str();

   p.SetValue(value.c_str(), StrP);
  }
  p.SetListed(false);
  p.SetDesc(xml.GetNodeAttribute("Header").c_str());
  frame->m_storage.AddProperty(p);


  xml.SelectUp();
 }

 frame->Refresh();
}
__finally
{
  frame->BasicStringGrid->Perform(WM_SETREDRAW, 1, (NativeInt)0);
  frame->BasicStringGrid->Invalidate();
}

 UpdateInterfaceFlag=false;
}

// Обновляет данные переделанного списка переменных состояния
void TUComponentsListFrame::UpdateNiceStatesList(TEnchancedSG *frame)
{
 if(PageControl1->ActivePage != TabSheet7)
  return;
 UpdateInterfaceFlag=true;

 std::string xml_data;

 const char* p_data=Model_GetComponentState(GetSelectedComponentLongName().c_str());
 if(p_data)
  xml_data=p_data;
 Engine_FreeBufString(p_data);

 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("State");
 int num=xml.GetNumNodes();

 frame->BasicStringGrid->Perform(WM_SETREDRAW, 0, (NativeInt)0);
try
{
if(num>0)
	frame->BasicStringGrid->RowCount=1+num;
 else
	frame->BasicStringGrid->RowCount=2;

 frame->BasicStringGrid->ColCount=1+2;
 for(int i=0;i<num;i++)
 {
	frame->BasicStringGrid->Cells[2][i+1]="";
 }
 frame->txtBorderedIntEdit->Visible = false;
 frame->udBorderedInt->Visible = false;
 frame->txtIntEdit->Visible = false;
 frame->txtStringEdit->Visible = false;
 frame->txtDoubleEdit->Visible = false;
 frame->chbBoolEdit->Visible = false;
 frame->udBorderedInt->Visible = false;

// if(ParametersListStringGrid->RowCount>1)
//  ParametersListStringGrid->FixedRows=1;

 frame->BasicStringGrid->Cells[0][0]="#";
 frame->BasicStringGrid->Cells[1][0]="Name";
 frame->BasicStringGrid->Cells[2][0]="Value";
// frame->BasicStringGrid->Cells[3][0]="Description";

 for(int i=1;i<frame->BasicStringGrid->ColCount;i++)
  frame->BasicStringGrid->ColWidths[i]=10;

 frame->m_storage.Clear();

  for(int i=0;i<num;i++)
 {

  xml.SelectNode(i);
  //------------------------------------
  frame->BasicStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  TProperty p;
  p.SetName(xml.GetNodeName().c_str());
  p.SetNumber(i);
  if(xml.GetNumNodes() == 0)
  {
   //ParametersListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
   String type = xml.GetNodeAttribute("Type").c_str();
   PropertyType t;
   if(type=="bool")
   {
	t = BoolP;
   }
   else if(type=="int")
   {
	t = IntP;
   }
   else if(type=="double")
   {
	t = DoubleP;
   }
   else if(type=="unsigned int")
   {
	t = IntP;
   }
   else
   {
	t = StrP;
   }
   p.SetValue(xml.GetNodeText().c_str(), t);
  }
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   //ParametersListStringGrid->Cells[2][i+1]=value.c_str();

   p.SetValue(value.c_str(), StrP);
  }
  p.SetListed(false);
  p.SetDesc(xml.GetNodeAttribute("Header").c_str());
  frame->m_storage.AddProperty(p);

  xml.SelectUp();
 }
 frame->Refresh();

}
__finally
{
  frame->BasicStringGrid->Perform(WM_SETREDRAW, 1, (NativeInt)0);
  frame->BasicStringGrid->Invalidate();
}

 UpdateInterfaceFlag=false;
}

// Обновляет данные списка переменных состояния
void TUComponentsListFrame::UpdateStatesList(void)
{
/* if(PageControl1->ActivePage != TabSheet5)
  return;
 UpdateInterfaceFlag=true;

 std::string xml_data=Model_GetComponentState(GetSelectedComponentLongName().c_str());
 RDK::USerStorageXML xml;
 xml.Load(xml_data,"");
 xml.SelectNode("State");
 int num=xml.GetNumNodes();


 StatesListStringGrid->RowCount=1+num;
 StatesListStringGrid->ColCount=1+3;

 if(StatesListStringGrid->RowCount>1)
  StatesListStringGrid->FixedRows=1;

 StatesListStringGrid->Cells[0][0]="#";
 StatesListStringGrid->Cells[1][0]="Name";
 StatesListStringGrid->Cells[2][0]="Value";
 StatesListStringGrid->Cells[3][0]="Description";

 for(int i=1;i<StatesListStringGrid->ColCount;i++)
  StatesListStringGrid->ColWidths[i]=10;

 for(int i=0;i<num;i++)
 {
  StatesListStringGrid->Cells[0][i+1]=IntToStr(i);
  xml.SelectNode(i);
  StatesListStringGrid->Cells[1][i+1]=xml.GetNodeName().c_str();
  if(xml.GetNumNodes() == 0)
   StatesListStringGrid->Cells[2][i+1]=xml.GetNodeText().c_str();
  else
  {
   std::string value;
   xml.SaveFromNode(value);
   StatesListStringGrid->Cells[2][i+1]=value.c_str();
  }
  int global_width=StatesListStringGrid->ColWidths[0];
  StatesListStringGrid->Cells[3][i+1]=xml.GetNodeAttribute("Header").c_str();
  int width=StatesListStringGrid->Canvas->TextWidth(StatesListStringGrid->Cells[1][i+1])+10;
  if(width>StatesListStringGrid->Width/2)
   width=StatesListStringGrid->Width/2;
  if(StatesListStringGrid->ColWidths[1]<width)
   StatesListStringGrid->ColWidths[1]=width;
  global_width+=StatesListStringGrid->ColWidths[1];

  width=StatesListStringGrid->Canvas->TextWidth(StatesListStringGrid->Cells[2][i+1])+10;
  if(width>StatesListStringGrid->Width/2)
   width=StatesListStringGrid->Width/2;
  if(StatesListStringGrid->ColWidths[2]<width)
   StatesListStringGrid->ColWidths[2]=width;
  global_width+=StatesListStringGrid->ColWidths[2];

  width=StatesListStringGrid->Canvas->TextWidth(StatesListStringGrid->Cells[3][i+1])+10;
  if(width < StatesListStringGrid->Width-global_width)
   width=StatesListStringGrid->Width-global_width;

  if(StatesListStringGrid->ColWidths[3]<width)
   StatesListStringGrid->ColWidths[3]=width;
  xml.SelectUp();
 }

 SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][1]).c_str();
 UpdateInterfaceFlag=false;    */
}

// Обновляет длинные имена выбранных компонент
void TUComponentsListFrame::UpdateSelectedComponentInfo(void)
{
 if(StringGrid->Row<1)
 {
  SelectedComponentName="";
  SelectedComponentId="";
  return;
 }
 else
 if(StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  SelectedComponentName=CurrentComponentName;
  SelectedComponentId=CurrentComponentId;
  return;
 }

 SelectedComponentName=AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
 if(CurrentComponentName.size()>0)
  SelectedComponentName=CurrentComponentName+std::string(".")+SelectedComponentName;

 SelectedComponentId=AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
 if(CurrentComponentId.size()>0)
  SelectedComponentId=CurrentComponentId+std::string(".")+SelectedComponentId;

 const char *p_data=Model_GetComponentClassName(SelectedComponentName.c_str());
 if(p_data)
  ClassNamePanel->Caption=p_data;
 Engine_FreeBufString(p_data);
}

// -----------------------
// Методы доступа к физическим данным
// -----------------------
// Возвращает id выбранного компонента
const std::string TUComponentsListFrame::GetSelectedComponentId(void) const
{
 return AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
}

// Возвращает имя выбранного компонента
const std::string TUComponentsListFrame::GetSelectedComponentName(void) const
{
 return AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
}

// Выбирает компонент по заданному короткому имени
void TUComponentsListFrame::SetSelectedComponentName(const std::string &name)
{
 for(int i=0;i<StringGrid->RowCount;i++)
 {
  if(StringGrid->Cells[1][i] == name.c_str())
  {
   StringGrid->Row=i;
   bool CanSelect;
   StringGridSelectCell(this, StringGrid->Col, i, CanSelect);
   break;
  }
 }
}

// Длинное имя выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentLongName(void) const
{
 return SelectedComponentName;
}

// Длинный строковой Id выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentLongId(void) const
{
 return SelectedComponentId;
}

// Имя выделенного параметра выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentParameterName(void) const
{
 return SelectedComponentParameterName;
}

// Имя выделенной переменной состояния выделенного компонента
const std::string& TUComponentsListFrame::GetSelectedComponentStateName(void) const
{
 return SelectedComponentStateName;
}

// Длинное имя текущего компонента
const std::string& TUComponentsListFrame::GetCurrentComponentName(void) const
{
 return CurrentComponentName;
}

// Опускается на уровень вниз по имени выбранного компонента
void TUComponentsListFrame::SelectComponentByName(const std::string& name)
{
 SetSelectedComponentName(name);
 StringGridDblClick(this);
}

// Поднимается на уровень вверх
void TUComponentsListFrame::SelectUp(void)
{
 SetSelectedComponentName("");
 StringGridDblClick(this);
}

// Длинный строковой id текущего компонента
const std::string& TUComponentsListFrame::GetCurrentComponentId(void) const
{
 return CurrentComponentId;
}

// Выбранный выход объекта
const std::string& TUComponentsListFrame::GetSelectedComponentOutput(void) const
{
 return SelectedComponentOutput;
}

// Выбранный вход объекта
const std::string& TUComponentsListFrame::GetSelectedComponentInput(void) const
{
 return SelectedComponentInput;
}

// Включение-выключение отображения параметров в виде xml
bool TUComponentsListFrame::GetShowXMLComponentParameters(void) const
{
 return ShowXMLComponentParameters;
}

// Возвращает флаг запрета редактирования дерева узлов
bool TUComponentsListFrame::GetTreeReadOnlyFlag(void)
{
 return TreeReadOnlyFlag;
}

// Возвращает флаг запрета редактирования параметров
bool TUComponentsListFrame::GetParamReadOnlyFlag(void)
{
 return ParamReadOnlyFlag;
}

// Возвращает признак наличия изменений в данных
bool TUComponentsListFrame::GetRegistryModified(void)
{
 return RegistryModified;
}
// -----------------------


// -----------------------
// Методы управления физическими данными
// -----------------------
// Очищает список изменений реестра
void __fastcall TUComponentsListFrame::ClearDiffRegistry(void)
{
// DiffRegistry.Clear();
}


bool __fastcall TUComponentsListFrame::SetCurrentSubTree(std::string &fullname)
{
// if(!Registry)
//  return false;

 return true;
}

// Включение-выключение отображения параметров в виде xml
void TUComponentsListFrame::SetShowXMLComponentParameters(bool flag)
{
 ShowXMLComponentParameters=flag;
}

void __fastcall TUComponentsListFrame::SetTreeReadOnlyFlag(bool flag)
{
 TreeReadOnlyFlag=flag;
}

void __fastcall TUComponentsListFrame::SetParamReadOnlyFlag(bool flag)
{
 ParamReadOnlyFlag=flag;
}

void __fastcall TUComponentsListFrame::SetRegistryModified(bool flag)
{
 RegistryModified=flag;
}
// -----------------------

//---------------------------------------------------------------------------
void __fastcall TUComponentsListFrame::FrameResize(TObject *Sender)
{
// UpdateInterface();
}
//---------------------------------------------------------------------------



void __fastcall TUComponentsListFrame::StringGridDblClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 if(StringGrid->Row <= 0)
  return;

// int num_components=Model_GetNumComponents(GetSelectedComponentLongName().c_str());
// if(!num_components)
//  return;

 if(StringGrid->Row == 1 && StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  std::string::size_type i=0,j=0;
  i=CurrentComponentName.find_last_of(".");
  j=CurrentComponentId.find_last_of(".");

  if(i != std::string::npos)
  {
   CurrentComponentName=CurrentComponentName.substr(0,i);
  }
  else
   CurrentComponentName="";
  if(j != std::string::npos)
  {
   SelectedId=StrToInt(CurrentComponentId.substr(j+1).c_str());
   CurrentComponentId=CurrentComponentId.substr(0,j);
  }
  else
  {
   if(CurrentComponentId != "")
	SelectedId=StrToInt(CurrentComponentId.c_str());
   else
    SelectedId=0;
   CurrentComponentId="";
  }

  UpdateInterface(true);
  if(DrawEngineFrame)
  {
   DrawEngineFrame->SetNet(GetCurrentComponentName());
   DrawEngineFrame->UpdateInterface(true);
  }
//   DrawEngineFrame->SelectComponent(GetCurrentComponentName());
  return;
 }

 if(CurrentComponentName.size()>0)
 {
  CurrentComponentName+=".";
  CurrentComponentId+=".";
 }
 SelectedId=StrToInt(StringGrid->Cells[0][StringGrid->Row]);
 CurrentComponentName+=AnsiString(StringGrid->Cells[1][StringGrid->Row]).c_str();
 CurrentComponentId+=AnsiString(StringGrid->Cells[0][StringGrid->Row]).c_str();
 UpdateInterface(true);

 if(DrawEngineFrame)
 {
  DrawEngineFrame->SetNet(GetCurrentComponentName());
  DrawEngineFrame->UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect)
{
/* if(UpdateInterfaceFlag)
  return;

 if(StringGrid->Row<=1 || StringGrid->Cells[0][StringGrid->Row] == "" || StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  SelectedId=0;
 }
 else
 if(SelectedId == StrToInt(StringGrid->Cells[0][StringGrid->Row]))
  return;
 else
  SelectedId=StrToInt(StringGrid->Cells[0][StringGrid->Row]);
 UpdateSelectedComponentInfo();
 UpdateParameters();
 UpdateState();
 UpdateIO();
 UpdateParametersList();
 UpdateStatesList();*/
 StringGridClick(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::StringGridClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 if(StringGrid->Row<=1 || StringGrid->Cells[0][StringGrid->Row] == "" || StringGrid->Cells[0][StringGrid->Row] == "..")
 {
  SelectedId=0;
 }
 else
 if(SelectedId == StrToInt(StringGrid->Cells[0][StringGrid->Row]))
  return;
 else
  SelectedId=StrToInt(StringGrid->Cells[0][StringGrid->Row]);
 UpdateSelectedComponentInfo();
 if(PageControl1->ActivePage == TabSheet1)
  UpdateParameters();
 if(PageControl1->ActivePage == TabSheet2)
  UpdateState();
 if(PageControl1->ActivePage == TabSheet3)
 UpdateIO();
// if(PageControl1->ActivePage == TabSheet4)
//  UpdateParametersList();
// if(PageControl1->ActivePage == TabSheet5)
//  UpdateStatesList();
 if(PageControl1->ActivePage == TabSheet6)
  UpdateNiceParamsList(EnchancedSG1);
 if(PageControl1->ActivePage == TabSheet7)
  UpdateNiceStatesList(EnchancedSG2);

 if(DrawEngineFrame)
  DrawEngineFrame->SelectComponent(GetSelectedComponentName());
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::ParametersRichEditChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 RegistryModified=true;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StateRichEditChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 RegistryModified=true;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StateHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(Section->Index == 0)
 {
  Model_SetComponentState(GetSelectedComponentLongName().c_str(),AnsiString(StateRichEdit->Text).c_str());
  UpdateInterface(true);
 }
 else
 if(Section->Index == 1)
 {
  UpdateState();
 }
 else
 if(Section->Index == 2)
 {

 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridClick(TObject *Sender)
{
 SelectedComponentOutput=AnsiString(OutputsStringGrid->Cells[0][OutputsStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridDblClick(TObject *Sender)
{
 OutputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 OutputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 InputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridDblClick(TObject *Sender)
{
 InputsStringGridClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridClick(TObject *Sender)
{
 SelectedComponentInput=AnsiString(InputsStringGrid->Cells[0][InputsStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::PageControl1Change(TObject *Sender)
{
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersListStringGridClick(TObject *Sender)

{
// SelectedComponentParameterName=AnsiString(EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersListStringGridDblClick(TObject *Sender)

{
// SelectedComponentParameterName=AnsiString(EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersListStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
// SelectedComponentParameterName=AnsiString(ParametersListStringGrid->Cells[1][ARow]).c_str();
// ParameterValueRichEdit->Text=ParametersListStringGrid->Cells[2][ARow];
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridClick(TObject *Sender)
{
//// SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridDblClick(TObject *Sender)
{
// SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridSelectCell(TObject *Sender, int ACol,
		  int ARow, bool &CanSelect)
{
// SelectedComponentStateName=AnsiString(StatesListStringGrid->Cells[1][ARow]).c_str();
// StateValueRichEdit->Text=StatesListStringGrid->Cells[2][ARow];
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::GUI1Click(TObject *Sender)
{
 const char *name=Model_GetComponentClassName(SelectedComponentName.c_str());
 std::map<std::string, TUVisualControllerForm*>::iterator I=ComponentControllers.find(name);
 Engine_FreeBufString(name);
 if(I != ComponentControllers.end() && I->second)
 {
  I->second->SetComponentControlName(SelectedComponentName);
  int id = Core_GetSelectedChannelIndex();
  I->second->SetComponentControlChannel(id);
  I->second->Show();
  I->second->UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
 if(Section->Index == 0)
 {
  Model_SetComponentParameters(GetSelectedComponentLongName().c_str(),AnsiString(ParametersRichEdit->Text).c_str());
  UpdateInterface(true);
 }
 else
 if(Section->Index == 1)
 {
  UpdateParameters();
 }
 else
 if(Section->Index == 2)
 {
  if(Application->MessageBox(L"Значения всех параметров этого компонента будут заменены на значения по умолчанию", L"Предупреждение", MB_YESNO) == ID_YES)
  {
   Env_Default(GetSelectedComponentLongName().c_str(), false);
   Env_Reset(GetSelectedComponentLongName().c_str());
   UpdateParameters();
  }
 }
 else
 if(Section->Index == 3)
 {
  if(Application->MessageBox(L"Значения всех параметров этого компонента и его дочерних компонент будут заменены на значения по умолчанию", L"Предупреждение", MB_YESNO) == ID_YES)
  {
   Env_Default(GetSelectedComponentLongName().c_str(), true);
   Env_Reset(GetSelectedComponentLongName().c_str());
   UpdateParameters();
  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::HeaderControl3SectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
/*
 if(ParametersListStringGrid->Row<0 || ParametersListStringGrid->Row>=ParametersListStringGrid->RowCount)
  return;
 if(Section->Index == 0)
 {
  Model_SetComponentParameterValue(GetSelectedComponentLongName().c_str(),AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str(), AnsiString(ParameterValueRichEdit->Text).c_str());
  UpdateInterface(true);
 }
 else
 if(Section->Index == 1)
 {
//  if(Application->MessageBox(L"Are you sure you want to change this value in all components of the same class?",L"Warning",MB_YESNO) != ID_YES)
//   return;
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   Model_SetGlobalComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str(), AnsiString(ParameterValueRichEdit->Text).c_str());
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 2)
 {
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   std::string global_owner_stringid=Model_GetComponentClassName(CurrentComponentName.c_str());
   Model_SetGlobalOwnerComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),global_owner_stringid.c_str(),AnsiString(ParametersListStringGrid->Cells[1][ParametersListStringGrid->Row]).c_str(), AnsiString(ParameterValueRichEdit->Text).c_str());
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 3)
 {
  UpdateParameters();
 }
 else
 if(Section->Index == 4)
 {
  if(Application->MessageBox(L"Значения всех параметров этого компонента будут заменены на значения по умолчанию", L"Предупреждение", MB_YESNO) == ID_YES)
  {
   Env_Default(GetSelectedComponentLongName().c_str(), false);
   UpdateParameters();
  }
 }
 else
 if(Section->Index == 5)
 {
  if(Application->MessageBox(L"Значения всех параметров этого компонента и его дочерних компонент будут заменены на значения по умолчанию", L"Предупреждение", MB_YESNO) == ID_YES)
  {
   Env_Default(GetSelectedComponentLongName().c_str(), true);
   Env_Reset(GetSelectedComponentLongName().c_str());
   UpdateParameters();
  }
 }     */
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::HeaderControl1SectionClick(THeaderControl *HeaderControl,
		  THeaderSection *Section)
{
/* if(StatesListStringGrid->Row<0 || StatesListStringGrid->Row>=StatesListStringGrid->RowCount)
  return;
 if(Section->Index == 0)
 {
  Model_SetComponentStateValue(GetSelectedComponentLongName().c_str(),AnsiString(StatesListStringGrid->Cells[1][StatesListStringGrid->Row]).c_str(), AnsiString(StateValueRichEdit->Text).c_str());
  UpdateInterface(true);
 }
 else
 if(Section->Index == 1)
 {
//  if(Application->MessageBox(L"Are you sure you want to change this value in all components of the same class?",L"Warning",MB_YESNO) != ID_YES)
//   return;
  if(UComponentsListForm->Visible)
   UComponentsListForm->Hide();
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   Model_SetGlobalComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(),Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),AnsiString(StatesListStringGrid->Cells[1][StatesListStringGrid->Row]).c_str(), AnsiString(StateValueRichEdit->Text).c_str());
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 2)
 {
  if(UComponentsListForm->Visible)
   UComponentsListForm->Hide();
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   std::string global_owner_stringid=Model_GetComponentClassName(CurrentComponentName.c_str());
   Model_SetGlobalOwnerComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(),Model_GetComponentClassName(GetSelectedComponentLongName().c_str()),global_owner_stringid.c_str(),AnsiString(StatesListStringGrid->Cells[1][StatesListStringGrid->Row]).c_str(), AnsiString(StateValueRichEdit->Text).c_str());
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 3)
 {
  UpdateState();
 }
 else
 if(Section->Index == 4)
 {

 }   */
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Moveup1Click(TObject *Sender)
{
 Model_ChangeComponentPosition(SelectedComponentName.c_str(),-1);
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Movedown1Click(TObject *Sender)
{
 Model_ChangeComponentPosition(SelectedComponentName.c_str(),1);
 UpdateInterface(true);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Rename1Click(TObject *Sender)
{
 std::string::size_type i=SelectedComponentName.find_last_of(".");
 std::string rename_name;
 if(i == std::string::npos)
  rename_name=SelectedComponentName;
 else
  rename_name=SelectedComponentName.substr(i+1);
 String value = InputBox("Rename component", "Please enter new component name", rename_name.c_str());
 if(value.Length()==0)
  return;

 std::string new_name=AnsiString(value).c_str();
 if(new_name == rename_name)
  return;

 Model_SetComponentPropertyData(SelectedComponentName.c_str(),"Name",&new_name);
 if(DrawEngineFrame)
  DrawEngineFrame->ReloadNet();
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Delete1Click(TObject *Sender)
{
 std::string stringcompid=GetSelectedComponentName();
 if(stringcompid == "..")
  return;

 TKeyboardState State;
 GetKeyboardState(State);
 bool result = ((State[VK_SHIFT] & 128) != 0);
 if(!result)
  if(Application->MessageBox(L"Are you sure you want to delete this component?",L"Warning",MB_YESNO) != ID_YES)
   return;

 std::string stringid=GetCurrentComponentId();
 Model_DelComponent(stringid.c_str(), stringcompid.c_str());

 if(DrawEngineFrame)
  DrawEngineFrame->ReloadNet();
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::StringGridKeyPress(TObject *Sender, System::WideChar &Key)

{
 //
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
 if(Key == VK_RETURN)
 {
  StringGridDblClick(Sender);
 }
 else
 {
  TKeyboardState State;
  GetKeyboardState(State);
  bool result = ((State[VK_SHIFT] & 128) != 0);
  if(result)
  {
   if(Key == VK_UP || Key == VK_RIGHT)
   {
	Model_ChangeComponentPosition(SelectedComponentName.c_str(),-1);
	std::string name=SelectedComponentName;
	if(StringGrid->Row > 2)
	 StringGrid->Row=StringGrid->Row-1;
//	SelectedComponentName=name;
	UpdateInterface(true);
   }
   else
   if(Key == VK_DOWN || Key == VK_LEFT)
   {
	Model_ChangeComponentPosition(SelectedComponentName.c_str(),+1);
	std::string name=SelectedComponentName;
	if(StringGrid->Row < StringGrid->RowCount-1)
	 StringGrid->Row=StringGrid->Row+1;
//	SelectedComponentName=name;
	UpdateInterface(true);
   }
  }
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Reset1Click(TObject *Sender)
{
 std::string stringcompid=GetSelectedComponentName();
 if(stringcompid == "..")
  return;

 std::string stringid=GetCurrentComponentId();
 Env_Reset(stringid.c_str());

 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------



void __fastcall TUComponentsListFrame::ParametersListStringGridMouseEnter(TObject *Sender)

{
// ParametersListStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StringGridMouseEnter(TObject *Sender)
{
 StringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StatesListStringGridMouseEnter(TObject *Sender)

{
// StatesListStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::OutputsStringGridMouseEnter(TObject *Sender)

{
 OutputsStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::InputsStringGridMouseEnter(TObject *Sender)

{
 InputsStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::ParameterValueRichEditMouseEnter(TObject *Sender)

{
// ParameterValueRichEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::StateValueRichEditMouseEnter(TObject *Sender)

{
// StateValueRichEdit->SetFocus();
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::EnchancedSG1BasicStringGridDrawCell(TObject *Sender,
		  int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
  EnchancedSG1->BasicStringGridDrawCell(Sender, ACol, ARow, Rect, State);
  if(EnchancedSG1->m_cellChanged)
  {
	 String n = GetSelectedComponentLongName().c_str();
	 String sn = EnchancedSG1->BasicStringGrid->Cells[1][ARow];
	 String v = EnchancedSG1->BasicStringGrid->Cells[2][ARow];
	 Model_SetComponentParameterValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::HeaderControl2SectionClick(THeaderControl *HeaderControl,
		  THeaderSection *Section)
{
 if(EnchancedSG1->BasicStringGrid->Row<0 || EnchancedSG1->BasicStringGrid->Row>=EnchancedSG1->BasicStringGrid->RowCount)
  return;
 if(Section->Index == 0)
 {
  Model_SetComponentParameterValue(GetSelectedComponentLongName().c_str(),AnsiString(EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row]).c_str(), AnsiString(NiceParamValRichEdit->Text).c_str());
  UpdateInterface(true);
 }
 else
 if(Section->Index == 1)
 {
//  if(Application->MessageBox(L"Are you sure you want to change this value in all components of the same class?",L"Warning",MB_YESNO) != ID_YES)
//   return;
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   const char* global_name=Model_GetComponentClassName(GetSelectedComponentLongName().c_str());
   Model_SetGlobalComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), global_name,AnsiString(EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row]).c_str(), AnsiString(NiceParamValRichEdit->Text).c_str());
   Engine_FreeBufString(global_name);
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 2)
 {
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   const char *global_owner_stringid=Model_GetComponentClassName(CurrentComponentName.c_str());
   const char *selected_comp_name=Model_GetComponentClassName(GetSelectedComponentLongName().c_str());
   Model_SetGlobalOwnerComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), selected_comp_name,global_owner_stringid,AnsiString(EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row]).c_str(), AnsiString(NiceParamValRichEdit->Text).c_str());
   Engine_FreeBufString(global_owner_stringid);
   Engine_FreeBufString(selected_comp_name);
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 3)
 {
  UpdateParameters();
 }
 else
 if(Section->Index == 4)
 {
  if(Application->MessageBox(L"Значения всех параметров этого компонента будут заменены на значения по умолчанию", L"Предупреждение", MB_YESNO) == ID_YES)
  {
   Env_Default(GetSelectedComponentLongName().c_str(), false);
   UpdateParameters();
  }
 }
 else
 if(Section->Index == 5)
 {
  if(Application->MessageBox(L"Значения всех параметров этого компонента и его дочерних компонент будут заменены на значения по умолчанию", L"Предупреждение", MB_YESNO) == ID_YES)
  {
   Env_Default(GetSelectedComponentLongName().c_str(), true);
   Env_Reset(GetSelectedComponentLongName().c_str());
   UpdateParameters();
  }
 }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::NiceParamValRichEditMouseEnter(TObject *Sender)

{
 NiceParamValRichEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1BasicStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
  EnchancedSG1->BasicStringGridSelectCell(Sender, ACol, ARow, CanSelect);

  if(ARow>0)
  {
	TProperty p;
	EnchancedSG1->m_storage.GetPropertyByIndex(ARow-1, &p);
	NiceParamValRichEdit->Text = p.GetString();
	SelectedComponentParameterName=AnsiString(EnchancedSG1->BasicStringGrid->Cells[1][ARow]).c_str();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2BasicStringGridSelectCell(TObject *Sender,
          int ACol, int ARow, bool &CanSelect)
{
  EnchancedSG2->BasicStringGridSelectCell(Sender, ACol, ARow, CanSelect);
  if(ARow>0)
  {
	TProperty p;
	EnchancedSG2->m_storage.GetPropertyByIndex(ARow-1, &p);
	NiceStateValRichEdit->Text = p.GetString();
	SelectedComponentStateName=AnsiString(EnchancedSG2->BasicStringGrid->Cells[1][ARow]).c_str();
  }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::NiceParamValRichEditChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 RegistryModified=true;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::NiceStateValRichEditChange(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;
 RegistryModified=true;
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::NiceStateValRichEditMouseEnter(TObject *Sender)

{
 NiceStateValRichEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::HeaderControl4SectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
if(EnchancedSG2->BasicStringGrid->Row<0 || EnchancedSG2->BasicStringGrid->Row>=EnchancedSG2->BasicStringGrid->RowCount)
  return;

 if(Section->Index == 0)
 {
  Model_SetComponentStateValue(GetSelectedComponentLongName().c_str(),AnsiString(EnchancedSG2->BasicStringGrid->Cells[1][EnchancedSG2->BasicStringGrid->Row]).c_str(), AnsiString(NiceStateValRichEdit->Text).c_str());
  UpdateInterface(true);
 }
 else
 if(Section->Index == 1)
 {
//  if(Application->MessageBox(L"Are you sure you want to change this value in all components of the same class?",L"Warning",MB_YESNO) != ID_YES)
//   return;
  if(UComponentsListForm->Visible)
   UComponentsListForm->Hide();
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   const char *class_name=Model_GetComponentClassName(GetSelectedComponentLongName().c_str());
   Model_SetGlobalComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(),
	 class_name,
	 AnsiString(EnchancedSG2->BasicStringGrid->Cells[1][EnchancedSG2->BasicStringGrid->Row]).c_str(),
	 AnsiString(NiceStateValRichEdit->Text).c_str());
   Engine_FreeBufString(class_name);
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 2)
 {
  if(UComponentsListForm->Visible)
   UComponentsListForm->Hide();
  if(UComponentsListForm->ShowComponentSelect() == mrOk)
  {
   const char* global_owner_stringid=Model_GetComponentClassName(CurrentComponentName.c_str());
   const char* selected_name=Model_GetComponentClassName(GetSelectedComponentLongName().c_str());
   Model_SetGlobalOwnerComponentPropertyValue(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(),
	 selected_name,
	 global_owner_stringid,
	 AnsiString(EnchancedSG2->BasicStringGrid->Cells[1][EnchancedSG2->BasicStringGrid->Row]).c_str(),
	 AnsiString(NiceStateValRichEdit->Text).c_str());
   Engine_FreeBufString(global_owner_stringid);
   Engine_FreeBufString(selected_name);
   UpdateInterface(true);
  }
 }
 else
 if(Section->Index == 3)
 {
  UpdateState();
 }
 else
 if(Section->Index == 4)
 {

 }
}
//---------------------------------------------------------------------------




void __fastcall TUComponentsListFrame::EnchancedSG2MouseEnter(TObject *Sender)
{
 EnchancedSG2->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1BasicStringGridMouseEnter(TObject *Sender)

{
 EnchancedSG1->BasicStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2BasicStringGridMouseEnter(TObject *Sender)

{
 EnchancedSG2->BasicStringGrid->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1BasicStringGridDblClick(TObject *Sender)

{
  EnchancedSG1->BasicStringGridDblClick(Sender);
  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row];
	  String v = p.GetString();
	  Model_SetComponentParameterValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  }



}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1BasicStringGridKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG1->BasicStringGridKeyPress(Sender, Key);

  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row];
	  String v = p.GetString();
	  Model_SetComponentParameterValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2BasicStringGridDblClick(TObject *Sender)

{
  EnchancedSG2->BasicStringGridDblClick(Sender);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();

	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG2->BasicStringGrid->Cells[1][EnchancedSG2->BasicStringGrid->Row];
	  String v = p.GetString();
	  Model_SetComponentStateValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2BasicStringGridKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG2->BasicStringGridKeyPress(Sender, Key);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();

	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG2->BasicStringGrid->Cells[1][EnchancedSG2->BasicStringGrid->Row];
	  String v = p.GetString();
	  Model_SetComponentStateValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2txtDoubleEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG2->txtDoubleEditKeyPress(Sender, Key);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();
  }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2txtStringEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG2->txtStringEditKeyPress(Sender, Key);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();
  }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2cmbListEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG2->cmbListEditKeyPress(Sender, Key);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2txtBorderedIntEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG2->txtBorderedIntEditKeyPress(Sender, Key);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2txtIntEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG2->txtIntEditKeyPress(Sender, Key);

  if(EnchancedSG2->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1txtDoubleEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG1->txtDoubleEditKeyPress(Sender, Key);

  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1txtStringEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG1->txtStringEditKeyPress(Sender, Key);

  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1cmbListEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG1->cmbListEditKeyPress(Sender, Key);

  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1txtBorderedIntEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG1->txtBorderedIntEditKeyPress(Sender, Key);

  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1txtIntEditKeyPress(TObject *Sender,
          System::WideChar &Key)
{
  EnchancedSG1->txtIntEditKeyPress(Sender, Key);

  if(EnchancedSG1->m_bCellChanged)
  {
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
  }

}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG2BasicStringGridDrawCell(TObject *Sender,
          int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
  EnchancedSG2->BasicStringGridDrawCell(Sender, ACol, ARow, Rect, State);
  if(EnchancedSG2->m_cellChanged)
  {
	 String n = GetSelectedComponentLongName().c_str();
	 String sn = EnchancedSG2->BasicStringGrid->Cells[1][ARow];
	 String v = EnchancedSG2->BasicStringGrid->Cells[2][ARow];
	 Model_SetComponentStateValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  }
}
//---------------------------------------------------------------------------
/*
void __fastcall TUComponentsListFrame::CopyNametoClipboard1Click(TObject *Sender)

{
 Clipboard()->AsText=GetSelectedComponentParameterName().c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::CopyValuetoClipboard1Click(TObject *Sender)

{
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row];
	  Clipboard()->AsText=p.GetString();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::PasteValuefromClipboard1Click(TObject *Sender)

{
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row];
	  String v = Clipboard()->AsText;//p.GetString();
	  Model_SetComponentParameterValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
} */
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::EnchancedSG1PastevaluefromClipboard1Click(TObject *Sender)

{
  EnchancedSG1->PastevaluefromClipboard1Click(Sender);
	  TProperty p;
	  EnchancedSG1->m_storage.GetPropertyByIndex(EnchancedSG1->BasicStringGrid->Row-1,&p);
	  NiceParamValRichEdit->Text = p.GetString();
	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG1->BasicStringGrid->Cells[1][EnchancedSG1->BasicStringGrid->Row];
	  String v = p.GetString();
	  Model_SetComponentParameterValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());

  UpdateNiceParamsList(EnchancedSG1);
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsListFrame::EnchancedSG2PastevaluefromClipboard1Click(TObject *Sender)

{
	  TProperty p;
	  EnchancedSG2->m_storage.GetPropertyByIndex(EnchancedSG2->BasicStringGrid->Row-1,&p);
	  NiceStateValRichEdit->Text = p.GetString();

	  String n = GetSelectedComponentLongName().c_str();
	  String sn = EnchancedSG2->BasicStringGrid->Cells[1][EnchancedSG2->BasicStringGrid->Row];
	  String v = p.GetString();
	  Model_SetComponentStateValue(AnsiString(n).c_str(),AnsiString(sn).c_str(), AnsiString(v).c_str());
  EnchancedSG2->PastevaluefromClipboard1Click(Sender);
  UpdateNiceStatesList(EnchancedSG2);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::Calculate1Click(TObject *Sender)
{
 std::string stringcompid=GetSelectedComponentName();
 if(stringcompid == "..")
  return;

 std::string stringid=GetCurrentComponentId();
 Env_Calculate(stringid.c_str());

 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::CopynametoClipboard1Click(TObject *Sender)

{
 std::string stringcompid=GetSelectedComponentName();
 if(stringcompid == "..")
  return;

 Clipboard()->AsText=stringcompid.c_str();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::CopyclasstoClipboard2Click(TObject *Sender)

{
 std::string stringcompid=GetSelectedComponentName();
 if(stringcompid == "..")
  return;

 Clipboard()->AsText=ClassNamePanel->Caption;
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListFrame::CopylongnametoClipboard1Click(TObject *Sender)

{
 std::string stringcompid=GetSelectedComponentLongName();
 Clipboard()->AsText=stringcompid.c_str();
}
//---------------------------------------------------------------------------


