#pragma once
// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#pragma region winapi_nt_types
using NTSTATUS = LONG;
using MEMORY_INFORMATION_CLASS = INT;
#pragma endregion

#pragma region winapi_nt_definitions
#define NtCurrentProcess() (reinterpret_cast<HANDLE>(-1))
#define NtCurrentThread() (reinterpret_cast<HANDLE>(-2))

/*
 * NT_SUCCESS =		[0x00000000 .. 0x3FFFFFFF]
 * NT_INFORMATION =	[0x40000000 .. 0x7FFFFFFF]
 * NT_WARNING =		[0x80000000 .. 0xBFFFFFFF]
 * NT_ERROR =		[0xC0000000 .. 0xFFFFFFFF]
 */
#define NT_SUCCESS(STATUS) (static_cast<NTSTATUS>(STATUS) >= 0)
#define NT_INFORMATION(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 1UL)
#define NT_WARNING(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 2UL)
#define NT_ERROR(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 3UL)
#pragma endregion

#pragma region winapi_nt
// @credits: https://www.vergiliusproject.com/kernels/x86/Windows%2010

typedef struct _UNICODE_STRING
{
	USHORT Length; // 0x0
	USHORT MaximumLength; // 0x2
	wchar_t* Buffer; // 0x8
} UNICODE_STRING, *PUNICODE_STRING;

static_assert(sizeof(_UNICODE_STRING) == 0x10);

struct _RTL_BALANCED_NODE
{
	_RTL_BALANCED_NODE* Children[2]; // 0x0
	ULONGLONG ParentValue; // 0x10
};

static_assert(sizeof(_RTL_BALANCED_NODE) == 0x18);

struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks; // 0x00
	LIST_ENTRY InMemoryOrderLinks; // 0x10
	LIST_ENTRY InInitializationOrderLinks; // 0x20
	void* DllBase; // 0x30
	void* EntryPoint; // 0x38
	ULONG SizeOfImage; // 0x40
	UNICODE_STRING FullDllName; // 0x48
	UNICODE_STRING BaseDllName; // 0x58
	ULONG Flags; // 0x68
	USHORT ObsoleteLoadCount; // 0x6C
	USHORT TlsIndex; // 0x6E - always
	LIST_ENTRY HashLinks; // 0x70
	ULONG TimeDateStamp; // 0x80
	void* EntryPointActivationContext; // 0x88
	void* Lock; // 0x90
	struct _LDR_DDAG_NODE* DdagNode; // 0x98
	LIST_ENTRY NodeModuleLink; // 0xA0
	struct _LDRP_LOAD_CONTEXT* LoadContext; // 0xB0
	void* ParentDllBase; // 0xB8
	void* SwitchBackContext; // 0xC0
	_RTL_BALANCED_NODE BaseAddressIndexNode; // 0xC8
	_RTL_BALANCED_NODE MappingInfoIndexNode; // 0xE8
	ULONG64 OriginalBase; // 0x108
	LONGLONG LoadTime; // 0x110
	ULONG BaseNameHashValue; // 0x120
	ULONG LoadReason; // 0x124
	ULONG ImplicitPathOptions; // 0x128
	ULONG ReferenceCount; // 0x12C
};

static_assert(sizeof(_LDR_DATA_TABLE_ENTRY) == 0x118);

struct _PEB_LDR_DATA
{
	ULONG Length; // 0x0
	BOOLEAN Initialized; // 0x4
	void* SsHandle; // 0x8
	LIST_ENTRY InLoadOrderModuleList; // 0x10
	LIST_ENTRY InMemoryOrderModuleList; // 0x20
	LIST_ENTRY InInitializationOrderModuleList; // 0x30
	void* EntryInProgress; // 0x40
	BOOLEAN ShutdownInProgress; // 0x48
	void* ShutdownThreadId; // 0x50
};

static_assert(sizeof(_PEB_LDR_DATA) == 0x58);

struct _CURDIR
{
	UNICODE_STRING DosPath; // 0x0
	void* Handle; // 0x10
};

static_assert(sizeof(_CURDIR) == 0x18);

struct _STRING
{
	USHORT Length; // 0x0
	USHORT MaximumLength; // 0x2
	char* Buffer; // 0x8
};

static_assert(sizeof(_STRING) == 0x10);

struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags; // 0x0
	USHORT Length; // 0x2
	ULONG Timestamp; // 0x4
	_STRING DosPath; // 0x8
};

static_assert(sizeof(_RTL_DRIVE_LETTER_CURDIR) == 0x18);

struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength; // 0x0
	ULONG Length; // 0x4
	ULONG Flags; // 0x8
	ULONG DebugFlags; // 0xC
	void* ConsoleHandle; // 0x10
	ULONG ConsoleFlags; // 0x18
	void* StandardInput; // 0x20
	void* StandardOutput; // 0x28
	void* StandardError; // 0x30
	_CURDIR CurrentDirectory; // 0x38
	UNICODE_STRING DllPath; // 0x48
	UNICODE_STRING ImagePathName; // 0x58
	UNICODE_STRING CommandLine; // 0x68
	void* Environment; // 0x78
	ULONG StartingX; // 0x80
	ULONG StartingY; // 0x84
	ULONG CountX; // 0x88
	ULONG CountY; // 0x8C
	ULONG CountCharsX; // 0x90
	ULONG CountCharsY; // 0x94
	ULONG FillAttribute; // 0x98
	ULONG WindowFlags; // 0x9C
	ULONG ShowWindowFlags; // 0xA0
	UNICODE_STRING WindowTitle; // 0xA8
	UNICODE_STRING DesktopInfo; // 0xB8
	UNICODE_STRING ShellInfo; // 0xC8
	UNICODE_STRING RuntimeData; // 0xD8
	_RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32]; // 0xE8
	ULONG EnvironmentSize; // 0x2E8
	ULONG EnvironmentVersion; // 0x2EC
	void* PackageDependencyData; // 0x2F0
	ULONG ProcessGroupId; // 0x2F8
	ULONG LoaderThreads; // 0x2FC
};

static_assert(sizeof(_RTL_USER_PROCESS_PARAMETERS) == 0x408);

struct _PEB
{
	BOOLEAN InheritedAddressSpace; // 0x0
	BOOLEAN ReadImageFileExecOptions; // 0x1
	BOOLEAN BeingDebugged; // 0x2

	union
	{
		BOOLEAN BitField; // 0x3

		struct
		{
			BOOLEAN ImageUsesLargePages : 1; // 0x3
			BOOLEAN IsProtectedProcess : 1; // 0x3
			BOOLEAN IsLegacyProcess : 1; // 0x3
			BOOLEAN IsImageDynamicallyRelocated : 1; // 0x3
			BOOLEAN SkipPatchingUser32Forwarders : 1; // 0x3
			BOOLEAN SpareBits : 3; // 0x3
		};
	};

	void* Mutant; // 0x8
	void* ImageBaseAddress; // 0x10
	_PEB_LDR_DATA* Ldr; // 0x18
	_RTL_USER_PROCESS_PARAMETERS* ProcessParameters; // 0x20
	void* SubSystemData; // 0x28
	void* ProcessHeap; // 0x30
	_RTL_CRITICAL_SECTION* FastPebLock; // 0x38
	void* AtlThunkSListPtr; // 0x40
	void* IFEOKey; // 0x48

	union
	{
		ULONG CrossProcessFlags; // 0x50

		struct
		{
			ULONG ProcessInJob : 1; // 0x50
			ULONG ProcessInitializing : 1; // 0x50
			ULONG ReservedBits0 : 30; // 0x50
		};
	};

	union
	{
		PVOID KernelCallbackTable; // 0x58
		PVOID UserSharedInfoPtr; // 0x58
	};

