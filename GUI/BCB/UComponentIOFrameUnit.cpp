//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentIOFrameUnit.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentIOFrame *UComponentIOFrame;
//---------------------------------------------------------------------------
__fastcall TUComponentIOFrame::TUComponentIOFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 Mode=1;
 ShowModifier=2;
}
//---------------------------------------------------------------------------

// -----------------

// -----------------
// Методы управления
// -----------------
// Очищает таблицу
void __fastcall TUComponentIOFrame::Clear(void)
{
 StringGrid->RowCount=1;
 StringGrid->ColCount=1;
}

// Заполняет таблицу входами сети
void __fastcall TUComponentIOFrame::ShowInputs(void)
{
 int current_row=StringGrid->Row;

 StringGrid->ColCount=4;
 StringGrid->Cells[1][0]="#";
 StringGrid->Cells[2][0]="Вход";
 StringGrid->Cells[3][0]="Имя входа";

 std::list<std::string> buffer;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-2;

 const char *p_buf=Model_GetComponentsListEx(ViewComponentLongId.c_str(),sublevel);
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::operator >> (storage,buffer);

 ShowInputs(StringGrid,buffer);

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу выходами сети
void __fastcall TUComponentIOFrame::ShowOutputs(void)
{        
 int current_row=StringGrid->Row;

 StringGrid->ColCount=4;
 StringGrid->Cells[1][0]="#";
 StringGrid->Cells[2][0]="Выход";
 StringGrid->Cells[3][0]="Имя выхода";

 std::list<std::string> buffer;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-2;

 const char *p_buf=Model_GetComponentsListEx(ViewComponentLongId.c_str(),sublevel);
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::operator >> (storage,buffer);

 ShowOutputs(StringGrid,buffer);

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу входами и выходами сети
void __fastcall TUComponentIOFrame::ShowInputsOutputs(void)
{
 int current_row=StringGrid->Row;

 StringGrid->ColCount=4;
 StringGrid->Cells[1][0]="#In";
 StringGrid->Cells[2][0]="Имя";
 StringGrid->Cells[3][0]="#Out";

 std::list<std::string> itemsbuffer;
 std::string stringid;
 std::string name;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-2;

 const char *p_buf=Model_GetComponentsListEx(ViewComponentLongId.c_str(),sublevel,ViewComponentOwnerLongId.c_str());
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::operator >> (storage,itemsbuffer);

 std::list<std::string>::iterator I=itemsbuffer.begin();
 int i(0);
 for(;I!=itemsbuffer.end();I++,i++)
 {
  const char *p_buf=Model_GetComponentLongName(I->c_str());
  if(p_buf)
   name=p_buf;
  else
   name.clear();
  Engine_FreeBufString(p_buf);

  int comp_outputs=Model_GetComponentNumInputs(I->c_str());
  int comp_inputs=Model_GetComponentNumOutputs(I->c_str());
  int max_ios=(comp_inputs>comp_outputs)?comp_inputs:comp_outputs;

  for(int j=0;j<max_ios;j++)
  {
   StringGrid->RowCount=StringGrid->RowCount+1;
   StringGrid->Cells[0][StringGrid->RowCount-1]=i;
   if(j<comp_inputs)
	StringGrid->Cells[1][StringGrid->RowCount-1]=IntToStr(j);
   else
	StringGrid->Cells[1][StringGrid->RowCount-1]="";

   if(j<comp_outputs)
	StringGrid->Cells[2][StringGrid->RowCount-1]=IntToStr(j);
   else
	StringGrid->Cells[2][StringGrid->RowCount-1]="";

   if(j == 0)
	StringGrid->Cells[3][StringGrid->RowCount-1]=name.c_str();
   else
	StringGrid->Cells[3][StringGrid->RowCount-1]="";

  }
 }

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу установленными связями
void __fastcall TUComponentIOFrame::ShowLinks(void)
{
 int current_row=StringGrid->Row;
 StringGrid->ColCount=7;
 StringGrid->Cells[1][0]="Out #";
 StringGrid->Cells[2][0]="In #";
 StringGrid->Cells[3][0]="Выход";
 StringGrid->Cells[4][0]="Имя выхода";
 StringGrid->Cells[5][0]="Вход";
 StringGrid->Cells[6][0]="Имя входа";

 RDK::ULinksList linkslist;
 std::string stringid;
 std::string conn_properties_names,item_properties_names;
 std::map<int,std::string> conn_propertries_names_list,item_propertries_names_list;

 const char *p_buf=Model_GetComponentPersonalLinks(ViewComponentLongId.c_str(),ViewComponentOwnerLongId.c_str());
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Links");
 RDK::operator >> (storage,linkslist);

 StringGrid->RowCount=1;

 std::string itemname,connname;
 int k=1;
 for(int i=0;i<linkslist.GetSize();i++)
  {
   itemname=linkslist[i].Item.ComponentName;
   StringGrid->RowCount=StringGrid->RowCount+linkslist[i].Connector.size();
   for(size_t j=0;j<linkslist[i].Connector.size();j++)
	{
	 connname=linkslist[i].Connector[j].ComponentName;
	 StringGrid->Cells[0][k]=IntToStr(int(i));
	 StringGrid->Cells[1][k]=StrToInt(linkslist[i].Item.Index);
	 StringGrid->Cells[2][k]=StrToInt(linkslist[i].Connector[j].Index);
	 StringGrid->Cells[3][k]=itemname.c_str();

	 StringGrid->Cells[4][k]=linkslist[i].Item.PropertyName.c_str();
	 StringGrid->Cells[5][k]=connname.c_str();
     StringGrid->Cells[6][k]=linkslist[i].Connector[j].PropertyName.c_str();
	 ++k;
	}
  }

 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу входящими связями
void __fastcall TUComponentIOFrame::ShowInputLinks(void)
{
 int current_row=StringGrid->Row;
 StringGrid->ColCount=3;
 StringGrid->Cells[0][0]="#";
 StringGrid->Cells[1][0]="Item";
 StringGrid->Cells[2][0]="Connector";

 RDK::ULinksList linkslist;
 std::string stringid;
 std::string conn_properties_names,item_properties_names;
 std::map<int,std::string> conn_propertries_names_list,item_propertries_names_list;

 const char *p_buf=Model_GetComponentPersonalLinks(ViewComponentLongId.c_str(),ViewComponentOwnerLongId.c_str());
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Links");
 RDK::operator >> (storage,linkslist);

 StringGrid->RowCount=1;

 std::string itemname,connname;
 int k=1;
 StringGrid->ColWidths[0]=20;
 for(int i=0;i<linkslist.GetSize();i++)
  {
   itemname=linkslist[i].Item.ComponentName;
   StringGrid->RowCount=StringGrid->RowCount+linkslist[i].Connector.size();
   for(size_t j=0;j<linkslist[i].Connector.size();j++)
	{
	 connname=linkslist[i].Connector[j].ComponentName;
	 StringGrid->Cells[0][k]=IntToStr(int(i));
	 std::string::size_type dot_n=connname.find(".",ViewComponentLongId.size());
	 std::string::size_type n=connname.find(ViewComponentLongId);
//	 std::string part_name=connname.substr(0,n);
//	 if(part_name == ViewComponentLongId)
	 if(n==0 && (dot_n == std::string::npos || dot_n == ViewComponentLongId.size()))
	 {
	  StringGrid->Cells[1][k]=(itemname+std::string(":")+linkslist[i].Item.PropertyName).c_str();
	  StringGrid->Cells[2][k]=(connname+std::string(":")+linkslist[i].Connector[j].PropertyName).c_str();
	  int line_size=StringGrid->Canvas->TextWidth(StringGrid->Cells[1][k]);
	  if(StringGrid->ColWidths[1]<line_size+10)
	   StringGrid->ColWidths[1]=line_size+10;
	  line_size=StringGrid->Canvas->TextWidth(StringGrid->Cells[2][k]);
	  if(StringGrid->ColWidths[2]<line_size+10)
	   StringGrid->ColWidths[2]=line_size+10;
	  ++k;
	 }
	}
  }
 StringGrid->RowCount=k;
 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}

// Заполняет таблицу исходящими связями
void __fastcall TUComponentIOFrame::ShowOutputLinks(void)
{
 int current_row=StringGrid->Row;
 StringGrid->ColCount=3;
 StringGrid->Cells[0][0]="#";
 StringGrid->Cells[1][0]="Item";
 StringGrid->Cells[2][0]="Connector";

 RDK::ULinksList linkslist;
 std::string stringid;
 std::string conn_properties_names,item_properties_names;
 std::map<int,std::string> conn_propertries_names_list,item_propertries_names_list;

 const char *p_buf=Model_GetComponentPersonalLinks(ViewComponentLongId.c_str(),ViewComponentOwnerLongId.c_str());
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Links");
 RDK::operator >> (storage,linkslist);

 StringGrid->RowCount=1;

 std::string itemname,connname;
 int k=1;
 StringGrid->ColWidths[0]=20;
 for(int i=0;i<linkslist.GetSize();i++)
  {
   itemname=linkslist[i].Item.ComponentName;
   StringGrid->RowCount=StringGrid->RowCount+linkslist[i].Connector.size();
   for(size_t j=0;j<linkslist[i].Connector.size();j++)
	{
	 connname=linkslist[i].Connector[j].ComponentName;
	 StringGrid->Cells[0][k]=IntToStr(int(i));
//	 std::string::size_type n=itemname.find(".");
//	 std::string part_name=itemname.substr(0,n);
	 std::string::size_type dot_n=itemname.find(".",ViewComponentLongId.size());
	 std::string::size_type n=itemname.find(ViewComponentLongId);
//	 if(itemname.substr(0,n) == ViewComponentLongId)
	 if(n==0 && (dot_n == std::string::npos || dot_n == ViewComponentLongId.size()))
	 {
      StringGrid->Cells[0][k]=k;
	  StringGrid->Cells[1][k]=(itemname+std::string(":")+linkslist[i].Item.PropertyName).c_str();
	  StringGrid->Cells[2][k]=(connname+std::string(":")+linkslist[i].Connector[j].PropertyName).c_str();
	  int line_size=StringGrid->Canvas->TextWidth(StringGrid->Cells[1][k]);
	  if(StringGrid->ColWidths[1]<line_size+10)
	   StringGrid->ColWidths[1]=line_size+10;
	  line_size=StringGrid->Canvas->TextWidth(StringGrid->Cells[2][k]);
	  if(StringGrid->ColWidths[2]<line_size+10)
	   StringGrid->ColWidths[2]=line_size+10;
	  ++k;
	 }
	}
  }

 StringGrid->RowCount=k;
 if(StringGrid->RowCount>1)
 {
  StringGrid->FixedRows=1;
  StringGrid->FixedCols=1;
 }

 if(StringGrid->RowCount>current_row && current_row>0)
  StringGrid->Row=current_row;
}



// Декодирует список свойств-входов/выходов в map
void TUComponentIOFrame::DecodePropertiesIOList(const std::string &source, std::map<int, std::string> &result)
{
 result.clear();
 std::vector<std::string> propertries_names_list;
 std::vector<std::string> name_list;
 RDK::separatestring(source,propertries_names_list,',');

 int nonamed_num=0;
 bool is_named = false;
 for(size_t i=0;i<propertries_names_list.size();i++)
 {
  if((propertries_names_list[i].find("DataOutput")!=std::string::npos)||(propertries_names_list[i].find("DataInput")!=std::string::npos))
   nonamed_num++;
 }
 if(nonamed_num<int(propertries_names_list.size()))
 {
  is_named = true;
 }
 for(size_t i=0, j=0;i<propertries_names_list.size();i++)
 {
  RDK::separatestring(propertries_names_list[i],name_list,':');
  if(name_list.size() == 2)
  {
//   result[RDK::atoi(name_list[1])]=name_list[0];
   if(is_named)
   {
	if((name_list[0].find("DataOutput")==std::string::npos)&&(name_list[0].find("DataInput")==std::string::npos))
	{
	 result[j]=name_list[0];
	 j++;
	}
   }
   else
   {
	if(name_list[0].find("DataInput")!=std::string::npos)
	{
	 if(j==0)
	 {
	  result[0]="-1";
	  j++;
	 }
	 result[j]=name_list[0];
	 j++;
	}
	else if(name_list[0].find("DataOutput")!=std::string::npos)
	{

	 result[j]=name_list[0];
	 j++;
	}
   }
  }
 }
}
// -----------------

// -----------------
// Вспомогательные методы управления
// -----------------
// Отображает данные выходов в таблицу
void __fastcall TUComponentIOFrame::ShowOutputs(TStringGrid *string_grid, std::list<std::string> &linkslist)
{
 std::string stringid;

 std::string properties_names;
 std::list<std::string>::iterator I=linkslist.begin();
 int i(0);
 for(;I != linkslist.end();I++,i++)
 {
  std::map<int, std::string> propertries_names_list;

  const char *p_buf=Model_GetComponentPropertiesLookupList(I->c_str(),ptPubOutput);
  if(p_buf)
   properties_names=p_buf;
  else
   properties_names.clear();
  Engine_FreeBufString(p_buf);

  DecodePropertiesIOList(properties_names, propertries_names_list);
  for(size_t j=0;j<propertries_names_list.size();j++)
  {
   string_grid->RowCount=string_grid->RowCount+1;
   if(j == 0)
   {
	string_grid->Cells[0][string_grid->RowCount-1]=IntToStr(i);
	const char *p_buf=Model_GetComponentLongName(I->c_str(),ViewComponentOwnerLongId.c_str());

	if(p_buf)
	 string_grid->Cells[2][string_grid->RowCount-1]=p_buf;
	else
	 string_grid->Cells[2][string_grid->RowCount-1]="";
	Engine_FreeBufString(p_buf);

	if(propertries_names_list.size()>j)
	 string_grid->Cells[3][string_grid->RowCount-1]=propertries_names_list[j].c_str();
	else
	 string_grid->Cells[3][string_grid->RowCount-1]="";
   }
   else
   {
	string_grid->Cells[0][string_grid->RowCount-1]="";
	string_grid->Cells[2][string_grid->RowCount-1]="";
	if(propertries_names_list.size()>j)
	 string_grid->Cells[3][string_grid->RowCount-1]=propertries_names_list[j].c_str();
	else
	 string_grid->Cells[3][string_grid->RowCount-1]="";
   }
   string_grid->Cells[1][string_grid->RowCount-1]=IntToStr(int(j));
  }
 }

}

// Отображает данные входов в таблицу
void __fastcall TUComponentIOFrame::ShowInputs(TStringGrid *string_grid, std::list<std::string> &linkslist)
{
 std::list<std::string>::iterator I=linkslist.begin();
 int i(0);
 for(;I != linkslist.end();I++,i++)
 {
  std::string properties_names;
  std::map<int, std::string> propertries_names_list;

  const char *p_buf=Model_GetComponentPropertiesLookupList(I->c_str(),ptPubInput);
  if(p_buf)
   properties_names=p_buf;
  else
   properties_names.clear();
  Engine_FreeBufString(p_buf);

  DecodePropertiesIOList(properties_names, propertries_names_list);
  for(size_t j=0;j<propertries_names_list.size();j++)
  {
   string_grid->RowCount=string_grid->RowCount+1;
   if(j==0)
   {
	string_grid->Cells[0][string_grid->RowCount-1]=IntToStr(i);

	const char *p_buf2=Model_GetComponentLongName(I->c_str(),ViewComponentOwnerLongId.c_str());
	if(p_buf2)
	 string_grid->Cells[2][string_grid->RowCount-1]=p_buf2;
	else
	 string_grid->Cells[2][string_grid->RowCount-1]="";
    Engine_FreeBufString(p_buf2);
   }
   else
   {
	string_grid->Cells[0][string_grid->RowCount-1]="";
	string_grid->Cells[2][string_grid->RowCount-1]="";
   }
   if(propertries_names_list.size()>j)
	string_grid->Cells[3][string_grid->RowCount-1]=propertries_names_list[j].c_str();
   else
	string_grid->Cells[3][string_grid->RowCount-1]="";
   string_grid->Cells[1][string_grid->RowCount-1]=IntToStr(int(j));
  }
 }


}
// -----------------


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TUComponentIOFrame::ABeforeCalculate(void)
{

}

// Метод, вызываемый после шага расчета
void TUComponentIOFrame::AAfterCalculate(void)
{

}

// Обновление интерфейса
void TUComponentIOFrame::AUpdateInterface(void)
{
 Clear();
 switch(Mode)
 {
 case 1:
  ShowInputs();
 break;

 case 2:
  ShowOutputs();
 break;

 case 3:
  ShowInputsOutputs();
 break;

 case 4:
  ShowLinks();
 break;

 case 5:
  ShowInputLinks();
 break;

 case 6:
  ShowOutputLinks();
 break;
 }

 FrameResize(this);
}

// Возврат интерфейса в исходное состояние
void TUComponentIOFrame::AClearInterface(void)
{
 ViewComponentOwnerLongId="";
 ViewComponentLongId="";
}

// Сохраняет параметры интерфейса в xml
void TUComponentIOFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUComponentIOFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 ViewComponentOwnerLongId="";
 ViewComponentLongId="";
}
// -----------------------------

void __fastcall TUComponentIOFrame::FrameResize(TObject *Sender)
{
 switch(Mode)
 {
 case 1:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=(StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1])/2;
  StringGrid->ColWidths[3]=(StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1])/2;
 break;

 case 2:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=(StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1])/2;
  StringGrid->ColWidths[3]=(StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1])/2;
 break;

 case 3:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=40;
  StringGrid->ColWidths[3]=StringGrid->Width-(StringGrid->ColWidths[0]+
						StringGrid->ColWidths[1]+StringGrid->ColWidths[2]);
 break;

 case 4:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=0;
  StringGrid->ColWidths[2]=0;
  StringGrid->ColWidths[3]=(StringGrid->Width-120)/4;
  StringGrid->ColWidths[4]=(StringGrid->Width-120)/4;
  StringGrid->ColWidths[5]=(StringGrid->Width-120)/4;
  StringGrid->ColWidths[6]=(StringGrid->Width-120)/4;
//  StringGrid->ColWidths[4]=StringGrid->Width-(StringGrid->ColWidths[0]+
//						StringGrid->ColWidths[1]+StringGrid->ColWidths[2]);
 break;
 }
}
//---------------------------------------------------------------------------

