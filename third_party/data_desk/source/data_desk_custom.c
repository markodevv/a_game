/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Data Desk

Author  : Ryan Fleury
Updated : 5 December 2019
License : MIT, at end of file.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct DataDeskCustom DataDeskCustom;
struct DataDeskCustom
{
    DataDeskInitCallback    *InitCallback;
    DataDeskParseCallback   *ParseCallback;
    DataDeskCleanUpCallback *CleanUpCallback;
    
#if BUILD_WIN32
    HANDLE custom_dll;
#elif BUILD_LINUX
    void *custom_dll;
#endif
    
};

static DataDeskCustom
DataDeskCustomLoad(char *custom_dll_path)
{
    DataDeskCustom custom = {0};
    
#if BUILD_WIN32
    custom.custom_dll = LoadLibraryA(custom_dll_path);
    if(custom.custom_dll)
    {
        Log("Custom layer successfully loaded from \"%s\".", custom_dll_path);
        custom.InitCallback      = (void *)GetProcAddress(custom.custom_dll, "DataDeskCustomInitCallback"   );
        custom.ParseCallback     = (void *)GetProcAddress(custom.custom_dll, "DataDeskCustomParseCallback"  );
        custom.CleanUpCallback   = (void *)GetProcAddress(custom.custom_dll, "DataDeskCustomCleanUpCallback");
    }
#elif BUILD_LINUX
    custom.custom_dll = dlopen(custom_dll_path, RTLD_NOW);
    if(custom.custom_dll)
    {
        Log("Custom layer successfully loaded from \"%s\".", custom_dll_path);
        custom.InitCallback      = dlsym(custom.custom_dll, "DataDeskCustomInitCallback"   );
        custom.ParseCallback     = dlsym(custom.custom_dll, "DataDeskCustomParseCallback"  );
        custom.CleanUpCallback   = dlsym(custom.custom_dll, "DataDeskCustomCleanUpCallback");
    }
#endif
    
    if(!custom.InitCallback && !custom.ParseCallback && !custom.CleanUpCallback)
    {
        LogError("WARNING: No callbacks successfully loaded in custom layer.");
    }
    
    return custom;
}

static void
DataDeskCustomUnload(DataDeskCustom *custom)
{
    
#if BUILD_WIN32
    FreeLibrary(custom->custom_dll);
#elif BUILD_LINUX
    if(custom->custom_dll)
    {
        dlclose(custom->custom_dll);
    }
#endif
    
    custom->InitCallback = 0;
    custom->ParseCallback = 0;
    custom->CleanUpCallback = 0;
    custom->custom_dll = 0;
}

/*
Copyright 2019 Ryan Fleury

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