	ULONG SystemReserved[1]; // 0x60
	ULONG AtlThunkSListPtr32; // 0x64
	PVOID ApiSetMap; // 0x68
	ULONG TlsExpansionCounter; // 0x70
	PVOID TlsBitmap; // 0x78
	ULONG TlsBitmapBits[2]; // 0x80
	PVOID ReadOnlySharedMemoryBase; // 0x88
	PVOID SparePvoid0; // 0x90
	PVOID ReadOnlyStaticServerData; // 0x98
	PVOID AnsiCodePageData; // 0xA0
	PVOID OemCodePageData; // 0xA8
	PVOID UnicodeCaseTableData; // 0xB0
	ULONG NumberOfProcessors; // 0xB8
	ULONG NtGlobalFlag; // 0xBC
	LONGLONG CriticalSectionTimeout; // 0xC0
	ULONG64 HeapSegmentReserve; // 0xC8
	ULONG64 HeapSegmentCommit; // 0xD0
	ULONG64 HeapDeCommitTotalFreeThreshold; // 0xD8
	ULONG64 HeapDeCommitFreeBlockThreshold; // 0xE0
	ULONG NumberOfHeaps; // 0xE8
	ULONG MaximumNumberOfHeaps; // 0xEC
	PVOID* ProcessHeaps; // 0xF0
	PVOID GdiSharedHandleTable; // 0xF8
	PVOID ProcessStarterHelper; // 0x100
	ULONG GdiDCAttributeList; // 0x108
	_RTL_CRITICAL_SECTION* LoaderLock; // 0x110
	ULONG OSMajorVersion; // 0x118
	ULONG OSMinorVersion; // 0x11C
	USHORT OSBuildNumber; // 0x120
	USHORT OSCSDVersion; // 0x122
	ULONG OSPlatformId; // 0x124
	ULONG ImageSubsystem; // 0x128
	ULONG ImageSubsystemMajorVersion; // 0x12C
	ULONG ImageSubsystemMinorVersion; // 0x130
	ULONG ActiveProcessAffinityMask; // 0x138
	ULONG GdiHandleBuffer[60]; // 0x140
	void (*PostProcessInitRoutine)(); // 0x230
	PVOID TlsExpansionBitmap; // 0x238
	ULONG TlsExpansionBitmapBits[32]; // 0x240
	ULONG SessionId; // 0x2C0
	ULONGLONG AppCompatFlags; // 0x2C8
	ULONGLONG AppCompatFlagsUser; // 0x2D0
	PVOID pShimData; // 0x2D8
	PVOID AppCompatInfo; // 0x2E0
	UNICODE_STRING CSDVersion; // 0x2E8
	PVOID ActivationContextData; // 0x2F8
	PVOID ProcessAssemblyStorageMap; // 0x300
	PVOID SystemDefaultActivationContextData; // 0x308
	PVOID SystemAssemblyStorageMap; // 0x310
	ULONG64 MinimumStackCommit; // 0x318
	PVOID FlsCallback; // 0x320
	LIST_ENTRY FlsListHead; // 0x328
	PVOID FlsBitmap; // 0x338
	ULONG FlsBitmapBits[4]; // 0x340
	ULONG FlsHighIndex; // 0x350
	PVOID WerRegistrationData; // 0x358
	PVOID WerShipAssertPtr; // 0x360
	PVOID pUnused; // 0x368
	PVOID pImageHeaderHash; // 0x370
	ULONG TracingFlags; // 0x378
	ULONGLONG CsrServerReadOnlySharedMemoryBase; // 0x380
};

static_assert(sizeof(_PEB) == 0x380);

struct _CLIENT_ID
{
	PVOID UniqueProcess; // 0x0
	PVOID UniqueThread; // 0x8
};

static_assert(sizeof(_CLIENT_ID) == 0x10);

struct _GDI_TEB_BATCH
{
	ULONG Offset : 31; // 0x0
	ULONG HasRenderingCommand : 1; // 0x0
	PVOID HDC; // 0x8
	ULONG Buffer[310]; // 0x10
};

static_assert(sizeof(_GDI_TEB_BATCH) == 0x4E8);

