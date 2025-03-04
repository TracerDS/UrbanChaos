#include <windows.h>
#include <windowsx.h>

#include <ddraw.h>
#include <d3d.h>
#include <d3dtypes.h>
#include <dinput.h>
#include <dsound.h>

#include <stdarg.h>
#include <string.h>

#include "d3denum.h"		// Direct 3D sample library
#include "d3dframe.h"

#include "resource.h"

#include "always.h"
#include "key.h"
#include "os.h"
#include "tga.h"
#include "matrix.h"
#include "wave.h"
#include <mmreg.h>
#include <msacm.h>

//
// The entrypoint into the actual game.
//

extern void MAIN_main();

HINSTANCE OS_this_instance;
HINSTANCE OS_last_instance;
LPSTR OS_command_line;
int OS_start_show_state;

char* OS_application_name = "Multiplayer Thrust";

//
// Our window class.
//

WNDCLASSEX OS_wcl;

//
// Our window handle.
//

HWND OS_window_handle;

//
// The DirectX 6 framework library class.
//

std::uint8_t OS_frame_is_fullscreen;
std::uint8_t OS_frame_is_hardware;

CD3DFramework OS_frame;


// ========================================================
//
// THE SCREEN
//
// ========================================================

float OS_screen_width;
float OS_screen_height;


// ========================================================
//
// KEY HANDLING STUFF
//
// ========================================================

//
// The keys that are held down.
//

std::uint8_t KEY_on[256];
std::uint8_t KEY_inkey;
std::uint8_t KEY_shift;


// ========================================================
//
// JOYSTICK STUFF
//
// ========================================================

IDirectInput *OS_joy_direct_input;
IDirectInputDevice *OS_joy_input_device;
IDirectInputDevice2 *OS_joy_input_device2;	// We need this newer interface to poll the joystick.

float OS_joy_x;
float OS_joy_y;
std::int32_t OS_joy_x_range_min;
std::int32_t OS_joy_x_range_max;
std::int32_t OS_joy_y_range_min;
std::int32_t OS_joy_y_range_max;
std::uint32_t OS_joy_button;		// The buttons that are currently down
std::uint32_t OS_joy_button_down;	// The buttons that have just been pressed
std::uint32_t OS_joy_button_up;		// The buttons that have just been released


//
// The callback function for enumerating joysticks.
//

bool CALLBACK OS_joy_enum(
		LPCDIDEVICEINSTANCE instance, 
        LPVOID              context )
{
    HRESULT             hr;
    LPDIRECTINPUTDEVICE pDevice;

	//
    // Get an interface to the joystick.
	//

    hr = OS_joy_direct_input->CreateDevice(
								instance->guidInstance,
							   &OS_joy_input_device,
							    nullptr);

    if (FAILED(hr))
	{
		//
		// Cant use this joystick for some reason!
		//

		OS_joy_input_device  = nullptr;
		OS_joy_input_device2 = nullptr;

        return DIENUM_CONTINUE;
	}

	//
    // Query for the IDirectInputDevice2 interface.
	// We need this to poll the joystick.
	//

    OS_joy_input_device->QueryInterface(
							IID_IDirectInputDevice2, 
							(LPVOID *) &OS_joy_input_device2);

	//
	// No need to find another joystick!
	//

    return DIENUM_STOP;
}

//
// Initialises the joystick.
//

