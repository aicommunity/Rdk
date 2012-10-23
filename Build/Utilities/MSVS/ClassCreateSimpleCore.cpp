#include "ClassCreateSimpleCore.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>



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
  FileIO.GetDataString();
  RDK::separatestring(FileIO.GetDataString(), data, '\n', 0, 0);
 }
}
		
void CreateFiles (void)
{}