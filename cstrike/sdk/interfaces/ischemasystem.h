#pragma once

// used: utlthash
#include "../datatypes/utlthash.h"
// used: utlvector
#include "../datatypes/utlvector.h"
// used: callvfunc
#include "../../utilities/memory.h"

#define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x188
#define SCHEMASYSTEMTYPESCOPE_OFF1 0x3F8
#define SCHEMASYSTEMTYPESCOPE_OFF2 0x8

using SchemaString_t = const char*;
struct SchemaMetadataEntryData_t;

class CSchemaSystemTypeScope;
class CSchemaType;

struct CSchemaClassBinding
{
	CSchemaClassBinding* pParent;
	const char* szBinaryName; // ex: C_World
	const char* szModuleName; // ex: libclient.so
	const char* szClassName; // ex: client
	void* pClassInfoOldSynthesized;
	void* pClassInfo;
	void* pThisModuleBindingPointer;
	CSchemaType* pSchemaType;
};

class CSchemaType
{
public:
	bool GetSizes(int* pOutSize, uint8_t* unkPtr)
	{
		return MEM::CallVFunc<bool, 3U>(this, pOutSize, unkPtr);
	}

public:
	bool GetSize(int* out_size)
	{
		uint8_t smh = 0;
		return GetSizes(out_size, &smh);
	}

public:
	void* pVtable; // 0x0000
	const char* szName; // 0x0008

	CSchemaSystemTypeScope* pSystemTypeScope; // 0x0010
	uint8_t nTypeCategory; // ETypeCategory 0x0018
	uint8_t nAatomicCategory; // EAtomicCategory 0x0019
};

struct SchemaClassFieldData_t
{
	SchemaString_t szName; // 0x0000
	CSchemaType* pSchemaType; // 0x0008
	std::uint32_t nSingleInheritanceOffset; // 0x0010
	std::int32_t nMetadataSize; // 0x0014
	SchemaMetadataEntryData_t* pMetaData; // 0x0018
};

struct SchemaClassInfoData_t;

struct SchemaBaseClassInfoData_t
{
	int32_t nOffset;
	SchemaClassInfoData_t* pClass;
};

struct SchemaClassInfoData_t
{
private:
	void* pVtable; // 0x0000
public:
	const char* szName; // 0x0008
	char* szDescription; // 0x0010

	int m_nSize; // 0x0018
	std::int16_t nFieldSize; // 0x001C
	std::int16_t nStaticSize; // 0x001E
	std::int16_t nMetadataSize; // 0x0020

	std::uint8_t nAlignOf; // 0x0022
	std::uint8_t nBaseClassesCount; // 0x0023
	char pad2[0x4]; // 0x0024
	SchemaClassFieldData_t* pFields; // 0x0028
	char pad3[0x8]; // 0x0030
	SchemaBaseClassInfoData_t* pBaseClasses; // 0x0038
	char pad4[0x28]; // 0x0040

	//public:
	//SchemaClassFieldData_t* pFields; // 0x0028

	bool InheritsFrom(SchemaClassInfoData_t* pClassInfo)
	{
		if (pClassInfo == this && pClassInfo != nullptr)
			return true;
		else if (pBaseClasses == nullptr || pClassInfo == nullptr)
			return false;

		for (int i = 0; i < nBaseClassesCount; i++)
		{
			auto& baseClass = pBaseClasses[i];
			if (baseClass.pClass->InheritsFrom(pClassInfo))
				return true;
		}

		return false;
	}
};

struct SchemaEnumeratorInfoData_t
{
	SchemaString_t szName;

	union
	{
		unsigned char value_char;
		unsigned short value_short;
		unsigned int value_int;
		unsigned long long value;
	};

	MEM_PAD(0x10); // 0x0010
};

class CSchemaEnumInfo
{
public:
	SchemaEnumeratorInfoData_t enumInfoData;
};

class CSchemaEnumBinding
{
public:
	virtual const char* GetBindingName() = 0;
	virtual CSchemaClassBinding* AsClassBinding() = 0;
	virtual CSchemaEnumBinding* AsEnumBinding() = 0;
	virtual const char* GetBinaryName() = 0;
	virtual const char* GetProjectName() = 0;

public:
	char* szBindingName_; // 0x0008
	char* szDllName_; // 0x0010
	std::int8_t nAlign_; // 0x0018
	MEM_PAD(0x3); // 0x0019
	std::int16_t nSize_; // 0x001C
	std::int16_t nFlags_; // 0x001E
	SchemaEnumeratorInfoData_t* pEnumInfo_;
	MEM_PAD(0x8); // 0x0028
	CSchemaSystemTypeScope* pTypeScope_; // 0x0030
	MEM_PAD(0x8); // 0x0038
	std::int32_t unk1_; // 0x0040
};

class CSchemaSystemTypeScope
{
public:
	void FindDeclaredClass(SchemaClassInfoData_t** pReturnClass, const char* szClassName)
	{
		return MEM::CallVFunc<void, 2U>(this, pReturnClass, szClassName);
	}

	CSchemaType* FindSchemaTypeByName(const char* szName, std::uintptr_t* pSchema)
	{
		return MEM::CallVFunc<CSchemaType*, 4U>(this, szName, pSchema);
	}

	CSchemaType* FindTypeDeclaredClass(const char* szName)
	{
		return MEM::CallVFunc<CSchemaType*, 5U>(this, szName);
	}

	CSchemaType* FindTypeDeclaredEnum(const char* szName)
	{
		return MEM::CallVFunc<CSchemaType*, 6U>(this, szName);
	}

	CSchemaClassBinding* FindRawClassBinding(const char* szName)
	{
		return MEM::CallVFunc<CSchemaClassBinding*, 7U>(this, szName);
	}

	void* pVtable; // 0x0000
	char szName[256U]; // 0x0008
	MEM_PAD(SCHEMASYSTEMTYPESCOPE_OFF1); // 0x0108
	CUtlTSHash<CSchemaClassBinding*, 256, unsigned int> hashClasses; // 0x0588
	MEM_PAD(SCHEMASYSTEMTYPESCOPE_OFF2); // 0x05C8
	CUtlTSHash<CSchemaEnumBinding*, 256, unsigned int> hashEnumes; // 0x2DD0
};

class ISchemaSystem
{
public:
	CSchemaSystemTypeScope* FindTypeScopeForModule(const char* m_module_name)
	{
		return MEM::CallVFunc<CSchemaSystemTypeScope*, 13U>(this, m_module_name, nullptr);
	}

private:
	MEM_PAD(SCHEMASYSTEM_TYPE_SCOPES_OFFSET); // 0x0000
public:
	// table of type scopes
	CUtlVector<CSchemaSystemTypeScope*> vecTypeScopes; // SCHEMASYSTEM_TYPE_SCOPES_OFFSET
};
