#include <Windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <winnls.h>

int ListDirectoryContents(const wchar_t *sDir)
{ 
    WIN32_FIND_DATA fdFile; 
    HANDLE hFind = NULL; 

    wchar_t sPath[2048]; 

    wsprintf(sPath, L"%s\\*.*", sDir); 

    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) 
    { 
        wprintf(L"Path not found: %s\n", sDir); 
        return 0; 
    } 

    do
    { 
        if(wcscmp(fdFile.cFileName, L".") != 0
                && wcscmp(fdFile.cFileName, L"..") != 0) 
        { 
            wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName); 

            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) 
            { 
                wprintf(L"Directory: %-50s ", sPath); 
            } 
            else{ 
                wprintf(L"File:      %-50s ", sPath); 
            } 

            wchar_t bufferDate[2048];
            wchar_t bufferTime[2048];
            SYSTEMTIME st;
            FileTimeToSystemTime(&fdFile.ftLastWriteTime, &st);

            GetDateFormat(GetSystemDefaultLCID(), DATE_SHORTDATE, &st, NULL, bufferDate, _countof(bufferDate));
            GetTimeFormat(GetSystemDefaultLCID(), 0, &st, NULL, bufferTime, _countof(bufferTime));
            wprintf(L"%5d %10s %10s\n", fdFile.nFileSizeLow, bufferDate, bufferTime);
        }
    } 
    while(FindNextFile(hFind, &fdFile));

    FindClose(hFind);

    return 1; 
} 

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    if (argc < 2)
    {
        printf("Please specify path\n");
        return -1;
    }
    for (int i = 1; i < argc; i++)
        ListDirectoryContents(argv[i]);
    return 0;
}
