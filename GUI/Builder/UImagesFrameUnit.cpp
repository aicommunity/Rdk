//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UImagesFrameUnit.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
__fastcall TUImagesFrame::TUImagesFrame(TComponent* Owner)
    : TFrame(Owner)
{
 ReflectionXFlag=false;
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
void TUImagesFrame::UpdateInterface(void)
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

// ��������� ���������� �� ���������� ������ � �������� ini ����
void TUImagesFrame::SaveToIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 ini->WriteInteger(section,"NumCellWidth",GetNumCellWidth());
 ini->WriteInteger(section,"NumCellHeight",GetNumCellHeight());

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   String name=IntToStr(int(i))+String("_")+IntToStr(int(j));
   ini->WriteString(section,String("CellName")+name,StringIds[i][j].c_str());
   ini->WriteInteger(section,String("CellIndex")+name,ComponentIndexes[i][j]);
  }
 }
}

// ��������� ���������� �� ���������� ������ �� ��������� ini �����
void TUImagesFrame::LoadFromIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 int x=ini->ReadInteger(section,"NumCellWidth",1);
 int y=ini->ReadInteger(section,"NumCellHeight",1);
 SetNumCells(x,y);

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   String name=IntToStr(int(i))+String("_")+IntToStr(int(j));
   StringIds[i][j]=AnsiString(ini->ReadString(section,String("CellName")+name,"")).c_str();
   ComponentIndexes[i][j]=ini->ReadInteger(section,String("CellIndex")+name,0);
  }
 }
 UpdateInterface();
}
// --------------------------

//---------------------------------------------------------------------------
void __fastcall TUImagesFrame::DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
 if(int(Images.size())<=ACol || int(Images[ACol].size())<=ARow)
  return;

    dynamic_cast<TDrawGrid *>(Sender)->Canvas->
        StretchDraw(Rect, Images[ACol][ARow]->Picture->Graphic);
}
//---------------------------------------------------------------------------

