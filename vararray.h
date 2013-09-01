
#ifndef _VARARRAY_H
#define _VARARRAY_H

#include <stdlib.h>

template <typename T>
class VarArray
{
public:
	VarArray()
	{
		nitems = 0;
		items = NULL;
	}
	
	~VarArray()
	{
		if (items)
			free(items);
	}
	
	// retrieves the item at index. if index is outside
	// the bounds of the array, returns 0/null.
	T get(int index)
	{
		if (index < 0 || index >= nitems)
			return (T)0;
		
		return items[index];
	}
	
	// put an item into the array at index.
	// if index is past the end of the array, the array is expanded.
	void put(int index, T value)
	{
		if (index >= 0)
		{
			EnsureAlloc(index + 1);
			items[index] = value;
		}
	}
	
	// make sure the array is big enough to contain up to allocnum items.
	// any unused items are initilized to 0/null.
	void EnsureAlloc(int allocnum)
	{
		if (allocnum > nitems)
		{
			if (items == NULL)
				items = (T *)malloc(sizeof(T) * allocnum);
			else
				items = (T *)realloc(items, (sizeof(T) * allocnum));
			
			memset(&items[nitems], 0, ((allocnum - nitems) * sizeof(T)));
			nitems = allocnum;
		}
	}
	
	// convenience function to access it like an array.
	// however unlike put(), you cannot use it to expand the array--
	// if you try to use it to write outside the bounds of the array,
	// the value will simply be lost.
	T& operator[] (const int index)
	{
		if (index < 0 || index >= nitems)
		{
			static T ZERO_T;
			memset(&ZERO_T, 0, sizeof(T));
			return ZERO_T;
		}
		
		return items[index];
	}
	
	// set the size of the array to 0 items.
	void MakeEmpty()
	{
		if (items)
		{
			free(items);
			items = NULL;
		}
		
		nitems = 0;
	}
	
	int nitems;
	
private:
	T *items;
};




#endif