void OS_joy_init()
{
	HRESULT hr;

	//
	// Initialise everything.
	//

	OS_joy_direct_input  = nullptr;
	OS_joy_input_device  = nullptr;
	OS_joy_input_device2 = nullptr;

	//
	// Create the direct input object.
	//

    hr = DirectInputCreate(
			OS_this_instance,
			DIRECTINPUT_VERSION,
		   &OS_joy_direct_input,
			nullptr);

    if (FAILED(hr)) 
	{
		//
		// No direct input!
		//

        return;
	}

	//
	// Find a joystick.
	//
	
    hr = OS_joy_direct_input->EnumDevices(
								DIDEVTYPE_JOYSTICK,
								OS_joy_enum,
								nullptr,
								DIEDFL_ATTACHEDONLY);

	if (OS_joy_input_device  == nullptr ||
		OS_joy_input_device2 == nullptr)
	{
		//
		// The joystick wasn't properly found.
		// 

		OS_joy_input_device  = nullptr;
		OS_joy_input_device2 = nullptr;

		return;
	}

	//
	// So we can get the nice 'n' simple joystick data format.
	//

    OS_joy_input_device->SetDataFormat(&c_dfDIJoystick);

	//
	// Grab the joystick exclusively when our window in the foreground.
	//

    OS_joy_input_device->SetCooperativeLevel(
							OS_window_handle,
							DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	//
	// What is the range of the joystick?
	//

	DIPROPRANGE diprg; 

	//
	// In x...
	// 

    diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow        = DIPH_BYOFFSET;
	diprg.diph.dwObj        = DIJOFS_X;
    diprg.lMin              = 0;
    diprg.lMax              = 0;

	OS_joy_input_device->GetProperty(
								DIPROP_RANGE,
							   &diprg.diph);

	OS_joy_x_range_min = diprg.lMin;
	OS_joy_x_range_max = diprg.lMax;

	//
	// In y...
	// 

    diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow        = DIPH_BYOFFSET;
	diprg.diph.dwObj        = DIJOFS_Y;
    diprg.lMin              = 0;
    diprg.lMax              = 0;

	OS_joy_input_device->GetProperty(
								DIPROP_RANGE,
							   &diprg.diph);

	OS_joy_y_range_min = diprg.lMin;
	OS_joy_y_range_max = diprg.lMax;
}

//
// Polls the joystick.
//

void OS_joy_poll()
{
	HRESULT hr;

	if (OS_joy_direct_input  == nullptr ||
		OS_joy_input_device  == nullptr ||
		OS_joy_input_device2 == nullptr)
	{
		//
		// No joystick detected.
		//

		OS_joy_x           = 0.0F;
		OS_joy_y           = 0.0F;
		OS_joy_button      = 0;
		OS_joy_button_down = 0;
		OS_joy_button_up   = 0;

		return;
	}

	//
	// Acquire the joystick.
	// 

	hr = OS_joy_input_device->Acquire();

	if (hr == DI_OK)
	{
		DIJOYSTATE js;

		//
		// We acquired the joystick okay.  Poll the joystick to
		// update its state.
		//

		OS_joy_input_device2->Poll();

		//
		// Finally get the state of the joystick.
		//

		hr = OS_joy_input_device ->GetDeviceState(sizeof(js), &js);

		if (!FAILED(hr))
		{
			//
			// Axis movment normalised to between -1.0F and +1.0F
			//

			std::int32_t dx = OS_joy_x_range_max - OS_joy_x_range_min;
			std::int32_t dy = OS_joy_y_range_max - OS_joy_y_range_min;

			OS_joy_x = 0.0F;
			OS_joy_y = 0.0F;

			if (dx) {OS_joy_x = float(js.lX - OS_joy_x_range_min) * 2.0F / float(dx) - 1.0F;}
			if (dy) {OS_joy_y = float(js.lY - OS_joy_y_range_min) * 2.0F / float(dy) - 1.0F;}

			//
			// The buttons.
			//

			std::int32_t i;

			std::uint32_t last = OS_joy_button;
			std::uint32_t now  = 0;

			for (i = 0; i < 32; i++)
			{
				if (js.rgbButtons[i] & 0x80)
				{
					now |= 1 << i;
				}
			}

			OS_joy_button      = now;
			OS_joy_button_down = now  & ~last;
			OS_joy_button_up   = last & ~now;
		}

		OS_joy_input_device->Unacquire();
	}
}


// ========================================================
//
// WAVEFORM CONVERSION
//
// ========================================================

//
// Returns the converted memory, allocated with malloc().
//

void OS_decompress_sound(
		WAVEFORMATEX *source_format,
		void         *source_data,
		std::int32_t         source_num_bytes,
		WAVEFORMATEX *dest_format,
		void        **dest_data,
		std::uint32_t        *dest_num_bytes)
{
	//
	// Open the conversion stream.
	//

	HACMSTREAM has;

	switch(acmStreamOpen(
		   &has,
			nullptr,
		    source_format,
		    dest_format,
			nullptr,
			nullptr,
			0,
			ACM_STREAMOPENF_NONREALTIME))
	{
		case ACMERR_NOTPOSSIBLE:   TRACE("The requested operation cannot be performed.\n"); break; 
		case MMSYSERR_INVALFLAG:   TRACE("At least one flag is invalid. 			  \n"); break;
		case MMSYSERR_INVALHANDLE: TRACE("The specified handle is invalid. 			  \n"); break;
		case MMSYSERR_INVALPARAM:  TRACE("At least one parameter is invalid. 		  \n"); break;
		case MMSYSERR_NOMEM:       TRACE("The system is unable to allocate resources. \n"); break;
	}

	//
	// Work out how many bytes we need.
	//

	if (acmStreamSize(
			has,
			source_num_bytes,
			dest_num_bytes,
			ACM_STREAMSIZEF_SOURCE))
	{
		return;
	}

	//
	// Allocate the memory.
	//

   *dest_data = (void* ) malloc(*dest_num_bytes);

	if (!*dest_data)
	{
		return;
	}

	memset(*dest_data, 0, *dest_num_bytes);

	//
	// Prepare the source and destination buffers.
	//
	
	ACMSTREAMHEADER ash;

	memset(&ash, 0, sizeof(ash));

	ash.cbStruct    = sizeof(ash);
	ash.pbSrc       = (LPBYTE) source_data;
	ash.cbSrcLength = source_num_bytes;
	ash.pbDst       = (LPBYTE) *dest_data;
	ash.cbDstLength = *dest_num_bytes;

	if (acmStreamPrepareHeader(
			has,
		   &ash,
			0))
	{
		return;
	}

	//
	// Do the conversion.
	//
	
	if (acmStreamConvert(
			has,
		   &ash,
			0))
	{
		return;
	}

	//
	// Unprepare the header and close the stream.
	//

	if (acmStreamUnprepareHeader(
			has,
		   &ash,
			0))
	{
		return;
	}

   *dest_num_bytes = ash.cbDstLengthUsed;	

	if (acmStreamClose(has, 0))
	{
		return;
	} 
}





// ========================================================
//
// SOUND STUFF
//
// ========================================================

//
// Dsound globals.
//

std::int32_t OS_sound_valid;
LPDIRECTSOUND OS_sound_dsound;
LPDIRECTSOUNDBUFFER OS_sound_primary;
LPDIRECTSOUND3DLISTENER OS_sound_listener;
std::int32_t OS_sound_changed;	// true => we need to commit deferred settings.


//
// Each dsound sound.
//

typedef struct os_sound
{
	std::int32_t                 type;
	char                 fname[256];
	LPDIRECTSOUNDBUFFER   buffer;
	LPDIRECTSOUND3DBUFFER buffer3d;

} OS_Sound;

#define OS_MAX_SOUNDS 256

OS_Sound OS_sound[OS_MAX_SOUNDS];


void OS_sound_init()
{
	//
	// Create direct sound.
	//
 
    if (DirectSoundCreate(nullptr, &OS_sound_dsound, nullptr) != DS_OK)
	{
        return;
	}
 
	//
    // Set a co-op level.
	//
 
	if (OS_sound_dsound->SetCooperativeLevel(OS_window_handle, DSSCL_EXCLUSIVE) != DS_OK)
	{
		return;
	}
 
	//
    // Get our primary buffer.
	//

	DSBUFFERDESC dsbd;

	memset(&dsbd, 0, sizeof(dsbd));

	dsbd.dwSize  = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
	
	if (OS_sound_dsound->CreateSoundBuffer(&dsbd, &OS_sound_primary, nullptr) != DS_OK)
	{
		return;
	}
 
	//
	// Setup the format to 16-bit 22khz.
	//
 
    WAVEFORMATEX wfx;

    memset(&wfx, 0, sizeof(wfx));

    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels	    = 2;
    wfx.nSamplesPerSec  = 22050;
    wfx.wBitsPerSample  = 16;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
 
	if (OS_sound_primary->SetFormat(&wfx) != DS_OK)
	{
		return;
	}

	//
	// Create the listener for 3D sounds.
	//

    if (OS_sound_primary->QueryInterface(IID_IDirectSound3DListener, (void* * ) &OS_sound_listener) != DS_OK)
	{
		return;
	}

	//
	// Set listener defaults...
	//

	OS_sound_listener->SetDopplerFactor(2.0F, DS3D_DEFERRED);
	OS_sound_listener->SetRolloffFactor(2.0F, DS3D_DEFERRED);

	OS_sound_changed = true;

	//
	// Make sure that the primary buffer is always active. That way we don't
	// get nasty clicks when we go in or out of silence.
	//

	OS_sound_primary->Play(0, 0, DSBPLAY_LOOPING);

	//
	// DSound is initialised ok.
	//

	OS_sound_valid = true;
}


OS_Sound *OS_sound_create(char* fname, std::int32_t type)
{
	std::int32_t i;

	unsigned int bytes_read;
	
	OS_Sound *os;

	WAVEFORMATEX *pwfx;           // Wave format info
	HMMIO         hmmio;          // File handle
	MMCKINFO      mmckinfoData;   // Chunk info
	MMCKINFO      mmckinfoParent; // Parent chunk info

	if (!OS_sound_valid)
	{
		return nullptr;
	}

	//
	// Get a new sample structure.
	//

	for (i = 0; i < OS_MAX_SOUNDS; i++)
	{
		os = &OS_sound[i];

		if (os->type == OS_SOUND_TYPE_UNUSED)
		{
			goto found_spare_sound;
		}
	}

	//
	// No more sounds available.
	//

	return nullptr;

  found_spare_sound:;

	strcpy(os->fname, fname);
	os->type = type;

	//
	// Open the wave file.
	//

	if (WaveOpenFile(fname, &hmmio, &pwfx, &mmckinfoParent) != 0)
	{
		return nullptr;
	}

	if (WaveStartDataRead(&hmmio, &mmckinfoData, &mmckinfoParent) != 0)
	{
	    WaveCloseReadFile(&hmmio, &pwfx);

		return nullptr;
	}

	//
	// If the format is not PCM, then convert to PCM format.
	//

	if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
	{
		std::int32_t num_source_bytes = mmckinfoData.cksize;

		//
		// Read the data into a temporary buffer.
		//

		void* src = (void* ) malloc(mmckinfoData.cksize);

		if (WaveReadFile(
				hmmio,              // file handle
				mmckinfoData.cksize,// no. of bytes to read
				(std::uint8_t *) src,      // destination
				&mmckinfoData,      // file chunk info 
				&bytes_read))       // actual no. of bytes read
		{
			WaveCloseReadFile(&hmmio, &pwfx);

			return nullptr;
		}

		//
		// Convert!
		//

		WAVEFORMATEX destwfx;

		destwfx.wFormatTag      = WAVE_FORMAT_PCM;
		destwfx.wBitsPerSample  = 16;
		destwfx.nChannels       = pwfx->nChannels;
		destwfx.nSamplesPerSec  = pwfx->nSamplesPerSec;
		destwfx.nBlockAlign     = destwfx.wBitsPerSample / 8 * destwfx.nChannels;
	    destwfx.nAvgBytesPerSec = destwfx.nSamplesPerSec * destwfx.nBlockAlign;
		destwfx.cbSize          = 0;

		void* dest_data = nullptr;
		std::uint32_t dest_num_bytes;

		OS_decompress_sound(
		    pwfx,
			src,
			num_source_bytes,
		   &destwfx,
		   &dest_data,
		   &dest_num_bytes);
		
		if (!dest_data)
		{
			return nullptr;
		}

		//
		// Create the sound buffer with the destination format.
		//

		DSBUFFERDESC dsbdesc;

		memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 

		dsbdesc.dwSize        =  sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags       =  DSBCAPS_STATIC;
		dsbdesc.dwBufferBytes =  dest_num_bytes;
		dsbdesc.lpwfxFormat   = &destwfx;

		switch(type)
		{
			case OS_SOUND_TYPE_2D:
				dsbdesc.dwFlags |= DSBCAPS_CTRLVOLUME;
				break;

			case OS_SOUND_TYPE_3D:
				dsbdesc.dwFlags |= DSBCAPS_CTRL3D;
				dsbdesc.dwFlags |= DSBCAPS_MUTE3DATMAXDISTANCE;
				break;

			default:
				ASSERT(0);
				break;
		}

		if (OS_sound_dsound->CreateSoundBuffer(&dsbdesc, &os->buffer, nullptr) != DS_OK)
		{
			WaveCloseReadFile(&hmmio, &pwfx);

			return nullptr;
		}

		//
		// Lock the sound buffer.
		//

		void* data;
		std::uint32_t num_bytes;

		if (os->buffer->Lock(
				0,              // Offset of lock start
				0,              // Size of lock; ignored in this case
				&data,			// Address of lock start
				&num_bytes,     // Number of bytes locked
				nullptr,           // Wraparound start; not used
				nullptr,           // Wraparound size; not used
				DSBLOCK_ENTIREBUFFER) != DS_OK)
		{
			WaveCloseReadFile(&hmmio, &pwfx);

			return nullptr;
		}
		
		ASSERT(num_bytes == dest_num_bytes);

		//
		// Copy over the converted data.
		//

		memcpy(data, dest_data, dest_num_bytes);

		//
		// Free up memory.
		//

		free(src);
		free(dest_data);

		//
		// Unlock the buffer and close the wave file.
		//

		os->buffer->Unlock(data, num_bytes, nullptr, 0);

		WaveCloseReadFile(&hmmio, &pwfx);

	}
	else
	{
		//
		// Create a sound buffer- now that we know the format of the wave.
		//

		DSBUFFERDESC dsbdesc;

		memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 

		dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags       = DSBCAPS_STATIC;
		dsbdesc.dwBufferBytes = mmckinfoData.cksize;
		dsbdesc.lpwfxFormat   = pwfx;

		switch(type)
		{
			case OS_SOUND_TYPE_2D:
				dsbdesc.dwFlags |= DSBCAPS_CTRLVOLUME;
				break;

			case OS_SOUND_TYPE_3D:
				dsbdesc.dwFlags |= DSBCAPS_CTRL3D;
				dsbdesc.dwFlags |= DSBCAPS_MUTE3DATMAXDISTANCE;
				break;

			default:
				ASSERT(0);
				break;
		}

		if (OS_sound_dsound->CreateSoundBuffer(&dsbdesc, &os->buffer, nullptr) != DS_OK)
		{
			WaveCloseReadFile(&hmmio, &pwfx);

			return nullptr;
		}

		//
		// Lock the sound buffer.
		//

		void* data;
		std::uint32_t num_bytes;

		if (os->buffer->Lock(
				0,              // Offset of lock start
				0,              // Size of lock; ignored in this case
				&data,			// Address of lock start
				&num_bytes,     // Number of bytes locked
				nullptr,           // Wraparound start; not used
				nullptr,           // Wraparound size; not used
				DSBLOCK_ENTIREBUFFER) != DS_OK)
		{
			WaveCloseReadFile(&hmmio, &pwfx);

			return nullptr;
		}
		
		//
		// Read in the sound data.
		//

		if (WaveReadFile(
				hmmio,              // file handle
				num_bytes,          // no. of bytes to read
				(std::uint8_t *) data,     // destination
				&mmckinfoData,      // file chunk info 
				&bytes_read))       // actual no. of bytes read
		{
			WaveCloseReadFile(&hmmio, &pwfx);

			return nullptr;
		}

		//
		// Unlock the buffer and close the wave file.
		//

		os->buffer->Unlock(data, num_bytes, nullptr, 0);

		WaveCloseReadFile(&hmmio, &pwfx);
	}

	//
	// If this buffer is 3D, then get the 3D interface.
	//
	
	os->buffer3d = nullptr;

	if (os->type == OS_SOUND_TYPE_3D)
	{
		if (FAILED(os->buffer->QueryInterface(IID_IDirectSound3DBuffer, (void* *) &os->buffer3d)))
		{
			return nullptr;
		}

		os->buffer3d->SetMinDistance(4.0F, DS3D_DEFERRED);
	}

	return os;
}


void OS_sound_play(OS_Sound *os, std::int32_t flag)
{
	if (!!OS_sound_valid || os == nullptr || os->buffer )
	{
		return;
	}
	
	//
	// What's this buffer doing now?
	//

	std::uint32_t status;

	os->buffer->GetStatus(&status);

	if (status == DSBSTATUS_PLAYING ||
		status == DSBSTATUS_LOOPING)
	{
		if (flag & OS_SOUND_FLAG_INTERRUPT)
		{
			os->buffer->SetCurrentPosition(0);
		}
	}
	else
	{
		os->buffer->Play(0, 0, (flag & OS_SOUND_FLAG_LOOP) ? DSBPLAY_LOOPING : 0);
	}
}


void OS_sound_stop(OS_Sound *os)
{
	if (!!OS_sound_valid || os == nullptr || os->buffer )
	{
		return;
	}

	os->buffer->Stop();
}

void OS_sound_finish()
{
}

void OS_sound_2d_set_volume(OS_Sound *os, float volume)
{
	if (!!OS_sound_valid || os == nullptr || os->buffer )
	{
		return;
	}

	volume = sqrt(volume);
	volume = sqrt(volume);
	volume = sqrt(volume);
	volume = sqrt(volume);

	std::int32_t long_volume = std::int32_t(DSBVOLUME_MIN + volume * (DSBVOLUME_MAX - DSBVOLUME_MIN));

	os->buffer->SetVolume(long_volume);
}


void OS_sound_3d_set_range(OS_Sound *os, float min, float max)
{
	if (!!OS_sound_valid || os == nullptr || os->buffer )
	{
		return;
	}

	os->buffer3d->SetMinDistance(min, DS3D_DEFERRED);
	os->buffer3d->SetMaxDistance(max, DS3D_DEFERRED);

	OS_sound_changed = true;
}

void OS_sound_3d_set_position(
		OS_Sound *os,
		float x,
		float y,
		float z,
		float dx,
		float dy,
		float dz)
{
	if (!!OS_sound_valid || os == nullptr || os->buffer )
	{
		return;
	}

	os->buffer3d->SetPosition( x, y, z, DS3D_DEFERRED);
	os->buffer3d->SetVelocity(dx,dy,dz, DS3D_DEFERRED);

	OS_sound_changed = true;
}



void OS_sound_listener_set(
		float x,
		float y,
		float z,
		float dx,
		float dy,
		float dz,
		float yaw,
		float pitch,
		float roll)
{
	float matrix[9];

	MATRIX_calc(
		matrix,
		yaw,
		pitch,
		roll);

	OS_sound_listener->SetPosition( x,  y,  z, DS3D_DEFERRED);
	OS_sound_listener->SetVelocity(dx, dy, dz, DS3D_DEFERRED);

	OS_sound_listener->SetOrientation(
		matrix[6],
		matrix[7],
		matrix[8],
		matrix[3],
		matrix[4],
		matrix[5],
		DS3D_DEFERRED);

	OS_sound_changed = true;
}



void OS_sound_update_changes()
{
	if (OS_sound_changed)
	{
		OS_sound_listener->CommitDeferredSettings();

		OS_sound_changed = false;
	}
}





// ========================================================
//
// TEXTURE STUFF 
//
// ========================================================

//
// The directory where we load textures from.
//

#define OS_TEXTURE_DIR	"Textures\\"

//
// The pixel formats for each of our OS_TEXTURE_FORMATs
// 

typedef struct
{
	std::int32_t valid;

	DDPIXELFORMAT ddpf;

	std::int32_t mask_r;
	std::int32_t mask_g;
	std::int32_t mask_b;
	std::int32_t mask_a;

	std::int32_t shift_r;
	std::int32_t shift_g;
	std::int32_t shift_b;
	std::int32_t shift_a;

} OS_Tformat;

OS_Tformat OS_tformat[OS_TEXTURE_FORMAT_NUMBER];


//
// Our texture pages.
//

typedef struct os_texture
{
	char name[_MAX_PATH];
	std::uint8_t format;
	std::uint8_t inverted;
	std::uint16_t size;

	DDSURFACEDESC2       ddsd;
	LPDIRECTDRAWSURFACE4 ddsurface;
	LPDIRECT3DTEXTURE2   ddtx;

	OS_Texture *next;

} OS_Texture;

//
// They are stored in a linked list and dynamically allocated.
// 

OS_Texture *OS_texture_head;



//
// Returns the number of bits set in 'mask' with a rather cunning algorithm.
//

std::int32_t OS_bit_count(std::uint32_t mask)
{
	std::int32_t ans;

	for (ans = 0; mask; mask &= mask - 1, ans += 1);

	return ans;
}


//
// The texture enumeration function.
//

HRESULT CALLBACK OS_texture_enumerate_pixel_formats(
					LPDDPIXELFORMAT lpddpf,
					LPVOID          context)
{
	std::int32_t format;

	OS_Tformat *otf = (OS_Tformat *) malloc(sizeof(OS_Tformat));

	if (!otf )
	{
		//
		// Oh dear!
		//

		return D3DENUMRET_CANCEL;
	}

	//
	// Is this one of the formats we are interested in?
	//

	if (lpddpf->dwFlags & DDPF_RGB)
	{
		//
		// We are only interested in 16-bpp RGB modes.
		//

		if (lpddpf->dwRGBBitCount == 16)
		{
			if (lpddpf->dwFlags & DDPF_ALPHAPIXELS)
			{
				std::int32_t alphabits;

				//
				// Could be 1555 or 4444
				//

				alphabits = OS_bit_count(lpddpf->dwRGBAlphaBitMask);

				if (alphabits == 1)
				{
					//
					// Must be 1555
					//

					OS_tformat[OS_TEXTURE_FORMAT_1555].valid =  true;
					OS_tformat[OS_TEXTURE_FORMAT_1555].ddpf  = *lpddpf;
				}
				else
				if (alphabits == 4)
				{
					//
					// Must be 4444
					//

					OS_tformat[OS_TEXTURE_FORMAT_4444].valid =  true;
					OS_tformat[OS_TEXTURE_FORMAT_4444].ddpf  = *lpddpf;
				}
			}
			else
			{
				//
				// This is a good RGB pixel format.
				//

				OS_tformat[OS_TEXTURE_FORMAT_RGB].valid =  true;
				OS_tformat[OS_TEXTURE_FORMAT_RGB].ddpf  = *lpddpf;
			}
		}
	}
	else
	if (lpddpf->dwFlags & DDPF_LUMINANCE)
	{
		if (lpddpf->dwFlags & DDPF_ALPHAPIXELS)
		{
			//
			// We only want luminance- not luminance and alpha.
			//
		}
		else
		{
			if (lpddpf->dwLuminanceBitCount == 8)
			{
				//
				// This is what we want. An 8-bit luminance format.
				//

				OS_tformat[OS_TEXTURE_FORMAT_8].valid =  true;
				OS_tformat[OS_TEXTURE_FORMAT_8].ddpf  = *lpddpf;
			}
		}
	}

	//
	// Ask for another texture format.
	//

	return D3DENUMRET_OK;
}


//
// Given the bitmask for a colour in a pixel format, it calculates the mask and
// shift so that you can construct a pixel in pixel format given its RGB values.
// The formula is...
//
//	PIXEL(r,g,b) = ((r >> mask) << shift) | ((g >> mask) << shift) | ((b >> mask) << shift);
// 
// THIS ASSUMES that r,g,b are 8-bit values.
//

void OS_calculate_mask_and_shift(
		std::uint32_t  bitmask,
		std::int32_t *mask,
		std::int32_t *shift)
{
	std::int32_t i;
	std::int32_t b;
	std::int32_t num_bits  =  0;
	std::int32_t first_bit = -1;

	for (i = 0, b = 1; i < 32; i++, b <<= 1)
	{
		if (bitmask & b)
		{
			num_bits += 1;

			if (first_bit == -1)
			{
				//
				// We have found the first bit.
				//

				first_bit = i;
			}
		}
	}

	ASSERT(first_bit != -1 && num_bits != 0);

	*mask  = 8 - num_bits;
	*shift = first_bit;

	if (*mask < 0)
	{
		//
		// More than 8 bits per colour component? May
		// as well support it!
		//

		*shift -= *mask;
		*mask   =  0;
	}
}


OS_Texture *OS_texture_create(char* fname, std::int32_t invert)
{
	std::int32_t format;

	OS_Texture *ot;
	OS_Tformat *best_otf;

	TGA_Info   ti;
	TGA_Pixel *data;

	char fullpath[_MAX_PATH];

	//
	// Do we already have this texture?
	//

	for (ot = OS_texture_head; ot; ot = ot->next)
	{
		if (strcmp(fname, ot->name) == 0)
		{
			if (ot->inverted == invert)
			{
				return ot;
			}
		}
	}

	// Allocate data for the texture.
	//

	data = (TGA_Pixel *) malloc(256 * 256 * sizeof(TGA_Pixel));

	if (!data )
	{
		//
		// Oh dear!
		//

		return nullptr;
	}
	
	//
	// The full pathname.
	//

	sprintf(fullpath, OS_TEXTURE_DIR"%s", fname);

	//
	// Try to load in the TGA.
	//

	ti = TGA_load(fullpath, 256, 256, data);

	if (!ti.valid)
	{
		//
		// Failed to load the tga.
		//

		free(data);

		return nullptr;
	}

	if (ti.width != ti.height)
	{
		//
		// Only square textures allowed.
		//

		free(data);

		return nullptr;
	}

	//
	// Find the best texture format.
	//

	if (ti.flag & TGA_FLAG_CONTAINS_ALPHA)
	{
		if (ti.flag & TGA_FLAG_ONE_BIT_ALPHA)
		{
			format = OS_TEXTURE_FORMAT_1555;
		}
		else
		{
			format = OS_TEXTURE_FORMAT_4444;
		}
	}
	else
	if (ti.flag & TGA_FLAG_GRAYSCALE)
	{
		if (OS_tformat[OS_TEXTURE_FORMAT_8].valid)
		{
			//
			// This card has a luminance only texture format.
			//

			format = OS_TEXTURE_FORMAT_8;
		}
		else
		{
			//
			// Use the RGB format as the next-best thing.
			//

			format = OS_TEXTURE_FORMAT_RGB;
		}
	}
	else
	{
		//
		// A normal RGB texture
		//

		format = OS_TEXTURE_FORMAT_RGB;
	}

	best_otf = &OS_tformat[format];

	if (!best_otf->valid)
	{
		//
		// No good texture format.
		//

		free(data);

		return nullptr;
	}

	//
	// Create a new texture.
	//

	ot = (OS_Texture *) malloc(sizeof(OS_Texture));
	
	if (!ot )
	{
		//
		// It's really not worth checking for this... but anyway!
		//

		free(data);

		return nullptr;
	}	

	strncpy(ot->name, fname, _MAX_PATH);

	ot->format   = format;
	ot->inverted = invert;

	//
	// Create a managed texture surface.
	//

	memset(&ot->ddsd, 0, sizeof(ot->ddsd));

	ot->ddsd.dwSize  = sizeof(DDSURFACEDESC2);
	ot->ddsd.dwFlags =
				DDSD_CAPS        |
				DDSD_HEIGHT      |
				DDSD_WIDTH       |
				DDSD_MIPMAPCOUNT |
				DDSD_PIXELFORMAT;
	ot->ddsd.dwWidth         = ti.width;
	ot->ddsd.dwHeight        = ti.height;
	ot->ddsd.dwMipMapCount   = 1;
	ot->ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
	ot->ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE | DDSCAPS2_HINTSTATIC;
	ot->ddsd.ddpfPixelFormat = best_otf->ddpf;

	VERIFY(OS_frame.direct_draw->CreateSurface(
										&ot->ddsd,
										&ot->ddsurface,
										nullptr) == DD_OK);

	if (invert)
	{
		std::int32_t i;
		std::int32_t j;

		TGA_Pixel *tp;

		//
		// Invert the texture.
		//

		tp = data;

		for (i = 0; i < ti.width; i++)
		{
			for (j = 0; j < ti.height; j++)
			{
				tp->alpha = 255 - tp->alpha;
				tp->red   = 255 - tp->red;
				tp->green = 255 - tp->green;
				tp->blue  = 255 - tp->blue;

				tp += 1;
			}
		}
	}

	//
	// Lock the surface.
	//

	DDSURFACEDESC2 ddsd;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	VERIFY(ot->ddsurface->Lock(nullptr, &ddsd, DDLOCK_WAIT, nullptr) == DD_OK);

	//
	// Copy the tga data into the surface.
	//

	if (format != OS_TEXTURE_FORMAT_8)
	{
		std::int32_t     i;
		std::int32_t     j;
		std::uint16_t     pixel_our;
		TGA_Pixel pixel_tga;
		std::uint16_t    *wscreen = (std::uint16_t *) ddsd.lpSurface;

		//
		// 16 bits per pixel.
		//

		for (i = 0; i < ti.width; i++)
		{
			for (j = 0; j < ti.height; j++)
			{
				pixel_tga = data[i + j * ti.width];
				pixel_our = 0;
				
				pixel_our |= (pixel_tga.red   >> best_otf->mask_r) << best_otf->shift_r;
				pixel_our |= (pixel_tga.green >> best_otf->mask_g) << best_otf->shift_g;
				pixel_our |= (pixel_tga.blue  >> best_otf->mask_b) << best_otf->shift_b;

				if (best_otf->ddpf.dwFlags & DDPF_ALPHAPIXELS)
				{
					pixel_our |= (pixel_tga.alpha >> best_otf->mask_a) << best_otf->shift_a;
				}

				wscreen[i + j * (ddsd.lPitch >> 1)] = pixel_our;
			}
		}
	}
	else
	{
		std::int32_t  i;
		std::int32_t  j;
		std::uint8_t *wscreen = (std::uint8_t *) ddsd.lpSurface;

		//
		// 8 bits per pixel.
		//

		for (i = 0; i < ti.width; i++)
		{
			for (j = 0; j < ti.height; j++)
			{
				wscreen[i + j * ddsd.lPitch] = data[i + j * ti.width].red;
			}
		}
	}
	
	//
	// Unlock the surface.
	//

	ot->ddsurface->Unlock(nullptr);

	//
	// Query the texture interface from the surface.
	//

	VERIFY(ot->ddsurface->QueryInterface(IID_IDirect3DTexture2, (void* *) &ot->ddtx) == DD_OK);

	//
	// Insert this texture into the array.
	//

	ot->next        = OS_texture_head;
	OS_texture_head = ot;

	//
	// Remember the size!
	//

	ot->size = ti.width;

	return ot;
}


OS_Texture *OS_texture_create(std::int32_t size, std::int32_t format)
{
	OS_Texture *ot;
	OS_Tformat *otf;

	//
	// Make sure this texture is not too big.
	// 

	{
		D3DDEVICEDESC dh;
		D3DDEVICEDESC ds;

		memset(&dh, 0, sizeof(dh));
		memset(&ds, 0, sizeof(ds));

		dh.dwSize = sizeof(dh);
		ds.dwSize = sizeof(ds);

		VERIFY(OS_frame.direct_3d->GetCaps(&dh, &ds) == D3D_OK);

		if (size > dh.dwMaxTextureWidth ||
			size > dh.dwMaxTextureHeight)
		{
			return nullptr;
		}
	}

	if (!OS_tformat[format].valid)
	{
		//
		// The requested texture format does not exist. Is there
		// another one we can try?
		//

		switch(format)
		{
			case OS_TEXTURE_FORMAT_8:    format = OS_TEXTURE_FORMAT_RGB;  break;
			case OS_TEXTURE_FORMAT_1555: format = OS_TEXTURE_FORMAT_4444; break;
			case OS_TEXTURE_FORMAT_4444: format = OS_TEXTURE_FORMAT_1555; break;
		}

		if (!OS_tformat[format].valid)
		{
			//
			// We have no suitable texture format.
			//

			return nullptr;
		}
	}

	//
	// The texture format we are going to use.
	//

	otf = &OS_tformat[format];

	//
	// Create a new texture.
	//

	ot = (OS_Texture *) malloc(sizeof(OS_Texture));
	
	if (!ot )
	{
		//
		// It's really not worth checking for this... but anyway!
		//

		return nullptr;
	}	

	sprintf(ot->name, "Generated");

	ot->format = format;
	ot->size   = size;

	//
	// Create a managed texture surface.
	//

	memset(&ot->ddsd, 0, sizeof(ot->ddsd));

	ot->ddsd.dwSize  = sizeof(DDSURFACEDESC2);
	ot->ddsd.dwFlags =
				DDSD_CAPS        |
				DDSD_HEIGHT      |
				DDSD_WIDTH       |
				DDSD_MIPMAPCOUNT |
				DDSD_PIXELFORMAT;
	ot->ddsd.dwWidth         = size;
	ot->ddsd.dwHeight        = size;
	ot->ddsd.dwMipMapCount   = 1;
	ot->ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
	ot->ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE | DDSCAPS2_HINTDYNAMIC;
	ot->ddsd.ddpfPixelFormat = otf->ddpf;

	if (OS_frame.direct_draw->CreateSurface(
										&ot->ddsd,
										&ot->ddsurface,
										nullptr) != DD_OK)
	{
		//
		// Oh dear...
		//

		free(ot);

		return nullptr;
	}

	
	//
	// The surface probably contains junk...
	//

	//
	// Query the texture interface from the surface.
	//

	VERIFY(ot->ddsurface->QueryInterface(IID_IDirect3DTexture2, (void* *) &ot->ddtx) == DD_OK);

	//
	// Insert this texture into the array.
	//

	ot->next        = OS_texture_head;
	OS_texture_head = ot;

	return ot;
}



void OS_texture_finished_creating()
{
	/*

	std::int32_t i;

	OS_Texture *ot;
	OS_Point    op;
	std::uint16_t       index[3];

	//
	// Go through all the textures and draw something with them.
	//

	OS_scene_begin();
	OS_init_renderstates();

	for (ot = OS_texture_head; ot; ot = ot->next)
	{
		OS_page_lock(ot);

		for (i = 0; i < 3; i++)
		{
			op.x    = frand() * OS_screen_width;
			op.y    = frand() * OS_screen_height;
			op.z    = 0.5F;
			op.rhw  = 0.5F;
			op.clip = OS_CLIP_TRANSFORMED;

			index[i] = OS_page_add_point(ot, &op, 0x00000000, 0x00000000, frand(), frand(), 0.0F);
		}

		OS_page_add_triangle(ot, index[0], index[1], index[2]);
		OS_page_unlock(ot);
		OS_page_draw(ot, OS_TEXTURE_TYPE_DOUBLESIDED | OS_TEXTURE_TYPE_ZALWAYS);
	}

	OS_scene_end();
	OS_show();

	*/
}


std::int32_t OS_texture_size(OS_Texture *ot)
{
	return ot->size;
}



std::int32_t OS_bitmap_format;		// OS_TEXTURE_FORMAT_*
std::uint16_t *OS_bitmap_uword_screen;	// For 16-bit formats.
std::int32_t OS_bitmap_uword_pitch;	// Pitch in UWORDS
std::uint8_t *OS_bitmap_ubyte_screen;	// For the grayscale format.
std::int32_t OS_bitmap_ubyte_pitch;	// Pitch in UBYTES
std::int32_t OS_bitmap_width;
std::int32_t OS_bitmap_height;
std::int32_t OS_bitmap_mask_r;
std::int32_t OS_bitmap_mask_g;
std::int32_t OS_bitmap_mask_b;
std::int32_t OS_bitmap_mask_a;
std::int32_t OS_bitmap_shift_r;
std::int32_t OS_bitmap_shift_g;
std::int32_t OS_bitmap_shift_b;
std::int32_t OS_bitmap_shift_a;

void OS_texture_lock(OS_Texture *ot)
{
	OS_Tformat *otf;

	HRESULT res;

	DDSURFACEDESC2 ddsd;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	VERIFY((res = ot->ddsurface->Lock(
					nullptr,
				   &ddsd,
					DDLOCK_WAIT      |
					DDLOCK_WRITEONLY |
					DDLOCK_NOSYSLOCK,
					nullptr)) == DD_OK);

	ASSERT(WITHIN(ot->format, 0, OS_TEXTURE_FORMAT_NUMBER - 1));

	otf = &OS_tformat[ot->format];

	if (ot->format == OS_TEXTURE_FORMAT_8)
	{
		//
		// 8-bits per pixel.
		//

		OS_bitmap_ubyte_screen = (std::uint8_t *) ddsd.lpSurface;
		OS_bitmap_ubyte_pitch  = ddsd.lPitch;

		OS_bitmap_uword_screen = nullptr;
		OS_bitmap_uword_pitch  = 0;
	}
	else
	{
		OS_bitmap_ubyte_screen = nullptr;
		OS_bitmap_ubyte_pitch  = 0;

		OS_bitmap_uword_screen = (std::uint16_t *) ddsd.lpSurface;
		OS_bitmap_uword_pitch  = ddsd.lPitch >> 1;
	}

	OS_bitmap_format  = ot->format;
	OS_bitmap_width   = ddsd.dwWidth;
	OS_bitmap_height  = ddsd.dwHeight;
	OS_bitmap_mask_r  = otf->mask_r;
	OS_bitmap_mask_g  = otf->mask_g;
	OS_bitmap_mask_b  = otf->mask_b;
	OS_bitmap_mask_a  = otf->mask_a;
	OS_bitmap_shift_r = otf->shift_r;
	OS_bitmap_shift_g = otf->shift_g;
	OS_bitmap_shift_b = otf->shift_b;
	OS_bitmap_shift_a = otf->shift_a;
}

void OS_texture_unlock(OS_Texture *ot)
{
	//
	// Unlock the surface.
	//

	ot->ddsurface->Unlock(nullptr);
}




// ========================================================
//
// PIPELINE SETUP AND VALIDATION
//
// ========================================================



void OS_init_renderstates()
{
	LPDIRECT3DDEVICE3 d3d = OS_frame.direct_3d;

	//
	// Setup renderstates.
	// 

	d3d->SetRenderState(D3DRENDERSTATE_SHADEMODE,          D3DSHADE_GOURAUD);
	d3d->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, true);
	d3d->SetRenderState(D3DRENDERSTATE_DITHERENABLE,       true);
	d3d->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,     true);
	d3d->SetRenderState(D3DRENDERSTATE_SUBPIXEL,           true);
	d3d->SetRenderState(D3DRENDERSTATE_ZENABLE,            true);
	d3d->SetRenderState(D3DRENDERSTATE_ZFUNC,              D3DCMP_LESSEQUAL);
	d3d->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,       true);
	d3d->SetRenderState(D3DRENDERSTATE_CULLMODE,           D3DCULL_CCW);
	d3d->SetRenderState(D3DRENDERSTATE_FOGENABLE,          false);
	d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   false);
	d3d->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,    false);

	if (KEY_on[KEY_A])
	{
		d3d->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_SORTINDEPENDENT);
	}
	else
	{
		d3d->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
	}

	//
	// Setup pipeline for one-texture gouraud shaded.
	//

	d3d->SetTextureStageState(0, D3DTSS_COLOROP,       D3DTOP_MODULATE);
	d3d->SetTextureStageState(0, D3DTSS_COLORARG1,     D3DTA_TEXTURE);
	d3d->SetTextureStageState(0, D3DTSS_COLORARG2,     D3DTA_DIFFUSE);
    d3d->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

	d3d->SetTextureStageState(1, D3DTSS_COLOROP,       D3DTOP_DISABLE);
	d3d->SetTextureStageState(1, D3DTSS_COLORARG1,     D3DTA_TEXTURE);
	d3d->SetTextureStageState(1, D3DTSS_COLORARG2,     D3DTA_CURRENT);
    d3d->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	d3d->SetTextureStageState(2, D3DTSS_COLOROP,       D3DTOP_DISABLE);

	d3d->SetTextureStageState(0, D3DTSS_MINFILTER,     D3DTFG_LINEAR);
	d3d->SetTextureStageState(0, D3DTSS_MAGFILTER,     D3DTFG_LINEAR);
	d3d->SetTextureStageState(0, D3DTSS_ADDRESS,       D3DTADDRESS_WRAP);

	d3d->SetTextureStageState(1, D3DTSS_MINFILTER,     D3DTFG_LINEAR);
	d3d->SetTextureStageState(1, D3DTSS_MAGFILTER,     D3DTFG_LINEAR);
	d3d->SetTextureStageState(1, D3DTSS_ADDRESS,       D3DTADDRESS_WRAP);

	//
	// No alpha.
	//

	d3d->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	d3d->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	d3d->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}



