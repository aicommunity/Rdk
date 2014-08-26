//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentIOFrameUnit.h"
#include "rdk_initdll.h"
#include "myrdk.h"
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
 // Указатель на рассматриваемую сеть
// Net=0;

 // Режим работы:
 // 1 - показывать входы
 // 2 - показывать выходы
 // 3 - показывать входы и выходы
 // 4 - показывать существующие связи
 Mode=1;

 // Модификатор режима показа
 // 1 - показывать только входы (выходы, связи) своего уровня
 // 2 - показывать входы (выходы, связи) своего уровня, и всех вложенных сетей
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

 RDK::ULongIdVector buffer;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-2;

 const char *p_buf=Model_GetItemsList(ViewComponentLongId.c_str(),sublevel);
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

 RDK::ULongIdVector buffer;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-2;

 const char *p_buf=Model_GetItemsList(ViewComponentLongId.c_str(),sublevel);
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

 RDK::ULongIdVector itemsbuffer;
 std::string stringid;
 std::string name;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-2;

 const char *p_buf=Model_GetItemsList(ViewComponentLongId.c_str(),sublevel,ViewComponentOwnerLongId.c_str());
 std::string xmlbuffer;
 if(p_buf)
  xmlbuffer=p_buf;
 Engine_FreeBufString(p_buf);

 RDK::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::operator >> (storage,itemsbuffer);

 for(int i=0;i<itemsbuffer.GetSize();i++)
 {
  itemsbuffer[i].EncodeToString(stringid);

  const char *p_buf=Model_GetComponentLongName(stringid.c_str());
  if(p_buf)
   name=p_buf;
  else
   name.clear();
  Engine_FreeBufString(p_buf);

  int comp_outputs=Model_GetComponentNumInputs(stringid.c_str());
  int comp_inputs=Model_GetComponentNumOutputs(stringid.c_str());
  int max_ios=(comp_inputs>comp_outputs)?comp_inputs:comp_outputs;

  for(int j=0;j<max_ios;j++)
  {
   StringGrid->RowCount=StringGrid->RowCount+1;
   StringGrid->Cells[0][StringGrid->RowCount-1]=i;//IntToStr(int(i))+":"+IntToStr(int(j));
   if(j<comp_inputs)
	StringGrid->Cells[1][StringGrid->RowCount-1]=IntToStr(j);
   else
	StringGrid->Cells[1][StringGrid->RowCount-1]="";

   if(j<comp_outputs)
	StringGrid->Cells[2][StringGrid->RowCount-1]=IntToStr(j);//name.c_str();
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

 RDK::UStringLinksList linkslist;
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

// LinksListCIterator K,I,J,endI;
// pair<LinksListCIterator,LinksListCIterator> b;

// NAItem *item;
// NAConnector *connector;
// NameT s1,s2;

// I=linkslist.begin();
// endI=linkslist.end();
 std::string itemname,connname;
 int k=1;
 for(int i=0;i<linkslist.GetSize();i++)
  {
   itemname=linkslist[i].Item.Id;//Model_GetComponentLongName(linkslist[i].Item.Id.EncodeToString(stringid).c_str());
   StringGrid->RowCount=StringGrid->RowCount+linkslist[i].Connector.size();

   const char *p_buf=Model_GetComponentPropertiesLookupList(itemname.c_str(),ptPubOutput);
   if(p_buf)
	item_properties_names=p_buf;
   else
    item_properties_names.clear();
   Engine_FreeBufString(p_buf);

   DecodePropertiesIOList(item_properties_names,item_propertries_names_list);

   for(size_t j=0;j<linkslist[i].Connector.size();j++)
	{
	 connname=linkslist[i].Connector[j].Id;

	 const char *p_buf=Model_GetComponentPropertiesLookupList(connname.c_str(),ptPubInput);
	 if(p_buf)
	  conn_properties_names=p_buf;
	 else
	  conn_properties_names.clear();
	 Engine_FreeBufString(p_buf);

	 DecodePropertiesIOList(conn_properties_names,conn_propertries_names_list);

	 StringGrid->Cells[0][k]=IntToStr(int(i));
	 StringGrid->Cells[1][k]=StrToInt(linkslist[i].Item.Index);
	 StringGrid->Cells[2][k]=StrToInt(linkslist[i].Connector[j].Index);
	 StringGrid->Cells[3][k]=itemname.c_str();

	 if(int(item_propertries_names_list.size())>linkslist[i].Item.Index)
	  StringGrid->Cells[4][k]=item_propertries_names_list[linkslist[i].Item.Index].c_str();
	 else
      StringGrid->Cells[4][k]="";
	 StringGrid->Cells[5][k]=connname.c_str();
	 if(int(conn_propertries_names_list.size())>linkslist[i].Connector[j].Index)
	  StringGrid->Cells[6][k]=conn_propertries_names_list[linkslist[i].Connector[j].Index].c_str();
	 else
	  StringGrid->Cells[6][k]="";
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

// Декодирует список свойств-входов/выходов в map
void TUComponentIOFrame::DecodePropertiesIOList(const std::string &source, std::map<int, std::string> &result)
{
 result.clear();
 std::vector<std::string> propertries_names_list;
 std::vector<std::string> name_list;
 RDK::separatestring(source,propertries_names_list,',');

 for(size_t i=0;i<propertries_names_list.size();i++)
 {
  RDK::separatestring(propertries_names_list[i],name_list,':');
  if(name_list.size() == 2)
   result[RDK::atoi(name_list[1])]=name_list[0];
 }
}
// -----------------

// -----------------
// Вспомогательные методы управления
// -----------------
// Отображает данные выходов в таблицу
void __fastcall TUComponentIOFrame::ShowOutputs(TStringGrid *string_grid, RDK::ULongIdVector &linkslist)
{
 std::string stringid;

 std::string properties_names;
 for(int i=0;i<linkslist.GetSize();i++)
 {
  std::map<int, std::string> propertries_names_list;

  const char *p_buf=Model_GetComponentPropertiesLookupList(linkslist[i].EncodeToString(stringid).c_str(),ptPubOutput);
  if(p_buf)
   properties_names=p_buf;
  else
   properties_names.clear();
  Engine_FreeBufString(p_buf);

  DecodePropertiesIOList(properties_names, propertries_names_list);
  for(int j=0;j<Model_GetComponentNumOutputs(linkslist[i].EncodeToString(stringid).c_str());j++)
  {
   string_grid->RowCount=string_grid->RowCount+1;
   if(j == 0)
   {
	string_grid->Cells[0][string_grid->RowCount-1]=IntToStr(i);
	const char *p_buf=Model_GetComponentLongName(stringid.c_str(),ViewComponentOwnerLongId.c_str());

	if(p_buf)
	 string_grid->Cells[2][string_grid->RowCount-1]=p_buf;
	else
	 string_grid->Cells[2][string_grid->RowCount-1]="";
	Engine_FreeBufString(p_buf);

	if(int(propertries_names_list.size())>j)
	 string_grid->Cells[3][string_grid->RowCount-1]=propertries_names_list[j].c_str();
	else
	 string_grid->Cells[3][string_grid->RowCount-1]="";
   }
   else
   {
	string_grid->Cells[0][string_grid->RowCount-1]="";
	string_grid->Cells[2][string_grid->RowCount-1]="";
	if(int(propertries_names_list.size())>j)
	 string_grid->Cells[3][string_grid->RowCount-1]=propertries_names_list[j].c_str();
	else
	 string_grid->Cells[3][string_grid->RowCount-1]="";
   }
   string_grid->Cells[1][string_grid->RowCount-1]=IntToStr(int(j));
  }
 }

}

// Отображает данные входов в таблицу
void __fastcall TUComponentIOFrame::ShowInputs(TStringGrid *string_grid, RDK::ULongIdVector &linkslist)
{
 std::string stringid;
 for(int i=0;i<linkslist.GetSize();i++)
 {
  std::string properties_names;
  std::map<int, std::string> propertries_names_list;

  const char *p_buf=Model_GetComponentPropertiesLookupList(linkslist[i].EncodeToString(stringid).c_str(),ptPubInput);
  if(p_buf)
   properties_names=p_buf;
  else
   properties_names.clear();
  Engine_FreeBufString(p_buf);

  DecodePropertiesIOList(properties_names, propertries_names_list);
  for(int j=-1;j<Model_GetComponentNumInputs(linkslist[i].EncodeToString(stringid).c_str());j++)
  {
   string_grid->RowCount=string_grid->RowCount+1;
   if(j==-1)
   {
	string_grid->Cells[0][string_grid->RowCount-1]=IntToStr(i);

	const char *p_buf2=Model_GetComponentLongName(stringid.c_str(),ViewComponentOwnerLongId.c_str());
	if(p_buf2)
	 string_grid->Cells[2][string_grid->RowCount-1]=p_buf2;
	else
	 string_grid->Cells[2][string_grid->RowCount-1]="";
    Engine_FreeBufString(p_buf2);

	string_grid->Cells[3][string_grid->RowCount-1]="";
   }
   else
   {
	string_grid->Cells[0][string_grid->RowCount-1]="";
	string_grid->Cells[2][string_grid->RowCount-1]="";
	if(int(propertries_names_list.size())>j)
	 string_grid->Cells[3][string_grid->RowCount-1]=propertries_names_list[j].c_str();
	else
	 string_grid->Cells[3][string_grid->RowCount-1]="";
   }
   string_grid->Cells[1][string_grid->RowCount-1]=IntToStr(j);
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
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=40;
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

