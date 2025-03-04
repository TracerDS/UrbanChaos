// FileClump.h
//
// File clump class - contains multiple files

#pragma once

// FileClump
//
// a bunch of files stored in a metafile

class FileClump
{
public:
	FileClump(const char* clumpfn, std::uint32_t max_id, bool readonly);
	~FileClump();

	bool	Exists(std::uint32_t id);								// sees if a file exists

	std::uint8_t*	Read(std::uint32_t id);									// read a whole file
	bool	Write(void* buffer, size_t nbytes, std::uint32_t id);	// write a whole file

private:
	FILE*			ClumpFD;	// FILE* for the clump, may be nullptr if the open failed
	std::uint32_t			MaxID;		// maximum ID
	size_t*			Offsets;	// MaxID offsets
	size_t*			Lengths;	// MaxID lengths
	size_t			NextOffset;	// next offset for writing
	bool			ReadOnly;	// read-only flag
};

