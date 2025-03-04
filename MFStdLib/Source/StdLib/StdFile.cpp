// StdFile.cpp
// Guy Simmons, 18th December 1997.

#include	<MFStdLib.h>

#define MAX_LENGTH_OF_BASE_PATH 64
char cBasePath[MAX_LENGTH_OF_BASE_PATH+1];

#ifdef DEBUG
int m_iNumOpenFiles_FileOpen = 0;
int m_iNumOpenFiles_MF_Fopen = 0;
char pcPrevFilenameOpened[256];
#endif

//---------------------------------------------------------------

#define MAX_LENGTH_OF_FULL_NAME (MAX_LENGTH_OF_BASE_PATH+128)
char cTempFilename[MAX_LENGTH_OF_FULL_NAME+1];

char* MakeFullPathName ( const char* cFilename )
{
	strcpy ( cTempFilename, cBasePath );
	ASSERT ( strlen ( cFilename ) < ( MAX_LENGTH_OF_FULL_NAME - MAX_LENGTH_OF_BASE_PATH ) );
	strcat ( cTempFilename, cFilename );
	return ( cTempFilename );
}

//---------------------------------------------------------------

bool FileExists( char* file_name)
{
	file_name = MakeFullPathName ( file_name );

	if(GetFileAttributes(file_name)==0xffffffff)
		return	false;
	else
		return	true;
}

//---------------------------------------------------------------

