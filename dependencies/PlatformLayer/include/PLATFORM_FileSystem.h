#pragma once
#include "PLATFORM_Types.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <glob.h>
#endif

namespace PLATFORM{
  struct FileContent{
    u64 size;
    char* content;
  };
  struct FileTime{
    u16 low, hight;
  };
  struct FileHandle{
#if defined(_WIN32)
    HANDLE handle;
#elif defined(__linux__)
    int handle;
#endif
  };
  struct SearchHandle{
#if defined(_WIN32)
    HANDLE handle;
    const char* searchPath; //FindFile doesn't give us the full name, which we want. So we have to combine it 
#elif defined(__linux__)
    size_t searchIndex; //Incremented by call to SearchForFile_Next
    glob_t handle;
#endif
  };
  ///////////////////////////////////////////////////////////////////////////////
  //<Paths>
  const char* GetExecutablePath();
  const char* GetExecutableDir();
  char* CombinePath(const char* end, const char* start, void* (*Alloc)(size_t));
  char* GetRelativePathToFile(const char* currentDir, const char* file, void* (*Alloc)(size_t));
  ///////////////////////////////////////////////////////////////////////////////
  //<Directory functions>
  
  // GetParentDir()
  // Moves null terminator back until a directory name is removed
  // [ <path> / <to> / <folder> / =>  <path> / <to> ]
  // 
  // @args
  // - path is a valid path in the system, if path is not found, then returns false
  //
  // @REMARKS
  // - If parent folder is not found, then returns 'false'
  // - If Path has a trailing backslash, then trailing backslashes are removed first
  // - New path does not have a slash at the end
  bool GetParentDir(char* path);
  
  bool IsDir(const char* path);
  bool CreateDir(const char* dirName, const char* path); 
  bool RemoveDir(const char* dirName, const char* path);
  
  ///////////////////////////////////////////////////////////////////////////////
  // <File Section> 
  //File Paths

  //File Manip
  bool RenameFile(const char* fileName, const char* directory, const char* newFileName, const char* newDirectory);
  
  //FileSize
  u64 GetFileSize(const char* fileName, const char* filePath);

  //File Read
  FileContent ReadEntireFile(const char* fileName, const char* filePath, void* buffer, u64 bufferSize);
  FileContent ReadEntireFile(const char* fileName, const char* filePath, void* (*Allocate)(size_t), void (*Free)(void*));

  
  //File Write
  bool WriteEntireFile(const char* fileName, const char*filePath, void* buffer, u32 bufferSize);

  /////////////////////////////////////////////////////

  SearchHandle SearchForFile_Start(const char* fileName, const char* dir, char** dstFoundFileName, void* (*Alloc)(size_t));
  void SearchForFile_End(SearchHandle* searchHandle);
  bool SearchForFile_Next(SearchHandle* searchHandle, char** dstFoundFileName, void* (*Alloc)(size_t));
  ////////////////////////////////////////////////////

  //File Operations via file handle Handle
  FileHandle OpenFileForWriting(const char* fileName, const char* filePath);
  bool  WriteToFile(FileHandle* fileHandle, void* data, u32 size);
  void  CloseFile(FileHandle* fileHandle);
};