//
// Works out how to setup the pipeline for additive and multiplicitive
// multi-texturing.
// 

#define OS_METHOD_NUMBER_MUL 2

std::int32_t OS_pipeline_method_mul;

void OS_pipeline_calculate()
{
	std::uint32_t num_passes;

	LPDIRECT3DDEVICE3 d3d = OS_frame.direct_3d;

	OS_pipeline_method_mul = 0;

	OS_Texture *ot1 = OS_texture_create(32, OS_TEXTURE_FORMAT_RGB);
	OS_Texture *ot2 = OS_texture_create(32, OS_TEXTURE_FORMAT_RGB);

	while(1)
	{
		OS_init_renderstates();

		d3d->SetTexture(0, ot1->ddtx);
		d3d->SetTexture(1, ot2->ddtx);

		switch(OS_pipeline_method_mul)
		{
			case 1:

				d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

				d3d->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				
				break;

			case 0:

				d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
				d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

				d3d->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

				d3d->SetTextureStageState(2, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				d3d->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
				d3d->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);

				break;
			
			default:

				//
				// Didn't find any way to do mulitexturing!
				//

				break;
		}

		if (OS_pipeline_method_mul == OS_METHOD_NUMBER_MUL)
		{
			//
			// No multitexturing! :(
			//

			break;
		}

		if (d3d->ValidateDevice(&num_passes) == D3D_OK)
		{
			if (num_passes != 0)
			{
				//
				// Found a methed for doing additive multi-texturing.
				//

				OS_string("Validated %d with %d passes\n", OS_pipeline_method_mul, num_passes);

				break;
			}
		}

		OS_pipeline_method_mul += 1;
	}

	OS_string("Multitexture method %d\n", OS_pipeline_method_mul);
}


