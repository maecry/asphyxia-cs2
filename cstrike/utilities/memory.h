#pragma once
// used: memory api
#include <memory>
// used: std::vector
#include <vector>

#include "../common.h"

#pragma region memory_definitions
#pragma warning(push)
#pragma warning(disable: 6255) // '_alloca' indicates failure by raising a stack overflow exception. consider using '_malloca' instead
#define MEM_STACKALLOC(SIZE) _malloca(SIZE)
#pragma warning(pop)
#define MEM_STACKFREE(MEMORY) static_cast<void>(0)

#define MEM_PAD(SIZE)										\
private:												\
	char CS_CONCATENATE(pad_0, __COUNTER__)[SIZE]; \
public:
#pragma endregion

class CUtlBuffer;
class CKeyValues3;
struct KV3ID_t;

namespace MEM
{
	bool Setup();

	/* @section: allocation */
	// allocate a block of memory from a heap
	[[nodiscard]] void* HeapAlloc(const std::size_t nSize);
	// free a memory block allocated from a heap
	void HeapFree(void* pMemory);
	// reallocate a block of memory from a heap
	// @note: we're expect this to allocate instead when passed null, and free if size is null
	void* HeapRealloc(void* pMemory, const std::size_t nNewSize);

	/* @section: get */
	/// alternative of 'GetModuleHandle()'
	/// @param[in] wszModuleName module name to search base handle for, null means current process
	/// @returns: base handle of module with given name if it exist, null otherwise
	[[nodiscard]] void* GetModuleBaseHandle(const wchar_t* wszModuleName);
	/// alternative of 'GetModuleFileName()'
	/// @param[in] hModuleBase module base to search filename for, null means current process
	/// @returns: name of given module if it's valid, null otherwise
	[[nodiscard]] const wchar_t* GetModuleBaseFileName(const void* hModuleBase, const bool bGetFullPath = false);
	/// alternative of 'GetProcAddress()'
	/// @remarks: doesn't support forwarded exports, this means you may need to manual call 'LoadLibrary'/'FreeLibrary' for export library
	/// @returns: pointer to exported procedure
	[[nodiscard]] void* GetExportAddress(const void* hModuleBase, const char* szProcedureName);

	/// @param[in] szSectionName section to get info of (e.g. ".rdata", ".text", etc)
	/// @param[out] ppSectionStart output for section start address
	/// @param[out] pnSectionSize output for section size
	/// @returns: true if code section has been found, false otherwise
	[[nodiscard]] bool GetSectionInfo(const void* hModuleBase, const char* szSectionName, std::uint8_t** ppSectionStart, std::size_t* pnSectionSize);
	/// get absolute address from relative address
	/// @param[in] pRelativeAddress pointer to relative address, e.g. destination address from JMP, JE, JNE and others instructions
	/// @param[in] nPreOffset offset before relative address
	/// @param[in] nPostOffset offset after relative address
	/// @returns: pointer to absolute address
	template <typename T = std::uint8_t>
	[[nodiscard]] T* GetAbsoluteAddress(T* pRelativeAddress, int nPreOffset = 0x0, int nPostOffset = 0x0)
	{
		pRelativeAddress += nPreOffset;
		pRelativeAddress += sizeof(std::int32_t) + *reinterpret_cast<std::int32_t*>(pRelativeAddress);
		pRelativeAddress += nPostOffset;
		return pRelativeAddress;
	}
	/// resolve rip relative address
	/// @param[in] nAddressBytes as byte for the address we want to resolve
	/// @param[in] nRVAOffset offset of the relative address
	/// @param[in] nRIPOffset offset of the instruction pointer
	/// @returns: pointer to resolved address
	[[nodiscard]] CS_INLINE std::uint8_t* ResolveRelativeAddress(std::uint8_t* nAddressBytes, std::uint32_t nRVAOffset, std::uint32_t nRIPOffset)
	{
		std::uint32_t nRVA = *reinterpret_cast<std::uint32_t*>(nAddressBytes + nRVAOffset);
		std::uint64_t nRIP = reinterpret_cast<std::uint64_t>(nAddressBytes) + nRIPOffset;

		return reinterpret_cast<std::uint8_t*>(nRVA + nRIP);
	}

