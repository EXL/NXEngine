
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "DString.h"
#include "DString.fdh"


DString::DString()
{
}

DString::DString(const char *str)
{
	SetTo(str);
}

DString::DString(const char *str, int length)
{
	SetTo(str, length);
}

DString::DString(DString &other)
{
	SetTo(other.String(), other.Length());
}

/*
void c------------------------------() {}
*/

void DString::SetTo(const char *str, int length)
{
	fBuffer.SetTo((uint8_t *)str, length);
}

void DString::SetTo(const char *str)
{
	fBuffer.SetTo((uint8_t *)str, strlen(str));
}

void DString::SetTo(DString *other)
{
	fBuffer.SetTo(other->fBuffer.Data(), other->fBuffer.Length());
}

void DString::SetTo(DString &other)
{
	fBuffer.SetTo(other.fBuffer.Data(), other.fBuffer.Length());
}

/*
void c------------------------------() {}
*/

void DString::AppendString(const char *str)
{
	fBuffer.AppendData((uint8_t *)str, strlen(str));
}

void DString::AppendString(const char *str, int length)
{
	fBuffer.AppendData((uint8_t *)str, length);
}

void DString::AppendChar(uchar ch)
{
	fBuffer.AppendData((uint8_t *)&ch, 1);
}

/*
void c------------------------------() {}
*/

void DString::ReplaceString(const char *repstr_old, const char *repstr_new)
{
	DString newString;
	char *str = String();
	char *ptr = str;
	int oldLength, newLength;
	char *hit;
	
	for(;;)
	{
		hit = strstr(ptr, repstr_old);
		if (!hit)
		{
			// first time around the loop? if so then we don't need
			// to do any copying as we won't modify ourselves anyway.
			if (ptr == str)
				return;
			
			newString.AppendString(ptr);
			break;
		}
		
		// defer calling the strlens until we're sure we need them
		// (by now we know there is an occurance of repstr_old within the original string).
		if (ptr == str)
		{
			oldLength = strlen(repstr_old);
			if (oldLength == 0) return;
			
			newLength = strlen(repstr_new);
		}
		
		// add substring up to the hit
		newString.AppendString(ptr, (hit - ptr));
		newString.AppendString(repstr_new, newLength);
		
		ptr = (hit + oldLength);
	}
	
	SetTo(newString);
}

/*{
	char *str = String();
	char *hit = strstr(str, oldstring);
	if (!hit) return;	// avoid strlens in common case of no hits
	
	int oldlen = strlen(oldstring);
	int newlen = strlen(newstring);
	
	for(;;)
	{
		DString temp(str, (hit - str));
		temp.AppendString(newstring);
		temp.AppendString(hit + oldlen);
		
		SetTo(temp.String());
		
		int index = (hit - str) + newlen;
		str = String() + index;
		
		hit = strstr(str, oldstring);
		if (!hit) break;
	}
}*/

/*
void c------------------------------() {}
*/

void DString::EnsureAlloc(int min_required)
{
	fBuffer.EnsureAlloc(min_required);
}

void DString::ReplaceUnprintableChars()
{
	fBuffer.ReplaceUnprintableChars();
}

/*
void c------------------------------() {}
*/

void DString::Clear()
{
	fBuffer.Clear();
}

int DString::Length()
{
	return fBuffer.Length();
}

char *DString::String()
{
	return fBuffer.String();
}
	