void OS_change_renderstate_for_type(std::uint32_t draw)
{
	LPDIRECT3DDEVICE3 d3d = OS_frame.direct_3d;

	if (draw & OS_DRAW_ADD)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
		d3d->SetRenderState(D3DRENDERSTATE_SRCBLEND,         D3DBLEND_ONE);
		d3d->SetRenderState(D3DRENDERSTATE_DESTBLEND,        D3DBLEND_ONE);
	}

	if (draw & OS_DRAW_MULTIPLY)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
		d3d->SetRenderState(D3DRENDERSTATE_SRCBLEND,         D3DBLEND_DESTCOLOR);
		d3d->SetRenderState(D3DRENDERSTATE_DESTBLEND,        D3DBLEND_SRCCOLOR);
	}

	if (draw & OS_DRAW_MULBYONE)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
		d3d->SetRenderState(D3DRENDERSTATE_SRCBLEND,         D3DBLEND_DESTCOLOR);
		d3d->SetRenderState(D3DRENDERSTATE_DESTBLEND,        D3DBLEND_ZERO);
	}

	if (draw & OS_DRAW_CLAMP)
	{
		d3d->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
	}

	if (draw & OS_DRAW_DECAL)
	{
		d3d->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	}

	if (draw & OS_DRAW_TRANSPARENT)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
		d3d->SetRenderState(D3DRENDERSTATE_SRCBLEND,         D3DBLEND_ZERO);
		d3d->SetRenderState(D3DRENDERSTATE_DESTBLEND,        D3DBLEND_ONE);
	}

	if (draw & OS_DRAW_DOUBLESIDED)
	{
		d3d->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	}

	if (draw & OS_DRAW_NOZWRITE)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false);
	}

	if (draw & OS_DRAW_ALPHAREF)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_NOTEQUAL);
		d3d->SetRenderState(D3DRENDERSTATE_ALPHAREF,0);
		d3d->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,true);

		//
		// Make sure the alpha from the texture gets through.
		//

		d3d->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3d->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	}

	if (draw & OS_DRAW_ZREVERSE)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
	}

	if (draw & OS_DRAW_ZALWAYS)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
	}

	if (draw & OS_DRAW_CULLREVERSE)
	{
		d3d->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
	}

	if (draw & OS_DRAW_NODITHER)
	{
		d3d->SetRenderState(D3DRENDERSTATE_DITHERENABLE, false);
	}

	if (draw & OS_DRAW_ALPHABLEND)
	{
		d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);
		d3d->SetRenderState(D3DRENDERSTATE_SRCBLEND,         D3DBLEND_SRCALPHA);
		d3d->SetRenderState(D3DRENDERSTATE_DESTBLEND,        D3DBLEND_INVSRCALPHA);

		d3d->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3d->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		d3d->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	}

	if (draw & OS_DRAW_TEX_NONE)
	{
		d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	}

	if (draw & OS_DRAW_TEX_MUL)
	{
		switch(OS_pipeline_method_mul)
		{
			case 1:

				if (draw & OS_DRAW_DECAL)
				{
					//
					// Don't multiply by diffuse colour...
					//

					d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
					d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					d3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
				}
				else
				{
					d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
					d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					d3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				}

				d3d->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				
				break;

			case 0:

				d3d->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
				d3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

				d3d->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				d3d->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

				if (draw & OS_DRAW_DECAL)
				{
					//
					// Don't multiply by diffuse colour...
					//
				}
				else
				{
					d3d->SetTextureStageState(2, D3DTSS_COLOROP,   D3DTOP_MODULATE);
					d3d->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
					d3d->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);
				}

				break;
			
			default:
				break;
		}
	}

	if (draw & OS_DRAW_NOFILTER)
	{
		d3d->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
		d3d->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);

		d3d->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTFG_POINT);
		d3d->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTFG_POINT);
	}
}


