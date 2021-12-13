#include <windows.h> 
#include <shobjidl.h>
#include "PLATFORM_DialogWindow.h"
namespace PLATFORM{
  static char* OpenDialogWindow(IID windowType, FileFilter* filters, u32 count, void* (Alloc)(size_t), void (*Free)(void*)){
    
    COMDLG_FILTERSPEC* filter = count ? (COMDLG_FILTERSPEC*)Alloc(sizeof(*filter) * count) : nullptr;
    for(u32 i = 0; i < count; i++){
      { //Copy name
        size_t len = strlen(filters[i].name) + 1;
        u16* name = (u16*)Alloc(len * 2);
        for(u32 y = 0; y < len; y++){
          name[y] = (u16)filters[i].name[y];
        }
        memcpy(&filter[i].pszName, &name, 8);
      }
      { //Copy name
        size_t len = strlen(filters[i].spec) + 1;
        u16* spec = (u16*)Alloc(len * 2);
        for(u32 y = 0; y < len; y++){
          spec[y] = (u16)filters[i].spec[y];
        }
        memcpy(&filter[i].pszSpec, &spec, 8);
      }
      
      
    };

    
    char* path = nullptr;
    IFileDialog *pfd = NULL;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
                                COINIT_DISABLE_OLE1DDE);
    hr = CoCreateInstance(windowType,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&pfd));
    
    if(!SUCCEEDED(hr)) goto End;

    DWORD dwFlags;
    hr = pfd->GetOptions(&dwFlags);
    if(!SUCCEEDED(hr)) goto End;
    
    // In this case, get shell items only for file system items.
    hr = pfd->SetOptions(dwFlags | FOS_NOCHANGEDIR);
    if(!SUCCEEDED(hr)) goto End;
  
    if(filter){
      LPCWSTR extension = filter[0].pszSpec;
      HRESULT result = pfd->SetDefaultExtension(extension);
    }
                                               
    hr = pfd->SetFileTypes(count, filter);
    
    hr = pfd->Show(NULL);
    if(!SUCCEEDED(hr)) goto End;
    // Obtain the result, once the user clicks the 'Open' button.
    // The result is an IShellItem object.
    IShellItem *psiResult;

    hr = pfd->GetResult(&psiResult);
    if (SUCCEEDED(hr)) {
      // We are just going to print out the name of the file for sample sake.
      PWSTR pszFilePath = NULL;
      hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

      u32 len = 0;
      u16* at = (u16*)pszFilePath;
      while(*at){
        at++;
        len++;
      }
      path = (char*)Alloc(len + 1);
      for(u32 i = 0; i < len; i++){
        path[i] = (char)(pszFilePath[i]);
      }
      path[len] = 0;
      
      CoTaskMemFree(pszFilePath);
    }
    psiResult->Release();
    CoUninitialize();
    
  End:
    if(pfd)
      pfd->Release();

    for(u32 i = 0; i < count; i++){
      Free((void*)filter[i].pszName);
      Free((void*)filter[i].pszSpec);  
    };
    if(count)
      Free(filter);
    
    return path;
  }
  char* BasicFileSave(FileFilter* filters, u32 count, void* (Alloc)(size_t), void (*Free)(void*)){ //Pass in restricted file types
    return OpenDialogWindow(CLSID_FileSaveDialog, filters, count, Alloc, Free);
  }
  char* BasicFileOpen(FileFilter* filters, u32 count, void* (Alloc)(size_t), void (*Free)(void*)){ //Pass in restricted file types
    return OpenDialogWindow(CLSID_FileOpenDialog, filters, count, Alloc, Free);
  }
};
