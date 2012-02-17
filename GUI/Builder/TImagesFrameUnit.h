//---------------------------------------------------------------------------

#ifndef TImagesFrameUnitH
#define TImagesFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <vector>
//---------------------------------------------------------------------------
class TImagesFrame : public TFrame
{
__published:    // IDE-managed Components
    TDrawGrid *DrawGrid;
    void __fastcall DrawGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
private:    // User declarations
public:        // User declarations
    __fastcall TImagesFrame(TComponent* Owner);


// ������ �����������
std::vector<std::vector<TImage*> > Images;

// ���� ��������� ������ ��� X ����������� ��� ������
bool ReflectionXFlag;

// --------------------------
// ������ ���������� �����������
// --------------------------
// ������������� ������ ������
void SetCellRes(int width, int height);

// ������������� ����� �����
void SetNumCells(int width, int height);

// ���������� ����� �����
int GetNumCellWidth(void);
int GetNumCellHeight(void);

// ���� ��������� ������ ��� X ����������� ��� ������
bool GetReflectionXFlag(void);
void SetReflectionXFlag(bool value);
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
// ������������� �������� ����������� � ������ � ������������ i,j
//bool SetBitmap(int i, int j, RDK::UBitmap &bitmap);
bool SetImage(int i, int j, const TImage *image);
bool SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer);

// ������������� �������� ����������� � ������� ������
//bool SetBitmap(RDK::UBitmap &bitmap);
bool SetImage(const TImage *image);
//bool SetImage(int width, int height, int colormodel, const unsigned char *buffer);

// ���������� ���������� ������� ������
int GetX(void);
int GetY(void);

// ���������� ����������� � ��������� ������
Graphics::TBitmap* GetImage(int i, int j);

// ���������� ����������� � ������� ������
Graphics::TBitmap* GetImage(void);
// --------------------------

};
//---------------------------------------------------------------------------
extern PACKAGE TImagesFrame *ImagesFrame;
//---------------------------------------------------------------------------
#endif
