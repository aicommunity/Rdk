// ClassCreateSimple.cpp : Defines the entry point for the console application.
//
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "../ClassCreateSimpleCore.h"


int _tmain(int argc, _TCHAR* argv[])
{
	
	ACreateSimpleCore CreateSimpleCore;
	CreateSimpleCore.LoadFromFile();

		return 0;
}
