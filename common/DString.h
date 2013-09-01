
#ifndef _DSTRING_H
#define _DSTRING_H

#include "basics.h"
#include "DBuffer.h"

/*
	DString vs. DBuffer
	
	The difference is that with a DBuffer, if you AppendString() multiple times,
	you will get null-terminators in between each string. With a DString,
	the strings will be concatenated. You can override this behavior in a DBuffer
	by calling AppendStringNoNull instead of AppendString, but there is no function
	for inserting NULLs into a DString, as that doesn't make sense.
*/

class DString
{
public:
	DString();
	DString(const char *string);
	DString(const char *string, int length);
	DString(DString &other);
	
	void SetTo(const char *string);
	void SetTo(const char *string, int length);
	void SetTo(DString *other);
	void SetTo(DString &other);
	
	void AppendString(const char *str);
	void AppendString(const char *str, int length);
	void AppendChar(uchar ch);
	
	void ReplaceString(const char *repstr_old, const char *repstr_new);
	void EnsureAlloc(int min_required);
	
	void Clear();
	char *String();
	int Length();
	
	void ReplaceUnprintableChars();
	
private:
	DBuffer fBuffer;
};

#endif
