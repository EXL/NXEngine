
#ifndef _SECT_STRINGARRAY_H
#define _SECT_STRINGARRAY_H

class StringList;
class DString;
class DBuffer;

// the SectStringArray handler decodes sif sections which consist of an array
// of pascal strings. This includes SIF_SECTION_SHEETS and SIF_SECTION_DIRECTORY.

class SIFStringArraySect
{
public:
	static bool Decode(const uint8_t *data, int datalen, StringList *out);
	static uint8_t *Encode(StringList *strings, int *datalen_out);
	
	static void ReadPascalString(const uint8_t **data, const uint8_t *data_end, DString *out);
	static void WritePascalString(const char *str, DBuffer *out);
};


#endif
