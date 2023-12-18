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
	WCHAR* Buffer; // 0x8
} UNICODE_STRING, *PUNICODE_STRING;

static_assert(sizeof(_UNICODE_STRING) == 0x10);

struct _RTL_BALANCED_NODE
{
	union
	{
		struct _RTL_BALANCED_NODE* Children[2]; //0x0

		struct
		{
			struct _RTL_BALANCED_NODE* Left; //0x0
			struct _RTL_BALANCED_NODE* Right; //0x8
		};
	};

	union
	{
		struct
		{
			UCHAR Red : 1; //0x10
			UCHAR Balance : 2; //0x10
		};

		ULONGLONG ParentValue; //0x10
	};
};

static_assert(sizeof(_RTL_BALANCED_NODE) == 0x18);

struct _LDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks; //0x0
	struct _LIST_ENTRY InMemoryOrderLinks; //0x10
	struct _LIST_ENTRY InInitializationOrderLinks; //0x20
	VOID* DllBase; //0x30
	VOID* EntryPoint; //0x38
	ULONG SizeOfImage; //0x40
	struct _UNICODE_STRING FullDllName; //0x48
	struct _UNICODE_STRING BaseDllName; //0x58

	union
	{
		UCHAR FlagGroup[4]; //0x68
		ULONG Flags; //0x68

		struct
		{
			ULONG PackagedBinary : 1; //0x68
			ULONG MarkedForRemoval : 1; //0x68
			ULONG ImageDll : 1; //0x68
			ULONG LoadNotificationsSent : 1; //0x68
			ULONG TelemetryEntryProcessed : 1; //0x68
			ULONG ProcessStaticImport : 1; //0x68
			ULONG InLegacyLists : 1; //0x68
			ULONG InIndexes : 1; //0x68
			ULONG ShimDll : 1; //0x68
			ULONG InExceptionTable : 1; //0x68
			ULONG ReservedFlags1 : 2; //0x68
			ULONG LoadInProgress : 1; //0x68
			ULONG LoadConfigProcessed : 1; //0x68
			ULONG EntryProcessed : 1; //0x68
			ULONG ProtectDelayLoad : 1; //0x68
			ULONG ReservedFlags3 : 2; //0x68
			ULONG DontCallForThreads : 1; //0x68
			ULONG ProcessAttachCalled : 1; //0x68
			ULONG ProcessAttachFailed : 1; //0x68
			ULONG CorDeferredValidate : 1; //0x68
			ULONG CorImage : 1; //0x68
			ULONG DontRelocate : 1; //0x68
			ULONG CorILOnly : 1; //0x68
			ULONG ChpeImage : 1; //0x68
			ULONG ChpeEmulatorImage : 1; //0x68
			ULONG ReservedFlags5 : 1; //0x68
			ULONG Redirected : 1; //0x68
			ULONG ReservedFlags6 : 2; //0x68
			ULONG CompatDatabaseProcessed : 1; //0x68
		};
	};

	USHORT ObsoleteLoadCount; //0x6c
	USHORT TlsIndex; //0x6e
	struct _LIST_ENTRY HashLinks; //0x70
	ULONG TimeDateStamp; //0x80
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext; //0x88
	VOID* Lock; //0x90
	struct _LDR_DDAG_NODE* DdagNode; //0x98
	struct _LIST_ENTRY NodeModuleLink; //0xa0
	struct _LDRP_LOAD_CONTEXT* LoadContext; //0xb0
	VOID* ParentDllBase; //0xb8
	VOID* SwitchBackContext; //0xc0
	_RTL_BALANCED_NODE BaseAddressIndexNode; //0xc8
	_RTL_BALANCED_NODE MappingInfoIndexNode; //0xe0
	ULONGLONG OriginalBase; //0xf8
	union _LARGE_INTEGER LoadTime; //0x100
	ULONG BaseNameHashValue; //0x108
	enum _LDR_DLL_LOAD_REASON LoadReason; //0x10c
	ULONG ImplicitPathOptions; //0x110
	ULONG ReferenceCount; //0x114
	ULONG DependentLoadFlags; //0x118
	UCHAR SigningLevel; //0x11c
	ULONG CheckSum; //0x120
	VOID* ActivePatchImageBase; //0x128
	enum _LDR_HOT_PATCH_STATE HotPatchState; //0x130
};

