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
		//�����������
		ACreateSimpleCore();
		//����������
		~ACreateSimpleCore();

		//����������
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

		//���������� ��� ����� ������ ��� � h ������
		string new_cpp_h;
		
		//������
		//�������� ������ �� ���������� �����
		void LoadFromFile (void);
		string GetData(void);
		string GetData(int num);
		int GetDataSize(void);
		
		//������ @
		string GetSymbol(void);
		
		//�������� ������� - �������� �����
		void Replace_Data(void);
		void RegisterData(const string &str_in, string& str_out);
		string GetReplaceData(int num);
		string GetReplaceData();
		string GetRegisterData(int num);

		void SetCommandData(char* argv[]);
		string GetCommandData(int num);
		
		//��������� �� ��� �����
		void LoadFromFileCPP (void);
		string GetCPPData(void);	//���� ���� - �������
		string GetCPPData(int num); //�� �������
		vector<string> GetCPPDataV(void);//���� ���� - ������
		int GetCPPDataSize(void);
		
		//��������� �� h �����
		void LoadFromFileH (void);
		string GetHData(void);     //���� ���� - �������
		string GetHData(int num);  //�� �������
		vector<string> GetHDataV(void); //���� ���� ������
		int GetHDataSize(void);
		
		//������� ����������� �����: str - ����������, name - ��� �����
		void SaveToFileH (string str, string name);
		void SaveToFileCPP (string str, string name);
		
		void CreateStrCPP_H(vector<string> smth); // smth - ���� GetCPPDataV(void), ���� GetHDataV(void);
		string GetNew_CPP_H(void); // �������� ������ ��� ������������ �����.
		string FindString(vector<string> strV, string str);

		void init(void);
};

#endif