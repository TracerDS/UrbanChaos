//
// Interface to all the OS functions.
//

#pragma once

#include <cstdint>

// ========================================================
//
// WINDOWS AND MISCELLANEOUS STUFF
//
// ========================================================

//
// Processes the windows messages.  If windows wants the program to
// quit it returns OS_QUIT_FRAME.
//

#define OS_CARRY_ON	 0
#define OS_QUIT_GAME 1

std::int32_t OS_process_messages();

//
// Outputs a debug string.
// Returns the number of milliseconds since MAIN_main was called. It is very accurate.
// Resets OS_ticks()
// Returns the Mhz of the current processor.
//

void OS_string       (char* fmt, ...);
void OS_reset_ticks  ();
std::int32_t OS_ticks        ();
void OS_ticks_reset  ();


// ========================================================
//
// TEXTURE STUFF
//
// ========================================================

typedef struct os_texture OS_Texture;

//
// You can create a texture either from a TGA file or just by
// specifying a size and format.  If the file could not be loaded
// then a blank texture is used instead.  If you try and create
// a texture from the same TGA file twice, the same pointer will
// be returned both times.
//

#define OS_TEXTURE_FORMAT_RGB	 0		// RGB
#define OS_TEXTURE_FORMAT_1555	 1		// ARGB
#define OS_TEXTURE_FORMAT_4444	 2		// ARGB
#define OS_TEXTURE_FORMAT_8		 3		// Grayscale
#define OS_TEXTURE_FORMAT_NUMBER 4

OS_Texture *OS_texture_create(char* fname, std::int32_t invert = false);
OS_Texture *OS_texture_create(std::int32_t size,   std::int32_t format);

//
// Call this function once you have loaded all textures.  It ensures that
// as many of the textures as possible start off on the card instead of
// only in the texture cache in system memory.
//

void OS_texture_finished_creating();

//
// Returns the size of the texture.
//

std::int32_t OS_texture_size(OS_Texture *ot);

//
// To updating a texture yourself, lock the texture and write
// into the bitmap. When you unlock it, the new texture will
// be blitted to the hardware.
//
// You're not allowed to READ from the texture! Only write.
//
// On success one of OS_bitmap_uword_screen or OS_bitmap_ubyte_screen will
// point to the screen memory.
//

void OS_texture_lock  (OS_Texture *ot);		// OS_bitmap_screen == nullptr => lock failed.
void OS_texture_unlock(OS_Texture *ot);

//
// The bitmap returned by OS_texture_lock. To create a pixel, use the masks and
// shifts like this:
//
//	pixel  = (r >> mask_r) << shift_r;
//	pixel |= (g >> mask_g) << shift_g;
//	pixel |= (b >> mask_b) << shift_b;
//
// Include alpha only if the original texture had alpha.
//

extern std::int32_t  OS_bitmap_format;			// OS_TEXTURE_FORMAT_*
extern std::uint16_t *OS_bitmap_uword_screen;	// For 16-bit formats.
extern std::int32_t  OS_bitmap_uword_pitch;	// Pitch in UWORDS
extern std::uint8_t *OS_bitmap_ubyte_screen;	// For the grayscale format.
extern std::int32_t  OS_bitmap_ubyte_pitch;	// Pitch in UBYTES
extern std::int32_t  OS_bitmap_width;
extern std::int32_t  OS_bitmap_height;
extern std::int32_t  OS_bitmap_mask_r;
extern std::int32_t  OS_bitmap_mask_g;
extern std::int32_t  OS_bitmap_mask_b;
extern std::int32_t  OS_bitmap_mask_a;
extern std::int32_t  OS_bitmap_shift_r;
extern std::int32_t  OS_bitmap_shift_g;
extern std::int32_t  OS_bitmap_shift_b;
extern std::int32_t  OS_bitmap_shift_a;

//
// Writes a pixel to a 16-bit bitmap.
//

#define OS_BITMAP_UWORD_PIXEL(x,y)      (OS_bitmap_uword_screen + (x) + (y) * OS_bitmap_uword_pitch)
#define OS_BITMAP_UWORD_COLOUR(r,g,b)   ((((r) >> OS_bitmap_mask_r) << OS_bitmap_shift_r) | (((g) >> OS_bitmap_mask_g) << OS_bitmap_shift_g) | (((b) >> OS_bitmap_mask_b) << OS_bitmap_shift_b))
#define OS_BITMAP_UWORD_PLOT(x,y,r,g,b) {*OS_BITMAP_UWORD_PIXEL(x,y) = OS_BITMAP_UWORD_COLOUR(r,g,b);}

//
// Write a pixel to an 8-bit bitmap
//

#define OS_BITMAP_UBYTE_PIXEL(x,y)	(OS_bitmap_ubyte_screen + (x) + (y) * OS_bitmap_ubyte_pitch)
#define OS_BITMAP_UBYTE_PLOT(x,y,c) {*OS_BITMAP_UBYTE_PIXEL(x,y) = (c);}


