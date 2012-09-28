/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UFontH
#define UFontH

#include <vector>
#include <map>
#include <string>
#include "../Utilities/UIniFile.h"
#include "UBitmap.h"

namespace RDK {

class UAGraphics;

// �������� �����
class UFontLine
{
public:
int x1,y1,x2,y2;
};


// �������� ���������� �������
class UBitmapFontSymbol
{
public:
// ����������� �������
UBitmap Data;
};

// �������� ���������� �������
class UVectorFontSymbol
{
public:
// ����� �����, ����������� ������
// �� ������ ���������, ������������� � ������ ������� ���� �������
std::vector<UFontLine> Lines;
};

class UAFont
{
protected:

protected: // ���������
// ��� ������
string Name;

// ������ ������ � ��������
int Height;

// ������������� �������� � ��������� �� ������ ������
float Interval;

// ������� � % �� ���������
float Scale;

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UAFont(void);
UAFont(const UAFont &font);
~UAFont(void);
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ��� ������
const string& GetName(void) const;
virtual bool SetName(const string& name);

// ������ ������ � ��������
int GetHeight(void) const;

// ������������� �������� � ��������� �� ������ ������
float GetInterval(void) const;
virtual bool SetInterval(float value);

// ������� � % �� ���������
float GetScale(void) const;
virtual bool SetScale(float value);
// --------------------------


// --------------------------
// ������ ����������
// --------------------------
// ���������� ������ ������� � ��������
virtual int CalcWidth(wchar_t ch)=0;
virtual int CalcWidth(char ch)=0;
int CalcWidth(const wstring &str);
int CalcWidth(const string &str);

// ������������ �������� ������ � ������� ������� �����
bool Draw(wchar_t ch, UAGraphics *graphics);
bool Draw(char ch, UAGraphics *graphics);
bool Draw(const wstring &str, UAGraphics *graphics);
bool Draw(const string &str, UAGraphics *graphics);
// --------------------------

// --------------------------
// ������ �������� � ����������
// ��������� ������ ��������� ������� ch �� �����
virtual bool Load(wchar_t ch, const string &filename)=0;
virtual bool Save(wchar_t ch, const string &filename)=0;
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� �����������
UAFont& operator = (const UAFont &font);
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
protected:
// ������������ �������� ������ � ������� ������� �����
virtual void DrawSymbol(wchar_t ch, UAGraphics *graphics)=0;
virtual void DrawSymbol(char ch, UAGraphics *graphics)=0;
// --------------------------
};


class UBitmapFont: public UAFont
{
protected: // ���������


protected: // ������
// ������� �������� ��������
std::map<wchar_t,UBitmapFontSymbol> Table;

// ������� ���������������� �������� ��������
std::map<wchar_t,UBitmapFontSymbol> ScaledTable;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UBitmapFont(void);
UBitmapFont(const UBitmapFont &font);
~UBitmapFont(void);
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ������� � % �� ���������
bool SetScale(float value);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ���������� ������ ������� � ��������
int CalcWidth(wchar_t ch);
int CalcWidth(char ch);
// --------------------------

// --------------------------
// ������ �������� � ����������
// --------------------------
// ���������/��������� ������ ��������� ������� ch �� �����
bool Load(wchar_t ch, const string &filename);
bool Save(wchar_t ch, const string &filename);

// ���������/��������� ������ �������� � ������ �� ��������� [ch1,ch2] �� �����
// ������� �������� � ����� ����������� ��������������� rect
// ������ ������� size;
bool Load(const string &filename, const UBPoint &size=UBPoint(16,16), const UBRect &rect=UBRect(-1,-1,-1,-1), wchar_t ch1=0, wchar_t ch2=255);
bool Save(const string &filename, const UBPoint &size=UBPoint(16,16), const UBRect &rect=UBRect(-1,-1,-1,-1), wchar_t ch1=0, wchar_t ch2=255);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� �����������
UBitmapFont& operator = (const UBitmapFont &font);
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
protected:
// ������������ �������� ������ � ������� ������� �����
void DrawSymbol(wchar_t ch, UAGraphics *graphics);
void DrawSymbol(char ch, UAGraphics *graphics);
// --------------------------
};

class UVectorFont: public UAFont
{
protected: // ���������
// ������� ����� ������
int PenSize;

protected: // ������
// ������� �������� ��������
std::map<wchar_t,UVectorFontSymbol> Table;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVectorFont(void);
UVectorFont(const UVectorFont &font);
~UVectorFont(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ���������� ������ ������� � ��������
int CalcWidth(wchar_t ch);
int CalcWidth(char ch);
// --------------------------

// --------------------------
// ������ �������� � ����������
// --------------------------
// ��������� ������ ��������� ������� ch �� �����
bool Load(wchar_t ch, const string &filename);
bool Save(wchar_t ch, const string &filename);
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� �����������
UVectorFont& operator = (const UVectorFont &font);
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
protected:
// ������������ �������� ������ � ������� ������� �����
void DrawSymbol(wchar_t ch, UAGraphics *graphics);
void DrawSymbol(char ch, UAGraphics *graphics);
// --------------------------
};

}

#endif

