//	MFTypes.h
//	Guy Simmons, 6th October 1996.

// Type definitions for 'Mucky Foot' Libraries.


#ifndef _MF_TYPES_H_
#define _MF_TYPES_H_

#define	true				1
#define	false				0

#ifdef	_MF_DOSX
typedef	unsigned long		bool;
#endif

typedef	unsigned char		UBYTE;
typedef signed char			SBYTE;
typedef char				CBYTE;
typedef unsigned short		UWORD;
typedef signed short		SWORD;
typedef unsigned long		ULONG;
typedef signed long			SLONG;


#endif
