// StdFile.h
// Guy Simmons, 18th December 1997.

#pragma once

//---------------------------------------------------------------

typedef HANDLE		MFFileHandle;

#define	FILE_OPEN_ERROR			((MFFileHandle)-100)
#define	FILE_CLOSE_ERROR		((MFFileHandle)-101)
#define	FILE_CREATION_ERROR		((MFFileHandle)-102)
#define	FILE_SIZE_ERROR			((std::int32_t)-103)
#define	FILE_READ_ERROR			((std::int32_t)-104)
#define	FILE_WRITE_ERROR		((std::int32_t)-105)
#define	FILE_SEEK_ERROR			((std::int32_t)-106)
#define	FILE_LOAD_AT_ERROR		((std::int32_t)-107)

#define	SEEK_MODE_BEGINNING		0
#define	SEEK_MODE_CURRENT		1
#define	SEEK_MODE_END			2

bool FileExists(char* file_name);
MFFileHandle FileOpen(char* file_name);
void FileClose(MFFileHandle file_handle);
MFFileHandle FileCreate(char* file_name,bool overwrite);
void FileDelete(char* file_name);
std::int32_t FileSize(MFFileHandle file_handle);
std::int32_t FileRead(MFFileHandle file_handle,void* buffer,std::uint32_t size);
std::int32_t FileWrite(MFFileHandle file_handle,void* buffer,std::uint32_t size);
std::int32_t FileSeek(MFFileHandle file_handle,const int mode,std::int32_t offset);
std::int32_t FileLoadAt(char* file_name,void* buffer);
void FileSetBasePath(char* path_name);



// Plug'n'play replacements for fopen/fclose.
// Except they do something cunning on the DC.
// These can be used with sscanf() and so on,
// whereas the above can't.
FILE *MF_Fopen ( const char *file_name, const char *mode );
int MF_Fclose( FILE *stream );