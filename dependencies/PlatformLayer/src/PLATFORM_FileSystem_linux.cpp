#include <linux/limits.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>

#include "PLATFORM_FileSystem.h"
namespace PLATFORM{
#define PRINT_ERROR() perror(__func__);
  ///////////////////////////////////////////////////////////////////////////////
  //<Paths>
  ///////////////////////////////////////////////////////////////////////////////
  char exePath[PATH_MAX];
  char exeDir[PATH_MAX];
  
  const char* GetExecutablePath(){
    if(exePath[0] == 0){
      char arg1[20];
      
      sprintf(arg1, "/proc/%d/exe", getpid());
      readlink(arg1, exePath, PATH_MAX);
    }
    return exePath;    
  }
  const char* GetExecutableDir(){
    if(exeDir[0] == 0){
      if(exePath[0] == 0){
        GetExecutablePath();
      }
      memcpy(exeDir, exePath, PATH_MAX);
      GetParentDir(exeDir);
    }
    return exeDir;
  }
  char* CombinePath(const char* end, const char* start, void* (*Alloc)(size_t)){
    if(!end || !start) return nullptr;
    
    auto endLen = strlen(end);
    auto startLen = strlen(start);

    char* pathName = (char*)malloc(startLen + endLen + 2);
    memcpy(pathName, start, startLen);
    
    pathName[startLen] = '/';
    memcpy(&pathName[startLen + 1], end, endLen + 1);
    for(u32 i = 0; i < startLen + endLen + 2; i++){
      if(pathName[i] == '\\') pathName[i] = '/';
    }
    
    char* buffer = (char*)Alloc(PATH_MAX);
    realpath(pathName, buffer); 
           
    free(pathName);
        
    return buffer;
  }
  char* GetRelativePathToFile(const char* currentDir, const char* file, void* (*Alloc)(size_t)){
    if(!currentDir || ! file) return nullptr;
    
    char buffer1[PATH_MAX];
    char buffer2[PATH_MAX];
    
    realpath(currentDir, buffer1);
    realpath(file, buffer2);

    u32 buffer1DirCount = 1;
    u32 buffer2DirCount = 0;
    for(char* at = buffer1; *at != 0; at++)
      if(*at == '/') buffer1DirCount++;
    for(char* at = buffer2; *at != 0; at++)
      if(*at == '/') buffer2DirCount++;


    u32 i = 0;

    u32 folderCount = buffer1DirCount;
    auto len = strlen(buffer2);
    for(i = 0; i < len; i++){
      if(buffer1[i] == '/') folderCount --;
      if(buffer1[i] != buffer2[i]) break;
      if(!buffer1[i] || !buffer2[i]) return nullptr;
    }
    if(buffer1[i] == 0) folderCount = 0;
    printf("%s\n", buffer1);
    printf("%s\n", buffer2);
    
    printf("broke at %d %c folders left %d\n", i, buffer2[i], folderCount);

    
    size_t newLen = len - i;
    if(folderCount)
      newLen += folderCount * 3;
    else
      newLen += 1;
    
    char* buffer = (char*)Alloc(newLen); 
    char* start;
    if(folderCount){
      start = buffer + (folderCount * 3);
      for(u32 y = 0; y < folderCount; y++){
        buffer[y * 3] = '.';
        buffer[y * 3 + 1] = '.';
        buffer[y * 3 + 2] = '/';
      }
    }
    else{
      start = buffer + 1;
      buffer[0] = '.';
    }
    memcpy(start, &buffer2[i], len - i + 1); 
    
    return buffer;
  }
  //<Directory Section>
  ///////////////////////////////////////////////////////////////////////////////
  bool GetParentDir(char* fullPath){
    if(!fullPath) return false;
    auto path = dirname(fullPath);
    if(*path == '.') return false;
    
    return true;
  }
  static bool DoesPathExist(const char* path){
    struct stat path_stat;
    if(stat(path, &path_stat) != 0)
      return false;
    return true;
  }
  bool IsDir(const char* path){
    struct stat path_stat;
    if(stat(path, &path_stat) != 0)
      return false;
    return S_ISDIR(path_stat.st_mode);
 
    return false;
  }
  bool CreateDir(const char* dirName, const char* path){ 
    char* pathName = CombinePath(dirName, path, malloc);
    if(!pathName) return false;
    
    auto error = mkdir(pathName, 0700);
    if(error) PRINT_ERROR();
    
    free(pathName);
    return !error;
  } 
  bool RemoveDir(const char* dirName, const char* path){
    char* pathName = CombinePath(dirName, path, malloc);
    if(!pathName) return false;
    
    auto error = rmdir(pathName);
    if(error) PRINT_ERROR();
    
    free(pathName);
    return !error;
    
  }
  ///////////////////////////////////////////////////////////////////////////////
  // <File Section> 
  //File Paths

