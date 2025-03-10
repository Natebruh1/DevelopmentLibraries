//---------------------------------------------------------------------------------------------------------------------
// id_array class
//---------------------------------------------------------------------------------------------------------------------
// A class somewhere between a map and an array. Good as a fast data store of dynamic objects referred to from multiple places.
// - insert and erase are fast O(1)
// - insertion returns an ID, which is how you look up data using get(id)
// - Get is also O(1)
// - Cannot iterate through the contents, use for when other data structures refer to elements stored here 
// - Not an STL-like container - not intended for that
// See https://bitsquid.blogspot.com/2011/09/managing-decoupling-part-4-id-lookup.html and the "array with holes" for background
// Example use-case. Store entities here and use the ids from this data structure to refer to entities everywhere
//                   Would never iterate this data structure (cannot), instead the renderer might have a scene graph
//                   of visible IDs and the gameplay a grid-structure with each cell holding IDs of the entities there etc.
// Downside: no attempt to optimise for cache performance - Data is stored contiguously, but after many insertions and 
//           deletions the data will not be in any particular order and access will become more random. Having said that, 
//           the same problem occurs in most containers with O(1) insertion and deletion, without a more inflexible DOD approach

#if !defined PACKED_ARRAY_INCLUDED
#define PACKED_ARRAY_INCLUDED

#include <stdint.h>
#include <stdexcept>
#include <limits>

template <typename T, typename INDEX_TYPE = uint32_t, typename VERSION_TYPE = uint32_t>
class PackedArray
{

	//----------------------------------------------------------------------------------------------
	// Public Types
	//----------------------------------------------------------------------------------------------
public:
	// Id of an element is its array index and a version number - this is how the user refers to elements
	// When a element is deleted then the gap reused, the version increases to give a new id for the same index
	struct element_id
	{
		INDEX_TYPE   index;
		VERSION_TYPE version;
	};


	//----------------------------------------------------------------------------------------------
	// Private Types / Constants
	//----------------------------------------------------------------------------------------------
private:
	// Gaps in the array are chained into a singly linked list.
	// Each gap in the array stores the index of the next gap in its unused data
	// The final gap stores this sentinel value
	static const INDEX_TYPE GAP_SENTINEL = std::numeric_limits<INDEX_TYPE>::max();

	// For each element, store the user data plus the version number - see above
	struct element
	{
		VERSION_TYPE version;
		INDEX_TYPE contiguousPointer;
		//T            data;

	};

	// Gaps left by deleting elements are chained into a linked list. The unused memory of the gap holds the index of the next gap
	// This leads to a subtle size requirement for the stored type, although with the default INDEX_TYPE and VERSION_TYPE parameters this can not occur
	static_assert(sizeof(element) >= sizeof(INDEX_TYPE), "Stored element must be larger than index type");


	//----------------------------------------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------------------------------------
public:
	// Allocates a fixed block of data on construction, which is left uninitialised.
	// This implementation only supports POD types for T, look at the dynamic_array class from earlier lab to see
	// how we could support arrays of more complex types requiring construction
	PackedArray(INDEX_TYPE capacity)
		: mCapacity(capacity), mElements(new element[capacity]), mContiguousElements(new T[capacity]) {
	}


	~PackedArray()
	{
		delete[] mElements;
		delete[] mContiguousElements;
	}


	//----------------------------------------------------------------------------------------------
	// Access
	//----------------------------------------------------------------------------------------------
public:
	INDEX_TYPE size() { return mSize; }
	INDEX_TYPE capacity() { return mCapacity; }

	// Get element of given id, return nullptr if none
	[[no_discard]] T* get(const element_id& id) noexcept
	{
		element* get_element = &mElements[id.index];
		if (get_element->version != id.version)  return nullptr;
		return mContiguousElements + (get_element->contiguousPointer);
	}

