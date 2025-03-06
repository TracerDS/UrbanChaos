// MFFile.h
// Guy Simmons, 10th February 1997.

#ifndef _MF_FILE_H_
#define _MF_FILE_H_

#ifdef _MF_WINDOWS
typedef HANDLE MFFileHandle;
#else
typedef std::int32_t MFFileHandle;
#endif

#define FILE_OPEN_ERROR ((MFFileHandle) FileOpenError)
#define FILE_CLOSE_ERROR ((MFFileHandle) FileCloseError)
#define FILE_CREATION_ERROR ((MFFileHandle) FileCreationError)
#define FILE_SIZE_ERROR ((std::int32_t) FileSizeError)
#define FILE_READ_ERROR ((std::int32_t) FileReadError)
#define FILE_WRITE_ERROR ((std::int32_t) FileWriteError)
#define FILE_SEEK_ERROR ((std::int32_t) FileSeekError)
#define FILE_LOAD_AT_ERROR ((std::int32_t) FileLoadAtError)

enum SeekModes {
    SEEK_MODE_BEGINNING,
    SEEK_MODE_CURRENT,
    SEEK_MODE_END
};

bool FileExists(char* file_name);
MFFileHandle FileOpen(char* file_name);
void FileClose(MFFileHandle file_handle);
MFFileHandle FileCreate(char* file_name, bool overwrite);
void FileDelete(char* file_name);
std::int32_t FileSize(MFFileHandle file_handle);
std::int32_t FileRead(MFFileHandle file_handle, void* buffer, std::uint32_t size);
std::int32_t FileWrite(MFFileHandle file_handle, void* buffer, std::uint32_t size);
std::int32_t FileSeek(MFFileHandle file_handle, enum SeekModes mode, std::int32_t offset);
std::int32_t FileLoadAt(char* file_name, void* buffer);

#endif