// ========================================================
//
// THE CAMERA
//
// ========================================================

//
// Sets up the camera used to transform points added to the OS_Buffers.
//

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
		float screen_y2);

extern float OS_cam_x;	// For reference only! Use the function above to set the camera
extern float OS_cam_y;
extern float OS_cam_z;
extern float OS_cam_lens;
extern float OS_cam_view_matrix[9];

//
// The clipping flags.
//

#define OS_CLIP_TOP			(1 << 0)
#define OS_CLIP_BOTTOM		(1 << 1)
#define OS_CLIP_LEFT		(1 << 2)
#define OS_CLIP_RIGHT		(1 << 3)
#define OS_CLIP_FAR			(1 << 4)
#define OS_CLIP_NEAR		(1 << 5)
#define OS_CLIP_ROTATED		(1 << 6)
#define OS_CLIP_TRANSFORMED	(1 << 7)

#define OS_CLIP_OFFSCREEN	(OS_CLIP_TOP|OS_CLIP_BOTTOM|OS_CLIP_LEFT|OS_CLIP_RIGHT)
#define OS_ZCLIP_PLANE		(64.0F / 65536.0F)

//
// Transformed points.
//

typedef struct
{
	float x;
	float y;
	float z;

	float X;
	float Y;
	float Z;

	std::uint32_t clip;

} OS_Trans;

#define OS_MAX_TRANS 8192

extern OS_Trans OS_trans[OS_MAX_TRANS];
extern std::int32_t    OS_trans_upto;

//
// Transforming points.
//

void OS_transform(
		float     world_x,
		float     world_y,
		float     world_z,
		OS_Trans *os);


// ========================================================
//
// BUFFERS FOR COLLECTING TOGETHER POINTS AND TRIANGLES
//
// ========================================================

typedef struct os_buffer OS_Buffer;

typedef struct
{
	std::uint16_t trans;	// Index into the OS_trans array for the transformed point.
	std::uint16_t index;	// Before you add any OS_Verts to a buffer make sure they all have this field set to nullptr
	float u1;
	float v1;
	float u2;		// For multitexturing.
	float v2;
	std::uint32_t colour;
	std::uint32_t specular;

} OS_Vert;

//
// Returns a new buffer to which you can add triangles.
//

OS_Buffer *OS_buffer_new();

//
// Adds a triangle to the buffer. The triangle is given by three vertices
// into an OS_Vert array whose tlvert field is all set to zero.
//

void OS_buffer_add_triangle(
		OS_Buffer *ob,
		OS_Vert   *ov1,
		OS_Vert   *ov2,
		OS_Vert   *ov3);

//
// Adds a 2D sprite to the buffer.
// 

#define OS_FADE_TOP    (1 << 0)
#define OS_FADE_BOTTOM (1 << 1)
#define OS_FADE_LEFT   (1 << 2)
#define OS_FADE_RIGHT  (1 << 3)

void OS_buffer_add_sprite(
		OS_Buffer *ob,
		float x1,			// Normalised to 0.0F - 1.0F
		float y1,			// Normalised to 0.0F - 1.0F
		float x2,			// Normalised to 0.0F - 1.0F
		float y2,			// Normalised to 0.0F - 1.0F
		float u1 = 0.0F, float v1 = 0.0F,
		float u2 = 1.0F, float v2 = 1.0F,
		float z  = 0.0F,
		std::uint32_t colour   = 0x00ffffff,
		std::uint32_t specular = 0x00000000,
		std::uint32_t fade     = 0);

//
// Adds a rotated 2D sprite to the buffer.
//

void OS_buffer_add_sprite_rot(
		OS_Buffer *ob,
		float x_mid,
		float y_mid,
		float radius,			// As a percentage of the width of the screen.
		float angle,
		float u1 = 0.0F, float v1 = 0.0F,
		float u2 = 1.0F, float v2 = 1.0F,
		float z  = 0.0F,
		std::uint32_t colour   = 0x00ffffff,
		std::uint32_t specular = 0x00000000,
		float tu1 = 0.0F, float tv1 = 0.0F,	// For the second pass...
		float tu2 = 1.0F, float tv2 = 1.0F);

//
// Adds an arbitrary-shaped sprite to the buffer. The points are 
// given in this order.
//
//    0-------1
//	 /		   \
//	2-----------3

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
		float u1 = 0.0F, float v0 = 0.0F,
		float u2 = 1.0F, float v2 = 0.0F,
		float u3 = 0.0F, float v3 = 1.0F,
		float u4 = 1.0F, float v4 = 1.0F,
		float z  = 0.0F,
		std::uint32_t colour   = 0x00ffffff,
		std::uint32_t specular = 0x00000000);


//
// Adds a line.
//

