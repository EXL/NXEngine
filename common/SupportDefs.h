/*
 * Copyright 2004-2007, Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Author:
 *		Erik Jaesler (erik@cgsoftware.com)
 */
#ifndef _SUPPORT_DEFS_H
#define _SUPPORT_DEFS_H

#include <stdint.h>

/* this must be located before the include of sys/types.h */
#if !defined(_SYS_TYPES_H) && !defined(_SYS_TYPES_H_)
typedef unsigned long			ulong;
typedef unsigned int			uint;
typedef unsigned short			ushort;
#endif

//#include <BeBuild.h>
//#include <Errors.h>

//#include <sys/types.h>


/* Shorthand type formats */
typedef	int8_t			int8;
typedef uint8_t			uint8;
typedef volatile int8_t		vint8;
typedef volatile uint8_t	vuint8;

typedef	int16_t			int16;
typedef uint16_t 		uint16;
typedef volatile int16_t	vint16;
typedef volatile uint16_t	vuint16;

typedef	int32_t			int32;
typedef uint32_t		uint32;
typedef volatile int32_t	vint32;
typedef volatile uint32_t	vuint32;

typedef	int64_t			int64;
typedef uint64_t		uint64;
typedef volatile int64_t	vint64;
typedef volatile uint64_t	vuint64;

typedef volatile long		vlong;
typedef volatile int		vint;
typedef volatile short		vshort;
typedef volatile char		vchar;

typedef volatile unsigned long	vulong;
typedef volatile unsigned int	vuint;
typedef volatile unsigned short	vushort;
typedef volatile unsigned char	vuchar;

typedef unsigned char		uchar;
typedef unsigned short          unichar;


/* Descriptive formats */
typedef int32			status_t;
typedef int64			bigtime_t;
typedef uint32			type_code;
typedef uint32			perform_code;


/* Empty string ("") */
#ifdef __cplusplus
extern const char *B_EMPTY_STRING;
#endif

#ifndef INT8_MAX
#define INT8_MAX	0x7f
#endif
#ifndef INT16_MAX
#define INT16_MAX	0x7fff
#endif
#ifndef INT32_MAX
#define INT32_MAX	0x7fffffff
#endif
#ifndef INT64_MAX
#define INT64_MAX	0x7fffffffffffffff
#endif
#ifndef UINT8_MAX
#define UINT8_MAX	0xff
#endif
#ifndef UINT16_MAX
#define UINT16_MAX	0xffff
#endif
#ifndef UINT32_MAX
#define UINT32_MAX	0xffffffffffffffff
#endif

/* min and max comparisons */
#ifndef __cplusplus
#	ifndef min
#		define min(a,b) ((a)>(b)?(b):(a))
#	endif
#	ifndef max
#		define max(a,b) ((a)>(b)?(a):(b))
#	endif
#endif 

/* min() and max() won't work in C++ */
#define min_c(a,b) ((a)>(b)?(b):(a))
#define max_c(a,b) ((a)>(b)?(a):(b))


/* Grandfathering */
//#ifndef __cplusplus
//#	include <stdbool.h>
//#endif 

#ifndef NULL
#	define NULL (0)
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Atomic functions; previous value is returned */
extern int32	atomic_set(vint32 *value, int32 newValue);
extern int32	atomic_test_and_set(vint32 *value, int32 newValue, int32 testAgainst);
extern int32	atomic_add(vint32 *value, int32 addValue);
extern int32	atomic_and(vint32 *value, int32 andValue);
extern int32	atomic_or(vint32 *value, int32 orValue);	
extern int32	atomic_get(vint32 *value);

extern int64	atomic_set64(vint64 *value, int64 newValue);
extern int64	atomic_test_and_set64(vint64 *value, int64 newValue, int64 testAgainst);
extern int64	atomic_add64(vint64 *value, int64 addValue);
extern int64	atomic_and64(vint64 *value, int64 andValue);
extern int64	atomic_or64(vint64 *value, int64 orValue);	
extern int64	atomic_get64(vint64 *value);

/* Other stuff */
extern void*	get_stack_frame(void);

#ifdef __cplusplus
}
#endif

/* Obsolete or discouraged API */

/* use 'true' and 'false' */
#ifndef FALSE
#	define FALSE	0
#endif
#ifndef TRUE
#	define TRUE		1
#endif

#endif	/* _SUPPORT_DEFS_H */
