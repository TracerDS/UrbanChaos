//
// A font! That's all there is to it.
//

#include "always.h"
#include "font.h"
#include "os.h"
#include "tga.h"


//
// Each of the letters
//

typedef struct
{
	float u;
	float v;
	float uwidth;

} FONT_Letter;

#define FONT_LETTER_HEIGHT 20

#define FONT_LOWERCASE		 0
#define FONT_UPPERCASE		 26
#define FONT_NUMBERS		 52
#define FONT_PUNCT_PLING	 62
#define FONT_PUNCT_DQUOTE	 63
#define FONT_PUNCT_POUND	 64
#define FONT_PUNCT_DOLLAR	 65
#define FONT_PUNCT_PERCENT	 66
#define FONT_PUNCT_POWER	 67
#define FONT_PUNCT_AMPERSAND 68
#define FONT_PUNCT_ASTERISK	 69
#define FONT_PUNCT_OPEN		 70
#define FONT_PUNCT_CLOSE	 71
#define FONT_PUNCT_COPEN	 72
#define FONT_PUNCT_CCLOSE	 73
#define FONT_PUNCT_SOPEN	 74
#define FONT_PUNCT_SCLOSE	 75
#define FONT_PUNCT_LT		 76
#define FONT_PUNCT_GT		 77
#define FONT_PUNCT_BSLASH	 78
#define FONT_PUNCT_FSLASH	 79
#define FONT_PUNCT_COLON	 80
#define FONT_PUNCT_SEMICOLON 81
#define FONT_PUNCT_QUOTE	 82
#define FONT_PUNCT_AT		 83
#define FONT_PUNCT_HASH		 84
#define FONT_PUNCT_TILDE	 85
#define FONT_PUNCT_QMARK	 86
#define FONT_PUNCT_MINUS	 87
#define FONT_PUNCT_EQUALS	 88
#define FONT_PUNCT_PLUS		 89
#define FONT_PUNCT_DOT		 90
#define FONT_PUNCT_COMMA	 81
#define FONT_NUM_LETTERS	 92

FONT_Letter FONT_letter[FONT_NUM_LETTERS];

//
// This is the order the punctuation characters come in.
//

char FONT_punct[] =
{
	"!\"�$%^&*(){}[]<>\\/:;'@#~?-=+.,"
};


//
// The texture and another place where we store the font data
// apart from the texture!
//

OS_Texture *FONT_ot;
TGA_Pixel FONT_data[256][256];



//
// The format of the text.
//

std::uint32_t FONT_flag;
float FONT_bbox_x1;
float FONT_bbox_y1;
float FONT_bbox_x2;
float FONT_bbox_y2;



void FONT_format(
		std::uint32_t flag,
		float bbox_x1,
		float bbox_y1,
		float bbox_x2,
		float bbox_y2)
{
	FONT_flag    = flag;
	FONT_bbox_x1 = bbox_x1;
	FONT_bbox_y1 = bbox_y1;
	FONT_bbox_x2 = bbox_x2;
	FONT_bbox_y2 = bbox_y2;
}



//
// Returns true if it finds pixel data at (x,y)
// 

std::int32_t FONT_found_data(std::int32_t x, std::int32_t y)
{
	std::int32_t dy;

	std::int32_t px;
	std::int32_t py;

	ASSERT(WITHIN(x, 0, 255));

	for (dy = -15; dy <= 4; dy++)
	{
		px = x;
		py = y + dy;

		if (WITHIN(py, 0, 255))
		{
			if (FONT_data[255- py][px].alpha)
			{
				return true;
			}
		}
	}

	return false;
}



void FONT_init()
{
	std::int32_t i;
	std::int32_t y;
	std::int32_t x;
	std::int32_t line;

	FONT_Letter *fl;

	//
	// Load the texture.
	//

	FONT_ot = OS_texture_create("font.tga");

	//
	// Load in the font bitmap.
	//

	TGA_Info  ti;

	ti = TGA_load(
			"Textures\\font.tga",
			256,
			256,
		   &FONT_data[0][0]);

	ASSERT(ti.valid);
	ASSERT(ti.width  == 256);
	ASSERT(ti.height == 256);

	//
	// Work out the position of each of the letters.
	//

	#define FONT_NUM_BASELINES 8

	std::int32_t baseline[FONT_NUM_BASELINES] =
	{
		17, 42, 67, 90, 113, 134, 160
	};

	x    = 0;
	y    = baseline[0];
	line = 0;

	for (i = 0; i < FONT_NUM_LETTERS; i++)
	{
		fl = &FONT_letter[i];

		//
		// Look for the start of the letter.
		//

		while(!FONT_found_data(x,y))
		{
			x += 1;

			if (x >= 256)
			{
				x     = 0;
				line += 1;

				ASSERT(WITHIN(line, 0, FONT_NUM_BASELINES - 1));

				y = baseline[line];
			}
		}

		fl->u = float(x);
		fl->v = float(y);

		//
		// Look for the end of the letter.
		//

		x += 3;

		while(FONT_found_data(x,y))
		{
			x += 1;
		}

		fl->uwidth = (x - fl->u) * (1.0F / 256.0F);

		//
		// Convert the (u,v)s
		//

		fl->u *= 1.0F / 256.0F;
		fl->v *= 1.0F / 256.0F;

		fl->v -= 15.0F / 256.0F;
	}
}


//
// Returns the index of the given character
// 

