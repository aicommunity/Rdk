#include "TUFileSystem.h"
#include <windows.h>
#include <vcl.h>
#include <vector>
#include <string>


//---------------------------------------------------------------------------

// Получает список файлов или каталогов по заданному пути
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results)
{
   results.clear();
   HANDLE findhandle;
 std::string filemask=path+mask;
 WIN32_FIND_DATA finddata;
 std::string samplefilename;

 findhandle=FindFirstFile(
	filemask.c_str(),    // pointer to name of file to search for
	&finddata     // pointer to returned information
	);

 if(findhandle != INVALID_HANDLE_VALUE)
 {
  do
  {
   samplefilename=AnsiString(ExtractFileName(finddata.cFileName)).c_str();

   if(samplefilename != "." && samplefilename != ".." && ((!isfile && (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || isfile)))
   {
	results.push_back(samplefilename);
   }

  } while(FindNextFile(findhandle,&finddata));
 }
 FindClose(findhandle);
 return 0;
}

int CopyFile(const std::string &source_file, const std::string &dest_file)
{
 if(CopyFileEx(source_file.c_str(), dest_file.c_str(),0,0,
  false, COPY_FILE_OPEN_SOURCE_FOR_WRITE))
  return 0;

 return 1;
}

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask)
{
 std::vector<std::string> results;

 int res=FindFilesList(source_dir, mask, true, results);
 if(!res)
 {
  for(size_t i=0;i<results.size();i++)
   if(CopyFile(source_dir+results[i],dest_dir+results[i]))
    return 1;
 }
 return 0;
}

