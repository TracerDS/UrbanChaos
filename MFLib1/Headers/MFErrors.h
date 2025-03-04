// Errors.h
// Guy Simmons, 6th October 1996.

#pragma once

#ifdef	_DEBUG

#else
	
#endif

extern int MFMessage(const char *pMessage, const char *pFile, std::uint32_t dwLine);

#define	ERROR_MSG(e,m)		{ if(!e) MFMessage(m,__FILE__,__LINE__); }
#ifndef ASSERT
#define	ASSERT(e)			ERROR_MSG(e,"Assert failed");
#endif

// Library Errors.
enum
{
	NoError					=	0
};


// Display Errors.
enum
{
	DisplayCreationError	=	-100,
	DisplayShutdownError	=	-101
};

// File Errors.
enum
{
	FileOpenError			=	-1,
	FileCloseError			=	-2,
	FileCreationError		=	-3,
	FileSizeError			=	-4,
	FileReadError			=	-5,
	FileWriteError			=	-6,
	FileSeekError			=	-7,
	FileLoadAtError			=	-8
};
