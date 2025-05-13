#pragma once

#define PYONG_POOL_TAG 'dprk'

#define Log(...) DbgPrintEx( DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "" __VA_ARGS__ )

VOID KernelApcInjectCallback( PKAPC Apc, PKNORMAL_ROUTINE* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2 )
{
	UNREFERENCED_PARAMETER( SystemArgument1 );
	UNREFERENCED_PARAMETER( SystemArgument2 );

	if ( PsIsThreadTerminating( PsGetCurrentThread() ) )
		*NormalRoutine = NULL;

	if ( PsGetCurrentProcessWow64Process() != NULL )
		PsWrapApcWow64Thread( NormalContext, ( PVOID* ) NormalRoutine );

	ExFreePoolWithTag( Apc, PYONG_POOL_TAG );
}

NTSTATUS QueueUserApc( PETHREAD pThread, PVOID pUserFunc, PVOID Arg1 )
{
	PKAPC pInjectApc = ExAllocatePoolWithTag( NonPagedPool, sizeof( KAPC ), PYONG_POOL_TAG );
	if ( pInjectApc == NULL )
	{
		Log( "[-] ERROR: ExAllocatePoolWithTag failed to allocate APC\n" );
		return STATUS_NO_MEMORY;
	}

	KeInitializeApc( pInjectApc, ( PKTHREAD ) pThread, OriginalApcEnvironment, &KernelApcInjectCallback, NULL, ( PKNORMAL_ROUTINE ) pUserFunc, UserMode, Arg1 );

	if ( !KeInsertQueueApc( pInjectApc, NULL, NULL, IO_NO_INCREMENT ) )
	{
		Log( "[-] ERROR: KeInsertQueueApc failed to insert APC\n" );
		ExFreePoolWithTag( pInjectApc, PYONG_POOL_TAG );
		return STATUS_NOT_CAPABLE;
	}

	return STATUS_SUCCESS;
}

NTSTATUS InjectDll( PINJECT_DLL pData )
{
	PEPROCESS pProcess = NULL;
	NTSTATUS status = PsLookupProcessByProcessId( ( HANDLE ) pData->pid, &pProcess );
	if ( !NT_SUCCESS( status ) )
	{
		Log( "[-] ERROR: PsLookupProcessByProcessId failed with status 0x%X\n", status );
		return status;
	}

	ULONG_PTR LoadLibraryA = pData->LoadLibraryA;

	KAPC_STATE apcState;
	KeStackAttachProcess( pProcess, &apcState );

	const char dllPath[] = "C:\\Users\\User\\Desktop\\test64.dll";
	SIZE_T length = strlen( dllPath ) + 1;

	PVOID pBuffer = NULL;
	status = ZwAllocateVirtualMemory( ZwCurrentProcess(), &pBuffer, 0, &length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	if ( !NT_SUCCESS( status ) )
	{
		Log( "[-] ERROR: ZwAllocateVirtualMemory failed with status 0x%X\n", status );
		KeUnstackDetachProcess( &apcState );
		ObDereferenceObject( pProcess );
		return status;
	}

	RtlCopyMemory( pBuffer, dllPath, sizeof( dllPath ) );

	PETHREAD pThread = NULL;
	ULONG threadId[] = { 6984, 7224, 7208, 7236, 4248 };

	for ( ULONG i = 0; i <= 4; i++ )
	{
		status = PsLookupThreadByThreadId( ( HANDLE ) threadId[ i ], &pThread );
		if ( !NT_SUCCESS( status ) ) continue;

		QueueUserApc( pThread, ( PVOID ) LoadLibraryA, pBuffer );
	}

	ObDereferenceObject( pThread );
	KeUnstackDetachProcess( &apcState );
	ObDereferenceObject( pProcess );

	return status;
}