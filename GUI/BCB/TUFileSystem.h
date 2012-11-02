
#ifndef TUFileSystemH
#define TUFileSystemH

#include <vector>
#include <string>

// Получает список файлов или каталогов по заданному пути
int FindFilesList(const std::string &path, const std::string &mask, bool isfile, std::vector<std::string> &results);

int CopyFile(const std::string &source_file, const std::string &dest_file);

int CopyDir(const std::string &source_dir, const std::string &dest_dir, const std::string &mask);

 #endif
