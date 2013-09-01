
#ifndef _STRINGLIST_H
#define _STRINGLIST_H

#include "BList.h"

class StringList : protected BList
{
public:
	StringList() { }
	
	StringList(const StringList &other)
	{
		*this = other;
	}
	
	virtual ~StringList();
	
	void AddString(const char *str);
	char *StringAt(int index) const;
	bool SetString(int index, const char *newstring);
	void MakeEmpty();
	
	void Shuffle();
	bool ContainsString(const char *term);
	bool ContainsCaseString(const char *term);
	void RemoveString(int index);
	void RemoveString(const char *str);
	void RemoveIString(const char *str);
	
	void SwapItems(int index1, int index2);
	void DumpContents();
	
	int32_t CountItems() const { return BList::CountItems(); }
	
	StringList &operator= (const StringList &other);
	bool operator== (const StringList &other) const;
	bool operator!= (const StringList &other) const;
};



#endif
