#include "ClassCreateSimpleCore.h"
#include "../../../Core/Utilities/USupport.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>



ACreateSimpleCore::ACreateSimpleCore()
{}

ACreateSimpleCore::~ACreateSimpleCore()
{}


void ACreateSimpleCore::LoadFromFile (void)
{
 RDK::UFileIO FileIO;
 FileIO.Default();
 FileIO.SetDirection(0);
 FileIO.SetFileName("CodeTemplates/TemplateTags.txt");
 FileIO.Calculate();
 if(!FileIO.GetDataString().empty())
 {
  sdata=FileIO.GetDataString();
  RDK::separatestring(sdata, data, '\n', 0, 0);
 }
 Replace_Data();
}
	

string ACreateSimpleCore::GetData(void)
{
	return sdata;
}

string ACreateSimpleCore::GetData(int num)
{
	return data[num];
}

int ACreateSimpleCore::GetDataSize(void)
{
	return data.size();
}

string ACreateSimpleCore::GetCPPData(void)
{
	return s_cpp_data;
}

string ACreateSimpleCore::GetCPPData(int num)
{
	return cpp_data[num];
}

vector<string> ACreateSimpleCore::GetCPPDataV(void)
{
	return cpp_data;
}

int ACreateSimpleCore::GetCPPDataSize(void)
{
	return cpp_data.size();
}

string ACreateSimpleCore::GetHData(void)
{
	return s_h_data;
}

string ACreateSimpleCore::GetHData(int num)
{
	return h_data[num];
}

vector<string> ACreateSimpleCore::GetHDataV(void)
{
	return h_data;
}

int ACreateSimpleCore::GetHDataSize(void)
{
	return h_data.size();
}

void ACreateSimpleCore::init(void)
{
	symbol = "@";
}

string ACreateSimpleCore::GetSymbol(void)
{
	return symbol;
}

void ACreateSimpleCore::RegisterData(const string &str_in, string& str_out)
{

int TempNumOne=str_in.size();
str_out.resize(TempNumOne);

for (int i=0; i<TempNumOne; ++i)
        {
			str_out[i] = tolower(str_in[i]);
        }
}

void ACreateSimpleCore::Replace_Data(void)
{
		replace_data.resize(data.size());  
		command_data.resize(replace_data.size());  //10 элементов
		register_data.resize(data.size());
		for (int i=0; i<replace_data.size(); ++i)
		{
			string temp;
			
			if(data[i].find('@')==0)
			{
			int pos;
			replace_str.assign(data[i]);
			replace_str1 = replace_str.erase(0,1);
			pos = replace_str1.find('@');
			replace_data[i] = replace_str1.erase(pos,1);
			RegisterData(replace_data[i], temp);
			register_data[i] = temp;

			sreplace_data = sreplace_data + replace_str1.erase(pos,1);
			pos = 0;
			}
	}
}

string ACreateSimpleCore::GetReplaceData(int num)
{
	return replace_data[num];
}

string ACreateSimpleCore::GetReplaceData()
{
	return sreplace_data;
}

string ACreateSimpleCore::GetRegisterData(int num)
{
	return register_data[num];
}



void ACreateSimpleCore::SetCommandData(char* argv[])
{
	for(int i = 0; i < command_data.size(); ++i)
	{
		command_data[i] = argv[i+1];//RDK::narrow2(argv[i+1]);
	}
}

string ACreateSimpleCore::GetCommandData(int num)
{
	return command_data[num];
}

void ACreateSimpleCore::LoadFromFileCPP (void)
{
 RDK::UFileIO FileIO;
 FileIO.Default();
 FileIO.SetDirection(0);
 FileIO.SetFileName("CodeTemplates/EngineClassSimple.cpp");
 FileIO.Calculate();
 if(!FileIO.GetDataString().empty())
 {
  s_cpp_data=FileIO.GetDataString();
  RDK::separatestring(s_cpp_data, cpp_data, '\n', 0, 0);
 }
}

void ACreateSimpleCore::LoadFromFileH (void)
{
 RDK::UFileIO FileIO;
 FileIO.Default();
 FileIO.SetDirection(0);
 FileIO.SetFileName("CodeTemplates/EngineClassSimple.h");
 FileIO.Calculate();
 if(!FileIO.GetDataString().empty())
 {
  s_h_data=FileIO.GetDataString();
  RDK::separatestring(s_h_data, h_data, '\n', 0, 0);
 }
}

void ACreateSimpleCore::SaveToFileH (string str, string name)
{
	string Path = "CodeTemplates/";
	string PathPlusName = Path+name;

	fstream f(PathPlusName, ios::out | ios::trunc);
	//string str; // содержимое будущего файла
	f.write(str.c_str(),str.size());
}

void ACreateSimpleCore::SaveToFileCPP (string str, string name)
{
	string Path = "CodeTemplates/";
	string PathPlusName = Path+name;
	
	fstream f(PathPlusName, ios::out | ios::trunc);
	//string str; // содержимое будущего файла
	f.write(str.c_str(),str.size());
}

void ACreateSimpleCore::CreateStrCPP_H(vector<string> smth)
{
	new_cpp_h.erase();
	
	int pos;
	int pos_n;
	int pos_r;
	
	string temp, temp_d, temp_end, temp_reg;
	string end_n = "\n";
	string end_r = "\r";

	//str - только для теста!!!
	vector <string> str;
	str.resize(14);
	str[0] = "alex_0";
	str[1] = "alex_1";
	str[2] = "alex_2";
	str[3] = "alex_3";
	str[4] = "alex_4";
	str[5] = "alex_5";
	str[6] = "alex_6";
	str[7] = "alex_7";
	str[8] = "alex_8";
	str[9] = "alex_9";
	str[10] = "alex_10";
	str[11] = "alex_11";
	str[12] = "alex_12";
	

	for(int j=0; j<smth.size(); ++j)
	{
			temp = smth[j];
			
		for (int i=0; i<GetDataSize(); ++i)
	 {
		
		temp_d = GetData(i);
		pos_r = temp_d.find(end_r);
		pos_n = temp_d.find(end_n);
		temp_d.erase(pos_r);
		if(pos_n > 0)
			temp_d.erase(pos_n);
			
		pos = 0;
		while (pos < temp.size())
		{
			pos = temp.find(temp_d, pos);
			if ( pos >= 0 && temp.empty()!=true)
			{
				//RegisterData(temp_d, temp_reg);
				temp_reg = GetRegisterData(i);
				temp_end = FindString(command_data, temp_reg);
				temp.replace(pos, temp_d.size(), temp_end, 0, temp_end.size());
			}
		}
	 }
			new_cpp_h = new_cpp_h + temp;
		}
}

string ACreateSimpleCore::FindString(vector<string> strV, string str)
{
	
	int num = strV.size();
	
	string fstr, test1, test2;
	string end_n = "\n";
	string end_r = "\r";
	string rav = "=";

	int t1, pos_r, pos_n, pos_rav;
	int pos = 0;

	pos_r = str.find(end_r);
	pos_n = str.find(end_n);
	if(pos_r > 0)
	str.erase(pos_r);
		if(pos_n > 0)
			str.erase(pos_n);

	for(int i=0; i < num; ++i)
	{	
		pos_rav = strV[i].find(rav, 0);

		if(strV[i].substr(0, pos_rav) == str)
		{
			fstr = strV[i].substr(pos_rav + 1, strV[i].size());
		}
	}

	return fstr;
}

string ACreateSimpleCore::GetNew_CPP_H(void)
{
	return new_cpp_h;
}
