#pragma once

struct ResourceBinding_t
{
	void* pData;
};

template <typename T>
class CStrongHandle
{
public:
	operator T* () const
	{
		if (pBinding == nullptr)
			return nullptr;

		return static_cast<T*>(pBinding->pData);
	}

	T* operator->() const
	{
		if (pBinding == nullptr)
			return nullptr;

		return static_cast<T*>(pBinding->pData);
	}

	const ResourceBinding_t* pBinding;
};