MFFileHandle FileOpen(char* file_name)
{
	if (!FileExists(file_name))
		return FILE_OPEN_ERROR;

	file_name = MakeFullPathName ( file_name );

	MFFileHandle result;
	
    result = CreateFile (
		file_name,
		(GENERIC_READ),
		(FILE_SHARE_READ),
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (result == INVALID_HANDLE_VALUE)
		return FILE_OPEN_ERROR;

#ifdef DEBUG
	m_iNumOpenFiles_FileOpen++;
	if ( m_iNumOpenFiles_FileOpen > 1 )
	{
		TRACE ( "FileOpen nested %i\n", m_iNumOpenFiles_FileOpen );
	}
	strncpy ( pcPrevFilenameOpened, file_name, 256 );
#endif

	return result;
}

//---------------------------------------------------------------

void FileClose(MFFileHandle file_handle)
{
#ifdef DEBUG
	if ( m_iNumOpenFiles_FileOpen > 1 )
	{
		TRACE ( "FileClose nested %i\n", m_iNumOpenFiles_FileOpen );
	}
	m_iNumOpenFiles_FileOpen--;
#endif
	CloseHandle(file_handle);
}

//---------------------------------------------------------------

MFFileHandle FileCreate(char* file_name,bool overwrite)
{
	DWORD			creation_mode;
	MFFileHandle	result;

	file_name = MakeFullPathName ( file_name );

	if(overwrite)
	{
		creation_mode	=	CREATE_ALWAYS;
	}
	else
	{
		creation_mode	=	CREATE_NEW;
	}
	result	=	CreateFile	(
								file_name,
								(GENERIC_READ|GENERIC_WRITE),
								0,//(FILE_SHARE_READ|FILE_SHARE_WRITE),
								nullptr,
								creation_mode,
								FILE_ATTRIBUTE_NORMAL,
								nullptr
	                   		);
	if(result==INVALID_HANDLE_VALUE)
		result	=	FILE_CREATION_ERROR;

	return	result;
}

//---------------------------------------------------------------

void FileDelete(char* file_name)
{
	file_name = MakeFullPathName ( file_name );
	DeleteFile(file_name);
}

//---------------------------------------------------------------

std::int32_t FileSize(MFFileHandle file_handle)
{
	DWORD	result;


	result	=	GetFileSize(file_handle,nullptr);
	if(result==0xffffffff)
		return	FILE_SIZE_ERROR;
	else
		return	(std::int32_t)result;
}

//---------------------------------------------------------------

std::int32_t FileRead(MFFileHandle file_handle,void* buffer,std::uint32_t size)
{
	std::int32_t	bytes_read;


	if(ReadFile(file_handle,buffer,size,(LPDWORD)&bytes_read,nullptr)==false)
		return	FILE_READ_ERROR;
	else
		return	bytes_read;
}

//---------------------------------------------------------------

std::int32_t FileWrite(MFFileHandle file_handle,void* buffer,std::uint32_t size)
{
	std::int32_t	bytes_written;


	if(WriteFile(file_handle,buffer,size,(LPDWORD)&bytes_written,nullptr)==false)
		return	FILE_WRITE_ERROR;
	else
		return	bytes_written;
}

//---------------------------------------------------------------

std::int32_t FileSeek(MFFileHandle file_handle,const int mode,std::int32_t offset)
{
	DWORD		method;


	switch(mode)
	{
		case	SEEK_MODE_BEGINNING:
			method	=	FILE_BEGIN;
			break;
		case	SEEK_MODE_CURRENT:
			method	=	FILE_CURRENT;
			break;
		case	SEEK_MODE_END:
			method	=	FILE_END;
			break;
	}
	if(SetFilePointer(file_handle,offset,nullptr,method)==0xffffffff)
		return	FILE_SEEK_ERROR;
	else
		return	0;
}

//---------------------------------------------------------------

std::int32_t FileLoadAt(char* file_name,void* buffer)
{
	std::int32_t			size;
	MFFileHandle	handle;

	handle	=	FileOpen(file_name);
	if(handle!=FILE_OPEN_ERROR)
	{
		size	=	FileSize(handle);
		if(size>0)
		{
			if(FileRead(handle,buffer,size)==size)
			{
				FileClose(handle);
				return	size;
			}
		}
		FileClose(handle);
	}
	return	FILE_LOAD_AT_ERROR;
}

//---------------------------------------------------------------


void TraceText(const char* fmt, ...)
{
	//
	// Work out the real message.
	//

	char   message[512];
	va_list	ap;

	va_start(ap, fmt);
	vsprintf(message, fmt, ap);
	va_end  (ap);

	OutputDebugString(message);
}

//---------------------------------------------------------------

void FileSetBasePath(char* path_name)
{
	ASSERT ( strlen ( path_name ) < MAX_LENGTH_OF_BASE_PATH - 1 );
	strncpy ( cBasePath, path_name, MAX_LENGTH_OF_BASE_PATH - 1 );
	// Add a trailing slash if need be.
	char* pch = cBasePath;
	if ( *pch != '\0' )
	{
		while ( *++pch != '\0' ){}
		pch--;
		if ( *pch != '\\' )
		{
			*pch++ = '\\';
			*pch = '\0';
		}
	}
}

//---------------------------------------------------------------

// Do NOT mix and match MF_FOpen/MF_FClose with the above - they don't mingle.
#ifdef TARGET_DC
static TCHAR pchTcharVersion[100];
//static TCHAR pchTcharMode[5];
#endif
FILE *MF_Fopen ( const char *file_name, const char *mode )
{
	if ( !FileExists ( (char *)file_name ) )
	{
		return nullptr;
	}
	file_name = MakeFullPathName ( file_name );
#ifdef TARGET_DC

	// Apparently fopen causes a memory leak.

#if 1
	ASSERT ( mode[0] == 'r' );
	ASSERT ( mode[2] == '\0' );
	TCHAR *pchTcharMode;
	if ( mode[1] == 't' )
	{
		pchTcharMode = TEXT("rt");
	}
	else
	{
		ASSERT ( mode[1] == 'b' );
		pchTcharMode = TEXT("rb");
	}
#else
	pc1 = mode;
	ASSERT ( strlen ( mode) < 3 );
	pc2 = pchTcharMode;
	while ( *pc1 != '\0' )
	{
		*pc2++ = (TCHAR)( *pc1++ );
	}
	*pc2++ = TEXT('\0');
#endif

	const char *pc1 = file_name;
	ASSERT ( strlen ( file_name) < 80 );
	TCHAR *pc2 = pchTcharVersion;
	while ( *pc1 != '\0' )
	{
		*pc2++ = (TCHAR)( *pc1++ );
	}
	*pc2++ = TEXT('\0');

	FILE *res = _wfopen ( pchTcharVersion, pchTcharMode );
#ifdef DEBUG
	if ( res != nullptr )
	{
		m_iNumOpenFiles_MF_Fopen++;
		if ( m_iNumOpenFiles_MF_Fopen > 1 )
		{
			TRACE ( "MF_Fopen nested %i\n", m_iNumOpenFiles_MF_Fopen );
		}
		strncpy ( pcPrevFilenameOpened, file_name, 256 );
	}
#endif
	return ( res );
#else
	return ( fopen ( (char *)file_name, (char *)mode ) );
#endif
}

//---------------------------------------------------------------

int MF_Fclose( FILE *stream )
{
#ifdef DEBUG
	if ( m_iNumOpenFiles_MF_Fopen > 1 )
	{
		TRACE ( "MF_Fclose nested %i\n", m_iNumOpenFiles_MF_Fopen );
	}
	m_iNumOpenFiles_MF_Fopen--;
#endif
	return ( fclose ( stream ) );
}

//---------------------------------------------------------------



