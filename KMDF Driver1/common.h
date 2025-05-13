#pragma once

#define DEVICE_NAME     L"\\Device\\Pyongyang"
#define DOS_DEVICE_NAME L"\\DosDevices\\Pyongyang"

#define FILE_DEVICE_PYONGYANG  0x8005

#define IOCTL_PYONGYANG_INJECT_DLL CTL_CODE( FILE_DEVICE_PYONGYANG, 0x800, METHOD_NEITHER, FILE_WRITE_ACCESS )

typedef struct _INJECT_DLL
{
	ULONG pid;
	ULONG_PTR LoadLibraryA;
} INJECT_DLL, * PINJECT_DLL;