void OS_undo_renderstate_type_changes()
{
	LPDIRECT3DDEVICE3 d3d = OS_frame.direct_3d;

	d3d->SetTextureStageState(0, D3DTSS_COLOROP,       D3DTOP_MODULATE);
	d3d->SetTextureStageState(0, D3DTSS_COLORARG1,     D3DTA_TEXTURE);
	d3d->SetTextureStageState(0, D3DTSS_COLORARG2,     D3DTA_DIFFUSE);

	d3d->SetTextureStageState(1, D3DTSS_COLOROP,       D3DTOP_DISABLE);
	d3d->SetTextureStageState(1, D3DTSS_COLORARG1,     D3DTA_TEXTURE);
	d3d->SetTextureStageState(1, D3DTSS_COLORARG2,     D3DTA_CURRENT);

	d3d->SetTextureStageState(2, D3DTSS_COLOROP,       D3DTOP_DISABLE);

	d3d->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP);

	d3d->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, false);
	d3d->SetRenderState(D3DRENDERSTATE_CULLMODE,         D3DCULL_CCW);
	d3d->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,     true);
	d3d->SetRenderState(D3DRENDERSTATE_ZFUNC,            D3DCMP_LESSEQUAL);
	d3d->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,  false);
	d3d->SetRenderState(D3DRENDERSTATE_DITHERENABLE,     true);

	d3d->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_LINEAR);
	d3d->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

	d3d->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTFG_LINEAR);
	d3d->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
}

// ========================================================
//
// WINDOWS STUFF
//
// ========================================================

void OS_string(const char* fmt, ...)
{
	//
	// Work out the real message.
	//

	char message[512];
	va_list	ap;

	va_start(ap, fmt);
	vsprintf(message, fmt, ap);
	va_end  (ap);

	OutputDebugStringA(message);
}


std::int32_t OS_game_start_tick_count;

std::int32_t OS_ticks()
{
	return GetTickCount() - OS_game_start_tick_count;
}

void OS_ticks_reset()
{
	OS_game_start_tick_count = GetTickCount();
}


std::int32_t OS_mhz;

//
// Returns true if the processor has support for the RDTSC instruction.
// 