static_assert(sizeof(_LDR_DATA_TABLE_ENTRY) == 0x138);

struct _PEB_LDR_DATA
{
	ULONG Length; //0x0
	UCHAR Initialized; //0x4
	VOID* SsHandle; //0x8
	struct _LIST_ENTRY InLoadOrderModuleList; //0x10
	struct _LIST_ENTRY InMemoryOrderModuleList; //0x20
	struct _LIST_ENTRY InInitializationOrderModuleList; //0x30
	VOID* EntryInProgress; //0x40
	UCHAR ShutdownInProgress; //0x48
	VOID* ShutdownThreadId; //0x50
};

static_assert(sizeof(_PEB_LDR_DATA) == 0x58);

struct _CURDIR
{
	struct _UNICODE_STRING DosPath; //0x0
	VOID* Handle; //0x10
};

static_assert(sizeof(_CURDIR) == 0x18);

struct _STRING
{
	USHORT Length; //0x0
	USHORT MaximumLength; //0x2
	CHAR* Buffer; //0x8
};

static_assert(sizeof(_STRING) == 0x10);

struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags; //0x0
	USHORT Length; //0x2
	ULONG TimeStamp; //0x4
	struct _STRING DosPath; //0x8
};

static_assert(sizeof(_RTL_DRIVE_LETTER_CURDIR) == 0x18);

struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength; //0x0
	ULONG Length; //0x4
	ULONG Flags; //0x8
	ULONG DebugFlags; //0xc
	VOID* ConsoleHandle; //0x10
	ULONG ConsoleFlags; //0x18
	VOID* StandardInput; //0x20
	VOID* StandardOutput; //0x28
	VOID* StandardError; //0x30
	struct _CURDIR CurrentDirectory; //0x38
	struct _UNICODE_STRING DllPath; //0x50
	struct _UNICODE_STRING ImagePathName; //0x60
	struct _UNICODE_STRING CommandLine; //0x70
	VOID* Environment; //0x80
	ULONG StartingX; //0x88
	ULONG StartingY; //0x8c
	ULONG CountX; //0x90
	ULONG CountY; //0x94
	ULONG CountCharsX; //0x98
	ULONG CountCharsY; //0x9c
	ULONG FillAttribute; //0xa0
	ULONG WindowFlags; //0xa4
	ULONG ShowWindowFlags; //0xa8
	struct _UNICODE_STRING WindowTitle; //0xb0
	struct _UNICODE_STRING DesktopInfo; //0xc0
	struct _UNICODE_STRING ShellInfo; //0xd0
	struct _UNICODE_STRING RuntimeData; //0xe0
	struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32]; //0xf0
	ULONGLONG EnvironmentSize; //0x3f0
	ULONGLONG EnvironmentVersion; //0x3f8
	VOID* PackageDependencyData; //0x400
	ULONG ProcessGroupId; //0x408
	ULONG LoaderThreads; //0x40c
	struct _UNICODE_STRING RedirectionDllName; //0x410
	struct _UNICODE_STRING HeapPartitionName; //0x420
	ULONGLONG* DefaultThreadpoolCpuSetMasks; //0x430
	ULONG DefaultThreadpoolCpuSetMaskCount; //0x438
	ULONG DefaultThreadpoolThreadMaximum; //0x43c
	ULONG HeapMemoryTypeMask; //0x440
};

static_assert(sizeof(_RTL_USER_PROCESS_PARAMETERS) == 0x448);

struct _PEB
{
	UCHAR InheritedAddressSpace; //0x0
	UCHAR ReadImageFileExecOptions; //0x1
	UCHAR BeingDebugged; //0x2