std::int32_t FONT_get_index(char chr)
{
	std::int32_t letter;

	//
	// Find our letter index.
	//

	if (WITHIN(chr, 'a', 'z'))
	{
		letter = FONT_LOWERCASE + chr - 'a';
	}
	else
	if (WITHIN(chr, 'A', 'Z'))
	{
		letter = FONT_UPPERCASE + chr - 'A';
	}
	else
	if (WITHIN(chr, '0', '9'))
	{
		letter = FONT_NUMBERS + chr - '0';
	}
	else
	{
		//
		// Look for the punctuation letter.
		//

		letter = FONT_PUNCT_PLING;

		for (char* ch = FONT_punct; *ch && *ch != chr; ch++, letter++);
	}

	if (!WITHIN(letter, 0, FONT_NUM_LETTERS - 1))
	{
		letter = FONT_PUNCT_QMARK;
	}

	return letter;
}


std::int32_t FONT_char_is_valid(char ch)
{
	if (FONT_get_index(ch) == FONT_PUNCT_QMARK && ch != '?')
	{
		return false;
	}
	else
	{
		return true;
	}
}



float FONT_get_letter_width(char chr)
{
	std::int32_t letter;

	if (chr == ' ')
	{
		return 8.0F / 256.0F;
	}

	letter = FONT_get_index(chr);

	ASSERT(WITHIN(letter, 0, FONT_NUM_LETTERS - 1));

	return FONT_letter[letter].uwidth + (1.0F / 256.0F);
}



float FONT_draw_letter(
		OS_Buffer *ob,
		char      chr,
		float      x,
		float      y,
		std::uint32_t      colour = 0xffffffff,
		float      scale  = 1.0F)
{
	std::int32_t letter;
	float width;

	FONT_Letter *fl;

	//
	// Space is a special case!
	// 

	if (chr == ' ')
	{
		width = (8.0F / 256.0F) * scale;
	}
	else
	{
		letter = FONT_get_index(chr);

		ASSERT(WITHIN(letter, 0, FONT_NUM_LETTERS - 1));

		fl = &FONT_letter[letter];

		width = fl->uwidth;

		OS_buffer_add_sprite(
			ob,
			x,
			y,
			x + fl->uwidth * scale,
			y + (FONT_LETTER_HEIGHT * 1.33F / 256.0F) * scale,
			fl->u,
			1.0F - (fl->v),
			fl->u + fl->uwidth,
			1.0F - (fl->v + (FONT_LETTER_HEIGHT / 256.0F)),
			0.0F,
			colour);
	}

	return (width + 1.0F / 256.0F) * scale;
}

//
// Returns the width of the given string.
//

float FONT_get_width(char* str, float scale)
{
	float ans = 0.0F;

	for (char* ch = str; *ch; ch++)
	{
		ans += FONT_get_letter_width(*ch) * scale;
	}

	return ans;
}




void FONT_format(
		std::int32_t flag = FONT_FLAG_JUSTIFY_LEFT,
		float bbox_x1 = 0.0F, float bbox_y1 = 0.0F,
		float bbox_x2 = 1.0F, float bbox_y2 = 1.0F)
{
	FONT_flag    = flag;
	FONT_bbox_x1 = bbox_x1;
	FONT_bbox_y1 = bbox_y1;
	FONT_bbox_x2 = bbox_x2;
	FONT_bbox_y2 = bbox_y2;
}



void FONT_draw(float start_x, float start_y, std::uint32_t colour, float scale, std::int32_t cursor, char* fmt, ...)
{
	char   message[4096];
	va_list	ap;

	if (!fmt )
	{
		sprintf(message, "<nullptr>");
	}
	else
	{
		va_start(ap, fmt);
		vsprintf(message, fmt, ap);
		va_end  (ap);
	}
	
	//
	// So that a scale of 1.0F is normal size.
	//

	scale *= 0.5F;
	   
	//
	// The buffer we use to hold the sprites.
	//

	OS_Buffer *ob = OS_buffer_new();

	//
	// Make sure the colour component has alpha- otherwise the
	// font will be invisible!
	//

	colour |= 0xff000000;

	float x = start_x;
	float y = start_y;

	if (FONT_flag & FONT_FLAG_JUSTIFY_CENTRE)
	{
		x -= FONT_get_width(message, scale) * 0.5F;
	}
	else
	if (FONT_flag & FONT_FLAG_JUSTIFY_RIGHT)
	{
		x -= FONT_get_width(message, scale);
	}

	char* ch = message;

	while(*ch)
	{
		if (iscntrl(*ch))
		{
			if (*ch == '\n')
			{
				x  = start_x;
				y += (FONT_LETTER_HEIGHT + 1.0F) * scale;
			}
		}
		else
		{
			if (cursor-- == 0)
			{
				//
				// Draw a cursor here.
				//

				{
					OS_Buffer *ob = OS_buffer_new();

					OS_buffer_add_sprite(
						ob,
						x, y, x + 0.01F * scale, y + (FONT_LETTER_HEIGHT * 1.33F / 256.0F) * scale,
						0.0F, 0.0F,
						1.0F, 1.0F,
						0.0F,
						0xeeeeeff);

					OS_buffer_draw(ob, nullptr, nullptr);
				}
			}

			x += FONT_draw_letter(ob, *ch, x, y, colour, scale);
		}

		ch += 1;
	}

	if (cursor-- == 0)
	{
		//
		// Draw a cursor here.
		//

		{
			OS_Buffer *ob = OS_buffer_new();

			OS_buffer_add_sprite(
				ob,
				x, y, x + 0.01F * scale, y + (FONT_LETTER_HEIGHT * 1.33F / 256.0F) * scale,
				0.0F, 0.0F,
				1.0F, 1.0F,
				0.0F,
				0xeeeeeff);

			OS_buffer_draw(ob, nullptr, nullptr);
		}
	}

	OS_buffer_draw(ob, FONT_ot, nullptr, OS_DRAW_DOUBLESIDED | OS_DRAW_ZALWAYS | OS_DRAW_NOZWRITE | OS_DRAW_ALPHABLEND);
}
