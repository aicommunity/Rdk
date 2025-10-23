/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UINIFILE_H
#define UINIFILE_H

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

namespace RDK {

using namespace std;

/*
 ����� ��������� Ini-������.
 ������ ������ ���������� ������ � ���������� ini-������

 *=�����������=*
 ����� ������������ ������������ ����������� ���� <//����� �����������>
 (������� ������ �� �������� ������ ���������� �����������).
 ����������� ������ ������������������ "//" �� ����� ������, �� ����������
 �������� [ ] " =. ����� ������������������ ���������� ����������� ���� StringT.

 ����������� ����� ���������� � ������ ����� ������ ��� � ������ ����� ������
 ������. ������ ����������� �� ������������, ���� ��������� � ������� ������,
 ������������ �������� ��������� " ��� ����������� �������� [].
*/
template <typename CharT>
class UIniFile
{
typedef basic_string<CharT> StringT;
typedef list<StringT> StringListT;
typedef typename list<StringT>::iterator StringListIteratorT;
typedef typename StringT::size_type SizeT;

private: // ��������� ���������
// ������� ����������� � ������������
StringT NoCommentSymbols;

// �������, ����������� � ����� ������ ��� ����������
StringT NoNameSymbols;

protected: // ���������
// ������������������, ������������ ������ �����������
StringT SComment;

protected: // ������
// ������ ����� ini-�����
StringListT Lines;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UIniFile(void)
{
 SComment.resize(2);
 SComment[0]='/';
 SComment[1]='/';

 NoCommentSymbols.resize(6);
 NoCommentSymbols[0]='[';
 NoCommentSymbols[1]=']';
 NoCommentSymbols[2]=' ';
 NoCommentSymbols[3]='=';
 NoCommentSymbols[4]='\n';
 NoCommentSymbols[5]='\r';

 NoNameSymbols.resize(5);
 NoNameSymbols[0]='[';
 NoNameSymbols[1]=']';
 NoNameSymbols[2]='=';
 NoNameSymbols[3]='\n';
 NoNameSymbols[4]='\r';
};

UIniFile(const UIniFile<CharT> &ini)
 : SComment(ini.SComment), 
   Lines(ini.Lines)
{
};

~UIniFile(void)
{
};
// --------------------------

// --------------------------
// ������ ������� � ���������� ������ � ����������
// --------------------------
// ���������� ������������������, ������������ ������ �����������
inline const StringT& GetSComment(void)
 { return SComment; };
// --------------------------

// --------------------------
// ������ ��������� ���������� ����������
// --------------------------
// ������������� ������������������, ������������ ������ �����������
bool SetSComment(const StringT& sc)
{
 if(sc.find_first_of(NoCommentSymbols) != string::npos)
  return false;

 SComment=sc;
 return true;
};
// --------------------------

// -----------------
// ������ ����������
// -----------------
// ���������� true ���� ������ section ����������
bool Check(const StringT &section)
{
 StringT* result=0;
 SizeT startname=0, stopname=0;

 if(FindSection(section,result,startname,stopname))
  return true;

 return false;
};

// ���������� true ���� ���������� 'variable' ������ 'section' ����������
bool Check(const StringT &section, const StringT &variable)
{
 StringT* result=0;
 SizeT startname=0, stopname=0, startvalue=0;

 if(FindVariable(section,variable,result,startname,stopname,startvalue))
  return true;

 return false;
};

// ���������� ������ ���� ������
bool GetSectionList(vector<StringT> &buffer)
{
 typename list<StringT>::const_iterator I,J;
 SizeT start=0,stop=0;
 int count=0;

 I=Lines.begin(); J=Lines.end();
 buffer.resize(0);

 while(I != J)
 {
  if(DecodeAsSection(*I, start, stop))
  {
   if((int)buffer.size() <= count)
    buffer.resize(count+1);

   buffer[count]=I->substr(start,stop-start+1);
   ++count;
  }
  ++I;
 }

 return true;
};

// ���������� ������ ���� ���������� ������ 'section'
bool GetVariableList(const StringT &section, vector<StringT> &buffer)
{
 typename list<StringT>::const_iterator I,J;
 SizeT start=0,stop=0;
 SizeT startvalue=0;
 int count=0;

 I=FindSection(section);
 J=Lines.end();

 if(I == J)
  return false;

 buffer.resize(0);
 ++I;

 while(I != J)
 {
  if(DecodeAsSection(*I, start, stop))
   return true;

  if(DecodeAsVariable(*I, start, stop, startvalue))
  {
   if((int)buffer.size() <= count)
    buffer.resize(count+1);

   buffer[count]=I->substr(start,stop-start+1);
   ++count;
  }
  ++I;
 }

 return true;
};

// ���������� ������ ����� ������ 'section'
bool GetSectionLines(const StringT &section, vector<StringT> &buffer)
{
 typename list<StringT>::const_iterator I,J;
 SizeT start=0,stop=0;
 int count=0;

 I=FindSection(section);
 J=Lines.end();

 if(I == J)
  return false;

 buffer.resize(0);
 ++I;

 while(I != J)
 {
  if(DecodeAsSection(*I, start, stop))
   return true;

  if((int)buffer.size() <= count)
   buffer.resize(count+1);

  buffer[count]=*I;
  ++count;

  ++I;
 }

 return true;
};

// ���������� ������ ����� ini-�����
inline const list<StringT>& GetLines(void)
{ return Lines; };

// ��������� ������ �� ������������ � �������� ����� ������ ��� ����������
bool CheckName(const StringT &name)
{
 if(name.find_first_of(NoNameSymbols) != string::npos)
  return false;

 return true;
};

// ������� ����� ������ ������ 'section'
// ���� ��� ��� ���������� �� ���������� true � �� ������ ������
// ���� ��� ����������� �� ���������� false
bool Add(const StringT &section)
{
 typename list<StringT>::iterator I;

 if(!CheckName(section))
  return false;

 I=FindSection(section);

 if(I != Lines.end())
  return true;

 Lines.push_back(StringT(1,'[')+section+StringT(1,']'));

 return true;
};

// ������� ����� ���������� 'variable' � ������ 'section'
// ���� ��� ��� ���������� �� ���������� ����� �������� value
// ���� ��� ����������� �� ���������� false
// ���� ������ �� ���������� �� ��� ���������
bool Add(const StringT &section, const StringT &variable, const StringT &value)
{
 StringT* result=0;
 SizeT startname=0, stopname=0, startvalue=0;
 typename list<StringT>::iterator I;

 if(!CheckName(variable))
  return false;

 if(!FindVariable(section, variable, result, startname, stopname, startvalue))
 {
  I=FindSection(section);

  if(I == Lines.end())
  {
   if(!Add(section))
    return false;
   I=Lines.end();
  }
  else
   ++I;

  StringT temp=variable;
  temp+=' '; temp+='='; temp+=' '; temp+=value;
  Lines.insert(I,temp);
  return true;
 }
 else
 if(result)
 {
  *result=result->substr(0,startvalue)+value;
  return true;
 }

 return false;
};

// ���������������� ������ 'section' � ����� ��� 'newsection'
bool Rename(const StringT &section, const StringT &newsection)
{
 StringT* result=0;
 SizeT startname=0, stopname=0;

 if(!CheckName(newsection))
  return false;

 if(FindSection(section,result,startname,stopname) && result)
 {
  *result=result->substr(0,startname-1)+newsection+result->substr(stopname+1);
  return true;
 }

 return false;
};

// ���������������� ���������� 'variable' ������ 'section' � ����� ��� 'newvariable'
bool Rename(const StringT &section, const StringT &variable, const StringT &newvariable)
{
 StringT* result=0;
 SizeT startname=0, stopname=0, startvalue=0;

 if(!CheckName(newvariable))
  return false;

 if(FindVariable(section,variable,result,startname,stopname,startvalue) && result)
 {
  *result=result->substr(0,startname-1)+newvariable+result->substr(stopname+1);
  return true;
 }

 return false;
};

// ������� ini-����
bool Delete(void)
{
 Lines.clear();
 return true;
};

// ������� ������ 'section'
bool Delete(const StringT &section)
{
 typename list<StringT>::iterator I,J,K;
 SizeT start=0,stop=0;

 I=FindSection(section);
 J=Lines.end();

 if(I == J)
  return true;

 K=I;
 ++I;
 Lines.erase(K);
 while(I != J)
 {
  if(DecodeAsSection(*I, start, stop))
   return true;

  K=I; ++I;
  Lines.erase(K);
 }

 return true;
};

// ������� ���������� 'variable' ������ 'section'
bool Delete(const StringT &section, const StringT &variable)
{
 typename list<StringT>::iterator I;

 I=FindVariable(section,variable);

 Lines.erase(I);
 return true;
};

// �������� ���� � �����
// ����� �� ������ ������, ���� FileName == ""
bool LoadFromFile(const StringT &FileName)
{
 basic_fstream<CharT> file(FileName.c_str(), ios::in);
 StringT buffer;

 Lines.clear();

 if(!file || !file.is_open())
  return false;

 while(!file.eof() && !file.fail())
  {
   getline(file,buffer);
   if(!buffer.empty() && *buffer.rbegin() == '\r')
    buffer.erase(buffer.size() - 1);
   Lines.push_back(buffer);
  }

 file.close();
 return true;
};

// �������� ���� �� �����
// ����� �� ������ ������, ���� FileName == ""
bool SaveToFile(const StringT &FileName)
{
 basic_fstream<CharT> file(FileName.c_str(), ios::out | ios::trunc);

 if(!file || !file.is_open())
  return false;

 StringListIteratorT I,J;

 I=Lines.begin(); J=Lines.end();

 while(I != J)
 {
  if(file.fail())
   break;
  file.write(I->c_str(),I->size());
  ++I;
  if(I != J)
   file<<endl;//.write("\n",1);
 }

 file.close();
 return true;
};
// -----------------

// -----------------
// ������������� ���������
// -----------------
// �������� ������������
// ����� �� ������������ �� FileName � SearchErrorStringT
// ����� �� ������ ������, ���� FileName == ""
UIniFile& operator = (const UIniFile &inifile)
{
 Lines=inifile.Lines;
 SComment=inifile.SComment;

 return *this;
};

// ���������� �������� ���������� variable ������ section
// ���� ������ �/��� ���������� �� ����������, �� ������������ ������ 'default'
StringT operator () (const StringT &section, const StringT &variable, const CharT *def=0, int k=0)
{
 StringT* result=0;
 SizeT startname=0, stopname=0, startvalue=0;

 if(FindVariable(section, variable, result, startname, stopname, startvalue,k))
 {
  if(result)
  {
   if(startvalue != StringT::npos)
    return result->substr(startvalue);
   else
   {
    StringT nulres;
    return nulres;
   }
  }
 }

 if(def)
  return StringT(def);
 else
  {
   StringT nulres;
   return nulres;
  }
};

// ������������� �������� ���������� variable ������ section � �������� value
// ���� ������ �/��� ���������� �� ����������, �� ��� ���������
// ����� �� ������ ������, ���� FileName == ""
bool operator () (const StringT &section, const StringT &variable, const StringT &value)
{
 return Add(section,variable,value);
};
// -----------------

protected: // ������� ������
// ����� ������ ������ �������� ������ 'section'
// ����� ���������� �������� �� ��������� ������ ��� end
StringListIteratorT FindSection(const StringT &section)
{
 typename list<StringT>::iterator I,J;
 SizeT start=0,stop=0;

 I=Lines.begin(); J=Lines.end();

 while(I != J)
 {
  if(DecodeAsSection(*I, start, stop))
  {
 //  StringT str=I->substr(start,stop-start+1);
   if(section == I->substr(start,stop-start+1))
    break;
  }
  ++I;
 }

 return I;
};

// ����� ������ ������ �������� ������ 'section'
// ����� ���������� true ���� ������ �������
// �������������� ������������ ����������:
// result - ������ �� ��������� ������
// start - ������ ����� ������ (����� ����������� '[')
// stop - ����� ����� ������ (����� ����������� ']')
bool FindSection(const StringT &section, StringT* &result, SizeT &start, SizeT &stop)
{
 typename list<StringT>::iterator I,J;

 I=Lines.begin(); J=Lines.end();
 result=0;

 while(I != J)
 {
  if(DecodeAsSection(*I, start, stop))
  {
   if(section == I->substr(start,stop-start+1))
   {
    result=&(*I);
    return true;
   }
  }
  ++I;
 }

 return false;
};

// ����� ������ ������ �������� ���������� 'variable' ������ 'section'
// ����� ���������� �������� �� ��������� ������ ��� end
StringListIteratorT FindVariable(const StringT &section, const StringT &variable, int k=0)
{
 typename list<StringT>::iterator I,J;
 int startname=0, stopname=0, startvalue=0;

 I=FindSection(section);
 J=Lines.end();

 if(I == J)
  return I;

 ++I;
 int k_temp=0;
 while(I != J)
 {
  if(DecodeAsSection(*I, startname, stopname))
   return J;

  if(DecodeAsVariable(*I, startname, stopname, startvalue))
  {
   if(variable == I->substr(startname,stopname-startname+1))
   {
	if(k_temp == k)
	 break;
	else
	 ++k_temp;
   }
  }
  ++I;
 }

 return I;
};

// ����� ������ ������ �������� ���������� 'variable' ������ 'section'
// ����� ���������� true ���� ���������� �������
// �������������� ������������ ����������:
// result - ������ �� ��������� ������
// startname - ������ ����� ���������� (����� ����������� ��������)
// stopname - ����� ����� ���������� (����� '=' ��� ������������ ��� ���������)
// startvalue - ������ �������� ���������� (����� '=' � ����������� ��������)
bool FindVariable(const StringT &section, const StringT &variable,
            StringT* &result,
            SizeT &startname, SizeT &stopname,
            SizeT &startvalue, int k=0)
{
 typename list<StringT>::iterator I,J;

 result=0;

 I=FindSection(section);
 J=Lines.end();

 if(I == J)
  return false;

 ++I;
 int k_temp=0;
 while(I != J)
 {
  if(DecodeAsSection(*I, startname, stopname))
   return false;

  if(DecodeAsVariable(*I, startname, stopname, startvalue))
  {
   if(variable == I->substr(startname,stopname-startname+1))
   {
	if(k_temp == k)
	{
	 result=&(*I);
	 return true;
	}
	else
	 ++k_temp;
   }
  }
  ++I;
 }

 return false;
};

// �������� ������������ ������ 'str' ��� �����������
// � ������ ������ ���������� true � ������ ����������� 'start' (����� ����������� ��������)
bool DecodeAsComment(const StringT &str, SizeT &start)
{
 SizeT i=str.find_first_not_of(' ');
 if(i == StringT::npos)
  return false;

 if(str.substr(i,2) != SComment)
  return false;

 start=str.find_first_not_of(' ',i+2);
 if(start == StringT::npos)
  start=i+2;

 return true;
};

// �������� ������������ ������ 'str' ��� ������
// � ������ ������ ���������� true �
// start - ������ ����� ������ (����� ����������� '[')
// stop - ����� ����� ������ (����� ����������� ']')
bool DecodeAsSection(const StringT &str, SizeT &start, SizeT &stop)
{
 SizeT i=str.find_first_not_of(' ');
 if(i == StringT::npos)
  return false;

 if(str[i] != '[')
  return false;

 start=str.find_first_not_of(' ',i+1);
 if(start == StringT::npos)
  return false;

 stop=str.find_first_of(']',start);
 if(stop == StringT::npos)
  return false;

 --stop;
 while(stop > start && str[stop] == ' ')
  --stop;

 if(stop < start)
  return false;

 if(!CheckName(str.substr(start,stop-start+1)))
  return false;

 return true;
};

// �������� ������������ ������ 'str' ��� ����������
// � ������ ������ ���������� true �
// startname - ������ ����� ���������� (����� ����������� ��������)
// stopname - ����� ����� ���������� (����� '=' ��� ������������ ��� ���������)
// startvalue - ������ �������� ���������� (����� '=' � ����������� ��������)
bool DecodeAsVariable(const StringT &str, SizeT &startname, SizeT &stopname, SizeT &startvalue)
{
 startname=str.find_first_not_of(' ');
 if(startname == StringT::npos)
  return false;

 std::string::size_type i=str.find_first_of('=');

 if(i == StringT::npos)
 {
  i=str.find_first_of(" \r\n",startname);
  if(i == StringT::npos)
   stopname=str.size()-1;
  else
   stopname=i-1;
 }
 else
  stopname=i-1;


 while(stopname > startname && str[stopname] == ' ')
  --stopname;

 if(stopname < startname)
  return false;

 if(!CheckName(str.substr(startname,stopname-startname+1)))
  return false;

 startvalue=str.find_first_not_of(' ',i+1);
 return true;
};

};

}
#endif


