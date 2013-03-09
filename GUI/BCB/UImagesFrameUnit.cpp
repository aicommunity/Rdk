//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UImagesFrameUnit.h"
#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
__fastcall TUImagesFrame::TUImagesFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 ReflectionXFlag=false;
 MyComponentsListForm=new TUComponentsListForm(this);
 UpdateInterval=100;
}

__fastcall TUImagesFrame::~TUImagesFrame(void)
{
 if(MyComponentsListForm)
  delete MyComponentsListForm;
}


// --------------------------
// ������ ���������� �����������
// --------------------------
// ������������� ������ ������
void TUImagesFrame::SetCellRes(int width, int height)
{
 DrawGrid->DefaultColWidth=width;
 DrawGrid->DefaultRowHeight=height;
}

// ������������� ����� �����
void TUImagesFrame::SetNumCells(int width, int height)
{
 DrawGrid->ColCount=width;
 DrawGrid->RowCount=height;

 for(size_t i=0;i<Images.size();i++)
  for(size_t j=0;j<Images[i].size();j++)
   delete Images[i][j];

 Images.resize(width);
 StringIds.resize(width);
 ComponentIndexes.resize(width);
 for(size_t i=0;i<Images.size();i++)
 {
  Images[i].resize(height);
  StringIds[i].resize(height);
  ComponentIndexes[i].resize(height);
  for(size_t j=0;j<Images[i].size();j++)
   Images[i][j]=new TImage(this);
 }
}

// ���������� ����� �����
int TUImagesFrame::GetNumCellWidth(void)
{
 return DrawGrid->ColCount;
}

int TUImagesFrame::GetNumCellHeight(void)
{
 return DrawGrid->RowCount;
}

// ���� ��������� ������ ��� X ����������� ��� ������
bool TUImagesFrame::GetReflectionXFlag(void)
{
 return ReflectionXFlag;
}

void TUImagesFrame::SetReflectionXFlag(bool value)
{
 ReflectionXFlag=value;
}
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
// ��������� ������ � ��������������� ����������
void TUImagesFrame::LinkToComponent(int i, int j, const std::string &stringid, int index)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return;

 StringIds[i][j]=stringid;
 ComponentIndexes[i][j]=index;
}


// ������������� �������� ����������� � ������ � ������������ i,j
bool TUImagesFrame::SetBitmap(int i, int j, const RDK::UBitmap &bitmap)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return false;

// bitmap>>Images[i][j]->Picture->Bitmap;

 SetImage(i, j, bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetColorModel(), bitmap.GetData());
 return true;
}

bool TUImagesFrame::SetImage(int i, int j, const TImage *image)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return false;

 Images[i][j]->Assign(image->Picture->Bitmap);
 return true;
}

