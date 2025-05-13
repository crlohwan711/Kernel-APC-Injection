#include <stdio.h>
#include <Windows.h>

#include "..\KMDF Driver1\common.h"

int main( void )
{
    SetConsoleTitleA( "Seo Yea-ji" );
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 11 );

    HMODULE kernel32 = LoadLibraryA( "kernel32.dll" );
    if ( kernel32 == NULL )
    {
        printf( "[-] ERROR: LoadLibraryA failed with error 0x%X\n", GetLastError() );
        system( "pause" );
        return 0;
    }

    FARPROC pAddress = GetProcAddress( kernel32, "LoadLibraryA" );
    if ( pAddress == NULL )
    {
        printf( "[-] ERROR: GetProcAddress failed with error 0x%X\n", GetLastError() );
        FreeLibrary( kernel32 );
        system( "pause" );
        return 0;
    }

    FreeLibrary( kernel32 );

    printf( "[?] Make sure that the driver has completed all processes\n" );
    system( "pause" );

    HWND hWnd = FindWindowA( NULL, "Task Manager" );
    if ( hWnd == NULL )
    {
        printf( "[-] ERROR: FindWindowA failed with error 0x%X\n", GetLastError() );
        system( "pause" );
        return 0;
    }

    DWORD ProcessId = 0;
    GetWindowThreadProcessId( hWnd, &ProcessId );

    HANDLE hDevice = CreateFileA( "\\\\.\\Pyongyang", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
    if ( hDevice == INVALID_HANDLE_VALUE )
    {
        printf( "[-] ERROR: CreateFileA failed with error 0x%X\n", GetLastError() );
        system( "pause" );
        return 0;
    }

    INJECT_DLL data = { 0 };
    data.pid = ProcessId;
    data.LoadLibraryA = ( ULONG_PTR ) pAddress;

    DWORD bytes = 0;
    if ( !DeviceIoControl( hDevice, IOCTL_PYONGYANG_INJECT_DLL, &data, sizeof( data ), NULL, 0, &bytes, NULL ) )
    {
        printf( "[-] ERROR: DeviceIoControl failed with error 0x%X\n", GetLastError() );
        CloseHandle( hDevice );
        system( "pause" );
        return 0;
    }

    printf( "[+] Successfully done!\n" );
    CloseHandle( hDevice );

    return system( "pause" );
}