	union
	{
		UCHAR BitField; //0x3

		struct
		{
			UCHAR ImageUsesLargePages : 1; //0x3
			UCHAR IsProtectedProcess : 1; //0x3
			UCHAR IsImageDynamicallyRelocated : 1; //0x3
			UCHAR SkipPatchingUser32Forwarders : 1; //0x3
			UCHAR IsPackagedProcess : 1; //0x3
			UCHAR IsAppContainer : 1; //0x3
			UCHAR IsProtectedProcessLight : 1; //0x3
			UCHAR IsLongPathAwareProcess : 1; //0x3
		};
	};

	UCHAR Padding0[4]; //0x4
	VOID* Mutant; //0x8
	VOID* ImageBaseAddress; //0x10
	struct _PEB_LDR_DATA* Ldr; //0x18
	struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters; //0x20
	VOID* SubSystemData; //0x28
	VOID* ProcessHeap; //0x30
	struct _RTL_CRITICAL_SECTION* FastPebLock; //0x38
	union _SLIST_HEADER* volatile AtlThunkSListPtr; //0x40
	VOID* IFEOKey; //0x48

	union
	{
		ULONG CrossProcessFlags; //0x50

		struct
		{
			ULONG ProcessInJob : 1; //0x50
			ULONG ProcessInitializing : 1; //0x50
			ULONG ProcessUsingVEH : 1; //0x50
			ULONG ProcessUsingVCH : 1; //0x50
			ULONG ProcessUsingFTH : 1; //0x50
			ULONG ProcessPreviouslyThrottled : 1; //0x50
			ULONG ProcessCurrentlyThrottled : 1; //0x50
			ULONG ProcessImagesHotPatched : 1; //0x50
			ULONG ReservedBits0 : 24; //0x50
		};
	};

	UCHAR Padding1[4]; //0x54

	union
	{
		VOID* KernelCallbackTable; //0x58
		VOID* UserSharedInfoPtr; //0x58
	};

	ULONG SystemReserved; //0x60
	ULONG AtlThunkSListPtr32; //0x64
	VOID* ApiSetMap; //0x68
	ULONG TlsExpansionCounter; //0x70
	UCHAR Padding2[4]; //0x74
	struct _RTL_BITMAP* TlsBitmap; //0x78
	ULONG TlsBitmapBits[2]; //0x80
	VOID* ReadOnlySharedMemoryBase; //0x88
	VOID* SharedData; //0x90
	VOID** ReadOnlyStaticServerData; //0x98
	VOID* AnsiCodePageData; //0xa0
	VOID* OemCodePageData; //0xa8
	VOID* UnicodeCaseTableData; //0xb0
	ULONG NumberOfProcessors; //0xb8
	ULONG NtGlobalFlag; //0xbc
	union _LARGE_INTEGER CriticalSectionTimeout; //0xc0
	ULONGLONG HeapSegmentReserve; //0xc8
	ULONGLONG HeapSegmentCommit; //0xd0
	ULONGLONG HeapDeCommitTotalFreeThreshold; //0xd8
	ULONGLONG HeapDeCommitFreeBlockThreshold; //0xe0
	ULONG NumberOfHeaps; //0xe8
	ULONG MaximumNumberOfHeaps; //0xec
	VOID** ProcessHeaps; //0xf0
	VOID* GdiSharedHandleTable; //0xf8
	VOID* ProcessStarterHelper; //0x100
	ULONG GdiDCAttributeList; //0x108
	UCHAR Padding3[4]; //0x10c
	struct _RTL_CRITICAL_SECTION* LoaderLock; //0x110
	ULONG OSMajorVersion; //0x118
	ULONG OSMinorVersion; //0x11c
	USHORT OSBuildNumber; //0x120
	USHORT OSCSDVersion; //0x122
	ULONG OSPlatformId; //0x124
	ULONG ImageSubsystem; //0x128
	ULONG ImageSubsystemMajorVersion; //0x12c
	ULONG ImageSubsystemMinorVersion; //0x130
	UCHAR Padding4[4]; //0x134
	ULONGLONG ActiveProcessAffinityMask; //0x138
	ULONG GdiHandleBuffer[60]; //0x140
	VOID(*PostProcessInitRoutine)
	(); //0x230
	struct _RTL_BITMAP* TlsExpansionBitmap; //0x238
	ULONG TlsExpansionBitmapBits[32]; //0x240
	ULONG SessionId; //0x2c0
	UCHAR Padding5[4]; //0x2c4
	union _ULARGE_INTEGER AppCompatFlags; //0x2c8
	union _ULARGE_INTEGER AppCompatFlagsUser; //0x2d0
	VOID* pShimData; //0x2d8
	VOID* AppCompatInfo; //0x2e0
	struct _UNICODE_STRING CSDVersion; //0x2e8
	struct _ACTIVATION_CONTEXT_DATA* ActivationContextData; //0x2f8
	struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap; //0x300
	struct _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData; //0x308
	struct _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap; //0x310
	ULONGLONG MinimumStackCommit; //0x318
	VOID* SparePointers[2]; //0x320
	VOID* PatchLoaderData; //0x330
	struct _CHPEV2_PROCESS_INFO* ChpeV2ProcessInfo; //0x338
	ULONG AppModelFeatureState; //0x340
	ULONG SpareUlongs[2]; //0x344
	USHORT ActiveCodePage; //0x34c
	USHORT OemCodePage; //0x34e
	USHORT UseCaseMapping; //0x350
	USHORT UnusedNlsField; //0x352
	VOID* WerRegistrationData; //0x358
	VOID* WerShipAssertPtr; //0x360
	VOID* EcCodeBitMap; //0x368
	VOID* pImageHeaderHash; //0x370