bool TUImagesFrame::SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer)
{
 struct {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
 } SysPal;
 int linebytelength=0;

 Graphics::TBitmap* target=Images[i][j]->Picture->Bitmap;

 if(!target || !buffer)
  return false;

 target->Width=width;
 target->Height=height;

 if(!width || !height)
  return false;

 switch(colormodel)
 {
 case 1:
 break;

 case 3:
  target->HandleType=bmDIB;
  target->PixelFormat=pf24bit;
  /*
   if(!(width % 4))
	memcpy(target->ScanLine[height-1],buffer,
		  width*height*3);
   else */
   {
	linebytelength=width*3;
	if(ReflectionXFlag)
	 for(int i=0;i<height;++i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
	else
	 for(int i=height-1;i>=height;--i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
   }
 break;

 case 4:
   target->HandleType=bmDIB;
   target->PixelFormat=pf32bit;

/*   if(!(width % 4))
	memcpy(target->ScanLine[height-1],buffer,
		  width*height*4);
   else */
   {
    linebytelength=width*4;
	if(ReflectionXFlag)
	 for(int i=0;i<height;++i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
	else
	 for(int i=height-1;i>=0;--i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
   }
 break;

 case 400:
   target->HandleType=bmDIB;
   target->PixelFormat=pf8bit;
   SysPal.lpal.palVersion = 0x300;
   SysPal.lpal.palNumEntries = 256;
   for(int i=0;i<256;i++)
   {
    SysPal.lpal.palPalEntry[i].peRed=SysPal.lpal.palPalEntry[i].peGreen=SysPal.lpal.palPalEntry[i].peBlue=i;
    SysPal.lpal.palPalEntry[i].peFlags = 0;
   }

   if(target->Palette)
    DeleteObject(target->Palette);
   target->Palette = CreatePalette((const tagLOGPALETTE *)&SysPal.lpal);

 /*  if(!(width % 4))
	memcpy(target->ScanLine[height-1],buffer,
		  width*height);
   else */
   {
    linebytelength=width;
	if(ReflectionXFlag)
	 for(int i=0;i<height;++i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
	else
	 for(int i=height-1;i>=0;--i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
   }
 break;
 }
 return true;
}

// ������������� �������� ����������� � ������� ������
/*bool TImagesFrame::SetBitmap(RDK::UBitmap &bitmap)
{
 return SetBitmap(DrawGrid->Col,DrawGrid->Row,bitmap);
} */

bool TUImagesFrame::SetImage(const TImage *image)
{
 return SetImage(DrawGrid->Col,DrawGrid->Row,image);
}

// ���������� ���������� ������� ������
int TUImagesFrame::GetX(void)
{
 return DrawGrid->Col;
}

int TUImagesFrame::GetY(void)
{
 return DrawGrid->Row;
}

// ���������� ����������� � ��������� ������
Graphics::TBitmap* TUImagesFrame::GetImage(int i, int j)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return 0;

 return Images[i][j]->Picture->Bitmap;
}

// ���������� ����������� � ������� ������
Graphics::TBitmap* TUImagesFrame::GetImage(void)
{
 if(DrawGrid->Col>=0 && DrawGrid->Col<int(Images.size())
    && DrawGrid->Row>=0 && DrawGrid->Row<int(Images[DrawGrid->Col].size()))
  return Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap;

 return 0;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
void TUImagesFrame::ABeforeCalculate(void)
{

}

void TUImagesFrame::AAfterCalculate(void)
{

}

void TUImagesFrame::AUpdateInterface(void)
{
 if(DrawGrid->Visible)
 {
  for(size_t i=0;i<Images.size();i++)
  {
   for(size_t j=0;j<Images[i].size();j++)
   {
	const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentOutput(StringIds[i][j].c_str(), ComponentIndexes[i][j]);
	if(bmp)
	 SetBitmap(i, j, *bmp);
   }
  }
  DrawGrid->Repaint();
  DrawGrid->Update();
 }

 if(ScrollBox1->Visible)
 {
  if(DrawGrid->Col < 0 || DrawGrid->Row <0)
   return;
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentOutput(StringIds[DrawGrid->Col][DrawGrid->Row].c_str(), ComponentIndexes[DrawGrid->Col][DrawGrid->Row]);
  if(bmp)
   SetBitmap(DrawGrid->Col, DrawGrid->Row, *bmp);

  Graphics::TBitmap * tbmp=Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap;
  FullImage->Width=tbmp->Width;
  FullImage->Height=tbmp->Height;
  FullImage->Picture->Bitmap->Assign(tbmp);
  FullImage->Top=0;
  FullImage->Left=0;
  FullImage->Repaint();
 }

 Sleep(0);
}


// ��������� ��������� ���������� � xml
void TUImagesFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

 xml.WriteInteger("NumCellWidth",GetNumCellWidth());
 xml.WriteInteger("NumCellHeight",GetNumCellHeight());

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   std::string name=RDK::sntoa(int(i))+std::string("_")+RDK::sntoa(int(j));
   xml.WriteString(std::string("CellName")+name,StringIds[i][j].c_str());
   xml.WriteInteger(std::string("CellIndex")+name,ComponentIndexes[i][j]);
  }
 }

 xml.WriteBool("ShowLegendCheckBox",ShowLegendCheckBox->Checked);
}

// ��������� ��������� ���������� �� xml
void TUImagesFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 Images.clear();
 StringIds.clear();
 ComponentIndexes.clear();

 int x=xml.ReadInteger("NumCellWidth",1);
 int y=xml.ReadInteger("NumCellHeight",1);
 SetNumCells(x,y);

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   std::string name=RDK::sntoa(int(i))+std::string("_")+RDK::sntoa(int(j));
   StringIds[i][j]=xml.ReadString(std::string("CellName")+name,"");
   ComponentIndexes[i][j]=xml.ReadInteger(std::string("CellIndex")+name,0);
  }
 }

 ShowLegendCheckBox->Checked=xml.ReadBool("ShowLegendCheckBox",true);

 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUImagesFrame::DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
 if(int(Images.size())<=ACol || int(Images[ACol].size())<=ARow)
  return;

 dynamic_cast<TDrawGrid *>(Sender)->Canvas->
		StretchDraw(Rect, Images[ACol][ARow]->Picture->Graphic);
 if(ShowLegendCheckBox->Checked)
  dynamic_cast<TDrawGrid *>(Sender)->Canvas->TextOut(Rect.Left,Rect.Top,(std::string(Model_GetComponentLongName(StringIds[ACol][ARow].c_str()))+std::string("[")+RDK::sntoa(ComponentIndexes[ACol][ARow])+"]").c_str());
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::SaveToBmpClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 if(!SavePictureDialog->Execute())
  return;

 Images[DrawGrid->Col][DrawGrid->Row]->Picture->SaveToFile(SavePictureDialog->FileName);
}
//---------------------------------------------------------------------------