std::int32_t OS_has_rdtsc()
{
	std::int32_t res;

	_asm
	{
		mov		eax, 0
		cpuid
		mov		res, eax
	}

	if (res == 0)
	{
		//
		// This is an old 486!
		//

		return false;
	}

	//
	// Check the processor feature info.
	//

	_asm
	{
		mov		eax, 1
		cpuid
		mov		res, edx
	}

	if (res & (1 << 4))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//
// Returns the number of processor ticks since the processor was reset / 65536
//

std::uint32_t OS_rdtsc()
{
	std::uint32_t hi;
	std::uint32_t lo;

	_asm
	{
		rdtsc
		mov		hi, edx
		mov		lo, eax
	}

	std::uint32_t ans;

	ans  = lo >> 16;
	ans |= hi << 16;

	return ans;
}


void OS_work_out_mhz()
{
	if (OS_has_rdtsc())
	{
		std::int32_t tick;
		std::uint32_t tsc1 = OS_rdtsc();

		//
		// Wait a second.
		//

		tick = OS_ticks();

		while(OS_ticks() < tick + 1000);

		std::uint32_t tsc2 = OS_rdtsc();

		float persec = float(tsc2 - tsc1);

		persec *= 65536.0F / 1000000.0F;

		OS_mhz = std::int32_t(persec + 0.5F);
	}
	else
	{
		//
		// It must be a 486... lets say its 66Mhz and be hopeful.
		//

		OS_mhz = 66;
	}
}

std::int32_t OS_processor_mhz()
{
	return OS_mhz;
}



// ========================================================
//
// MOUSE STUFF
//
// ========================================================

void OS_mouse_get(std::int32_t *x, std::int32_t *y)
{
	POINT point;

	GetCursorPos(&point);

	*x = point.x;
	*y = point.y;
}

void OS_mouse_set(std::int32_t  x, std::int32_t  y)
{
	SetCursorPos(x, y);
}



LRESULT CALLBACK OS_message_handler(
					HWND   window_handle,
					UINT   message_type,
					WPARAM param_w,
					LPARAM param_l)
{
	std::uint8_t scancode;

	switch(message_type)
	{
		case WM_PAINT:

			//
			// The user callback function does all the screen drawing.
			// Do enough to appease windows.
			//

			HDC  		device_context_handle;
			PAINTSTRUCT	paintstruct;

			device_context_handle = BeginPaint(window_handle, &paintstruct);
			EndPaint(window_handle, &paintstruct);

			return 0;

		case WM_CHAR:
			KEY_inkey  = param_w;
			break;

		case WM_KEYDOWN: 
		case WM_KEYUP:	 

			//
			// Keyboard stuff.
			//

			scancode  = (param_l >> 16) & 0xff;
			scancode |= (param_l >> 17) & 0x80;

			if (message_type == WM_KEYDOWN)
			{
				KEY_on[scancode] = 1;
			}
			else
			{
				KEY_on[scancode] = 0;
			}
			
			//
			// Alt keys don't work.
			//

			KEY_on[KEY_LALT] = 0;
			KEY_on[KEY_RALT] = 0;

			//
			// Check for shift/alt/control keys.
			//

			KEY_shift = 0;

			if (KEY_on[KEY_LSHIFT  ] || KEY_on[KEY_RSHIFT  ]) KEY_shift |= KEY_SHIFT;
			if (KEY_on[KEY_LCONTROL] || KEY_on[KEY_RCONTROL]) KEY_shift |= KEY_CONTROL;
			if (KEY_on[KEY_LALT    ] || KEY_on[KEY_RALT    ]) KEY_shift |= KEY_ALT;

			return 0;

		case WM_MOVE:

			//
			// Tell the frame about the new position of the window.
			//

			OS_frame.Move(LOWORD(param_l),HIWORD(param_l));
			
			//
			// Fall through to the default handling.
			//

			break;

		default:
			break;
	}


	//
	// Just let windows do its normal thing.
	//

	return DefWindowProc(
				window_handle,
				message_type,
				param_w,
				param_l);

}


std::int32_t OS_process_messages()
{
	MSG msg;
	int ret;

	//
	// Poll the joystick.
	//

	OS_joy_poll();

	//
	// Activate the changes made to the direct sound settings...
	//

	OS_sound_update_changes();

	while(1)
	{
		if (!PeekMessage(&msg, nullptr, nullptr, nullptr, PM_NOREMOVE))
		{
			//
			// No messages pending.
			//

			return OS_CARRY_ON;
		}

		ret = GetMessage(&msg, nullptr, 0, 0);

		if (ret == 0 || ret == -1)
		{
			//
			// Kill the game!
			//

			return OS_QUIT_GAME;
		}

		TranslateMessage(&msg);
		DispatchMessage (&msg);
	}
}

//
// Valid devices.
//

typedef struct
{
	D3DEnum_DriverInfo *driver;
	D3DEnum_DeviceInfo *device;
	D3DEnum_ModeInfo   *mode;		// nullptr => Use windowed mode.

} OS_Mode;

#define OS_MAX_MODES 16

OS_Mode OS_mode[OS_MAX_MODES];
std::int32_t OS_mode_upto;
std::int32_t OS_mode_sel;

//
// Finds the valid devices from the D3DEnumerated choice and set
// the OS_mode_sel to the default.
//

void OS_mode_init()
{
	std::int32_t i;

	D3DEnum_DriverInfo *vi;
	D3DEnum_DeviceInfo *ci;
	D3DEnum_ModeInfo   *mi; 	// nullptr => Use windowed mode.

	OS_mode_upto = 0;
	OS_mode_sel  = 0;

	std::int32_t lookfor640x480;
	std::int32_t lookfor512x384;

	//
	// Find all valid modes.
	//

	for (vi = D3DEnum_GetFirstDriver(); vi; vi = vi->pNext)
	{
		for (ci = vi->pFirstDevice; ci; ci = ci->pNext)
		{
			if (ci->bIsHardware)
			{
				//
				// Found a hardware device.
				//

				if (WITHIN(OS_mode_upto, 0, OS_MAX_MODES - 1))
				{
					OS_mode[OS_mode_upto].driver = vi;
					OS_mode[OS_mode_upto].device = ci;
					OS_mode[OS_mode_upto].mode   = nullptr;

					#ifdef NDEBUG
					lookfor512x384 = false;//(vi != D3DEnum_GetFirstDriver());
					lookfor640x480 = true;
					#else
					lookfor512x384 = false;
					lookfor640x480 = !ci->bWindowed;
					#endif

					if (lookfor512x384)
					{
						//
						// Look for the first 512x384 mode.
						//

						for (mi = ci->pFirstMode; mi; mi = mi->pNext)
						{
							if (mi->ddsd.dwWidth  == 512 &&
								mi->ddsd.dwHeight == 384)
							{
								OS_mode[OS_mode_upto].mode = mi;

								//
								// We already have our mode.
								//

								lookfor640x480 = false;

								break;
							}
						}
					}

					if (lookfor640x480)
					{
						//
						// Look for the first 640x480 mode.
						//

						for (mi = ci->pFirstMode; mi; mi = mi->pNext)
						{
							if (mi->ddsd.dwWidth  == 640 &&
								mi->ddsd.dwHeight == 480)
							{
								OS_mode[OS_mode_upto].mode = mi;

								break;
							}
						}
					}

					if (!OS_mode[OS_mode_upto].mode )
					{
						//
						// Make sure this device support windowed mode!
						//

						if (OS_mode[OS_mode_upto].device->bWindowed)
						{
							//
							// We are ok.
							//
						}
						else
						{
							//
							// Use the first available mode.
							//

							OS_mode[OS_mode_upto].mode = OS_mode[OS_mode_upto].device->pFirstMode;
						}
					}

					OS_mode_upto += 1;
				}
			}
		}
	}

	#ifndef NDEBUG

	//
	// In debug build choose the first windowed mode.
	//

	for (i = 0; i < OS_mode_upto; i++)
	{
		if (OS_mode[i].device->bWindowed)
		{
			OS_mode_sel = i;
		}
	}

	#else

	//
	// In release build choose the last mode.
	//

	OS_mode_sel = OS_mode_upto - 1;

	#endif
}


//
// Adds the modes for the current selection to the combo box.
//

void OS_mydemo_setup_mode_combo(HWND combo_handle, std::int32_t mode)
{
	std::int32_t index;

	ASSERT(WITHIN(mode, 0, OS_mode_upto - 1));

	//
	// Clear all old modes.
	//

	SendMessage(combo_handle, CB_RESETCONTENT, 0, 0);

	//
	// Add each mode.
	//

	D3DEnum_ModeInfo *mi;

	if (OS_mode[mode].device->bWindowed)
	{
		index = SendMessage(combo_handle, CB_ADDSTRING, 0, (LPARAM) "In a window");

		SendMessage(combo_handle, CB_SETITEMDATA, (WPARAM) index, (LPARAM) nullptr);

		if (nullptr == OS_mode[mode].mode)
		{
			//
			// This is the current selection.
			//

			SendMessage(combo_handle, CB_SETCURSEL, index, 0);
		}
	}

	for (mi = OS_mode[mode].device->pFirstMode; mi; mi = mi->pNext)
	{
		index = SendMessage(combo_handle, CB_ADDSTRING, 0, (LPARAM) mi->strDesc);

		SendMessage(combo_handle, CB_SETITEMDATA, (WPARAM) index, (LPARAM) mi);

		if (mi == OS_mode[mode].mode)
		{
			//
			// This is the current selection.
			//

			SendMessage(combo_handle, CB_SETCURSEL, index, 0);
		}
	}
}



//
// The callback function for the MyDemo dialog box.
//

#define OS_MYDEMO_RUN	1
#define OS_MYDEMO_EXIT	2

bool CALLBACK OS_mydemo_proc(
				HWND   dialog_handle,
				UINT   message_type,
				WPARAM param_w,
				LPARAM param_l)
{
	std::int32_t i;
	std::int32_t d;
	std::int32_t res;
	std::int32_t index;

	RECT rect;

	HWND combo_handle;

	D3DEnum_DriverInfo *vi;
	D3DEnum_DeviceInfo *ci;

	switch(message_type)
	{
		case WM_INITDIALOG:

			//
			// Fill out the list boxes with the correct values.  First find
			// all compatible
			//

			combo_handle = GetDlgItem(dialog_handle, IDC_COMBO_DRIVER);

			for (i = 0; i < OS_mode_upto; i++)
			{
				SendMessage(combo_handle, CB_ADDSTRING, 0, (LPARAM) OS_mode[i].driver->strDesc);
			}

			//
			// Set the current selection.
			//

			SendMessage(combo_handle, CB_SETCURSEL, OS_mode_sel, 0);

			//
			// Add the modes for the current selection.
			//

			combo_handle = GetDlgItem(dialog_handle, IDC_COMBO_MODE);

			OS_mydemo_setup_mode_combo(combo_handle, OS_mode_sel);

			{
				char mhz[64];

				sprintf(mhz, "Detected a %dMhz processor", OS_mhz);

				//
				// What speed processor have we detected?
				//

				SetWindowText(GetDlgItem(dialog_handle, IDC_PROCESSOR), mhz);
			}

			return true;

		case WM_COMMAND:

			switch(LOWORD(param_w))
			{
				case IDOK:
					EndDialog(dialog_handle, OS_MYDEMO_RUN);
					return true;

				case IDCANCEL:
					EndDialog(dialog_handle, OS_MYDEMO_EXIT);
					return true;

				case IDC_COMBO_DRIVER:

					switch(HIWORD(param_w))
					{
						case CBN_SELCHANGE:
							
							//
							// Change the list of modes.
							//

							OS_mode_sel = SendMessage((HWND) param_l, CB_GETCURSEL, 0, 0);

							ASSERT(WITHIN(OS_mode_sel, 0, OS_mode_upto - 1));

							OS_mydemo_setup_mode_combo(
								GetDlgItem(dialog_handle, IDC_COMBO_MODE),
								OS_mode_sel);

							break;
					}
					
					break;

				case IDC_COMBO_MODE:

					switch(HIWORD(param_w))
					{
						case CBN_SELCHANGE:
							
							//
							// Update the current mode.
							//

							index = SendMessage((HWND) param_l, CB_GETCURSEL, 0, 0);

							ASSERT(WITHIN(OS_mode_sel, 0, OS_mode_upto - 1));

							OS_mode[OS_mode_sel].mode = (D3DEnum_ModeInfo * /* We hope */) SendMessage((HWND) param_l, CB_GETITEMDATA, (WPARAM) index, 0);

							break;
					}
					
					break;
			}

			break;

		case WM_CLOSE:
			EndDialog(dialog_handle, OS_MYDEMO_EXIT);
			return true;
	}

	return false;
}


//
// The entry point of the program.
//

int WINAPI WinMain(
			HINSTANCE this_instance,
			HINSTANCE last_instance,
			LPSTR	  command_line,
			int       start_show_state)
{
	HRESULT res;

	//
	// Remember the arguments passed to this function.
	//
	
	OS_this_instance	= this_instance;
	OS_last_instance	= last_instance;
	OS_command_line		= command_line;
	OS_start_show_state	= start_show_state;

	OS_wcl.hInstance		= this_instance;
	OS_wcl.lpszClassName	= OS_application_name;
	OS_wcl.lpfnWndProc		= OS_message_handler;
	OS_wcl.style			= 0;
	OS_wcl.cbSize			= sizeof(WNDCLASSEX);
	OS_wcl.cbClsExtra		= 0;
	OS_wcl.cbWndExtra		= 0;
	OS_wcl.lpszMenuName		= nullptr;
	OS_wcl.hIcon			= LoadIcon(this_instance, MAKEINTRESOURCE(IDI_ICON1));
	OS_wcl.hIconSm			= nullptr;//LoadIcon(this_instance, MAKEINTRESOURCE(IDI_ICON1));
	OS_wcl.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	OS_wcl.hbrBackground	= (HBRUSH) GetStockObject(GRAY_BRUSH);

	//
	// Register the window class.
	//
	
	if (RegisterClassEx(&OS_wcl) == 0)
	{
		//
		// Could not register the window class!
		//

		return 0;
	}

	//
	// Create a window 640 x 480.
	//

	RECT rect;

	rect.left   = 100 + 0;
	rect.right  = 100 + 640;
	rect.top    = 100 + 0;
	rect.bottom = 100 + 480;

	if (AdjustWindowRect(
			&rect,
			 WS_CAPTION,
			 false) == 0)
	{
		rect.right -= 200;
		rect.left  += 200;
	}
	else
	{
		OS_window_handle = CreateWindow(
							OS_application_name,
							OS_application_name,
							WS_CAPTION | WS_SYSMENU,
							50,
							50,
							rect.right  - rect.left,
							rect.bottom - rect.top,
							nullptr,
							nullptr,
							this_instance,
							nullptr);
	}

	//
	// Make sure it worked.
	//

	if (OS_window_handle == 0)
	{
		return 0;
	}

	//
	// Initialise joystick control.
	//

	OS_joy_init();

	//
	// Find out the speed of the machine we are running on.
	//

	OS_work_out_mhz();

	//
	// Enumerate the devices.
	//

	D3DEnum_EnumerateDevices(nullptr);

	D3DEnum_DriverInfo *di = D3DEnum_GetFirstDriver();

	//
	// Find valid modes.
	//

	OS_mode_init();

	if (OS_mode_upto == 0)
	{
		//
		// No valid screen mode!
		//

		MessageBox(
			OS_window_handle,
			"Could not find a 3d accelerator card.  Make sure that you have DirectX 6.0 or higher installed.", "Beat", MB_ICONERROR | MB_OK);

		exit(1);
	}

  have_another_go:;

	//
	// Promt the user to choose a mode.
	//

	switch(DialogBox(
			OS_this_instance,
			MAKEINTRESOURCE(IDD_DRIVERS),
			OS_window_handle,
			OS_mydemo_proc))
	{
		case OS_MYDEMO_RUN:
			break;

		case OS_MYDEMO_EXIT:

			//
			// Close gracefully...
			// 

			D3DEnum_FreeResources();

			return 0;

		default:
			exit(1);
			break;
	}

	{
		GUID           *driver;
		GUID           *device;
		DDSURFACEDESC2 *display_mode;
		bool            is_windowed;
		bool            is_hardware;

		#if WE_USE_THE_DEFAULT_DIALOG_BOX		

		//
		// Prompt the user for a device and a driver.
		//

		INT ret = D3DEnum_UserDlgSelectDriver(OS_window_handle, true);

		D3DEnum_GetSelectedDriver(
			&driver,
			&device,
			&display_mode,
			&is_windowed,
			&is_hardware);

		OS_frame_is_fullscreen = !is_windowed;
		OS_frame_is_hardware   =  is_hardware;

		#else

		driver = OS_mode[OS_mode_sel].driver->pGUID;
		device = OS_mode[OS_mode_sel].device->pGUID;

		if (OS_mode[OS_mode_sel].mode)
		{
			display_mode = &OS_mode[OS_mode_sel].mode->ddsd;

			OS_frame_is_fullscreen = true;
			OS_frame_is_hardware   = true;
		}
		else
		{
			display_mode = nullptr;

			OS_frame_is_fullscreen = false;
			OS_frame_is_hardware   = true;
		}

		#endif

		//
		// Initialise the framework.
		//

		DWORD flags;

		flags  = 0;
		flags |= D3DFW_BACKBUFFER;
		flags |= D3DFW_ZBUFFER;
		//flags |= D3DFW_NO_FPUSETUP;

		if (OS_frame_is_fullscreen)
		{
			flags |= D3DFW_FULLSCREEN;
		}

		if (OS_frame_is_fullscreen)
		{
			//
			// Hide the mouse.
			// 

			ShowCursor(false);

			//
			// Makes the window not redraw itself when we go fullscreen.
			//

			SetWindowLong(OS_window_handle, GWL_STYLE, WS_POPUP);
		}

		res = OS_frame.Initialize(
						OS_window_handle,
						driver,
						device,
						display_mode,
						flags);
		
		if (res == S_OK)
		{
			if (!OS_frame_is_fullscreen)
			{
				//
				// Show our window!
				//

				ShowWindow(OS_window_handle, SW_SHOW);
			}

			//
			// Enumerate texture formats.
			//

			{
				int i;
				
				OS_Tformat *otf;

				//
				// Find the texture formats.
				//

				OS_frame.direct_3d->EnumTextureFormats(OS_texture_enumerate_pixel_formats, nullptr);

				//
				// Set the masks and shifts for each texture format.
				//

				for (i = 0; i < OS_TEXTURE_FORMAT_NUMBER; i++)
				{
					otf = &OS_tformat[i];

					if (i == OS_TEXTURE_FORMAT_8)
					{
						//
						// We don't have to set the masks and shifts for grayscale textures.
						//

						continue;
					}

					if (otf->valid)
					{
						//
						// Calculate the masks and shifts.
						//

						OS_calculate_mask_and_shift(otf->ddpf.dwRBitMask, &otf->mask_r, &otf->shift_r);
						OS_calculate_mask_and_shift(otf->ddpf.dwGBitMask, &otf->mask_g, &otf->shift_g);
						OS_calculate_mask_and_shift(otf->ddpf.dwBBitMask, &otf->mask_b, &otf->shift_b);
									 
						if (otf->ddpf.dwFlags & DDPF_ALPHAPIXELS)
						{
							OS_calculate_mask_and_shift(otf->ddpf.dwRGBAlphaBitMask, &otf->mask_a, &otf->shift_a);
						}
					}
				}
			}

			//
			// What is the screen-res?
			//

			RECT *dimensions = OS_frame.GetViewportRect();

			OS_screen_width  = float(dimensions->right);
			OS_screen_height = float(dimensions->bottom);

			//
			// Make the fast floating point to std::int32_t conversion macro work. It
			// changes the default setting of the floating point unit to truncate
			// instead of round-to-nearest.
			//

			/*

			OS_string("%d, %d, %d, %d, %d, %d\n", std::int32_t(1.1F), std::int32_t(0.9F), std::int32_t(2.5F), std::int32_t(-1.1F), std::int32_t(-0.9F), std::int32_t(2.51F));

			OS_string("%d, %d, %d, %d, %d, %d\n", ftol(1.1F), ftol(0.9F), ftol(2.5F), ftol(-1.1F), ftol(-0.9F), ftol(2.51F));

			ftol_init();

			OS_string("%d, %d, %d, %d, %d, %d\n", ftol(1.1F), ftol(0.9F), ftol(2.5F), ftol(-1.1F), ftol(-0.9F), ftol(2.51F));
			
			*/

			//
			// Work out how to multi-texture
			//

			OS_pipeline_calculate();

			//
			// Initailise the sound system.
			//

			OS_sound_init();

			//
			// Time relative to the beginning of the program.
			//

			OS_game_start_tick_count = GetTickCount();

			//
			// Start the game.
			//

			MAIN_main();

			//
			// Clean up.
			//

			OS_frame.DestroyObjects();

			OS_sound_finish();
		}
		else
		{
			if (OS_frame_is_fullscreen)
			{
				//
				// Show the mouse.
				// 

				ShowCursor(true);
			}

			//
			// Could not set that mode!
			//

			char* err;

			if (res == D3DFWERR_NOZBUFFER)
			{
				err = "There was not enough memory to create the zbuffer. Try using a lower resolution mode.";

			}
			else
			{
				err = "Could not setup the display using those settings. Try changing driver or using another mode.";
			}

			MessageBox(
				OS_window_handle,
				err,
				"Beat",
				MB_ICONERROR | MB_OK);

			//
			// Have another go...
			//

			goto have_another_go;
		}
	}

	//
	// Free all enumeration resources.
	//

	D3DEnum_FreeResources();

	return 0;
}


// ========================================================
//
// ROTATING POINTS
//
// ========================================================

//
// The camera and the screen.
//

float OS_cam_x;
float OS_cam_y;
float OS_cam_z;
float OS_cam_aspect;
float OS_cam_lens;
float OS_cam_view_dist;
float OS_cam_over_view_dist;
float OS_cam_matrix[9];
float OS_cam_view_matrix[9];

float OS_cam_screen_x1;
float OS_cam_screen_y1;
float OS_cam_screen_x2;
float OS_cam_screen_y2;

float OS_cam_screen_width;
float OS_cam_screen_height;
float OS_cam_screen_mid_x;
float OS_cam_screen_mid_y;
float OS_cam_screen_mul_x;
float OS_cam_screen_mul_y;

void OS_camera_set(
		float world_x,
		float world_y,
		float world_z,
		float view_dist,
		float yaw,
		float pitch,
		float roll,
		float lens,
		float screen_x1,
		float screen_y1,
		float screen_x2,
		float screen_y2)
{
	OS_cam_screen_x1 = screen_x1 * OS_screen_width;
	OS_cam_screen_y1 = screen_y1 * OS_screen_height;
	OS_cam_screen_x2 = screen_x2 * OS_screen_width;
	OS_cam_screen_y2 = screen_y2 * OS_screen_height;

	OS_cam_screen_width  = OS_cam_screen_x2 - OS_cam_screen_x1;
	OS_cam_screen_height = OS_cam_screen_y2 - OS_cam_screen_y1;
	OS_cam_screen_mid_x  = OS_cam_screen_x1 + OS_cam_screen_width  * 0.5F;
	OS_cam_screen_mid_y  = OS_cam_screen_y1 + OS_cam_screen_height * 0.5F;
	OS_cam_screen_mul_x  = OS_cam_screen_width  * 0.5F / OS_ZCLIP_PLANE;
	OS_cam_screen_mul_y  = OS_cam_screen_height * 0.5F / OS_ZCLIP_PLANE;

	OS_cam_x = world_x;
	OS_cam_y = world_y;
	OS_cam_z = world_z;

	OS_cam_lens           = lens;
	OS_cam_view_dist      = view_dist;
	OS_cam_over_view_dist = 1.0F / view_dist;
	OS_cam_aspect         = OS_cam_screen_height / OS_cam_screen_width;

	MATRIX_calc(
		OS_cam_matrix,
		yaw,
		pitch,
		roll);

	memcpy(OS_cam_view_matrix, OS_cam_matrix, sizeof(OS_cam_view_matrix));

	MATRIX_skew(
		OS_cam_matrix,
		OS_cam_aspect,
		OS_cam_lens,
		OS_cam_over_view_dist);		// Shrink the matrix down so the furthest point has a view distance z of 1.0F
}

OS_Trans OS_trans[OS_MAX_TRANS];
std::int32_t OS_trans_upto;

void OS_transform(
		float     world_x,
		float     world_y,
		float     world_z,
		OS_Trans *os)
{
	os->x = world_x - OS_cam_x;
	os->y = world_y - OS_cam_y;
	os->z = world_z - OS_cam_z;

	MATRIX_MUL(
		OS_cam_matrix,
		os->x,
		os->y,
		os->z);

	os->clip = OS_CLIP_ROTATED;

	if (os->z < OS_ZCLIP_PLANE)
	{
		os->clip |= OS_CLIP_NEAR;

		return;
	}
	else
	if (os->z > 1.0F)
	{
		os->clip |= OS_CLIP_FAR;

		return;
	}
	else
	{
		//
		// The z-range of the point is okay.
		//

		os->Z = OS_ZCLIP_PLANE / os->z;

		os->X = OS_cam_screen_mid_x + OS_cam_screen_mul_x * os->x * os->Z;
		os->Y = OS_cam_screen_mid_y - OS_cam_screen_mul_y * os->y * os->Z;

		//
		// Set the clipping flags.
		//

		os->clip |= OS_CLIP_TRANSFORMED;

		     if (os->X < 0.0F           ) {os->clip |= OS_CLIP_LEFT;}
		else if (os->X > OS_screen_width) {os->clip |= OS_CLIP_RIGHT;}

		     if (os->Y < 0.0F            ) {os->clip |= OS_CLIP_TOP;}
		else if (os->Y > OS_screen_height) {os->clip |= OS_CLIP_BOTTOM;}

		return;
	}
}




// ========================================================
//
// DRAWING STUFF
//
// ========================================================

void OS_clear_screen(std::uint8_t r, std::uint8_t g, std::uint8_t b, float z)
{
	std::uint32_t colour = (r << 16) | (g << 8) | (b << 0);
	
	HRESULT ret = OS_frame.GetViewport()->Clear2(
								1,
								(D3DRECT *) OS_frame.GetViewportRect(),
								D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
								colour,
								z,
								0);
}



void OS_scene_begin()
{
	OS_frame.direct_3d->BeginScene();

	//
	// Set the render states to their default values.
	//

	OS_init_renderstates();
}

void OS_scene_end()
{
	OS_frame.direct_3d->EndScene();
}


void OS_fps()
{
	std::int32_t i;

	static std::int32_t fps;
	static std::int32_t last_time;
	static std::int32_t last_frame_count;
	static std::int32_t frame_count;

	float x1;
	float y1;
	float x2;
	float y2;

	float tick;

	std::int32_t now;

	now          = OS_ticks();
	frame_count += 1;

	if (now >= last_time + 1000)
	{
		fps              = frame_count - last_frame_count;
		last_frame_count = frame_count;
		last_time        = now;
	}

	OS_Buffer *ob = OS_buffer_new();

	for (i = 0; i < fps; i++)
	{
		switch((i + 1) % 10)
		{
			case 0:
				tick = 8.0F;
				break;

			case 5:
				tick = 5.0F;
				break;

			default:
				tick = 3.0F;
				break;
		}


		x1 = 5.0F + i * 4.0F;
		y1 = 5.0F;
		x2 = 5.0F + i * 4.0F + 2.0F;
		y2 = 5.0F + tick;

		x1 /= OS_screen_width;
		y1 /= OS_screen_height;
		x2 /= OS_screen_width;
		y2 /= OS_screen_height;

		OS_buffer_add_sprite(
			ob,
			x1,
			y1,
			x2,
			y2,
			0.0F, 1.0F,
			0.0F, 1.0F,
			0.0F,
			0x00ffffff,
			0x00000000,
			OS_FADE_BOTTOM);
	}

	OS_buffer_draw(ob, nullptr);
}


void OS_show()
{
	if (OS_frame_is_fullscreen)
	{
		//
		// Do we blit or do we flip?
		// 

		if (1)
		{
			//
			// Flip.
			// 

			OS_frame.ShowFrame();
		}
		else
		{
			//
			// Blit.
			//

			LPDIRECTDRAWSURFACE4 fb = OS_frame.GetFrontBuffer();
			LPDIRECTDRAWSURFACE4 bb = OS_frame.GetBackBuffer();

			fb->Blt(nullptr, bb, nullptr, DDBLT_WAIT, nullptr);
		}
	}
	else
	{
		OS_frame.ShowFrame();
	}
}

//
// Our flexible vertex format.
//

typedef struct
{
	float sx;
	float sy;
	float sz;
	float rhw;
	std::uint32_t colour;
	std::uint32_t specular;
	float tu1;
	float tv1;
	float tu2;
	float tv2;

} OS_Flert;

#define OS_FLERT_FORMAT	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2)

// ========================================================
//
// BUFFER STUFF
//
// ========================================================

typedef struct os_buffer
{
	std::int32_t num_flerts;
	std::int32_t num_indices;

	std::int32_t max_flerts;
	std::int32_t max_indices;

	OS_Flert *flert;
	std::uint16_t    *index;

	OS_Buffer *next;

} OS_Buffer;

OS_Buffer *OS_buffer_free;


//
// Creates a new buffer.
//

OS_Buffer *OS_buffer_create()
{
	//
	// Allocate the buffer.
	//

	OS_Buffer *ob = (OS_Buffer *) malloc(sizeof(OS_Buffer));

	//
	// Initialise the buffer.
	//

	ob->max_flerts  = 256;
	ob->max_indices = 1024;

	ob->num_flerts  = 0;
	ob->num_indices = 1;

	ob->flert = (OS_Flert *) malloc(sizeof(OS_Flert) * ob->max_flerts );
	ob->index = (std::uint16_t    *) malloc(sizeof(std::uint16_t   ) * ob->max_indices);

	memset(ob->flert, 0, sizeof(OS_Flert) * ob->max_flerts );
	memset(ob->index, 0, sizeof(std::uint16_t   ) * ob->max_indices);

	ob->next = nullptr;

	return ob;
}

//
// Gets a buffer from the linked list of free buffers. If the free list is empty,
// it creates a new one.
//

OS_Buffer *OS_buffer_get()
{
	OS_Buffer *ans;

	if (OS_buffer_free)
	{
		ans            = OS_buffer_free;
		OS_buffer_free = OS_buffer_free->next;
		ans->next      = nullptr;
	}
	else
	{
		ans = OS_buffer_create();
	}

	return ans;
}

//
// Returns a buffer to the free list.
//

void OS_buffer_give(OS_Buffer *ob)
{
	ob->next       = OS_buffer_free;
	OS_buffer_free = ob;
}

OS_Buffer *OS_buffer_new()
{
	OS_Buffer *ob = OS_buffer_get();

	ob->num_indices = 0;
	ob->num_flerts  = 1;

	return ob;
}

//
// Grows the size of the flert array.
//

void OS_buffer_grow_flerts(OS_Buffer *ob)
{
	ob->max_flerts *= 2;

	ob->flert = (OS_Flert *) realloc(ob->flert, ob->max_flerts * sizeof(OS_Flert));
}

void OS_buffer_grow_indices(OS_Buffer *ob)
{
	ob->max_indices *= 2;

	ob->index = (std::uint16_t *) realloc(ob->index, ob->max_indices * sizeof(std::uint16_t));
}

void OS_buffer_add_vert(OS_Buffer *ob, OS_Vert *ov)
{
	OS_Trans *ot;
	OS_Flert *of;

	//
	// Make sure we've got enough room for another vertex.
	//

	if (ob->num_flerts >= ob->max_flerts)
	{
		//
		// We need a bigger buffer.
		//

		OS_buffer_grow_flerts(ob);
	}

	ASSERT(WITHIN(ov->trans, 0, OS_MAX_TRANS - 1));

	of = &ob->flert[ob->num_flerts];
	ot = &OS_trans [ov->trans];

	//
	// Create the new tlvertex.
	//

	of->sx       = ot->X;
	of->sy       = ot->Y;
	of->sz       = 1.0F - ot->Z; //ot->z;
	of->rhw      = ot->Z;
	of->colour   = ov->colour;
	of->specular = ov->specular;
	of->tu1      = ov->u1;
	of->tv1      = ov->v1;
	of->tu2      = ov->u2;
	of->tv2      = ov->v2;

	//
	// Store the index of the flert inside the vertex.
	//

	ov->index = ob->num_flerts++;
}

void OS_buffer_add_triangle(
		OS_Buffer *ob,
		OS_Vert   *ov1,
		OS_Vert   *ov2,
		OS_Vert   *ov3)
{
	std::uint32_t clip_and =
			OS_trans[ov1->trans].clip &
			OS_trans[ov2->trans].clip &
			OS_trans[ov3->trans].clip;
	
	if (clip_and & OS_CLIP_TRANSFORMED)
	{
		if (clip_and & OS_CLIP_OFFSCREEN)
		{
			//
			// The triangle is completely off-screen.
			// 

			return;
		}
		else
		{
			if (!ov1->index ) {OS_buffer_add_vert(ob, ov1);}
			if (!ov2->index ) {OS_buffer_add_vert(ob, ov2);}
			if (!ov3->index ) {OS_buffer_add_vert(ob, ov3);}

			//
			// Add this triangle. All the points are transformed and at least
			// one is on screen.
			//

			if (ob->num_indices + 3 > ob->max_indices)
			{
				//
				// Need a bigger buffer.
				//

				OS_buffer_grow_indices(ob);
			}

			ob->index[ob->num_indices++] = ov1->index;
			ob->index[ob->num_indices++] = ov2->index;
			ob->index[ob->num_indices++] = ov3->index;

			return;
		}
	}
	else
	{
		std::uint32_t clip_or =
				OS_trans[ov1->trans].clip |
				OS_trans[ov2->trans].clip |
				OS_trans[ov3->trans].clip;

		if (clip_or & OS_CLIP_TRANSFORMED)
		{
			//
			// This triangle needs to be zclipped.
			//

			return;
		}
		else
		{
			//
			// The whole triangle is zclipped one way or another. We assume that
			// a single triangle is not going to span both the near and far zclip
			// planes...
			//

			return;
		}
	}
}


void OS_buffer_add_sprite(
		OS_Buffer *ob,
		float x1,			// Normalised to 0.0F - 1.0F
		float y1,			// Normalised to 0.0F - 1.0F
		float x2,			// Normalised to 0.0F - 1.0F
		float y2,			// Normalised to 0.0F - 1.0F
		float u1, float v1,
		float u2, float v2,
		float z,
		std::uint32_t colour,
		std::uint32_t specular,
		std::uint32_t fade)
{
	std::int32_t i;

	OS_Flert *of;

	//
	// Enough room in our buffer?
	//

	if (ob->num_indices + 6 > ob->max_indices) {OS_buffer_grow_indices(ob);}
	if (ob->num_flerts  + 4 > ob->max_flerts ) {OS_buffer_grow_flerts (ob);}

	//
	// Add four vertices.
	//
	
	for (i = 0; i < 4; i++)
	{
		of = &ob->flert[ob->num_flerts + i];

		of->sx       = ((i & 1) ? x1 : x2) * OS_screen_width;
		of->sy       = ((i & 2) ? y1 : y2) * OS_screen_height;
		of->sz       = z;
		of->rhw      = 0.5F;
		of->colour   = colour;
		of->specular = specular;
		of->tu1      = ((i & 1) ? u1 : u2);
		of->tv1      = ((i & 2) ? v1 : v2);
		of->tu2      = ((i & 1) ? u1 : u2);
		of->tv2      = ((i & 2) ? v1 : v2);
	}

	if (fade)
	{
		if (fade & OS_FADE_TOP)
		{
			ob->flert[ob->num_flerts + 2].colour = 0x00000000;
			ob->flert[ob->num_flerts + 3].colour = 0x00000000;
		}

		if (fade & OS_FADE_BOTTOM)
		{
			ob->flert[ob->num_flerts + 0].colour = 0x00000000;
			ob->flert[ob->num_flerts + 1].colour = 0x00000000;
		}

		if (fade & OS_FADE_LEFT)
		{
			ob->flert[ob->num_flerts + 1].colour = 0x00000000;
			ob->flert[ob->num_flerts + 3].colour = 0x00000000;
		}

		if (fade & OS_FADE_RIGHT)
		{
			ob->flert[ob->num_flerts + 0].colour = 0x00000000;
			ob->flert[ob->num_flerts + 2].colour = 0x00000000;
		}
	}

	//
	// Add two triangles.
	//

	ob->index[ob->num_indices + 0] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 1] = ob->num_flerts + 1;
	ob->index[ob->num_indices + 2] = ob->num_flerts + 3;

	ob->index[ob->num_indices + 3] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 4] = ob->num_flerts + 3;
	ob->index[ob->num_indices + 5] = ob->num_flerts + 2;

	ob->num_indices += 6;
	ob->num_flerts  += 4;
}



