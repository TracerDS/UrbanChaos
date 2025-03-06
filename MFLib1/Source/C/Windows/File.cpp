// File.cpp
// Guy Simmons, 10th February 1997.

#include <MFHeader.h>

//---------------------------------------------------------------

bool FileExists(char* file_name) {
    if (GetFileAttributes(file_name) == 0xffffffff)
        return false;
    else
        return true;
}

//---------------------------------------------------------------

MFFileHandle FileOpen(char* file_name) {
    MFFileHandle result = FILE_OPEN_ERROR;

    if (FileExists(file_name)) {
        result = CreateFile(
            file_name,
            (GENERIC_READ | GENERIC_WRITE),
            (FILE_SHARE_READ | FILE_SHARE_WRITE),
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr);
        if (result == INVALID_HANDLE_VALUE)
            result = FILE_OPEN_ERROR;
    }
    return result;
}

//---------------------------------------------------------------

void FileClose(MFFileHandle file_handle) {
    CloseHandle(file_handle);
}

//---------------------------------------------------------------

MFFileHandle FileCreate(char* file_name, bool overwrite) {
    DWORD creation_mode;
    MFFileHandle result;

    if (overwrite) {
        creation_mode = CREATE_ALWAYS;
    } else {
        creation_mode = CREATE_NEW;
    }
    result = CreateFile(
        file_name,
        (GENERIC_READ | GENERIC_WRITE),
        (FILE_SHARE_READ | FILE_SHARE_WRITE),
        nullptr,
        creation_mode,
        0,
        nullptr);
    if (result == INVALID_HANDLE_VALUE)
        result = FILE_CREATION_ERROR;

    return result;
}

//---------------------------------------------------------------

void FileDelete(char* file_name) {
    DeleteFile(file_name);
}

//---------------------------------------------------------------

std::int32_t FileSize(MFFileHandle file_handle) {
    DWORD result;

    result = GetFileSize(file_handle, nullptr);
    if (result == 0xffffffff)
        return FILE_SIZE_ERROR;
    else
        return (std::int32_t) result;
}

//---------------------------------------------------------------

std::int32_t FileRead(MFFileHandle file_handle, void* buffer, std::uint32_t size) {
    std::int32_t bytes_read;

    if (ReadFile(file_handle, buffer, size, (LPDWORD) &bytes_read, nullptr) == false)
        return FILE_READ_ERROR;
    else
        return bytes_read;
}

//---------------------------------------------------------------

std::int32_t FileWrite(MFFileHandle file_handle, void* buffer, std::uint32_t size) {
    std::int32_t bytes_written;

    if (WriteFile(file_handle, buffer, size, (LPDWORD) &bytes_written, nullptr) == false)
        return FILE_WRITE_ERROR;
    else
        return bytes_written;
}

//---------------------------------------------------------------

std::int32_t FileSeek(MFFileHandle file_handle, enum SeekModes mode, std::int32_t offset) {
    DWORD method;

    switch (mode) {
        case SEEK_MODE_BEGINNING:
            method = FILE_BEGIN;
            break;
        case SEEK_MODE_CURRENT:
            method = FILE_CURRENT;
            break;
        case SEEK_MODE_END:
            method = FILE_END;
            break;
    }
    if (SetFilePointer(file_handle, offset, nullptr, method) == 0xffffffff)
        return FILE_SEEK_ERROR;
    else
        return 0;
}

//---------------------------------------------------------------

std::int32_t FileLoadAt(char* file_name, void* buffer) {
    std::int32_t size;
    MFFileHandle handle;

    handle = FileOpen(file_name);
    if (handle != FILE_OPEN_ERROR) {
        size = FileSize(handle);
        if (size > 0) {
            if (FileRead(handle, buffer, size) == size) {
                FileClose(handle);
                return size;
            }
        }
        FileClose(handle);
    }
    return FILE_LOAD_AT_ERROR;
}

//---------------------------------------------------------------
