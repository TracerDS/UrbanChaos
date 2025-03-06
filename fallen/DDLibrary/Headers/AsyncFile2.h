// AsyncFile2
//
// asynchronous file loading for Win95

// InitAsyncFile
//
// initialize

void InitAsyncFile();

// TermAsyncFile
//
// terminate

void TermAsyncFile();

// LoadAsyncFile
//
// load <filename> into buffer

bool LoadAsyncFile(char* filename, void* buffer, DWORD blen, void* key);

// GetNextCompletedAsyncFile
//
// return key of next completed file, else nullptr

void* GetNextCompletedAsyncFile();

// CancelAsyncFile
//
// cancel async file loading - if nullptr, cancels all files

void CancelAsyncFile(void* key);

// AsyncFile
//
// control block

struct AsyncFile {
    HANDLE hFile;         // file handle
    std::uint8_t* buffer; // buffer for data
    int blen;             // amount to read
    void* hKey;           // user key
    AsyncFile* prev;      // previous in chain
    AsyncFile* next;      // next in chain
};

#define MAX_ASYNC_FILES 16
