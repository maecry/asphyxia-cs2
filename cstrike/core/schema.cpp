#include "schema.h"

// used: [stl] vector
#include <vector>
// used: [stl] find_if
#include <algorithm>

// used: getworkingpath
#include "../core.h"

// used: ischemasystem
#include "interfaces.h"
#include "../sdk/interfaces/ischemasystem.h"

// used: l_print
#include "../utilities/log.h"

struct SchemaData_t
{
	FNV1A_t uHashedFieldName = 0x0ULL;
	std::uint32_t uOffset = 0x0U;
};

static std::vector<SchemaData_t> vecSchemaData;

bool SCHEMA::Setup(const wchar_t* wszFileName, const char* szModuleName)
{
	wchar_t wszDumpFilePath[MAX_PATH];
	if (!CORE::GetWorkingPath(wszDumpFilePath))
		return false;

	CRT::StringCat(wszDumpFilePath, wszFileName);

	HANDLE hOutFile = ::CreateFileW(wszDumpFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hOutFile == INVALID_HANDLE_VALUE)
		return false;

	// @todo: maybe remove this redundant? and put it inside CRT::String_t c'tor
	const std::time_t time = std::time(nullptr);
	std::tm timePoint;
	localtime_s(&timePoint, &time);

	CRT::String_t<64> szTimeBuffer(CS_XOR("[%d-%m-%Y %T] asphyxia | schema dump\n\n"), &timePoint);

	// write current date, time and info
	::WriteFile(hOutFile, szTimeBuffer.Data(), szTimeBuffer.Length(), nullptr, nullptr);

	CSchemaSystemTypeScope* pTypeScope = I::SchemaSystem->FindTypeScopeForModule(szModuleName);
	if (pTypeScope == nullptr)
		return false;

	const int nTableSize = pTypeScope->hashClasses.Count();
	L_PRINT(LOG_INFO) << CS_XOR("found \"") << nTableSize << CS_XOR("\" schema classes in module");

	// allocate memory for elements
	UtlTSHashHandle_t* pElements = new UtlTSHashHandle_t[nTableSize + 1U];
	const auto nElements = pTypeScope->hashClasses.GetElements(0, nTableSize, pElements);

	for (int i = 0; i < nElements; i++)
	{
		const UtlTSHashHandle_t hElement = pElements[i];

		if (hElement == 0)
			continue;

		CSchemaClassBinding* pClassBinding = pTypeScope->hashClasses[hElement];
		if (pClassBinding == nullptr)
			continue;

		SchemaClassInfoData_t* pDeclaredClassInfo;
		pTypeScope->FindDeclaredClass(&pDeclaredClassInfo, pClassBinding->szBinaryName);

		if (pDeclaredClassInfo == nullptr)
			continue;

		if (pDeclaredClassInfo->nFieldSize == 0)
			continue;

		CRT::String_t<MAX_PATH> szClassBuffer(CS_XOR("class %s\n"), pDeclaredClassInfo->szName);
		::WriteFile(hOutFile, szClassBuffer.Data(), szClassBuffer.Length(), nullptr, nullptr);

		for (auto j = 0; j < pDeclaredClassInfo->nFieldSize; j++)
		{
			SchemaClassFieldData_t* pFields = pDeclaredClassInfo->pFields;
			CRT::String_t<MAX_PATH> szFieldClassBuffer(CS_XOR("%s->%s"), pClassBinding->szBinaryName, pFields[j].szName);
			// store field info
			vecSchemaData.emplace_back(FNV1A::Hash(szFieldClassBuffer.Data()), pFields[j].nSingleInheritanceOffset);

			CRT::String_t<MAX_PATH> szFieldBuffer(CS_XOR("    %s %s = 0x%X\n"), pFields[j].pSchemaType->szName, pFields[j].szName, pFields[j].nSingleInheritanceOffset);
			// write field info
			::WriteFile(hOutFile, szFieldBuffer.Data(), szFieldBuffer.Length(), nullptr, nullptr);
		}
		#ifdef _DEBUG
		L_PRINT(LOG_INFO) << CS_XOR("dumped \"") << pDeclaredClassInfo->szName << CS_XOR("\" (total: ") << pDeclaredClassInfo->nFieldSize << CS_XOR(" fields)");
		#endif
	}

	// free allocated memory
	delete[] pElements;

	// close file
	::CloseHandle(hOutFile);

	return true;
}

std::uint32_t SCHEMA::GetOffset(const FNV1A_t uHashedFieldName)
{
	if (const auto it = std::ranges::find_if(vecSchemaData, [uHashedFieldName](const SchemaData_t& data)
		{ return data.uHashedFieldName == uHashedFieldName; });
		it != vecSchemaData.end())
		return it->uOffset;

	L_PRINT(LOG_ERROR) << CS_XOR("failed to find offset for field with hash: ") << L::AddFlags(LOG_MODE_INT_FORMAT_HEX | LOG_MODE_INT_SHOWBASE) << uHashedFieldName;
	CS_ASSERT(false); // schema field not found
	return 0U;
}

// @todo: optimize this, this is really poorly do and can be done much better?
std::uint32_t SCHEMA::GetForeignOffset(const char* szModulenName, const FNV1A_t uHashedClassName, const FNV1A_t uHashedFieldName)
{
	CSchemaSystemTypeScope* pTypeScope = I::SchemaSystem->FindTypeScopeForModule(szModulenName);
	if (pTypeScope == nullptr)
		return false;

	const int nTableSize = pTypeScope->hashClasses.Count();
	// allocate memory for elements
	UtlTSHashHandle_t* pElements = new UtlTSHashHandle_t[nTableSize + 1U];
	const auto nElements = pTypeScope->hashClasses.GetElements(0, nTableSize, pElements);
	std::uint32_t uOffset = 0x0;

	for (int i = 0; i < nElements; i++)
	{
		const UtlTSHashHandle_t hElement = pElements[i];

		if (hElement == 0)
			continue;

		CSchemaClassBinding* pClassBinding = pTypeScope->hashClasses[hElement];
		if (pClassBinding == nullptr)
			continue;

		SchemaClassInfoData_t* pDeclaredClassInfo;
		pTypeScope->FindDeclaredClass(&pDeclaredClassInfo, pClassBinding->szBinaryName);

		if (pDeclaredClassInfo == nullptr)
			continue;

		if (pDeclaredClassInfo->nFieldSize == 0)
			continue;

		for (auto j = 0; j < pDeclaredClassInfo->nFieldSize; j++)
		{
			SchemaClassFieldData_t* pFields = pDeclaredClassInfo->pFields;
			if (pFields == nullptr)
				continue;

			SchemaClassFieldData_t field = pFields[j];
			if (FNV1A::Hash(pClassBinding->szBinaryName) == uHashedClassName && FNV1A::Hash(field.szName) == uHashedFieldName)
				uOffset = field.nSingleInheritanceOffset;
		}
	}

	if (uOffset == 0x0)
		L_PRINT(LOG_WARNING) << CS_XOR("failed to find offset for field with hash: ") << L::AddFlags(LOG_MODE_INT_FORMAT_HEX | LOG_MODE_INT_SHOWBASE) << uHashedFieldName;

	return uOffset;
}
