#include "Shlwapi.h"
#include <iostream>

#include "PLATFORM_FileSystem.h"
namespace PLATFORM{
#define PRINT_ERROR() printf("error on line [%d] :: %s -> %d\n", __LINE__, __func__, GetLastError());

  ///////////////////////////////////////////////////////////////////////////////
  //<Paths>
  ///////////////////////////////////////////////////////////////////////////////
  char exePath[MAX_PATH];
  char exeDir[MAX_PATH];
  const char* GetExecutablePath(){
    if(exePath[0] == 0){
      GetModuleFileName(NULL, exePath, MAX_PATH);
    }
    return exePath;    
  }
  const char* GetExecutableDir(){
    if(exeDir[0] == 0){
      if(exePath[0] == 0){
        GetExecutablePath();
      }
      memcpy(exeDir, exePath, MAX_PATH);
      GetParentDir(exeDir);
    }
    return exeDir;
  }
  char* CombinePath(const char* fileName, const char* path, void* (*Alloc)(size_t)){
    if(!fileName || !path) return nullptr;
    char fullPath[MAX_PATH];
    
    if(PathCombine(fullPath, path, fileName)){
      DWORD size = GetFullPathName(fullPath, 0, nullptr, NULL);
      char* p = (char*)Alloc(size);
      GetFullPathName(fullPath, size, p, NULL);
      
      return p;
    }
    PRINT_ERROR();
    return nullptr;    
  }
  char* GetRelativePathToFile(const char* currentDir, const char* file, void* (*Alloc)(size_t)){
    char relativePath[MAX_PATH];
    BOOL result = PathRelativePathTo(relativePath,
                                     currentDir,
                                     FILE_ATTRIBUTE_DIRECTORY,
                                     file,
                                     FILE_ATTRIBUTE_NORMAL);
    if(!result){
      PRINT_ERROR();
      return nullptr;
    }
    size_t len = strlen(relativePath);
    char* rel = (char*)Alloc(len + 1);
    memcpy(rel, relativePath, len +1 );
    return rel;
  };
  
  //<Directory Section>
  ///////////////////////////////////////////////////////////////////////////////
  bool GetParentDir(char* path){
    if(!path) return false;
    
    char buffer[MAX_PATH];
    auto len = (i32)strlen(path);
    memcpy(buffer, path, len);
    //Remove trailing slashes
    for(i32 i = len - 1; i >= 0; i--){
      char c = path[i];
      if(c == '/' || c == '\\'){
        path[i] = 0;
      }else{
        break;
      }
    }    
    bool result = PathRemoveFileSpec((LPSTR)path);
    auto newLen = strlen(path);    
    if(IsDir(path) && result){
      return true;
    }
    memcpy(path, buffer, len);
    return false;
  } 
  bool IsDir(const char* path){
    return PathFileExists(path);
  }
  bool CreateDir(const char* dirName, const char* dirPath){
    char* path = CombinePath(dirName, dirPath, malloc);
    if(!path){
      return 0;
    }
    auto success = CreateDirectory(path, NULL);
    if(!success) PRINT_ERROR();

    free(path);
    return success;
  }
  bool RemoveDir(const char* dirName, const char* dirPath){
    char* path = CombinePath(dirName, dirPath, malloc);
    if(!path){
      return path;
    }
    BOOL success = 0;
    if(IsDir(path)){
      success = RemoveDirectory(path);
      if(!success)
        PRINT_ERROR();
    }
    free(path);
    return success;
  }
  ///////////////////////////////////////////////////////////////////////////////
  //File Manip
  bool RenameFile(const char* oldFileName, const char* oldDirectory, const char* newFileName, const char* newDirectory){
    char* oldPath = CombinePath(oldFileName, oldDirectory, malloc);
    if(!oldPath) return false;

    char* newPath = CombinePath(newFileName, newDirectory, malloc);
    if(!newPath){
      free(oldPath);
      return false;
    }
    bool result = true;
    if(!MoveFile(oldPath, newPath)){
      PRINT_ERROR();
      result = false;
    }
    free(oldPath);
    free(newPath);
    return result;
  }
  //FileSize
  u64 GetFileSize(const char* fileName, const char* filePath){
    char* path = CombinePath(fileName, filePath, malloc);
    if(!path){
      PRINT_ERROR();
      return 0;
    }

    HANDLE fileHandle = CreateFile(path,
                                   GENERIC_READ,
                                   NULL,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_READONLY,
                                   NULL);

    LARGE_INTEGER size = {0};

    if(fileHandle == INVALID_HANDLE_VALUE){
      PRINT_ERROR();
    }else{
      auto result = GetFileSizeEx(fileHandle,&size);
      if(!result){
        PRINT_ERROR();
      }
      CloseHandle(fileHandle);
    }
    free(path);
    return size.QuadPart;  
  }

