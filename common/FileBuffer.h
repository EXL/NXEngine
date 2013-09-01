
#ifndef _FILEBUFFER_H
#define _FILEBUFFER_H

#include "DBuffer.h"


class FileBuffer
{
public:
	FileBuffer();
	void SetBufferSize(int maxsize);
	void SetFile(FILE *fp);
	
	void Write8(uint8_t data);
	void Write16(uint16_t data);
	void Write32(uint32_t data);
	
	void Flush();
	void Dump();
	
private:
	void CheckFlush(int maxsize);
	
	DBuffer fBuffer;
	int fMaxSize;
	
	FILE *fFP;
};



#endif
