#pragma once

#include <cstdint>
#include <limits>
#include <cstdint>

class CUtlMemoryPool
{
public:
	int Count() const
	{
		return nBlocksAllocated;
	}

	int PeakCount() const
	{
		return nPeakAlloc;
	}

	int BlockSize() const
	{
		return nBlockSize;
	}

protected:
	class CBlob
	{
	public:
		CBlob* pPrev;
		CBlob* pNext;
		int nNumBytes;
		char Data[1];
		char Padding[3];
	};

	int nBlockSize;
	int nBlocksPerBlob;
	int nGrowMode;
	int nBlocksAllocated;
	int nPeakAlloc;

	unsigned short nAlignment;
	unsigned short nNumBlobs;

	void* pHeadOfFreeList;
	void* pAllocOwner;
	CBlob BlobHead;
};

using UtlTSHashHandle_t = std::uintptr_t;

inline unsigned HashIntConventional(const int n)
{
	unsigned hash = 0xAAAAAAAA + (n & 0xFF);
	hash = (hash << 5) + hash + ((n >> 8) & 0xFF);
	hash = (hash << 5) + hash + ((n >> 16) & 0xFF);
	hash = (hash << 5) + hash + ((n >> 24) & 0xFF);

	return hash;
}

template <int nBucketCount, class tKey = std::uintptr_t>
class CUtlTSHashGenericHash
{
public:
	static int Hash(const tKey& Key, int nBucketMask)
	{
		int nHash = HashIntConventional(std::uintptr_t(Key));
		if (nBucketCount <= UINT16_MAX)
		{
			nHash ^= (nHash >> 16);
		}

		if (nBucketCount <= UINT8_MAX)
		{
			nHash ^= (nHash >> 8);
		}

		return (nHash & nBucketMask);
	}

	static bool Compare(const tKey& lhs, const tKey& rhs)
	{
		return lhs == rhs;
	}
};

template <class tElement, int nBucketCount, class tKey = std::uintptr_t, class tHashFuncs = CUtlTSHashGenericHash<nBucketCount, tKey>, int nAlignment = 0>
class CUtlTSHash
{
	static constexpr int nBucketMask = nBucketCount - 1;

public:
	static constexpr UtlTSHashHandle_t InvalidHandle()
	{
		return static_cast<UtlTSHashHandle_t>(0);
	}

	UtlTSHashHandle_t Find(tKey uiKey)
	{
		int iBucket = tHashFuncs::Hash(uiKey, nBucketCount);
		const HashBucket_t& hashBucket = aBuckets[iBucket];
		const UtlTSHashHandle_t hHash = Find(uiKey, hashBucket.pFirst, nullptr);
		return hHash ? hHash : Find(uiKey, hashBucket.pFirstUncommited, hashBucket.pFirst);
	}

	int Count() const
	{
		return EntryMemory.Count();
	}

	int GetElements(int nFirstElement, int nCount, UtlTSHashHandle_t* pHandles) const
	{
		int nIndex = 0;
		for (int nBucketIndex = 0; nBucketIndex < nBucketCount; nBucketIndex++)
		{
			const HashBucket_t& hashBucket = aBuckets[nBucketIndex];

			HashFixedData_t* pElement = hashBucket.pFirstUncommited;
			for (; pElement; pElement = pElement->pNext)
			{
				if (--nFirstElement >= 0)
					continue;

				pHandles[nIndex++] = reinterpret_cast<UtlTSHashHandle_t>(pElement);

				if (nIndex >= nCount)
					return nIndex;
			}
		}

		return nIndex;
	}

	tElement Element(UtlTSHashHandle_t hHash)
	{
		return ((HashFixedData_t*)(hHash))->Data;
	}

	const tElement& Element(UtlTSHashHandle_t hHash) const
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->Data;
	}

	tElement& operator[](UtlTSHashHandle_t hHash)
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->Data;
	}

	const tElement& operator[](UtlTSHashHandle_t hHash) const
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->Data;
	}

	tKey GetID(UtlTSHashHandle_t hHash) const
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->uiKey;
	}

private:
	template <typename tData>
	struct HashFixedDataInternal_t
	{
		tKey uiKey;
		HashFixedDataInternal_t<tData>* pNext;
		tData Data;
	};

	using HashFixedData_t = HashFixedDataInternal_t<tElement>;

	struct HashBucket_t
	{
	private:
		[[maybe_unused]] std::byte pad0[0x18];

	public:
		HashFixedData_t* pFirst;
		HashFixedData_t* pFirstUncommited;
	};

	UtlTSHashHandle_t Find(tKey uiKey, HashFixedData_t* pFirstElement, HashFixedData_t* pLastElement)
	{
		for (HashFixedData_t* pElement = pFirstElement; pElement != pLastElement; pElement = pElement->pNext)
		{
			if (tHashFuncs::Compare(pElement->uiKey, uiKey))
				return reinterpret_cast<UtlTSHashHandle_t>(pElement);
		}

		return InvalidHandle();
	}

	CUtlMemoryPool EntryMemory;
	MEM_PAD(0x40);
	HashBucket_t aBuckets[nBucketCount];
	bool bNeedsCommit;
};
