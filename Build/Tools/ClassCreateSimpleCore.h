#ifndef CLASSCREATESIMPLE_H
#define CLASSCREATESIMPLE_H

#include <string>
#include <vector>
#include "../../Core/Engine/Libraries/IO/UFileIO.h" 

using namespace std;

class ACreateSimpleCore
{ 
	int i;
	public:
		// онструктор
		ACreateSimpleCore();
		//ƒеструктор
		~ACreateSimpleCore();

		//ѕеременные
		vector<string> data;		
		vector<string> cpp_data;
		vector<string> h_data;

		string sdata;
		string s_cpp_data;
		string s_h_data;

		string replace_str;
		string replace_str1;
		string replace_str2;
		string sreplace_data;
		string sregister_data;

		vector<string> replace_data;
		vector<string> register_data;
		vector<string> command_data;

		string symbol;

		//переменные дл€ новых фаилов срр и h фаилов
		string new_cpp_h;
		
		//ћетоды
		//получаем данные из текстового фаила
		void LoadFromFile (void);
		string GetData(void);
		string GetData(int num);
		int GetDataSize(void);
		
		//символ @
		string GetSymbol(void);
		
		//замен€ем регистр - тестовый режим
		void Replace_Data(void);
		void RegisterData(const string &str_in, string& str_out);
		string GetReplaceData(int num);
		string GetReplaceData();
		string GetRegisterData(int num);

		void SetCommandData(char* argv[]);
		string GetCommandData(int num);
		
		//«агружаем из срр фаила
		void LoadFromFileCPP (void);
		string GetCPPData(void);	//весь фаил - строкой
		string GetCPPData(int num); //по строкам
		vector<string> GetCPPDataV(void);//весь фаил - вектор
		int GetCPPDataSize(void);
		
		//«агружаем из h фаила
		void LoadFromFileH (void);
		string GetHData(void);     //весь фаил - строкой
		string GetHData(int num);  //по строкам
		vector<string> GetHDataV(void); //весь фаил вектор
		int GetHDataSize(void);
		
		//создаем необходимые фаилы: str - содержимое, name - им€ фаила
		void SaveToFileH (string str, string name);
		void SaveToFileCPP (string str, string name);
		
		void CreateStrCPP_H(vector<string> smth); // smth - либо GetCPPDataV(void), либо GetHDataV(void);
		string GetNew_CPP_H(void); // забираем строку дл€ формировани€ фаила.
		string FindString(vector<string> strV, string str);

		void init(void);
};

#endif