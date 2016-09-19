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
 Класс поддержки Ini-файлов.
 Методы класса игнорируют ошибки в синтаксисе ini-файлов

 *=Комментарии=*
 Класс поддерживает однострочиые комментарии вида <//текст комментария>
 (угловые скобки не являются частью синтаксиса комментария).
 Допускается замена последовательности "//" на любую другую, не содержащую
 символов [ ] " =. Длина последовательности ограничена реализацией типа StringT.

 Комментарии могут начинаться с начала новой строки или с любого места строки
 данных. Начало комментария не распознается, если находится в участке строки,
 ограниченном двойными кавычками " или квадратными скобками [].
*/
template <typename CharT>
class UIniFile
{
typedef basic_string<CharT> StringT;
typedef list<StringT> StringListT;
typedef typename list<StringT>::iterator StringListIteratorT;
typedef typename StringT::size_type SizeT;

private: // Служебные константы
// Символы запрещенные в комментариях
StringT NoCommentSymbols;

// Символы, запрещенные в имени секции или переменной
StringT NoNameSymbols;

protected: // Параметры
// Последовательность, определяющая начало комментария
StringT SComment;

protected: // Данные
// список строк ini-файла
StringListT Lines;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы доступа к физическим данным и параметрам
// --------------------------
// Возвращает последовательность, определяющую начало комментария
inline const StringT& GetSComment(void)
 { return SComment; };
// --------------------------

// --------------------------
// Методы настройки физических параметров
// --------------------------
// Устанавливает последовательность, определяющую начало комментария
bool SetSComment(const StringT& sc)
{
 if(sc.find_first_of(NoCommentSymbols) != string::npos)
  return false;

 SComment=sc;
 return true;
};
// --------------------------

// -----------------
// Методы управления
// -----------------
// Возвращает true если секция section существует
bool Check(const StringT &section)
{
 StringT* result=0;
 SizeT startname=0, stopname=0;

 if(FindSection(section,result,startname,stopname))
  return true;

 return false;
};

// Возвращает true если переменная 'variable' секции 'section' существует
bool Check(const StringT &section, const StringT &variable)
{
 StringT* result=0;
 SizeT startname=0, stopname=0, startvalue=0;

 if(FindVariable(section,variable,result,startname,stopname,startvalue))
  return true;

 return false;
};

// Возвращает массив имен секций
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

// Возвращает массив имен переменных секции 'section'
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

// Возвращает массив строк секции 'section'
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

// Возвращает список строк ini-файла
inline const list<StringT>& GetLines(void)
{ return Lines; };

// Проверяет строку на допустимость в качестве имени секции или переменной
bool CheckName(const StringT &name)
{
 if(name.find_first_of(NoNameSymbols) != string::npos)
  return false;

 return true;
};

// Создает новую пустую секцию 'section'
// Если имя уже существует то возвращает true и не делает ничего
// Если имя недопустимо то возвращает false
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

// Создает новую переменную 'variable' в секции 'section'
// Если имя уже существует то записывает новое значение value
// Если имя недопустимо то возвращает false
// Если секции не существует то она создается
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

// Переименовывыает секцию 'section' в новое имя 'newsection'
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

// Переименовывыает переменную 'variable' секции 'section' в новое имя 'newvariable'
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

// Очищает ini-файл
bool Delete(void)
{
 Lines.clear();
 return true;
};

// Удаляет секцию 'section'
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

// Удаляет переменную 'variable' секции 'section'
bool Delete(const StringT &section, const StringT &variable)
{
 typename list<StringT>::iterator I;

 I=FindVariable(section,variable);

 Lines.erase(I);
 return true;
};

// Прочесть файл с диска
// Метод не делает ничего, если FileName == ""
bool LoadFromFile(const StringT &FileName)
{
 basic_fstream<CharT> file(FileName.c_str(), ios::in);
 CharT buffer[4096];

 Lines.clear();

 if(!file || !file.is_open())
  return false;

 while(!file.eof() && !file.fail())
  {
   file.getline(buffer,4096);
   Lines.push_back(buffer);
  }

 file.close();
 return true;
};

// Обновить файл на диске
// Метод не делает ничего, если FileName == ""
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
// Перегруженные операторы
// -----------------
// Оператор присваивания
// Метод не воздействует на FileName и SearchErrorStringT
// Метод не делает ничего, если FileName == ""
UIniFile& operator = (const UIniFile &inifile)
{
 Lines=inifile.Lines;
 SComment=inifile.SComment;

 return *this;
};

// Возвращает значение переменной variable секции section
// Если секции и/или переменной не существует, то возвращается строка 'default'
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

// Устанавливает значение переменной variable секции section в значение value
// Если секции и/или переменной не существует, то они создаются
// Метод не делает ничего, если FileName == ""
bool operator () (const StringT &section, const StringT &variable, const StringT &value)
{
 return Add(section,variable,value);
};
// -----------------

protected: // скрытые методы
// Метод поиска строки описания секции 'section'
// Метод возвращает итератор на найденную строку или end
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

// Метод поиска строки описания секции 'section'
// Метод возвращает true если секция найдена
// Дополнительная возвращаемая информация:
// result - ссылка на найденную строку
// start - начало имени секции (после открывающей '[')
// stop - конец имени секции (перед закрывающей ']')
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

// Метод поиска строки описания переменной 'variable' секции 'section'
// Метод возвращает итератор на найденную строку или end
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

// Метод поиска строки описания переменной 'variable' секции 'section'
// Метод возвращает true если переменная найдена
// Дополнительная возвращаемая информация:
// result - ссылка на найденную строку
// startname - начало имени переменной (после открывающих пробелов)
// stopname - конец имени переменной (перед '=' или завершающими имя пробелами)
// startvalue - начало значения переменной (после '=' и открывающих пробелов)
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

// Пытается декодировать строку 'str' как комментарий
// В случае успеха возвращает true и начало комментария 'start' (после открывающих пробелов)
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

// Пытается декодировать строку 'str' как секцию
// В случае успеха возвращает true и
// start - начало имени секции (после открывающей '[')
// stop - конец имени секции (перед закрывающей ']')
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

// Пытается декодировать строку 'str' как переменную
// В случае успеха возвращает true и
// startname - начало имени переменной (после открывающих пробелов)
// stopname - конец имени переменной (перед '=' или завершающими имя пробелами)
// startvalue - начало значения переменной (после '=' и открывающих пробелов)
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


