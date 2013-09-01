
#ifndef _BUFIO_H
#define _BUFIO_H

#include "bufio.fdh"

void write_Variable(DBuffer *out, const uint8_t *data, int len);
void write_Variable(DBuffer *out, DBuffer *in);



#endif
