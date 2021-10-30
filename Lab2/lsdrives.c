#include <Windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <winnls.h>
#include <fileapi.h>

int ListDrives()
{ 
    wchar_t sPath[2048]; 
    HANDLE hFind;

    wchar_t name[2048];
    DWORD number;
    wchar_t fsName[2048];
    wchar_t mountName[2048];
    wchar_t logNames[2048];
    if((hFind = FindFirstVolume(sPath, 2048)) == INVALID_HANDLE_VALUE) 
    { 
        return 0;
    } 

    do
    { 
        wchar_t* drawLeeter = L"NONE";
        DWORD size = GetLogicalDriveStrings(2048, logNames);
        for (int i = 0; 4*i < size; i += 4)
        { 
            GetVolumeNameForVolumeMountPoint(&logNames[i], mountName, 2048);
            if (wcscmp(mountName, sPath) == 0) {
                drawLeeter = &logNames[i];
                break;
            }
        }
        DWORD sectors;
        DWORD free;
        GetDiskFreeSpace(sPath,NULL,NULL,&free, &sectors);
        GetVolumeInformation(sPath, name, 2048, NULL, NULL, NULL, fsName, 2048);
        wprintf(L"%10s %10s %4s   %9d/%-9d\n", sPath, fsName, drawLeeter, free, sectors);
    } 
    while(FindNextVolume(hFind, sPath, 2048));

    FindVolumeClose(hFind);

    return 1; 
} 

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    ListDrives();
}