	/// get pointer to function of virtual-function table
	/// @returns: pointer to virtual function
	template <typename T = void*>
	[[nodiscard]] CS_INLINE T GetVFunc(const void* thisptr, std::size_t nIndex)
	{
		return (*static_cast<T* const*>(thisptr))[nIndex];
	}
	/// call virtual function of specified class at given index
	/// @note: reference and const reference arguments must be forwarded as pointers or wrapped with 'std::ref'/'std::cref' calls!
	/// @returns: result of virtual function call
	template <typename T, std::size_t nIndex, class CBaseClass, typename... Args_t>
	static CS_INLINE T CallVFunc(CBaseClass* thisptr, Args_t... argList)
	{
		using VirtualFn_t = T(__thiscall*)(const void*, decltype(argList)...);
		return (*reinterpret_cast<VirtualFn_t* const*>(reinterpret_cast<std::uintptr_t>(thisptr)))[nIndex](thisptr, argList...);
	}
	/* @section: search */
	/// ida style pattern byte comparison in a specific mo	dule
	/// @param[in] wszModuleName module name where to search for pattern
	/// @param[in] szPattern ida style pattern, e.g. "55 8B 40 ? 30", wildcard can be either '?' or "??", bytes always presented by two numbers in a row [00 .. FF], whitespaces can be omitted (wildcards in this case should be two-character)
	/// @returns: pointer to address of the first found occurrence with equal byte sequence on success, null otherwise
	[[nodiscard]] std::uint8_t* FindPattern(const wchar_t* wszModuleName, const char* szPattern);
	/// naive style pattern byte comparison in a specific module
	/// @param[in] wszModuleName module name where to search for pattern
	/// @param[in] szBytePattern naive style pattern, e.g. "\x55\x8B\x40\x00\x30", wildcard bytes value ignored
	/// @param[in] szByteMask wildcard mask for byte array, e.g. "xxx?x", should always correspond to bytes count
	/// @returns: pointer to address of the first found occurrence with equal byte sequence on success, null otherwise
	[[nodiscard]] std::uint8_t* FindPattern(const wchar_t* wszModuleName, const char* szBytePattern, const char* szByteMask);
	/// pattern byte comparison in the specific region
	/// @param[in] arrByteBuffer byte sequence to search
	/// @param[in] nByteCount count of search bytes
	/// @param[in] szByteMask [optional] wildcard mask for byte array
	/// @returns: pointer to address of the first found occurrence with equal byte sequence on success, null otherwise
	[[nodiscard]] std::uint8_t* FindPatternEx(const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask = nullptr);
	/// pattern byte comparison in the specific region
	/// @param[in] arrByteBuffer byte sequence to search
	/// @param[in] nByteCount count of search bytes
	/// @param[in] szByteMask [optional] wildcard mask for byte array
	/// @returns: pointers to addresses of the all found occurrences with equal byte sequence on success, empty otherwise
	[[nodiscard]] std::vector<std::uint8_t*> FindPatternAllOccurrencesEx(const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask = nullptr);
	/// class RTTI virtual table search in a specific module
	/// @returns: pointer to the found virtual table on success, null otherwise

	/* @section: extra */
	/// convert ida-style pattern to byte array
	/// @param[in] szPattern ida-style pattern, e.g. "55 8B 40 ? 30", wildcard can be either '?' or "??", bytes are always presented by two numbers in a row [00 .. FF], blank delimiters are ignored and not necessary (wildcard in this case should be two-character)
	/// @param[out] pOutByteBuffer output for converted, zero-terminated byte array
	/// @param[out] szOutMaskBuffer output for wildcard, zero-terminated byte mask
	/// @returns: count of the converted bytes from the pattern
	std::size_t PatternToBytes(const char* szPattern, std::uint8_t* pOutByteBuffer, char* szOutMaskBuffer);
	/// convert byte array to ida-style pattern
	/// @param[in] pByteBuffer buffer of bytes to convert
	/// @param[in] nByteCount count of bytes to convert
	/// @param[out] szOutBuffer output for converted pattern
	/// @returns: length of the converted ida-style pattern, not including the terminating null
	std::size_t BytesToPattern(const std::uint8_t* pByteBuffer, const std::size_t nByteCount, char* szOutBuffer);
	
	/* @section: game exports */
	inline unsigned long(CS_STDCALL* fnUnDecorateSymbolName)(const char* szName, char* pszOutput, unsigned long nMaxStringLength, unsigned long dwFlags) = nullptr;

	// SDL window stuffs
	// @note: helpful for doing our mouse cursor like force show it when our menu is opened
	inline int(CS_STDCALL* fnSetRelativeMouseMode)(int) = nullptr;
	inline int(CS_STDCALL* fnSetWindowMouseGrab)(void*, int) = nullptr;
	inline bool(CS_STDCALL* fnGetRelativeMouseMode)(void) = nullptr;
	inline int(CS_STDCALL* fnWarpMouseInWindow)(void*, float, float) = nullptr;
	inline bool(CS_FASTCALL* fnLoadKV3)(CKeyValues3*, void*, const char*, const KV3ID_t*, const char*);
	inline std::int64_t(CS_FASTCALL* fnCreateMaterial)(void*, void*, const char*, void*, unsigned int, unsigned int);

	inline void(CS_FASTCALL* fnUtlBufferInit)(CUtlBuffer*, int, int, int);
	inline void(CS_FASTCALL* fnUtlBufferPutString)(CUtlBuffer*, const char*);
	inline void(CS_FASTCALL* fnUtlBufferEnsureCapacity)(CUtlBuffer*, int);
}
