// DFile.cpp
// Guy Simmons, 10th February 1997.

#include	<MFHeader.h>


//---------------------------------------------------------------

bool FileExists(char* file_name)
{
	if(_access(file_name,0)==-1)
		return	false;
	else
		return	true;
}

//---------------------------------------------------------------

MFFileHandle FileOpen(char* file_name)
{
	MFFileHandle	result	=	FILE_OPEN_ERROR;


	if(FileExists(file_name))
	{
		result	=	sopen	(
								file_name,
								O_RDWR|O_BINARY,
								SH_DENYNO
							);
		if(result==-1)
			result	=	FILE_OPEN_ERROR;
	}
	return	result;
}

//---------------------------------------------------------------

void FileClose(MFFileHandle file_handle)
{
	close(file_handle);
}

//---------------------------------------------------------------

MFFileHandle FileCreate(char* file_name,bool overwrite)
{
	MFFileHandle	result;


	if(overwrite==false && FileExists(file_name))
	{
		return	result	=	FILE_CREATION_ERROR;
	}
	else
	{
		result	=	sopen	(
								file_name,
								O_CREAT|O_TRUNC|O_RDWR|O_BINARY,
								SH_DENYNO,
								S_IREAD|S_IWRITE
							);
		if(result==-1)
			result	=	FILE_CREATION_ERROR;
	}
	return	result;
}

//---------------------------------------------------------------

void FileDelete(char* file_name)
{
	remove(file_name);
}

//---------------------------------------------------------------

std::int32_t FileSize(MFFileHandle file_handle)
{
	std::int32_t		result;


	result	=	filelength(file_handle);
	if(result==-1L)
		return	FILE_SIZE_ERROR;
	else
		return	result;
}

//---------------------------------------------------------------

std::int32_t FileRead(MFFileHandle file_handle,void* buffer,std::uint32_t size)
{
	std::int32_t	bytes_read;


	bytes_read	=	read(file_handle,buffer,size);
	if(bytes_read<0)
		return	FILE_READ_ERROR;
	else
		return	bytes_read;
}

//---------------------------------------------------------------

std::int32_t FileWrite(MFFileHandle file_handle,void* buffer,std::uint32_t size)
{
	std::int32_t	bytes_written;


	bytes_written	=	write(file_handle,buffer,size);
	if(bytes_written<0)
		return	FILE_WRITE_ERROR;
	else
		return	bytes_written;
}

//---------------------------------------------------------------

std::int32_t FileSeek(MFFileHandle file_handle,enum SeekModes mode,std::int32_t offset)
{
	int		method;


	switch(mode)
	{
		case	SEEK_MODE_BEGINNING:
			method	=	SEEK_SET;
			break;
		case	SEEK_MODE_CURRENT:
			method	=	SEEK_CUR;
			break;
		case	SEEK_MODE_END:
			method	=	SEEK_END;
			break;
	}
	if(lseek(file_handle,offset,method)==-1L)
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