  //File Read
  FileContent ReadEntireFile(const char* fileName, const char* filePath, void* buffer, u64 bufferSize){
    char* path = CombinePath(fileName, filePath, malloc);
    if(!path){
      return FileContent{0};
    }
    
    HANDLE fileHandle = CreateFile(path,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_READONLY,
                                   NULL);

    free(path);
    if(fileHandle == INVALID_HANDLE_VALUE){
      return FileContent{0,nullptr};
    }
    
    DWORD size;
    BOOL result = ReadFile(fileHandle,
                           buffer,
                           (DWORD)bufferSize,
                           &size,
                           NULL);
    
    CloseHandle(fileHandle);
    if(result  == 0){
      return FileContent{0,nullptr};
    }
    FileContent file;
    file.size = size;
    file.content = (char*)buffer;
    return file;
  }
  FileContent ReadEntireFile(const char* fileName, const char* filePath, void* (*Allocate)(size_t), void (*Free)(void*)){
    char* path = CombinePath(fileName, filePath, malloc);
    if(!path){
      return FileContent{0, 0};
    }
    HANDLE fileHandle = CreateFile(path,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_READONLY,
                                   NULL);
    free(path);
    if(fileHandle == INVALID_HANDLE_VALUE)
      return FileContent{0, 0};
        
    LARGE_INTEGER fileSize;
    GetFileSizeEx(fileHandle, &fileSize);
    
    char* buffer = (char*)Allocate(fileSize.QuadPart + 1);
   
    if(!buffer){
      return FileContent{0, 0};
    }
    
    DWORD size;
    BOOL result = ReadFile(fileHandle,
                           buffer,
                           (DWORD)fileSize.QuadPart,
                           &size,
                           NULL);
    
    CloseHandle(fileHandle);
    if(result  == 0){
      Free(buffer);
      return FileContent{0,nullptr};
    }
    FileContent file;
    file.size = size;
    file.content = (char*)buffer;
    buffer[file.size] = 0;
    return file;
  }  
  //File Write
  bool WriteEntireFile(const char* fileName, const char*filePath, void* buffer, u32 bufferSize){
    char* path = CombinePath(fileName, filePath, malloc);
    if(!path){
      return false;
    }
    
    HANDLE fileHandle = CreateFile(path,
                                   GENERIC_WRITE,
                                   0,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);

    free(path);
    
    if(fileHandle == INVALID_HANDLE_VALUE){
      PRINT_ERROR();
      return false;
    }

    BOOL result = WriteFile(fileHandle,
                            buffer,
                            bufferSize,
                            NULL,
                            NULL);
    CloseHandle(fileHandle);
    if(result  == 0){
      PRINT_ERROR();
    }
    return result;
  }
  /////////////////////////////////////////////////////
  //Searching
  SearchHandle SearchForFile_Start(const char* fileName, const char* filePath, char** dstFoundFileName, void* (*Alloc)(size_t)){

    *dstFoundFileName = nullptr;
    
    char* path = CombinePath(fileName, filePath, malloc);
    if(!path){
      return SearchHandle{0};
    }
            
    WIN32_FIND_DATA findData;
    HANDLE handle = FindFirstFile(path, &findData);

    free(path);
    
    if(handle == INVALID_HANDLE_VALUE){
      return SearchHandle{0};
    }
    const char* parentPath;
    
    GetParentDir(path);
    
    size_t len = strlen(path) + 1;
    char* buffer = (char*)malloc(len);
    memcpy(buffer, path, len);
    parentPath = buffer;
    
    *dstFoundFileName = CombinePath(findData.cFileName, parentPath, Alloc);
    
    return SearchHandle{handle, parentPath};
  }
  bool SearchForFile_Next(SearchHandle* searchHandle, char** dstFoundFileName, void* (*Alloc)(size_t)){
    WIN32_FIND_DATA findData;
    BOOL found = FindNextFile(searchHandle->handle, &findData);
    
    if(!found){
      *dstFoundFileName = nullptr;
      return false;
    }
    
    *dstFoundFileName = CombinePath(findData.cFileName, searchHandle->searchPath, Alloc);
    
    return true;
  }
  void SearchForFile_End(SearchHandle* searchHandle){
    free((char*)searchHandle->searchPath);
    FindClose(searchHandle->handle);
  }
  ////////////////////////////////////////////////////
  //File Operations via file handle Handle
  FileHandle OpenFileForWriting(const char* fileName, const char* filePath){
    
    char* path = CombinePath(fileName, filePath, malloc);
    if(!path){
      return FileHandle{nullptr};
    }
    
    HANDLE fileHandle = CreateFile(path,
                                   GENERIC_WRITE,
                                   0,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    
    free(path);
    
    if(!fileHandle){
      PRINT_ERROR();
    }
    return {fileHandle};
  }
  bool WriteToFile(FileHandle* fileHandle, void* data, u32 size){
    if(!fileHandle->handle) return false;
    
    BOOL result = WriteFile(fileHandle->handle,
                            data,
                            size,
                            NULL,
                            NULL);
    if(!result){
      PRINT_ERROR();
    }
    return result;
  }
  void CloseFile(FileHandle* fileHandle){
    CloseHandle(fileHandle->handle);
  }
};