	union
	{
		ULONG TracingFlags; //0x378

		struct
		{
			ULONG HeapTracingEnabled : 1; //0x378
			ULONG CritSecTracingEnabled : 1; //0x378
			ULONG LibLoaderTracingEnabled : 1; //0x378
			ULONG SpareTracingBits : 29; //0x378
		};
	};

	UCHAR Padding6[4]; //0x37c
	ULONGLONG CsrServerReadOnlySharedMemoryBase; //0x380
	ULONGLONG TppWorkerpListLock; //0x388
	struct _LIST_ENTRY TppWorkerpList; //0x390
	VOID* WaitOnAddressHashTable[128]; //0x3a0
	VOID* TelemetryCoverageHeader; //0x7a0
	ULONG CloudFileFlags; //0x7a8
	ULONG CloudFileDiagFlags; //0x7ac
	CHAR PlaceholderCompatibilityMode; //0x7b0
	CHAR PlaceholderCompatibilityModeReserved[7]; //0x7b1
	struct _LEAP_SECOND_DATA* LeapSecondData; //0x7b8

	union
	{
		ULONG LeapSecondFlags; //0x7c0

		struct
		{
			ULONG SixtySecondEnabled : 1; //0x7c0
			ULONG Reserved : 31; //0x7c0
		};
	};

	ULONG NtGlobalFlag2; //0x7c4
	ULONGLONG ExtendedFeatureDisableMask; //0x7c8
};

static_assert(sizeof(_PEB) == 0x7d0);

struct _CLIENT_ID
{
	PVOID UniqueProcess; // 0x0
	PVOID UniqueThread; // 0x8
};

static_assert(sizeof(_CLIENT_ID) == 0x10);

struct _GDI_TEB_BATCH
{
	ULONG Offset : 31; //0x0
	ULONG HasRenderingCommand : 1; //0x0
	ULONGLONG HDC; //0x8
	ULONG Buffer[310]; //0x10
};

static_assert(sizeof(_GDI_TEB_BATCH) == 0x4E8);

