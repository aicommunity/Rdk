
#ifndef TUFileSystemH
#define TUFileSystemH

#include <vector>
#include <string>

using namespace std;

// �������� ������ ������ ��� ��������� �� ��������� ����
int FindFilesList(const string &path, const string &mask, bool isfile, vector<string> &results);
 #endif
