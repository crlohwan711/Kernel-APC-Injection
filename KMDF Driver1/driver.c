#include <ntifs.h>
#include <ntstrsafe.h>

#include "common.h"
#include "imports.h"
#include "functions.h"

NTSTATUS CreateClose( PDEVICE_OBJECT DeviceObject, PIRP Irp )
{
	UNREFERENCED_PARAMETER( DeviceObject );

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return STATUS_SUCCESS;
}

NTSTATUS DeviceControl( PDEVICE_OBJECT DeviceObject, PIRP Irp )
{
	UNREFERENCED_PARAMETER( DeviceObject );

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation( Irp );
	ULONG ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

	switch ( ioControlCode )
	{
	case IOCTL_PYONGYANG_INJECT_DLL:
	{
		if ( irpStack->Parameters.DeviceIoControl.InputBufferLength >= sizeof( INJECT_DLL ) )
		{
			PINJECT_DLL pData = ( PINJECT_DLL ) irpStack->Parameters.DeviceIoControl.Type3InputBuffer;

			Irp->IoStatus.Status = InjectDll( pData );
		}
		else
			Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;

		break;
	}

	default:
		Log( "[-] ERROR: Unrecognized IOCTL 0x%X\n", ioControlCode );
		Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	status = Irp->IoStatus.Status;
	IoCompleteRequest( Irp, IO_NO_INCREMENT );

	return status;
}

NTSTATUS DriverEntry( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	UNREFERENCED_PARAMETER( RegistryPath );

	UNICODE_STRING deviceName;
	RtlInitUnicodeString( &deviceName, DEVICE_NAME );

	PDEVICE_OBJECT deviceObject = NULL;
	NTSTATUS status = IoCreateDevice( DriverObject, 0, &deviceName, FILE_DEVICE_PYONGYANG, 0, FALSE, &deviceObject );
	if ( !NT_SUCCESS( status ) )
	{
		Log( "[-] ERROR: IoCreateDevice failed with status 0x%X\n", status );
		return status;
	}

	DriverObject->MajorFunction[ IRP_MJ_CREATE ] = CreateClose;
	DriverObject->MajorFunction[ IRP_MJ_CLOSE ] = CreateClose;
	DriverObject->MajorFunction[ IRP_MJ_DEVICE_CONTROL ] = DeviceControl;

	UNICODE_STRING deviceLink;
	RtlInitUnicodeString( &deviceLink, DOS_DEVICE_NAME );

	status = IoCreateSymbolicLink( &deviceLink, &deviceName );
	if ( !NT_SUCCESS( status ) )
	{
		Log( "[-] ERROR: IoCreateSymbolicLink failed with status 0x%X\n", status );
		IoDeleteDevice( deviceObject );
		return status;
	}

	return status;
}