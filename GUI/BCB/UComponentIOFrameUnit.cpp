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
 // ��������� �� ��������������� ����
// Net=0;

 // ����� ������:
 // 1 - ���������� �����
 // 2 - ���������� ������
 // 3 - ���������� ����� � ������
 // 4 - ���������� ������������ �����
 Mode=1;

 // ����������� ������ ������
 // 1 - ���������� ������ ����� (������, �����) ������ ������
 // 2 - ���������� ����� (������, �����) ������ ������, � ���� ��������� �����
 ShowModifier=1;
}
//---------------------------------------------------------------------------

// -----------------

// -----------------
// ������ ����������
// -----------------
// ������� �������
void __fastcall TUComponentIOFrame::Clear(void)
{
 StringGrid->RowCount=1;
 StringGrid->ColCount=1;
}

// ��������� ������� ������� ����
void __fastcall TUComponentIOFrame::ShowInputs(void)
{
 int current_row=StringGrid->Row;

 StringGrid->ColCount=3;
 StringGrid->Cells[1][0]="#";
 StringGrid->Cells[2][0]="����";

 RDK::ULongIdVector buffer;
 std::string stringid;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-1;

 std::string xmlbuffer=Model_GetItemsList(ViewComponentLongId.c_str(),sublevel);

 RDK::Serialize::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::Serialize::operator >> (storage,buffer);

 for(int i=0;i<buffer.GetSize();i++)
 {
  for(int j=-1;j<Model_GetComponentNumInputs(buffer[i].EncodeToString(stringid).c_str());j++)
  {
   StringGrid->RowCount=StringGrid->RowCount+1;
   if(j==-1)
   {
	StringGrid->Cells[0][StringGrid->RowCount-1]=IntToStr(i);
	StringGrid->Cells[2][StringGrid->RowCount-1]=Model_GetComponentLongName(stringid.c_str());
   }
   else
   {
	StringGrid->Cells[0][StringGrid->RowCount-1]="";
	StringGrid->Cells[2][StringGrid->RowCount-1]="";

   }
   StringGrid->Cells[1][StringGrid->RowCount-1]=IntToStr(j);
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

// ��������� ������� �������� ����
void __fastcall TUComponentIOFrame::ShowOutputs(void)
{        
 int current_row=StringGrid->Row;

 StringGrid->ColCount=3;
 StringGrid->Cells[1][0]="#";
 StringGrid->Cells[2][0]="�����";

 RDK::ULongIdVector buffer;
 std::string stringid;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-1;

 std::string xmlbuffer=Model_GetItemsList(ViewComponentLongId.c_str(),sublevel);

 RDK::Serialize::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::Serialize::operator >> (storage,buffer);

 for(int i=0;i<buffer.GetSize();i++)
 {
  for(int j=0;j<Model_GetComponentNumInputs(buffer[i].EncodeToString(stringid).c_str());j++)
  {
   StringGrid->RowCount=StringGrid->RowCount+1;
   if(j == 0)
   {
	StringGrid->Cells[0][StringGrid->RowCount-1]=IntToStr(i);
	StringGrid->Cells[2][StringGrid->RowCount-1]=Model_GetComponentLongName(stringid.c_str());
   }
   else
   {
	StringGrid->Cells[0][StringGrid->RowCount-1]="";
	StringGrid->Cells[2][StringGrid->RowCount-1]="";
   }
   StringGrid->Cells[1][StringGrid->RowCount-1]=IntToStr(int(j));
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

// ��������� ������� ������� � �������� ����
void __fastcall TUComponentIOFrame::ShowInputsOutputs(void)
{
 int current_row=StringGrid->Row;

 StringGrid->ColCount=4;
 StringGrid->Cells[1][0]="#In";
 StringGrid->Cells[2][0]="���";
 StringGrid->Cells[2][0]="#Out";

 RDK::ULongIdVector itemsbuffer;
 std::string stringid;
 std::string name;

 int sublevel=0;
 if(ShowModifier == 1)
  sublevel=0;
 else
 if(ShowModifier == 2)
  sublevel=-1;
 std::string xmlbuffer=Model_GetItemsList(ViewComponentLongId.c_str(),sublevel);

 RDK::Serialize::USerStorageXML storage;
 storage.Load(xmlbuffer,"Items");
 RDK::Serialize::operator >> (storage,itemsbuffer);

 for(int i=0;i<itemsbuffer.GetSize();i++)
 {
  itemsbuffer[i].EncodeToString(stringid);
  name=Model_GetComponentLongName(stringid.c_str());
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

// ��������� ������� �������������� �������
void __fastcall TUComponentIOFrame::ShowLinks(void)
{
 int current_row=StringGrid->Row;
 StringGrid->ColCount=5;
 StringGrid->Cells[1][0]="Out #";
 StringGrid->Cells[2][0]="In #";
 StringGrid->Cells[3][0]="�����";
 StringGrid->Cells[4][0]="����";

 RDK::UStringLinksList linkslist;
 std::string stringid;

 std::string xmlbuffer=Model_GetComponentInternalLinks(ViewComponentLongId.c_str());

 RDK::Serialize::USerStorageXML storage;
 storage.Load(xmlbuffer,"Links");
 RDK::Serialize::operator >> (storage,linkslist);

 StringGrid->RowCount=linkslist.GetSize()+1;

// LinksListCIterator K,I,J,endI;
// pair<LinksListCIterator,LinksListCIterator> b;

// NAItem *item;
// NAConnector *connector;
// NameT s1,s2;

// I=linkslist.begin();
// endI=linkslist.end();
 std::string itemname,connname;
 for(int i=0;i<linkslist.GetSize();i++)
  {
   itemname=linkslist[i].Item.Id;//Model_GetComponentLongName(linkslist[i].Item.Id.EncodeToString(stringid).c_str());
   for(size_t j=0;j<linkslist[i].Connector.size();j++)
	{
     connname=linkslist[i].Connector[j].Id;//Model_GetComponentLongName(linkslist[i].Connector[j].Id.EncodeToString(stringid).c_str());
	 StringGrid->Cells[0][i+1]=IntToStr(int(i));
	 StringGrid->Cells[1][i+1]=StrToInt(linkslist[i].Item.Index);
	 StringGrid->Cells[2][i+1]=StrToInt(linkslist[i].Connector[j].Index);
	 StringGrid->Cells[3][i+1]=itemname.c_str();
	 StringGrid->Cells[4][i+1]=connname.c_str();
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
// -----------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void TUComponentIOFrame::ABeforeCalculate(void)
{

}

// �����, ���������� ����� ���� �������
void TUComponentIOFrame::AAfterCalculate(void)
{

}

// ���������� ����������
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

// ��������� ��������� ���������� � xml
void TUComponentIOFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TUComponentIOFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{

}
// -----------------------------

void __fastcall TUComponentIOFrame::FrameResize(TObject *Sender)
{
 switch(Mode)
 {
 case 1:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1];
 break;

 case 2:
  StringGrid->ColWidths[0]=40;
  StringGrid->ColWidths[1]=40;
  StringGrid->ColWidths[2]=StringGrid->Width-StringGrid->ColWidths[0]-StringGrid->ColWidths[1];
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
  StringGrid->ColWidths[3]=(StringGrid->Width-120)/2;
  StringGrid->ColWidths[4]=StringGrid->Width-(StringGrid->ColWidths[0]+
						StringGrid->ColWidths[1]+StringGrid->ColWidths[2]);
 break;
 }
}
//---------------------------------------------------------------------------

