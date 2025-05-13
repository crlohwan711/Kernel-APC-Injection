#pragma once

typedef enum _KAPC_ENVIRONMENT
{
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment,
    InsertApcEnvironment
} KAPC_ENVIRONMENT, * PKAPC_ENVIRONMENT;

typedef VOID( NTAPI* PKNORMAL_ROUTINE )(
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

typedef VOID( NTAPI* PKKERNEL_ROUTINE )(
    IN PRKAPC Apc,
    IN OUT PKNORMAL_ROUTINE* NormalRoutine,
    IN OUT PVOID* NormalContext,
    IN OUT PVOID* SystemArgument1,
    IN OUT PVOID* SystemArgument2
    );

typedef VOID( NTAPI* PKRUNDOWN_ROUTINE )(
    IN PRKAPC Apc
    );

NTKERNELAPI
LPCSTR
NTAPI
PsGetProcessImageFileName(
    IN PEPROCESS pProcess
    );

NTKERNELAPI
PVOID
NTAPI
PsGetCurrentProcessWow64Process( VOID );

NTKERNELAPI
VOID
NTAPI
KeInitializeApc(
    OUT PKAPC Apc,
    IN PKTHREAD Thread,
    IN KAPC_ENVIRONMENT ApcStateIndex,
    IN PKKERNEL_ROUTINE KernelRoutine,
    IN OPTIONAL PKRUNDOWN_ROUTINE RundownRoutine,
    IN OPTIONAL PKNORMAL_ROUTINE NormalRoutine,
    IN OPTIONAL KPROCESSOR_MODE ApcMode,
    IN OPTIONAL PVOID NormalContext
    );

NTKERNELAPI
BOOLEAN
NTAPI
KeInsertQueueApc(
    IN OUT PKAPC Apc,
    IN OPTIONAL PVOID SystemArgument1,
    IN OPTIONAL PVOID SystemArgument2,
    IN KPRIORITY Increment
    );