struct _ACTIVATION_CONTEXT_STACK
{
	struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME* ActiveFrame; //0x0
	struct _LIST_ENTRY FrameListCache; //0x8
	ULONG Flags; //0x18
	ULONG NextCookieSequenceNumber; //0x1c
	ULONG StackId; //0x20
};

static_assert(sizeof(_ACTIVATION_CONTEXT_STACK) == 0x28);

struct _TEB
{
	struct _NT_TIB NtTib; //0x0
	VOID* EnvironmentPointer; //0x38
	struct _CLIENT_ID ClientId; //0x40
	VOID* ActiveRpcHandle; //0x50
	VOID* ThreadLocalStoragePointer; //0x58
	struct _PEB* ProcessEnvironmentBlock; //0x60
	ULONG LastErrorValue; //0x68
	ULONG CountOfOwnedCriticalSections; //0x6c
	VOID* CsrClientThread; //0x70
	VOID* Win32ThreadInfo; //0x78
	ULONG User32Reserved[26]; //0x80
	ULONG UserReserved[5]; //0xe8
	VOID* WOW32Reserved; //0x100
	ULONG CurrentLocale; //0x108
	ULONG FpSoftwareStatusRegister; //0x10c
	VOID* ReservedForDebuggerInstrumentation[16]; //0x110
	VOID* SystemReserved1[30]; //0x190
	CHAR PlaceholderCompatibilityMode; //0x280
	UCHAR PlaceholderHydrationAlwaysExplicit; //0x281
	CHAR PlaceholderReserved[10]; //0x282
	ULONG ProxiedProcessId; //0x28c
	struct _ACTIVATION_CONTEXT_STACK _ActivationStack; //0x290
	UCHAR WorkingOnBehalfTicket[8]; //0x2b8
	LONG ExceptionCode; //0x2c0
	UCHAR Padding0[4]; //0x2c4
	struct _ACTIVATION_CONTEXT_STACK* ActivationContextStackPointer; //0x2c8
	ULONGLONG InstrumentationCallbackSp; //0x2d0
	ULONGLONG InstrumentationCallbackPreviousPc; //0x2d8
	ULONGLONG InstrumentationCallbackPreviousSp; //0x2e0
	ULONG TxFsContext; //0x2e8
	UCHAR InstrumentationCallbackDisabled; //0x2ec
	UCHAR UnalignedLoadStoreExceptions; //0x2ed
	UCHAR Padding1[2]; //0x2ee
	struct _GDI_TEB_BATCH GdiTebBatch; //0x2f0
	struct _CLIENT_ID RealClientId; //0x7d8
	VOID* GdiCachedProcessHandle; //0x7e8
	ULONG GdiClientPID; //0x7f0
	ULONG GdiClientTID; //0x7f4
	VOID* GdiThreadLocalInfo; //0x7f8
	ULONGLONG Win32ClientInfo[62]; //0x800
	VOID* glDispatchTable[233]; //0x9f0
	ULONGLONG glReserved1[29]; //0x1138
	VOID* glReserved2; //0x1220
	VOID* glSectionInfo; //0x1228
	VOID* glSection; //0x1230
	VOID* glTable; //0x1238
	VOID* glCurrentRC; //0x1240
	VOID* glContext; //0x1248
	ULONG LastStatusValue; //0x1250
	UCHAR Padding2[4]; //0x1254
	struct _UNICODE_STRING StaticUnicodeString; //0x1258
	WCHAR StaticUnicodeBuffer[261]; //0x1268
	UCHAR Padding3[6]; //0x1472
	VOID* DeallocationStack; //0x1478
	VOID* TlsSlots[64]; //0x1480
	struct _LIST_ENTRY TlsLinks; //0x1680
	VOID* Vdm; //0x1690
	VOID* ReservedForNtRpc; //0x1698
	VOID* DbgSsReserved[2]; //0x16a0
	ULONG HardErrorMode; //0x16b0
	UCHAR Padding4[4]; //0x16b4
	VOID* Instrumentation[11]; //0x16b8
	struct _GUID ActivityId; //0x1710
	VOID* SubProcessTag; //0x1720
	VOID* PerflibData; //0x1728
	VOID* EtwTraceData; //0x1730
	VOID* WinSockData; //0x1738
	ULONG GdiBatchCount; //0x1740

