#pragma once
#include "utlmemory.h"

// used: memorymove
#include "../../utilities/crt.h"

// @source: master/public/tier1/utlvector.h

/*
 * a growable array class which doubles in size by default.
 * it will always keep all elements consecutive in memory, and may move the
 * elements around in memory (via a realloc) when elements are inserted or removed.
 * clients should therefore refer to the elements of the vector by index and not pointers
 *
 * @note: if variable that uses it intend to call any method that needs to allocate/deallocate should have overloaded constructor/destructor and/or new/delete operators respectively
 */
template <class T, class A = CUtlMemory<T>>
class CUtlVector
{
	using CAllocator = A;

public:
	explicit CUtlVector(const int nGrowSize = 0, const int nInitialCapacity = 0) :
		memory(nGrowSize, nInitialCapacity), nSize(0), pElements(memory.Base()) { }

	CUtlVector(T* pMemory, const int nInitialCapacity, const int nInitialCount = 0) :
		memory(pMemory, nInitialCapacity), nSize(nInitialCount), pElements(memory.Base()) { }

	CUtlVector(const CUtlVector&) = delete;

	~CUtlVector()
	{
		Purge();
	}

	CUtlVector& operator=(const CUtlVector& vecOther)
	{
		CS_ASSERT(&vecOther != this); // self-assignment isn't allowed

		const int nSourceCount = vecOther.Count();
		SetCount(nSourceCount);

		for (int i = 0; i < nSourceCount; i++)
			(*this)[i] = vecOther[i];

		return *this;
	}

	[[nodiscard]] T& operator[](const int nIndex)
	{
		CS_ASSERT(IsValidIndex(nIndex)); // given index is out of range
		return memory[nIndex];
	}

	[[nodiscard]] const T& operator[](const int nIndex) const
	{
		CS_ASSERT(IsValidIndex(nIndex)); // given index is out of range
		return memory[nIndex];
	}

	[[nodiscard]] T& Element(const int nIndex)
	{
		CS_ASSERT(IsValidIndex(nIndex)); // given index is out of range
		return memory[nIndex];
	}

	[[nodiscard]] const T& Element(const int nIndex) const
	{
		CS_ASSERT(IsValidIndex(nIndex)); // given index is out of range
		return memory[nIndex];
	}

	[[nodiscard]] T* Base()
	{
		return memory.Base();
	}

	[[nodiscard]] const T* Base() const
	{
		return memory.Base();
	}

	[[nodiscard]] int Count() const
	{
		return nSize;
	}

	[[nodiscard]] int& Size()
	{
		return nSize;
	}

	[[nodiscard]] bool IsValidIndex(const int nIndex) const
	{
		return (nIndex >= 0) && (nIndex < nSize);
	}

	void CopyFromArray(const T* pArraySource, int nArraySize)
	{
		// can't insert something that's in the list. reallocation may hose us
		CS_ASSERT(memory.Base() == nullptr || pArraySource == nullptr || begin() >= (pArraySource + nArraySize) || pArraySource >= end());

		// resize to accommodate array
		SetCount(nArraySize);

		for (int i = 0; i < nArraySize; i++)
			(*this)[i] = pArraySource[i];
	}

	void GrowVector(const int nCount = 1)
	{
		if (nSize + nCount > memory.AllocationCount())
			memory.Grow(nSize + nCount - memory.AllocationCount());

		nSize += nCount;
		pElements = memory.Base();
	}

	void EnsureCapacity(int nCapacity)
	{
		memory.EnsureCapacity(nCapacity);
		pElements = memory.Base();
	}

	void Purge()
	{
		RemoveAll();
		memory.Purge();
		pElements = memory.Base();
	}

	void ShiftElementsRight(const int nElement, const int nShift = 1)
	{
		CS_ASSERT(IsValidIndex(nElement) || nSize == 0 || nShift == 0);

		if (const int nToMove = nSize - nElement - nShift; nToMove > 0 && nShift > 0)
			CRT::MemoryMove(&Element(nElement + nShift), &Element(nElement), nToMove * sizeof(T));
	}

	void ShiftElementsLeft(const int nElement, const int nShift = 1)
	{
		CS_ASSERT(IsValidIndex(nElement) || nSize == 0 || nShift == 0);

		if (const int nToMove = nSize - nElement - nShift; nToMove > 0 && nShift > 0)
			CRT::MemoryMove(&Element(nElement), &Element(nElement + nShift), nToMove * sizeof(T));
	}

	int AddToHead()
	{
		return InsertBefore(0);
	}

	int AddToHead(const T& source)
	{
		// can't insert something that's in the list. reallocation may hose us
		CS_ASSERT(memory.Base() == nullptr || &source < begin() || &source >= end());
		return InsertBefore(0, source);
	}