void OS_buffer_add_sprite_rot(
		OS_Buffer *ob,
		float x_mid,
		float y_mid,
		float size,			// As a percentage of the width of the screen.
		float angle,
		float u1, float v1,
		float u2, float v2,
		float z,
		std::uint32_t colour,
		std::uint32_t specular,
		float tu1, float tv1,
		float tu2, float tv2)
{
	std::int32_t i;

	OS_Flert *of;

	float dx = sin(angle) * size;
	float dy = cos(angle) * size;

	float x;
	float y;
	
	x_mid *= OS_screen_width;
	y_mid *= OS_screen_height;

	//
	// Enough room in our buffer?
	//

	if (ob->num_indices + 6 > ob->max_indices) {OS_buffer_grow_indices(ob);}
	if (ob->num_flerts  + 4 > ob->max_flerts ) {OS_buffer_grow_flerts (ob);}

	//
	// Add four vertices.
	//
	
	for (i = 0; i < 4; i++)
	{
		of = &ob->flert[ob->num_flerts + i];
		
		x = 0.0F;
		y = 0.0F;

		if (i & 1)
		{
			x += dx;
			y += dy;
		}
		else
		{
			x -= dx;
			y -= dy;
		}

		if (i & 2)
		{
			x += -dy;
			y += +dx;
		}
		else
		{
			x -= -dy;
			y -= +dx;
		}

		x *= OS_screen_width;
		y *= OS_screen_height * 1.33F;

		x += x_mid;
		y += y_mid;

		of->sx       = x;
		of->sy       = y;
		of->sz       = z;
		of->rhw      = 0.5F;
		of->colour   = colour;
		of->specular = specular;
		of->tu1      = ((i & 1) ?  u1 :  u2);
		of->tv1      = ((i & 2) ?  v1 :  v2);
		of->tu2      = ((i & 1) ? tu1 : tu2);
		of->tv2      = ((i & 2) ? tv1 : tv2);
	}

	//
	// Add two triangles.
	//

	ob->index[ob->num_indices + 0] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 1] = ob->num_flerts + 1;
	ob->index[ob->num_indices + 2] = ob->num_flerts + 3;

	ob->index[ob->num_indices + 3] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 4] = ob->num_flerts + 3;
	ob->index[ob->num_indices + 5] = ob->num_flerts + 2;

	ob->num_indices += 6;
	ob->num_flerts  += 4;
}



