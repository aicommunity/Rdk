#ifndef CLASSCREATESIMPLE_H
#define CLASSCREATESIMPLE_H

#include <string>
#include <vector>
#include "../../../Core/Engine/Libraries/IO/UFileIO.h" 

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
		//vector<vector<string> > LoadData;
		
		//������
		void LoadFromFile (void);
		void CreateFiles (void);
};

#endif