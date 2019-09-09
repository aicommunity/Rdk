#ifndef UDLLLOADER_H
#define UDLLLOADER_H

#include "../../Deploy/Include/initdll_defs.h"
#include <string>

/// Dll loader abstaract class
class RDK_LIB_TYPE UDllLoader
{
public:
  UDllLoader(){}
  virtual ~UDllLoader(){}

  virtual bool Load(void) = 0;
  virtual bool Load(const std::string &dll_name) = 0;
  virtual bool isLoaded() = 0;
  virtual bool UnLoad(void) = 0;
  virtual void * Resolve(const std::string &symbol) = 0;
  virtual std::string GetErrorString() = 0;
};

#endif // UDLLLOADER_H
