#ifndef	_SCRFLC_H_
#define	_SCRFLC_H_

//**************************************|************************************

struct	FLCFileHeader
{
	std::uint32_t								Size;
	std::uint16_t								Magic;
	std::uint16_t								NumberOfFrames;
	std::uint16_t								Width;
	std::uint16_t								Height;
	std::uint16_t								Depth;
	std::uint16_t								Flags;
	std::uint32_t								Speed;
	std::uint16_t								Reserved_0;
	std::uint32_t								Created;
	std::uint32_t								Creator;
	std::uint32_t								Updated;
	std::uint32_t								Updater;
	std::uint16_t								AspectX;
	std::uint16_t								AspectY;
	std::uint8_t								Reserved_1[38];
	std::uint32_t								OFrame1;
	std::uint32_t								OFrame2;
	std::uint8_t								Reserved_2[40];
};

//**************************************|************************************

struct	FLCPrefixChunk
{
	std::uint32_t								Size;
	std::uint16_t								Type;
};

//**************************************|************************************

struct	FLCFrameChunk
{
	std::uint32_t								Size;
	std::uint16_t								Type;
	std::uint16_t								Chunks;
	std::uint8_t								Reserved_0[8];
};

//**************************************|************************************

struct	FLCFrameDataChunk
{
	std::uint32_t								Size;
	std::uint16_t								Type;
};

//**************************************|************************************

struct	FLCPostageStamp
{
	std::uint32_t								Size;
	std::uint16_t								Type;
	std::uint16_t								Height;
	std::uint16_t								Width;
	std::uint16_t								XLate;
};

//**************************************|************************************

union	MultiPointer
{
	std::uint8_t								*UByte;
	std::uint16_t								*UWord;
	std::uint32_t								*ULong;
	std::int8_t								*SByte;
	std::int16_t								*SWord;
	std::int32_t								*SLong;
	struct	FLCFileHeader				*FLCFileHeader;
	struct	FLCPrefixChunk				*FLCPrefixChunk;
	struct	FLCFrameChunk				*FLCFrameChunk;
	struct	FLCFrameDataChunk			*FLCFrameDataChunk;
	struct	FLCPostageStamp				*FLCPostageStamp;
};

//**************************************|************************************

struct	Animation
{
	std::int32_t								PlaybackMode;
	std::uint8_t								*LastFrame;
	std::uint8_t								*NextFrameBuffer;
	union	MultiPointer				NextFrameBufferPointer;
	MFFileHandle						PlayFileHandle;
	MFFileHandle						RecordFileHandle;
	std::int16_t								Xpos;
	std::int16_t								Ypos;
	std::uint8_t								Palette[256 * 3];
	std::int32_t								FrameNumber;
	std::int32_t								FrameSizeMaximum;
	std::int32_t								Active;
	struct	FLCFileHeader				FLCFileHeader;
	struct	FLCPrefixChunk				FLCPrefixChunk;
	struct	FLCFrameChunk				FLCFrameChunk;
	struct	FLCFrameDataChunk			FLCFrameDataChunk;
	struct	FLCPostageStamp				FLCPostageStamp;
};

//**************************************|************************************

#define		FLI_COLOUR256				4
#define		FLI_SS2						7
#define		FLI_COLOUR					11
#define		FLI_LC						12
#define		FLI_BLACK					13
#define		FLI_BRUN					15
#define		FLI_COPY					16
#define		FLI_PSTAMP					18

//**************************************|************************************

#define		PLAYBACK_MODE_STOPPED		0
#define		PLAYBACK_MODE_RECORD		(1 << 0)
#define		PLAYBACK_MODE_PLAY			(1 << 1)

//**************************************|************************************
extern std::int32_t	anim_stop();
extern std::int32_t	anim_record();

extern std::int32_t	anim_open(std::int8_t *file_name, std::int16_t xpos, std::int16_t ypos, std::int16_t width, std::int16_t height, std::int8_t *postage_stamp, std::int32_t	playback );
extern std::int32_t	anim_close(std::int32_t playback);
extern std::int32_t	anim_make_next_frame(std::uint8_t *WScreen, std::uint8_t *palette );
extern std::int32_t	anim_make_FLI_PSTAMP();
extern std::int32_t	anim_make_FLI_COLOUR256(std::uint8_t *palette);
extern std::int32_t	anim_make_FLI_COLOUR(std::uint8_t *palette);
extern std::int32_t	anim_make_FLI_SS2(std::uint8_t *wscreen, std::uint8_t *last_screen);
extern std::int32_t	anim_make_FLI_LC(std::uint8_t *wscreen, std::uint8_t *last_screen);
extern std::int32_t	anim_make_FLI_BLACK(std::uint8_t *wscreen );
extern std::int32_t	anim_make_FLI_BRUN(std::uint8_t *wscreen );
extern std::int32_t	anim_make_FLI_COPY(std::uint8_t *wscreen );
extern std::int32_t	anim_write_data(std::uint8_t *data, std::int32_t size);
extern std::int32_t	anim_store_data(std::uint8_t *data, std::int32_t size);
extern std::int32_t	anim_show_next_frame();
extern std::int32_t	anim_show_FLI_PSTAMP();
extern std::int32_t	anim_show_FLI_COLOUR256();
extern std::int32_t	anim_show_FLI_COLOUR();
extern std::int32_t	anim_show_FLI_SS2();
extern std::int32_t	anim_show_FLI_LC();
extern std::int32_t	anim_show_FLI_BLACK();
extern std::int32_t	anim_show_FLI_BRUN();
extern std::int32_t	anim_show_FLI_COPY();
extern std::int32_t	anim_read_data(std::uint8_t *data, std::int32_t size);

//**************************************|************************************
#endif
