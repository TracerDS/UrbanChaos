#include	"Editor.hpp"

//---------------------------------------------------------------

#define	MAX_STRANDS		1
#define	STRAND_PIECES	30
#define	STRAND_LENGTH	5
#define	STRAND_SHIFT	1
struct	Strand
{
	std::int32_t	X[STRAND_PIECES];
	std::int32_t	Y[STRAND_PIECES];
	std::int16_t	VX[STRAND_PIECES];
	std::int16_t	VY[STRAND_PIECES];
	std::int16_t	Length;
};

struct	Strand	strands[MAX_STRANDS];

void	init_hair(std::int32_t x,std::int32_t y,std::int32_t z)
{
	std::int32_t	c0,c1;

	for(c0=0;c0<MAX_STRANDS;c0++)
	{
		for (c1=0;c1<STRAND_PIECES;c1++)
		{
			strands[c0].X[c1]=(x+4*c0)<<STRAND_SHIFT;
			strands[c0].Y[c1]=(y+STRAND_LENGTH*c1)<<STRAND_SHIFT;
			strands[c0].VX[c1]=0;
			strands[c0].VY[c1]=0;
		}
	}
}


void	offset_strand(std::int32_t c0,std::int32_t c1,std::int32_t dx,std::int32_t dy)
{
	for (;c1<STRAND_PIECES;c1++)
	{
		strands[c0].X[c1]+=dx;
		strands[c0].X[c1]+=dy;
	}
}


inline	std::int32_t normalise_xy(std::int32_t *dx,std::int32_t *dy,std::int32_t size)
{
	std::int32_t	dist;
	std::int32_t	tx,ty;

	tx=abs(*dx);
	ty=abs(*dy);

	dist=QDIST2(tx,ty);
	if(dist==0)
		dist=1;

	*dx=(*dx*size)/dist;
	*dy=(*dy*size)/dist;

	return(dist);
}

void	move_hair_root(std::int32_t gdx,std::int32_t gdy,std::int32_t dz)
{
	std::int32_t	c0,c1;
	std::int32_t	dx,dy;
	for(c0=0;c0<MAX_STRANDS;c0++)
	{
		strands[c0].X[0]+=gdx<<STRAND_SHIFT;
		strands[c0].Y[0]+=gdy<<STRAND_SHIFT;
		for (c1=1;c1<STRAND_PIECES;c1++)
		{
			std::int32_t	dx,dy,vx,vy;
			dx=strands[c0].X[c1]-strands[c0].X[c1-1];
			dy=strands[c0].Y[c1]-strands[c0].Y[c1-1];
			normalise_xy(&dx,&dy,STRAND_LENGTH<<STRAND_SHIFT);


//			strands[c0].VX[c1]+=(gdx>>1)+(strands[c0].VX[c1-1]>>2);
//			strands[c0].VY[c1]+=(gdy>>1)+(strands[c0].VY[c1-1]>>2);

			strands[c0].X[c1]=strands[c0].X[c1-1]+dx;
			strands[c0].Y[c1]=strands[c0].Y[c1-1]+dy;
		}
	}
}

void	move_hair_velocity()
{
	std::int32_t	c0,c1;
	for(c0=0;c0<MAX_STRANDS;c0++)
	{
		for (c1=1;c1<STRAND_PIECES;c1++)
		{
			std::int32_t	dx,dy;
			strands[c0].VY[c1]+=1;
			strands[c0].X[c1]+=strands[c0].VX[c1];
			strands[c0].Y[c1]+=strands[c0].VY[c1];

			strands[c0].VX[c1]=(strands[c0].VX[c1]*200)>>8;
			strands[c0].VY[c1]=(strands[c0].VY[c1]*200)>>8;
			if(abs(strands[c0].VX[c1])<2)
				strands[c0].VX[c1]=0;
			if(abs(strands[c0].VY[c1])<2)
				strands[c0].VY[c1]=0;

		}
	}
}


void	draw_hair()
{
	static	std::int32_t	old_x=0,old_y=0,old_z=0;
	std::int32_t	dx,dy,dz;

	dz = 0;

	if(old_x==0)
	{
		dx=dy=0;
	}
	else
	{
		dx=MouseX-old_x;
		dy=MouseY-old_y;
		
	}
	old_x=MouseX;
	old_y=MouseY;
	move_hair_velocity();
//	if(dx||dy)
		move_hair_root(dx,dy,dz);


	{
		std::int32_t	c0,c1;
		for(c0=0;c0<MAX_STRANDS;c0++)
		{
			for (c1=0;c1<STRAND_PIECES-1;c1++)
			{
				char	str[100];
				DrawLineC(strands[c0].X[c1]>>STRAND_SHIFT,strands[c0].Y[c1]>>STRAND_SHIFT,strands[c0].X[c1+1]>>STRAND_SHIFT,strands[c0].Y[c1+1]>>STRAND_SHIFT,255);
				sprintf(str,"%d \n",strands[c0].VX[c1]);
				QuickTextC(0,c1*15,str,255);
			}
		}
	}
	if(Keys[KB_P])
	{
		while(Keys[KB_P])
		{
			
		}
	}
}


//---------------------------------------------------------------
