#pragma once

#include "PLATFORM_Types.h"
namespace PLATFORM{
  struct FileFilter{
    const char* name;
    const char* spec;
  };
  char* BasicFileSave(FileFilter* filters, u32 count, void* (Alloc)(size_t), void (*Free)(void*));
  char* BasicFileOpen(FileFilter* filters, u32 count, void* (Alloc)(size_t), void (*Free)(void*));
};
