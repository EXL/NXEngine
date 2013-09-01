
#include <stdio.h>
#include "FileBuffer.h"

FileBuffer::FileBuffer()
{
	fMaxSize = 0;
	fFP = NULL;
}

void FileBuffer::SetBufferSize(int maxsize)
{
	fMaxSize = maxsize;
}

void FileBuffer::SetFile(FILE *fp)
{
	fFP = fp;
}

/*
void c------------------------------() {}
*/

void FileBuffer::Write8(uint8_t data)
{
	fBuffer.Append8(data);
	CheckFlush(fMaxSize);
}

void FileBuffer::Write16(uint16_t data)
{
	fBuffer.Append16(data);
	CheckFlush(fMaxSize);
}

void FileBuffer::Write32(uint32_t data)
{
	fBuffer.Append32(data);
	CheckFlush(fMaxSize);
}

/*
void c------------------------------() {}
*/

void FileBuffer::CheckFlush(int maxsize)
{
	if (fBuffer.Length() >= maxsize)
	{
		if (fFP)
		{
			//stat("CheckFlush wrote %d bytes", fBuffer.Length());
			fwrite(fBuffer.Data(), fBuffer.Length(), 1, fFP);
			fBuffer.Clear();
		}
		else
		{
			staterr("CheckFlush: no file");
		}
	}
}

void FileBuffer::Flush()
{
	CheckFlush(0);
}

void FileBuffer::Dump()
{
	fBuffer.Clear();
}


