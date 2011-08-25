#include "TUFileSystem.h"
#include <windows.h>
#include <vcl.h>
#include <vector>
#include <string>


//---------------------------------------------------------------------------

// Получает список файлов или каталогов по заданному пути
int FindFilesList(const string &path, const string &mask, bool isfile, vector<string> &results)
{
   results.clear();
   HANDLE findhandle;
 string filemask=path+mask;
 WIN32_FIND_DATA finddata;
 string samplefilename;

 findhandle=FindFirstFile(
	filemask.c_str(),    // pointer to name of file to search for
	&finddata     // pointer to returned information
	);

 if(findhandle != INVALID_HANDLE_VALUE)
 {
  do
  {
   samplefilename=ExtractFileName(finddata.cFileName).t_str();

   if(samplefilename != "." && samplefilename != ".." && ((!isfile && (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || isfile)))
   {
	results.push_back(samplefilename);
   }

  } while(FindNextFile(findhandle,&finddata));
 }
 FindClose(findhandle);
 return 0;
}
