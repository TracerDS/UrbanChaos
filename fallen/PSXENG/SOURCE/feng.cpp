// FENG.cpp
//
// Feng (FrontEnd eNGine)
// is a replacement for the few functions that
// a seperate executable front end module would require.
// to make it totally self sufficent.

#include <MFStdLib.h>
#include <kernel.h>
#include <libgte.h>
#include <libgpu.h>
#include "psxeng.h"

#include "game.h"

std::int8_t f_width[104]={
	5,2,7,7,7,10,9,5,
	3,3,5,6,3,4,3,3,
	7,5,7,7,8,7,7,6,
	7,7,3,3,6,6,6,7,
	9,9,7,7,7,7,6,8,
	8,3,7,9,6,9,8,8,
	7,8,8,7,9,8,9,11,
	9,9,7,4,3,4,6,6,
	3,7,7,7,7,7,6,7,
	7,3,4,8,3,11,7,7,
	7,7,5,6,5,7,7,11,
	7,7,6,4,2,4,6,8,
	12,12,12,12,11,11,11,11
};

//char text_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!\":;'#$*-()[]\\/?^�@_";

void draw_text_at(std::int32_t x, std::int32_t y,char* message,std::int32_t font_id)
{
	SPRT *p;
	DR_TPAGE *tp;
	std::uint8_t* m=(std::uint8_t*)message;
	std::int32_t x0=x,y0=y;
	std::int32_t c;

	ALLOCPRIM(tp,DR_TPAGE);
	p=(SPRT*)the_display.CurrentPrim;
	setDrawTPage(tp,0,1,getTPage(0,0,896,256));
	while (*m)
	{
		switch(*m)
		{
		case '\t':
			x0=(x0+64)&0xfc0;
			if (x0>512)
			{
				x0=x;
				y0+=12;
			}
			break;
		case 10:
		case 13:
			x0=x;
			y0+=12;
			break;
		case 32:
			x0+=6;
			break;
		default:
			c=*m-32;

			check_prim_ptr((void**)&p);
			setSprt(p);
			setXY0(p,x0,y0);
			p->w=f_width[c];
			p->h=12;
			setUV0(p,(c&7)*12,(c>>3)*12);
			if (font_id&&((c<96)||(c>99)))
			{
				if (font_id&0xff000000)
					setSemiTrans(p,1);
				setRGB0(p,(font_id>>16)&0xff,(font_id>>8)&0xff,font_id&0xff);
			}
			else
				setRGB0(p,128,128,128);
			p->clut=getPSXClut(POLY_PAGE_FONT2D);
			DOPRIM(PANEL_OTZ,p);
			p++;
			x0+=f_width[c]+1;
			break;
		}
		m++;
	}
	DOPRIM(PANEL_OTZ,tp);
	check_prim_ptr((void**)&p);
	the_display.CurrentPrim=(std::uint8_t*)p;
}

std::int32_t text_width(char* message,std::int32_t font_id,std::int32_t *char_count)
{
	char *p=message;
	int width=0;

	while(*p)
	{
		if (*p>31 && *p<127)
			width+=f_width[*p-32]+1;
		p++;
		*char_count++;
	}
	return(width);
}

std::int32_t text_height(char* message,std::int32_t font_id,std::int32_t *char_count)
{
	*char_count=0;//strlen(message);
	return(12);
}

void draw_centre_text_at(std::int32_t x, std::int32_t y,char* message,std::int32_t font_id,std::int32_t flag)
{
	std::int32_t c;
	draw_text_at(x-(text_width(message,font_id,&c)/2),y-(text_height(message,font_id,&c)/2),message,font_id);
}

void AENG_flip2(std::uint32_t *back_image)
{
	DB	*old_buffer;

	ReadInputDevice();
	DrawSync(0);
	VSync(0);		/* wait for V-BLNK (1/60) */

	old_buffer=the_display.CurrentDisplayBuffer;
	if(the_display.CurrentDisplayBuffer==&the_display.DisplayBuffers[0])
   		the_display.CurrentDisplayBuffer=&the_display.DisplayBuffers[1];
	else
   		the_display.CurrentDisplayBuffer=&the_display.DisplayBuffers[0];
	PutDispEnv(&the_display.CurrentDisplayBuffer->Disp); 
	PutDrawEnv(&the_display.CurrentDisplayBuffer->Draw); 

	LoadImage(&the_display.CurrentDisplayBuffer->Draw.clip,back_image);
	DrawSync(0);
	DrawOTag(old_buffer->ot);	//draw the bucketlist
	DrawSync(0);
	ClearOTag(old_buffer->ot,OTSIZE);

	the_display.CurrentPrim=the_display.CurrentDisplayBuffer->PrimMem;

//	FntFlush(-1);

}

void AENG_flip()
{
	static	std::int32_t	prev_sync;
	std::int32_t	temp;
	DB	*old_buffer;
//	DrawSync(0);		/* wait for hardware to finish drawing */
		
	ReadInputDevice();
	VSync(0);		/* wait for V-BLNK (1/60) */

//	temp=VSync(-1);
//	sync_count=temp-prev_sync;
//	if(sync_count>4)
//		sync_count=4;

//	prev_sync=temp;

//	sync_count=2;

	temp=the_display.CurrentPrim-the_display.CurrentDisplayBuffer->PrimMem;

	ASSERT(temp<BUCKET_MEM);

	old_buffer=the_display.CurrentDisplayBuffer;
	if(the_display.CurrentDisplayBuffer==&the_display.DisplayBuffers[0])
   		the_display.CurrentDisplayBuffer=&the_display.DisplayBuffers[1];
	else
   		the_display.CurrentDisplayBuffer=&the_display.DisplayBuffers[0];
	PutDispEnv(&the_display.CurrentDisplayBuffer->Disp); 
	PutDrawEnv(&the_display.CurrentDisplayBuffer->Draw); 

	DrawOTag(old_buffer->ot);	//draw the bucketlist

	if (temp>the_display.Max_Used)
		the_display.Max_Used=temp;

	the_display.CurrentPrim=the_display.CurrentDisplayBuffer->PrimMem;

//	FntFlush(-1);

}

void main()
{
}