  //File Manip
  static bool RenameFile(const char* oldName, const char* newName){
   auto err = rename(oldName, newName);
   if(err) PRINT_ERROR();
   
   return !err;
  }
  bool RenameFile(const char* fileName, const char* directory, const char* newFileName, const char* newDirectory){
    
    char* oldName = CombinePath(fileName, directory, malloc);
    if(!oldName) return false;
    char* newName = CombinePath(newFileName, newDirectory, malloc);
    if(!newName){
      free(oldName);
      return false;
    }
    bool result = false;
    if(!DoesPathExist(newName))
      result = RenameFile(oldName, newName);
    
    free(oldName);
    free(newName);
    
    return result;
  }
  
  //FileSize
  u64 GetFileSize(const char* fileName, const char* path){
    char* filePath = CombinePath(fileName, path, malloc);
    struct stat fileStat;
    int result = stat(filePath, &fileStat);
    if(result){
      fileStat.st_size = 0;
      PRINT_ERROR();    
    }
    free(filePath);    
    return fileStat.st_size;
  }
  //File Read
  FileContent ReadEntireFile(const char* fileName, const char* filePath, void* buffer, u64 bufferSize){
    char* fullPath = CombinePath(fileName, filePath, malloc);

    FileContent fileContent = {0, nullptr};
    auto file = open(fullPath, O_RDONLY);
    
    if(file != -1){
      
      auto readBytes = read(file, buffer, bufferSize);
      close(file);
      
      fileContent.size = readBytes;
      fileContent.content = (char*)buffer;      
    }
    
    free(fullPath);
    return fileContent;
  }
  FileContent ReadEntireFile(const char* fileName, const char* filePath, void* (*Allocate)(size_t), void (*Free)(void*)){
    char* fullPath = CombinePath(fileName, filePath, malloc);
    FileContent fileContent = {0, nullptr};

    auto file = open(fullPath, O_RDONLY);
    if(file != -1){   
      
      struct stat fileStat = {0};
      int result = stat(fullPath, &fileStat);
      if(result){
        PRINT_ERROR();
      }
      
      auto fileSize =  fileStat.st_size;
      char* buffer = (char*)Allocate(fileSize + 1);

      auto readBytes = read(file, buffer, fileSize);
      close(file);
      
      fileContent.size = readBytes;
      buffer[fileContent.size] = 0;
      fileContent.content = buffer;
      
    }

    free(fullPath);
    return fileContent;
  }

  
  //File Write
  bool WriteEntireFile(const char* fileName, const char*filePath, void* buffer, u32 bufferSize){

    FileContent fileContent = {0, nullptr};
    bool result = false;
    char* fullPath = CombinePath(fileName, filePath, malloc);
    
    auto file = open(fullPath, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    if(file != -1){
      auto writtenBytes = write(file, buffer, bufferSize);
      if(writtenBytes != -1)
        result = true;
      close(file);
    }
    free(fullPath);
    return result;
  }

  /////////////////////////////////////////////////////
  SearchHandle SearchForFile_Start(const char* fileName, const char* dir, char** dstFoundFileName, void* (*Alloc)(size_t)){
    char* fullPath = CombinePath(fileName, dir, malloc);
    if(!fullPath){
      *dstFoundFileName = nullptr;
      return SearchHandle{0};
    }
    SearchHandle searchHandle = {0};

    if(!glob(fullPath, 0, NULL, &searchHandle.handle)){
      searchHandle.searchIndex = 0;
      
      u32 index = searchHandle.searchIndex;
      char* name = searchHandle.handle.gl_pathv[index];
    
      size_t len = strlen(name) + 1;
      char* buffer = (char*)Alloc(len);
      memcpy(buffer, name, len);
      *dstFoundFileName = buffer;
      
    }else{
      *dstFoundFileName = nullptr;
    }

    
    free(fullPath);

    return searchHandle;
  }
  void SearchForFile_End(SearchHandle* searchHandle){
    globfree(&searchHandle->handle);
  }
  bool SearchForFile_Next(SearchHandle* searchHandle, char** dstFoundFileName, void* (*Alloc)(size_t)){
    
    searchHandle->searchIndex++;
    if(searchHandle->searchIndex >= searchHandle->handle.gl_pathc){
      *dstFoundFileName = nullptr;
      return false;
    }
    
    u32 index = searchHandle->searchIndex;
    char* name = searchHandle->handle.gl_pathv[index];
    
    size_t len = strlen(name) + 1;
    char* buffer = (char*)Alloc(len);
    memcpy(buffer, name, len);
    *dstFoundFileName = buffer;
    return true;
  }
  ////////////////////////////////////////////////////

  //File Operations via file handle Handle
  FileHandle OpenFileForWriting(const char* fileName, const char* filePath){
    char* fullPath = CombinePath(fileName, filePath, malloc);
    auto file = open(fullPath, O_WRONLY);
    free(fullPath);
    return FileHandle{file};    
  }
  bool  WriteToFile(FileHandle* fileHandle, void* data, u32 size){
    if(fileHandle->handle == -1) return false;
    return (write(fileHandle->handle, data, size) != -1);    
  }
  void  CloseFile(FileHandle* fileHandle){
    close(fileHandle->handle);
  }
};