void OS_buffer_add_sprite_arbitrary(
		OS_Buffer *ob,
		float x1,			// Normalised to 0.0F - 1.0F
		float y1,			// Normalised to 0.0F - 1.0F
		float x2,			// Normalised to 0.0F - 1.0F
		float y2,			// Normalised to 0.0F - 1.0F
		float x3,			// Normalised to 0.0F - 1.0F
		float y3,			// Normalised to 0.0F - 1.0F
		float x4,			// Normalised to 0.0F - 1.0F
		float y4,			// Normalised to 0.0F - 1.0F
		float u1, float v1,
		float u2, float v2,
		float u3, float v3,
		float u4, float v4,
		float z ,
		std::uint32_t colour,
		std::uint32_t specular)
{
	std::int32_t i;

	float x;
	float y;
	float u;
	float v;

	OS_Flert *of;

	//
	// Enough room in our buffer?
	//

	if (ob->num_indices + 6 > ob->max_indices) {OS_buffer_grow_indices(ob);}
	if (ob->num_flerts  + 4 > ob->max_flerts ) {OS_buffer_grow_flerts (ob);}

	//
	// Add four vertices.
	//
	
	for (i = 0; i < 4; i++)
	{
		of = &ob->flert[ob->num_flerts + i];

		switch(i)
		{
			case 0:
				x = x1;
				y = y1;
				u = u1;
				v = v1;
				break;

			case 1:
				x = x2;
				y = y2;
				u = u2;
				v = v2;
				break;

			case 2:
				x = x3;
				y = y3;
				u = u3;
				v = v3;
				break;

			case 3:
				x = x4;
				y = y4;
				u = u4;
				v = v4;
				break;
			
			default:
				ASSERT(0);
				break;
		}		

		x *= OS_screen_width;
		y *= OS_screen_height;

		of->sx       = x;
		of->sy       = y;
		of->sz       = z;
		of->rhw      = 0.5F;
		of->colour   = colour;
		of->specular = specular;
		of->tu1      = u;
		of->tv1      = v;
		of->tu2      = u;
		of->tv2      = v;
	}

	//
	// Add two triangles.
	//

	ob->index[ob->num_indices + 0] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 1] = ob->num_flerts + 1;
	ob->index[ob->num_indices + 2] = ob->num_flerts + 3;

	ob->index[ob->num_indices + 3] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 4] = ob->num_flerts + 3;
	ob->index[ob->num_indices + 5] = ob->num_flerts + 2;

	ob->num_indices += 6;
	ob->num_flerts  += 4;
}


void OS_buffer_add_line_3d(
		OS_Buffer *ob,
		float X1,
		float Y1,
		float X2,
		float Y2,
		float width,
		float u1, float v1,
		float u2, float v2,
		float z1,
		float z2,
		std::uint32_t colour,
		std::uint32_t specular)
{
	std::int32_t i;

	OS_Flert *of;

	float dx;
	float dy;
	float len;
	float overlen;

	float x;
	float y;
	
	//
	// Enough room in our buffer?
	//

	if (ob->num_indices + 6 > ob->max_indices) {OS_buffer_grow_indices(ob);}
	if (ob->num_flerts  + 4 > ob->max_flerts ) {OS_buffer_grow_flerts (ob);}

	//
	// The width of the line.
	//
	
	dx = X2 - X1;
	dy = Y2 - Y1;

	len     = qdist2(fabs(dx),fabs(dy));
	overlen = width * OS_screen_width / len;

	dx *= overlen;
	dy *= overlen;

	//
	// Add four vertices.
	//
	
	for (i = 0; i < 4; i++)
	{
		of = &ob->flert[ob->num_flerts + i];
		
		x = 0.0F;
		y = 0.0F;

		if (i & 1)
		{
			x += -dy;
			y += +dx;
		}
		else
		{
			x -= -dy;
			y -= +dx;
		}

		if (i & 2)
		{
			x += X1;
			y += Y1;
		}
		else
		{
			x += X2;
			y += Y2;
		}

		of->sx       = x;
		of->sy       = y;
		of->sz       = ((i & 2) ? z1 : z2);
		of->rhw      = 0.5F;
		of->colour   = colour;
		of->specular = specular;
		of->tu1      = ((i & 1) ? u1 : u2);
		of->tv1      = ((i & 2) ? v1 : v2);
		of->tu2      = ((i & 1) ? u1 : u2);
		of->tv2      = ((i & 2) ? v1 : v2);
	}

	//
	// Add two triangles.
	//

	ob->index[ob->num_indices + 0] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 1] = ob->num_flerts + 1;
	ob->index[ob->num_indices + 2] = ob->num_flerts + 3;

	ob->index[ob->num_indices + 3] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 4] = ob->num_flerts + 3;
	ob->index[ob->num_indices + 5] = ob->num_flerts + 2;

	ob->num_indices += 6;
	ob->num_flerts  += 4;
}




void OS_buffer_add_line_2d(
		OS_Buffer *ob,
		float x1,
		float y1,
		float x2,
		float y2,
		float width,
		float u1, float v1,
		float u2, float v2,
		float z,
		std::uint32_t colour,
		std::uint32_t specular)
{
	std::int32_t i;

	OS_Flert *of;

	float dx;
	float dy;
	float len;
	float overlen;

	float x;
	float y;
	
	//
	// Enough room in our buffer?
	//

	if (ob->num_indices + 6 > ob->max_indices) {OS_buffer_grow_indices(ob);}
	if (ob->num_flerts  + 4 > ob->max_flerts ) {OS_buffer_grow_flerts (ob);}

	//
	// The width of the line.
	//
	
	x1 *= OS_screen_width;
	y1 *= OS_screen_height;

	x2 *= OS_screen_width;
	y2 *= OS_screen_height;

	dx = x2 - x1;
	dy = y2 - y1;

	len     = qdist2(fabs(dx),fabs(dy));
	overlen = width * OS_screen_width / len;

	dx *= overlen;
	dy *= overlen;

	//
	// Add four vertices.
	//
	
	for (i = 0; i < 4; i++)
	{
		of = &ob->flert[ob->num_flerts + i];
		
		x = 0.0F;
		y = 0.0F;

		if (i & 1)
		{
			x += -dy;
			y += +dx;
		}
		else
		{
			x -= -dy;
			y -= +dx;
		}

		if (i & 2)
		{
			x += x1;
			y += y1;
		}
		else
		{
			x += x2;
			y += y2;
		}

		of->sx       = x;
		of->sy       = y;
		of->sz       = z;
		of->rhw      = 0.5F;
		of->colour   = colour;
		of->specular = specular;
		of->tu1      = ((i & 1) ? u1 : u2);
		of->tv1      = ((i & 2) ? v1 : v2);
		of->tu2      = ((i & 1) ? u1 : u2);
		of->tv2      = ((i & 2) ? v1 : v2);
	}

	//
	// Add two triangles.
	//

	ob->index[ob->num_indices + 0] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 1] = ob->num_flerts + 1;
	ob->index[ob->num_indices + 2] = ob->num_flerts + 3;

	ob->index[ob->num_indices + 3] = ob->num_flerts + 0;
	ob->index[ob->num_indices + 4] = ob->num_flerts + 3;
	ob->index[ob->num_indices + 5] = ob->num_flerts + 2;

	ob->num_indices += 6;
	ob->num_flerts  += 4;
}



void OS_buffer_draw(
		OS_Buffer  *ob,
		OS_Texture *ot1,
		OS_Texture *ot2,
		std::uint32_t       draw)
{
	LPDIRECT3DDEVICE3 d3d = OS_frame.direct_3d;

	if (ob->num_flerts == 0)
	{
		//
		// Empty buffer!
		//

		OS_buffer_give(ob);

		return;
	}

	if (!ot1 )
	{
		//
		// No texturing.
		//

		draw |= OS_DRAW_TEX_NONE;
	}
	else
	{
		//
		// Make this texture the input into the pipeline.
		//
		
		d3d->SetTexture(0, ot1->ddtx);
	}

	if (ot2)
	{
		//
		// Make this texture the input into the second stage of the pipeline.
		//

		d3d->SetTexture(1, ot2->ddtx);
	}

	//
	// Update renderstates.
	//

	if (draw != OS_DRAW_NORMAL)
	{
		OS_change_renderstate_for_type(draw);
	}

	{
		//
		// Check that this will be okay.
		//
	
		std::uint32_t num_passes;

		if (d3d->ValidateDevice(&num_passes) != D3D_OK)
		{
			OS_string("Validation failed: draw = 0x%x\n", draw);
		}
	}

	//
	// Draw the triangles.
	//

	d3d->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			OS_FLERT_FORMAT,
			ob->flert,
			ob->num_flerts,
			ob->index,
			ob->num_indices,
			D3DDP_DONOTUPDATEEXTENTS);

	//
	// Put state back to normal.
	//

	if (draw != OS_DRAW_NORMAL)
	{
		OS_undo_renderstate_type_changes();
	}

	//
	// Returns the buffer to the free list.
	//

	OS_buffer_give(ob);
}