	// Const version of the get function above, in case caller has a const array. Can avoid code duplication in various complex ways - not worrying about it for this example
	[[no_discard]] const T* get(const element_id& id) const noexcept
	{
		element* get_element = &mElements[id.index];
		if (get_element->version != id.version)  return nullptr;
		return mContiguousElements + (get_element->contiguousPointer);
	}


	//----------------------------------------------------------------------------------------------
	// Usage
	//----------------------------------------------------------------------------------------------
public:
	// Insert given data, returns an id for user to access that data later
	// Assuming POD type data only, so no constructor is called, look at the dynamic_array class from earlier lab to see how construction of elements can be done
	element_id insert(const T& data)
	{
		// See if there are any gaps left from previously erased elements
		if (mFirstGapIndex != GAP_SENTINEL)
		{
			// Found a gap, get its ID (after increasing version number - this ensures requests for data previously stored at this index will fail)
			element* firstGap = &mElements[mFirstGapIndex];
			element_id newID{ mFirstGapIndex, firstGap->version };

			// Each gap contains the index of the next gap in its unused data - that will be the new first gap after this insertion
			mFirstGapIndex = next_gap(firstGap);

			firstGap->contiguousPointer = mSize;
			*(firstGap->contiguousPointer + mContiguousElements) = data;
			++mSize;

			return newID;
		}
		else
		{
			// No gaps to fill in, initialise a new element at current end of array, throw if out of space
			if (mSize == mCapacity)  throw std::bad_array_new_length();

			mElements[mSize].version = 0;
			mElements[mSize].contiguousPointer = mSize;
			*(mElements[mSize].contiguousPointer + mContiguousElements) = data;

			return { mSize++, 0 }; // Tricky use of ++, returns old value of mSize, then increases it
		}
	}


	// Erase element with given id, returns false if there is no matching element
	// Assuming POD type data only, so no destructor is called, look at the dynamic_array class from earlier lab to see how construction of elements can be done
	bool erase(element_id id)
	{
		// Check element exists
		element* erase_element = &mElements[id.index];
		if (erase_element->version != id.version)  return false;

		// When an element is erased, it becomes the first available gap to fill for subsequent insertions
		// Connect this new gap to the existing gap linked list


		T* idxErase = erase_element->contiguousPointer + mContiguousElements;
		T* idxFinal = mSize - 1 + mContiguousElements;

		//Change the pointer of the final element to not be the end
		mElements[mSize - 1].contiguousPointer = erase_element->contiguousPointer;

		//Swap the values of the elements
		std::swap(*idxErase, *idxFinal);


		next_gap(erase_element) = mFirstGapIndex;
		mFirstGapIndex = id.index;
		erase_element->version++;
		--mSize;
		return true;
	}

	T* begin()
	{
		return mContiguousElements;
	}

	T* end()
	{
		return mContiguousElements + mSize;
	}

	//----------------------------------------------------------------------------------------------
	// Private Helpers
	//----------------------------------------------------------------------------------------------
private:
	// Access unused element data as an index - used to link gaps in the array into a linked list
	INDEX_TYPE& next_gap(element* empty_element)
	{
		return *reinterpret_cast<INDEX_TYPE*>(&empty_element->contiguousPointer);
	}


	//----------------------------------------------------------------------------------------------
	// Private Data
	//----------------------------------------------------------------------------------------------
private:
	INDEX_TYPE mSize = 0;
	INDEX_TYPE mCapacity = 0;

	element* mElements = nullptr;
	T* mContiguousElements = nullptr;
	INDEX_TYPE finalIndex = mSize;
	INDEX_TYPE mFirstGapIndex = GAP_SENTINEL; // First element that has been used before but is empty now - a "gap"
	// May not be first sequentially - gaps are connected into a linked list - this is the first gap in that list
	// Sentinel value used to mark list end - default value here indicates there aren't any gaps at first

};


#endif // ID_ARRAY_INCLUDED
