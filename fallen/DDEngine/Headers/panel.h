#ifndef	AENG_PANEL_HPP

#define	AENG_PANEL_HPP


extern std::uint8_t	PANEL_scanner_poo; // sets whether you want a poo scanner or not

extern void	PANEL_start();
extern void	PANEL_draw_gun_sight(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t radius,std::int32_t scale);
extern void	PANEL_draw_timer(std::int32_t time_in_hundreths, std::int32_t x, std::int32_t y);
extern void	PANEL_draw_buffered();	// Actually draws the timers....
extern void	PANEL_draw_health_bar(std::int32_t x,std::int32_t y,std::int32_t percentage);
#if 0
// Never used!
extern void	PANEL_draw_angelic_status(std::int32_t x, std::int32_t y, std::int32_t size, std::int32_t am_i_an_angel);
extern void	PANEL_draw_press_button(std::int32_t x, std::int32_t y, std::int32_t size, std::int32_t frame);	// Even/odd frame
#endif
extern void	PANEL_finish();
extern void	PANEL_inventory(Thing *darci, Thing *player);


//
// The new funky panel!
//
#ifdef PSX
void PANEL_new_funky();
#endif

//
// Clears all new text messages.
//

void PANEL_new_text_init();

//
// The new funky messages-from-people system.
//

void PANEL_new_text(Thing *who, std::int32_t delay, char* fmt, ...);


//
// Help system messages.
//

void PANEL_new_help_message(char* fmt, ...);



//
// Messages that give you info. They go where the beacon text
// normally goes.
//

void PANEL_new_info_message(char* fmt, ...);


//
// Flashes up a sign on the screen for the next few second.
//

#define PANEL_SIGN_WHICH_UTURN                0
#define PANEL_SIGN_WHICH_TURN_RIGHT           1
#define PANEL_SIGN_WHICH_DONT_TAKE_RIGHT_TURN 2
#define PANEL_SIGN_WHICH_STOP                 3

#define PANEL_SIGN_FLIP_LEFT_AND_RIGHT (1 << 0)
#define PANEL_SIGN_FLIP_TOP_AND_BOTTOM (1 << 1)

void PANEL_flash_sign(std::int32_t which, std::int32_t flip);


//
// Darkens the screen where x goes from 0 to 640.
//

void PANEL_darken_screen(std::int32_t x);


//
// How to fade out the screen.
//

void  PANEL_fadeout_init    ();
void  PANEL_fadeout_start   ();	// Only starts if the fadeout is not already going...
void  PANEL_fadeout_draw    ();
std::int32_t PANEL_fadeout_finished();




//
// The last panel for Urban Chaos (with a bit of luck).
//

void PANEL_last();



//
// Where  completion goes from 0 to 256
//

void PANEL_draw_completion_bar(std::int32_t completion);



// Returns a value to use for RHW that says "this is on top".
// Use 1.0f - this for a Z value.
// Each time this is called, it increases a little to help
// with cards that depth buffer oddly.
float PANEL_GetNextDepthBodge();

// Call at the start of each frame.
void PANEL_ResetDepthBodge();


// Screensaver stuff.
void PANEL_enable_screensaver();
void PANEL_disable_screensaver ( bool bImmediately=false );
void PANEL_screensaver_draw();



void PANEL_draw_quad(
				float left,
				float top,
				float right,
				float bottom,
				std::int32_t page,
				std::uint32_t colour = 0xffffffff,
				float u1 = 0.0F,
				float v1 = 0.0F,
				float u2 = 1.0F,
				float v2 = 1.0F);


#endif