void OS_buffer_add_line_2d(
		OS_Buffer *ob,
		float x1,		// Normalised to 0.0F - 1.0F
		float y1,		// Normalised to 0.0F - 1.0F
		float x2,		// Normalised to 0.0F - 1.0F
		float y2,		// Normalised to 0.0F - 1.0F
		float width = 0.01F,	// As a percentage of the width of the screen.
		float u1 = 0.0F, float v1 = 0.0F,
		float u2 = 1.0F, float v2 = 1.0F,
		float z  = 0.0F,
		std::uint32_t colour   = 0x00ffffff,
		std::uint32_t specular = 0x00000000);

void OS_buffer_add_line_3d(
		OS_Buffer *ob,
		float X1,		// These are in REAL SCREEN COORDINATES!
		float Y1,		// These are in REAL SCREEN COORDINATES!
		float X2,		// These are in REAL SCREEN COORDINATES!
		float Y2,		// These are in REAL SCREEN COORDINATES!
		float width = 0.01F,	// As a percentage of the width of the screen.
		float u1 = 0.0F, float v1 = 0.0F,
		float u2 = 1.0F, float v2 = 1.0F,
		float z1 = 0.0F,
		float z2 = 0.0F,
		std::uint32_t colour   = 0x00ffffff,
		std::uint32_t specular = 0x00000000);




//
// Draws the buffer in the way defined by the OS_DRAW_* flags and the texture. After
// drawing a buffer it becomes invalid. You can't add triangles to it anymore.
//

#define OS_DRAW_NORMAL			0		// Normal zbuffered gouraud textured triangle
#define OS_DRAW_ADD			(1 <<  0)	// Pixel = source + dest
#define OS_DRAW_MULTIPLY	(1 <<  1)	// Pixel = source * dest * 2
#define OS_DRAW_CLAMP		(1 <<  2)	// No texture wrapping
#define OS_DRAW_DECAL		(1 <<  3)	// No gouraud
#define OS_DRAW_TRANSPARENT	(1 <<  4)	// Just the zbuffer written to (Pixel = dest)
#define OS_DRAW_DOUBLESIDED	(1 <<  5)	// Don't backface cull
#define OS_DRAW_NOZWRITE	(1 <<  6)	// Don't write to the zbuffer
#define OS_DRAW_ALPHAREF	(1 <<  7)	// Don't draw if alpha channel is zero
#define OS_DRAW_ZREVERSE	(1 <<  8)	// Reverses the order or the zbuffer
#define OS_DRAW_ZALWAYS		(1 <<  9)	// Never z-reject.
#define OS_DRAW_CULLREVERSE (1 << 10)	// Flip the direction of backface culling
#define OS_DRAW_NODITHER    (1 << 11)	// Turn of gouraud-shade dithering
#define OS_DRAW_ALPHABLEND	(1 << 12)	// Alphablending
#define OS_DRAW_TEX_NONE	(1 << 13)	// No texture mapping
#define OS_DRAW_TEX_MUL		(1 << 14)	// Two textures multiplied together
#define OS_DRAW_NOFILTER	(1 << 15)	// No filtering
#define OS_DRAW_MULBYONE	(1 << 16)	// Pixel = source * dest * 1

void OS_buffer_draw(
		OS_Buffer  *ob,
		OS_Texture *ot1,		// nullptr => No texture mapping
		OS_Texture *ot2  = nullptr,
		std::uint32_t       draw = OS_DRAW_NORMAL);


// ========================================================
//
// RENDER LOOP STUFF
//
// ========================================================

extern float OS_screen_width;
extern float OS_screen_height;

//
// Call at the beginning and end of drawing the triangles.
//

void OS_scene_begin();
void OS_scene_end  ();

//
// Clears the screen.
//

void OS_clear_screen(
		std::uint8_t r = 0,
		std::uint8_t g = 0,
		std::uint8_t b = 0,
		float z = 1.0F);

//
// Draws the FPS onto the screen. Call ONCE a frame!
//

void OS_fps();

//
// Flips the screen.
//

void OS_show();


// ========================================================
//
// MOUSE STUFF
//
// ========================================================

void OS_mouse_get(std::int32_t *x, std::int32_t *y);
void OS_mouse_set(std::int32_t  x, std::int32_t  y);

// ========================================================
//
// SOUND STUFF
//
// ========================================================

//
// Call once at the start of the whole program.
// Starts the soundtrack.
// Sets the volume. It start off at 1.0F- the maximum.
// 

void OS_sound_init  ();
void OS_sound_start ();
void OS_sound_volume(float vol);


//
// Hacked in stuff...
//

void OS_sound_loop_start();
void OS_sound_loop_process();


// ========================================================
//
// JOYSTICK INPUT
//
// ========================================================

//
// These values are updated after each call to OS_process_messages()
// 

extern float OS_joy_x;				// -1.0F to +1.0F
extern float OS_joy_y;				// -1.0F to +1.0F
extern std::uint32_t OS_joy_button;			// The buttons that are currently down
extern std::uint32_t OS_joy_button_down;	// The buttons that have just been pressed
extern std::uint32_t OS_joy_button_up;		// The buttons that have just been released