#pragma once

#include	<d3dtypes.h>
//DEFINES

#define	POLY_FLAG_GOURAD		(1<<0)
#define	POLY_FLAG_TEXTURED		(1<<1)
#define	POLY_FLAG_MASKED		(1<<2)
#define	POLY_FLAG_SEMI_TRANS	(1<<3)
#define	POLY_FLAG_ALPHA			(1<<4)
#define	POLY_FLAG_TILED			(1<<5)
#define	POLY_FLAG_DOUBLESIDED	(1<<6)
#define	POLY_FLAG_WALKABLE		(1<<7)
//#define	POLY_FLAG_	(1<<)
#define	VM_GT			1
#define	POLY_MODES			0x3f

// STRUCTS

struct	EnginePoint //redesign this
{
	std::int32_t	X;			// these
	std::int32_t	Y;			// elements
	std::int32_t	TMapX;		// fixed
	std::int32_t	TMapY;		// by
	std::int32_t	Shade;		// trig function
	std::int32_t	X3d;
	std::int32_t	Y3d;
	std::int32_t	Z3d;
	std::int32_t	DistSqr;
	std::uint16_t	padw;
	std::uint8_t	Flags;
	std::uint8_t	padb;
};

struct	MfEnginePoint //redesign this
{
	std::int32_t	X;			// these
	std::int32_t	Y;			// elements
	std::int32_t	Z3d;
	std::uint8_t	TX;		// fixed
	std::uint8_t	TY;		// by
	std::uint8_t	Shade;		// trig function
	std::uint8_t	padw;
};

struct	MFDXEnginePoint
{
	float		ScrX,
				ScrY,
				ScrZ,
				U,
				V;
	D3DCOLOR	Color;
};

struct	PolyInfo
{
	std::uint16_t	*PTexture;
	std::uint16_t	Col;
	std::uint16_t	DrawFlags;
	std::uint8_t	Page;
};


//DATA

extern std::int32_t	div_table[65536];
extern std::uint16_t	*tmaps[];
extern std::uint8_t	*pals[];
extern std::uint8_t	tmap[];
extern std::uint8_t	tmap2[];
extern std::uint8_t	fade_tables[256*65];
extern std::uint8_t	mix_map[256*256];
extern std::uint16_t	pal_to_16[256];
extern struct	PolyInfo	poly_info;

// FUNCTIONS

extern std::uint16_t	is_it_clockwise(const struct	EnginePoint	*point1,const struct	EnginePoint *point2,const struct	EnginePoint *point3);
extern void	my_trig(struct MfEnginePoint *p3,struct MfEnginePoint *p2,struct MfEnginePoint *p1);
extern void	my_trigp(struct MfEnginePoint *p3,struct MfEnginePoint *p2,struct MfEnginePoint *p1);
//extern void	my_trig(struct EnginePoint *p3,struct EnginePoint *p2,struct EnginePoint *p1);
extern void	init_tmap();
extern void	make_fade_table(std::uint8_t *pal);
extern void	make_mix_map(std::uint8_t *pal);
extern void	double_work_window();
extern void	init_poly_system();
extern void	my_quad(struct MfEnginePoint *p4,struct MfEnginePoint *p3,struct MfEnginePoint *p2,struct MfEnginePoint *p1);

extern void	render_span8(struct	Boint *p_b,std::uint8_t	*ptr_screen,std::int32_t draw_flags);
extern void	render_span16(struct	Boint *p_b,std::uint8_t	*ptr_screen,std::int32_t draw_flags);
extern void	render_span32(struct	Boint *p_b,std::uint8_t	*ptr_screen,std::int32_t draw_flags);

extern void	(*render_span)(struct	Boint *p_b,std::uint8_t	*ptr_screen,std::int32_t draw_flags);


extern void	draw_all_spans();
extern void	my_quad_noz(struct MfEnginePoint *p4,struct MfEnginePoint *p3,struct MfEnginePoint *p2,struct MfEnginePoint *p1);
extern void	my_trig_noz(struct MfEnginePoint *p4,struct MfEnginePoint *p3,struct MfEnginePoint *p2);


