// ClassCreateSimple.cpp : Defines the entry point for the console application.
//
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "../../ClassCreateSimpleCore.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int c;
	string name1 = "aaa.h";
	string name2 = "aaa.cpp";

	ACreateSimpleCore CreateSimpleCore;
	//Инициаллизация;
	CreateSimpleCore.init();
	//Загрузка из фаила;
	CreateSimpleCore.LoadFromFile();
	CreateSimpleCore.LoadFromFileCPP();
	CreateSimpleCore.LoadFromFileH();
	//cout<<CreateSimpleCore.GetData()<<"\n";
	
	CreateSimpleCore.SetCommandData(argv);
	for(int i=0; i<9; ++i)
	{
	cout<<CreateSimpleCore.GetCommandData(i)<<"\n";
	}

	cout<<"\n";
	CreateSimpleCore.CreateStrCPP_H(CreateSimpleCore.GetHDataV());
	CreateSimpleCore.SaveToFileH (CreateSimpleCore.GetNew_CPP_H(), name1);
	
	CreateSimpleCore.CreateStrCPP_H(CreateSimpleCore.GetCPPDataV());
	CreateSimpleCore.SaveToFileCPP (CreateSimpleCore.GetNew_CPP_H(), name2);
	for(int i=0; i<10; ++i)
	{
		cout<<CreateSimpleCore.GetRegisterData(i)<<"\n";
	}

	cout<<CreateSimpleCore.GetReplaceData()<<"\n";
	cin>>c;
	return 0;
}