struct _TEB
{
	NT_TIB NtTib; // 0x0
	PVOID EnvironmentPointer; // 0x30
	_CLIENT_ID ClientId; // 0x38
	PVOID ActiveRpcHandle; // 0x48
	PVOID ThreadLocalStoragePointer; // 0x50
	PVOID ProcessEnvironmentBlock; // 0x58
	ULONG LastErrorValue; // 0x60
	ULONG CountOfOwnedCriticalSections; // 0x64
	PVOID CsrClientThread; // 0x68
	PVOID Win32ThreadInfo; // 0x70
	ULONG User32Reserved[26]; // 0x78
	ULONG UserReserved[5]; // 0xE0
	PVOID WOW32Reserved; // 0xF8
	ULONG CurrentLocale; // 0x100
	ULONG FpSoftwareStatusRegister; // 0x104
	PVOID ReservedForDebuggerInstrumentation[16]; // 0x108
	PVOID SystemReserved1[38]; // 0x148
	LONG ExceptionCode; // 0x1C8
	PVOID ActivationContextStackPointer; // 0x1D0
	ULONG InstrumentationCallbackSp; // 0x1D8
	ULONG InstrumentationCallbackPreviousPc; // 0x1DC
	ULONG InstrumentationCallbackPreviousSp; // 0x1E0
	UCHAR InstrumentationCallbackDisabled; // 0x1E4
	UCHAR SpareBytes[23]; // 0x1E5
	ULONG TxFsContext; // 0x1FC
	_GDI_TEB_BATCH GdiTebBatch; // 0x200
	_CLIENT_ID RealClientId; // 0x6E8
	PVOID GdiCachedProcessHandle; // 0x6F0
	ULONG GdiClientPID; // 0x6F4
	ULONG GdiClientTID; // 0x6F8
	PVOID GdiThreadLocalInfo; // 0x700
	ULONG Win32ClientInfo[62]; // 0x708
	PVOID glDispatchTable[233]; // 0x800
	ULONG glReserved1[29]; // 0xCE0
	PVOID glReserved2; // 0xD50
	PVOID glSectionInfo; // 0xD58
	PVOID glSection; // 0xD60
	PVOID glTable; // 0xD68
	PVOID glCurrentRC; // 0xD70
	PVOID glContext; // 0xD78
	ULONG LastStatusValue; // 0xD80
	UNICODE_STRING StaticUnicodeString; // 0xD88
	WCHAR StaticUnicodeBuffer[261]; // 0xD98
	PVOID DeallocationStack; // 0xFA0
	PVOID TlsSlots[64]; // 0xFA8
	LIST_ENTRY TlsLinks; // 0x11A8
	PVOID Vdm; // 0x11B8
	PVOID ReservedForNtRpc; // 0x11C0
	PVOID DbgSsReserved[2]; // 0x11C8
	ULONG HardErrorMode; // 0x11D8
	PVOID Instrumentation[9]; // 0x11E0
	GUID ActivityId; // 0x1208
	PVOID SubProcessTag; // 0x1218
	PVOID PerflibData; // 0x1220
	PVOID EtwTraceData; // 0x1228
	PVOID WinSockData; // 0x1230
	ULONG GdiBatchCount; // 0x1238
	PROCESSOR_NUMBER CurrentIdealProcessor; // 0x123C
	ULONG GuaranteedStackBytes; // 0x1244
	PVOID ReservedForPerf; // 0x1248
	PVOID ReservedForOle; // 0x1250
	ULONG WaitingOnLoaderLock; // 0x1258
	PVOID SavedPriorityState; // 0x1260
	ULONG ReservedForCodeCoverage; // 0x1268
	PVOID ThreadPoolData; // 0x1270
	PVOID TlsExpansionSlots; // 0x1278
	ULONG MuiGeneration; // 0x1280
	ULONG IsImpersonating; // 0x1284
	PVOID NlsCache; // 0x1288
	PVOID pShimData; // 0x1290
	USHORT HeapVirtualAffinity; // 0x1298
	USHORT LowFragHeapDataSlot; // 0x129A
	PVOID CurrentTransactionHandle; // 0x129C
	PVOID ActiveFrame; // 0x12A0
	PVOID FlsData; // 0x12A8
	PVOID PreferredLanguages; // 0x12B0
	PVOID UserPrefLanguages; // 0x12B8
	PVOID MergedPrefLanguages; // 0x12C0
	ULONG MuiImpersonation; // 0x12C8
	volatile USHORT CrossTebFlags; // 0x12CC
	USHORT SameTebFlags; // 0x12CE
	PVOID TxnScopeEnterCallback; // 0x12D0
	PVOID TxnScopeExitCallback; // 0x12D8
	PVOID TxnScopeContext; // 0x12E0
	ULONG LockCount; // 0x12E8
	LONG WowTebOffset; // 0x12EC
	PVOID ResourceRetValue; // 0x12F0
	PVOID ReservedForWdf; // 0x12F8
	ULONGLONG ReservedForCrt; // 0x1300
	GUID EffectiveContainerId; // 0x1308
};

static_assert(sizeof(_TEB) == 0x16B8);
#pragma endregion