void __fastcall TUImagesFrame::SelectSourceClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 StringIds[DrawGrid->Col][DrawGrid->Row]=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 ComponentIndexes[DrawGrid->Col][DrawGrid->Row]=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::AddColumnClick(TObject *Sender)
{
 SetNumCells(GetNumCellWidth()+1, GetNumCellHeight());
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::AddRowClick(TObject *Sender)
{
 SetNumCells(GetNumCellWidth(), GetNumCellHeight()+1);
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::DeleteColumnClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 StringIds.erase(StringIds.begin()+DrawGrid->Col);

 ComponentIndexes.erase(ComponentIndexes.begin()+DrawGrid->Col);

 Images.erase(Images.begin()+DrawGrid->Col);

 SetNumCells(GetNumCellWidth()-1,GetNumCellHeight());
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::DeleteRowClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 for(size_t i=0;i<StringIds.size();i++)
  StringIds[i].erase(StringIds[i].begin()+DrawGrid->Row);

 for(size_t i=0;i<ComponentIndexes.size();i++)
  ComponentIndexes[i].erase(ComponentIndexes[i].begin()+DrawGrid->Row);

 for(size_t i=0;i<Images.size();i++)
  Images[i].erase(Images[i].begin()+DrawGrid->Row);

 SetNumCells(GetNumCellWidth(),GetNumCellHeight()-1);
 UpdateInterface();
}
//---------------------------------------------------------------------------



void __fastcall TUImagesFrame::DrawGridDblClick(TObject *Sender)
{
  if(DrawGrid->Col < 0 || DrawGrid->Row <0)
   return;
  DrawGrid->Visible=false;
  ScrollBox1->Visible=true;
 // FullImage->Align=alClient;
  Graphics::TBitmap * bmp=Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap;
  FullImage->Width=bmp->Width;
  FullImage->Height=bmp->Height;
  FullImage->Picture->Bitmap->Assign(bmp);
  FullImage->Top=0;
  FullImage->Left=0;
  FullImage->Repaint();
  if(ShowLegendCheckBox->Checked)
  {
   FullImage->Canvas->Font->Size=12;
   FullImage->Canvas->TextOut(0,0,(std::string(Model_GetComponentLongName(StringIds[DrawGrid->Col][DrawGrid->Row].c_str()))+std::string("[")+RDK::sntoa(ComponentIndexes[DrawGrid->Col][DrawGrid->Row])+"]").c_str());
  }
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::FullImageDblClick(TObject *Sender)
{
  DrawGrid->Visible=true;
  ScrollBox1->Visible=false;
  UpdateInterface(true);
}
//---------------------------------------------------------------------------
void __fastcall TUImagesFrame::FullImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  presIm=false;
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::FullImageMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
  if(presIm)
  {
	ScrollBox1->HorzScrollBar->Position = ScrollBox1->HorzScrollBar->Position + (x2 - X);
	ScrollBox1->VertScrollBar->Position = ScrollBox1->VertScrollBar->Position + (y2 - Y);
  }
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::FullImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  presIm=true;
  x2=X;
  y2=Y;
}
//---------------------------------------------------------------------------

