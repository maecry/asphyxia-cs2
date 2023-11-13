#pragma once

class CKeyValues3
{
public:
	std::uint64_t uKey;
	void* pValue;

	std::byte pad[0x8];
};

struct KV3ID_t
{
	const char* szName;
	std::uint64_t unk0;
	std::uint64_t unk1;
};