	union
	{
		struct _PROCESSOR_NUMBER CurrentIdealProcessor; //0x1744
		ULONG IdealProcessorValue; //0x1744

		struct
		{
			UCHAR ReservedPad0; //0x1744
			UCHAR ReservedPad1; //0x1745
			UCHAR ReservedPad2; //0x1746
			UCHAR IdealProcessor; //0x1747
		};
	};

	ULONG GuaranteedStackBytes; //0x1748
	UCHAR Padding5[4]; //0x174c
	VOID* ReservedForPerf; //0x1750
	VOID* ReservedForOle; //0x1758
	ULONG WaitingOnLoaderLock; //0x1760
	UCHAR Padding6[4]; //0x1764
	VOID* SavedPriorityState; //0x1768
	ULONGLONG ReservedForCodeCoverage; //0x1770
	VOID* ThreadPoolData; //0x1778
	VOID** TlsExpansionSlots; //0x1780
	struct _CHPEV2_CPUAREA_INFO* ChpeV2CpuAreaInfo; //0x1788
	VOID* Unused; //0x1790
	ULONG MuiGeneration; //0x1798
	ULONG IsImpersonating; //0x179c
	VOID* NlsCache; //0x17a0
	VOID* pShimData; //0x17a8
	ULONG HeapData; //0x17b0
	UCHAR Padding7[4]; //0x17b4
	VOID* CurrentTransactionHandle; //0x17b8
	struct _TEB_ACTIVE_FRAME* ActiveFrame; //0x17c0
	VOID* FlsData; //0x17c8
	VOID* PreferredLanguages; //0x17d0
	VOID* UserPrefLanguages; //0x17d8
	VOID* MergedPrefLanguages; //0x17e0
	ULONG MuiImpersonation; //0x17e8

	union
	{
		volatile USHORT CrossTebFlags; //0x17ec
		USHORT SpareCrossTebBits : 16; //0x17ec
	};

	union
	{
		USHORT SameTebFlags; //0x17ee

		struct
		{
			USHORT SafeThunkCall : 1; //0x17ee
			USHORT InDebugPrint : 1; //0x17ee
			USHORT HasFiberData : 1; //0x17ee
			USHORT SkipThreadAttach : 1; //0x17ee
			USHORT WerInShipAssertCode : 1; //0x17ee
			USHORT RanProcessInit : 1; //0x17ee
			USHORT ClonedThread : 1; //0x17ee
			USHORT SuppressDebugMsg : 1; //0x17ee
			USHORT DisableUserStackWalk : 1; //0x17ee
			USHORT RtlExceptionAttached : 1; //0x17ee
			USHORT InitialThread : 1; //0x17ee
			USHORT SessionAware : 1; //0x17ee
			USHORT LoadOwner : 1; //0x17ee
			USHORT LoaderWorker : 1; //0x17ee
			USHORT SkipLoaderInit : 1; //0x17ee
			USHORT SkipFileAPIBrokering : 1; //0x17ee
		};
	};

	VOID* TxnScopeEnterCallback; //0x17f0
	VOID* TxnScopeExitCallback; //0x17f8
	VOID* TxnScopeContext; //0x1800
	ULONG LockCount; //0x1808
	LONG WowTebOffset; //0x180c
	VOID* ResourceRetValue; //0x1810
	VOID* ReservedForWdf; //0x1818
	ULONGLONG ReservedForCrt; //0x1820
	struct _GUID EffectiveContainerId; //0x1828
	ULONGLONG LastSleepCounter; //0x1838
	ULONG SpinCallCount; //0x1840
	UCHAR Padding8[4]; //0x1844
	ULONGLONG ExtendedFeatureDisableMask; //0x1848
};

static_assert(sizeof(_TEB) == 0x1850);
#pragma endregion
