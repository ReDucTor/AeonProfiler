
#pragma once

#include <type_traits>

class CAllocator  // super simple allocator that uses VirtualAlloc to allocate memory
{
private:
	struct AllocHeader
	{
		char* NextBlock;		// pointer to next block allocated using VirtualAlloc
		char* FreePointer;		// pointer to first byte free in this block
		size_t Size;			// size of this block (in bytes)
		size_t FreeRemaining;	// number of bytes free in this block
	};

	char* FirstBlock;			// pointer to first block that was allocated (head of list)
	char* CurrentBlock;			// pointer to current block (tail of list)

	HANDLE ghAllocatorMutex;
	int WaitOnMutex;			// whether this allocator needs a threading mutex (to keep it thread safe)

public:
	CAllocator(int InWaitOnMutex = 0);
	~CAllocator();

	void FreeBlocks();			// free all the allocations by freeing the blocks allocated from the operating system
	void GetAllocationStats(size_t& TotalSize, size_t& FreeSize);
	void PrintStats(char* Header, int NestLevel);

	void* AllocateBytes(size_t NumBytes, int Alignment);

	template<typename T>
	T * New()
	{
		T * obj = (T*)AllocateBytes(sizeof(T), std::alignment_of<T>::value);
		return new (obj) T();
	}

	// TODO: Variadic template.
	template<typename T, typename Arg1>
	T * New(Arg1 && arg1)
	{
		T * obj = (T*)AllocateBytes(sizeof(T), std::alignment_of<T>::value);
		return new (obj) T(std::forward<Arg1>(arg1));
	}

	template<typename T, typename Arg1, typename Arg2>
	T * New(Arg1 && arg1, Arg2 && arg2)
	{
		T * obj = (T*)AllocateBytes(sizeof(T), std::alignment_of<T>::value);
		return new (obj) T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
	}
};