	int AddMultipleToHead(const int nCount)
	{
		return InsertMultipleBefore(0, nCount);
	}

	int AddToTail()
	{
		return InsertBefore(nSize);
	}

	int AddToTail(const T& source)
	{
		// can't insert something that's in the list. reallocation may hose us
		CS_ASSERT(memory.Base() == nullptr || &source < begin() || &source >= end());
		return InsertBefore(nSize, source);
	}

	int AddMultipleToTail(const int nCount)
	{
		return InsertMultipleBefore(nSize, nCount);
	}

	void SetCount(const int nCount)
	{
		RemoveAll();
		AddMultipleToTail(nCount);
	}

	int InsertBefore(const int nElement)
	{
		// can insert at the end
		CS_ASSERT(nElement == nSize || IsValidIndex(nElement));

		GrowVector();
		ShiftElementsRight(nElement);
		new (&Element(nElement)) T; // @todo: all functions of game classes that using this should be wrapped with constraints 'requires (std::is_pointer_v<T> || have_constructor_overload<T> || have_new_overload<T>)'
		return nElement;
	}

	int InsertMultipleBefore(const int nElement, const int nCount)
	{
		if (nCount == 0)
			return nElement;

		// can insert at the end
		CS_ASSERT(nElement == nSize || IsValidIndex(nElement));

		GrowVector(nCount);
		ShiftElementsRight(nElement, nCount);

		// invoke default constructors
		for (int i = 0; i < nElement; ++i)
			new (&Element(nElement + i)) T;

		return nElement;
	}

	int InsertBefore(const int nElement, const T& source)
	{
		// can't insert something that's in the list. reallocation may hose us
		CS_ASSERT(memory.Base() == nullptr || &source < begin() || &source >= end());

		// can insert at the end
		CS_ASSERT(nElement == nSize || IsValidIndex(nElement));

		// reallocate if can't insert something that's in the list
		GrowVector();
		ShiftElementsRight(nElement);
		new (&Element(nElement)) T(source);
		return nElement;
	}

	int InsertMultipleBefore(const int nElement, const int nCount, const T* pSource)
	{
		if (nCount == 0)
			return nElement;

		// can insert at the end
		CS_ASSERT(nElement == nSize || IsValidIndex(nElement));

		GrowVector(nCount);
		ShiftElementsRight(nElement, nCount);

		// invoke default constructors
		if (pSource == nullptr)
		{
			for (int i = 0; i < nCount; ++i)
				new (&Element(nElement + i)) T;
		}
		else
		{
			for (int i = 0; i < nCount; i++)
				new (&Element(nElement)) T(pSource[i]);
		}

		return nElement;
	}

	[[nodiscard]] int Find(const T& source) const
	{
		for (int i = 0; i < nSize; ++i)
		{
			if (Element(i) == source)
				return i;
		}

		return -1;
	}

	bool FindAndRemove(const T& source)
	{
		if (const int nElement = Find(source); nElement != -1)
		{
			Remove(nElement);
			return true;
		}

		return false;
	}

	void Remove(const int nElement)
	{
		(&Element(nElement))->~T();
		ShiftElementsLeft(nElement);
		--nSize;
	}

	void RemoveAll()
	{
		for (int i = nSize; --i >= 0;)
			(&Element(i))->~T();

		nSize = 0;
	}

	[[nodiscard]] auto begin() noexcept
	{
		return memory.Base();
	}

	[[nodiscard]] auto end() noexcept
	{
		return memory.Base() + nSize;
	}

	[[nodiscard]] auto begin() const noexcept
	{
		return memory.Base();
	}

	[[nodiscard]] auto end() const noexcept
	{
		return memory.Base() + nSize;
	}

protected:
	int nSize;
	CAllocator memory;
	T* pElements;
};

template <class T>
class CUtlVectorAligned : public CUtlVector<T, CUtlMemoryAligned<T, alignof(T)>>
{
};

//a array class with a fixed allocation scheme
template <class T, std::size_t MAX_SIZE>
class CUtlVectorFixed : public CUtlVector<T, CUtlMemoryFixed<T, MAX_SIZE>>
{
	using CBaseClass = CUtlVector<T, CUtlMemoryFixed<T, MAX_SIZE>>;

public:
	explicit CUtlVectorFixed(int nGrowSize = 0, int nInitialCapacity = 0) :
		CBaseClass(nGrowSize, nInitialCapacity) { }

	CUtlVectorFixed(T* pMemory, int nElements) :
		CBaseClass(pMemory, nElements) { }
};

template <typename T>
class C_NetworkUtlVectorBase
{
public:
	std::uint32_t nSize;
	T* pElements;
};