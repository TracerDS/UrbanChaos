#include	"Editor.hpp"

#include	"game.h"
#include	"engine.h"
#include	"math.h"
#include	"thing.h"
#include	"map.h"

#pragma warning( disable : 4244)

extern void	do_quad_clip_list(std::int16_t face,std::int32_t p0,std::int32_t p1,std::int32_t p2,std::int32_t p3); //prim.cpp
extern void	do_tri_clip_list(std::int16_t face,std::int32_t p0,std::int32_t p1,std::int32_t p2); //prim.cpp
extern std::uint16_t	calc_lights(std::int32_t x,std::int32_t y,std::int32_t z,struct SVECTOR *p_vect);

extern struct	SVECTOR			global_res[]; //max points per object?
extern std::int32_t	global_flags[];
extern std::uint16_t	global_bright[];
extern float	global_light[];

#define	SORT_LEVEL_LONG_LEDGE	1
#define	SORT_LEVEL_FIRE_ESCAPE	3

extern std::int32_t	calc_height_at(std::int32_t x,std::int32_t z);
extern void	insert_collision_vect(std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2,std::uint8_t prim,std::uint8_t prim_extra,std::int16_t face);
extern std::int32_t	dist_between_vertex_and_vector(std::int32_t x1,std::int32_t y1,std::int32_t x2,std::int32_t y2,std::int32_t px,std::int32_t py);

struct	PrimFace4*	create_a_quad(std::uint16_t p1,std::uint16_t p0,std::uint16_t p3,std::uint16_t p2,std::int16_t	texture_style,std::int16_t texture_piece);
void	build_face_texture_info(struct PrimFace4* p_f4,std::uint16_t texture);
struct	PrimFace3*	create_a_tri(std::uint16_t p2,std::uint16_t p1,std::uint16_t p0,std::int16_t	texture_id,std::int16_t texture_piece);

struct	BuildingFacet	building_facets[MAX_BUILDING_FACETS];
struct	BuildingObject	building_objects[MAX_BUILDING_OBJECTS];

std::uint16_t	next_building_object=1;
std::uint16_t	end_building_object=MAX_BUILDING_OBJECTS-2;

std::uint16_t	next_building_facet=1;
std::uint16_t	end_building_facet=MAX_BUILDING_FACETS-2;


//data

struct	FWindow		window_list[MAX_WINDOWS];
struct	FWall		wall_list[MAX_WALLS];
struct	FStorey		storey_list[MAX_STOREYS];
struct	FBuilding	building_list[MAX_BUILDINGS];


/*

	**************************
	Create city code
	----------------
	**************************

*/

static	std::int32_t	build_seed=0x12345678;

std::int32_t	build_rand()
{
	build_seed=(build_seed*12345678)+12345678;
//	LogText(" build_seed %x \n",build_seed);
	return(build_seed>>16);
}

void	set_build_seed(std::int32_t seed)
{
	build_seed=seed;
}


void	add_walk_face_to_map(std::int16_t face,std::int32_t x,std::int32_t z)
{
	if(next_walk_link>=(MAX_WALK_POOL-4))
	{
		LogText(" failed out of walk mem \n");
		return;
	}
	walk_links[next_walk_link].Face=face;
	walk_links[next_walk_link].Next=edit_map[x][z].Walkable;
	edit_map[x][z].Walkable=next_walk_link;
	next_walk_link++;
}


void scan_walk_triangle(std::int32_t x0, std::int32_t y0, std::int32_t z0,std::int32_t x1, std::int32_t y1, std::int32_t z1,std::int32_t x2, std::int32_t y2, std::int32_t z2,std::int32_t face)
{

	std::int32_t	px,py,pz;
	std::int32_t	face_x,face_y,face_z;
	std::int32_t	c0;
	std::int32_t	s,t,step_s,step_t;
	std::int32_t	vx,vy,vz,wx,wy,wz;
	struct	DepthStrip *me;
	std::int32_t	prev_x,prev_z;
	std::int32_t	quad;
	std::int32_t	len;
	char	str[100];
	std::uint8_t	info=0;

	face_x = x0;
	face_y = y0;
	face_z = z0;

	vx = x1 - x0;
	vy = y1 - y0;   //vector from point 0 to point 1
	vz = z1 - z0;

	wx = x2 - x0;   //vector from point 1 to point 2 
	wy = y2 - y0;
	wz = z2 - z0;
	
	len=(sqrl(vx*vx+vy*vy+vz*vz)>>7);
	if(len<2)
		len=2;
	step_s=(1<<7)/len;
	len=(sqrl(wx*wx+wy*wy+wz*wz)>>7);
	if(len<2)
		len=2;
	step_t=(1<<7)/len;

	if(step_s==0)
		step_s=256;
	if(step_t==0)
		step_t=256;

	prev_x=-1;
	prev_z=-1;
	for(s=5; s<(255)                 ; s+=step_s)
	for(t=5; t<(255) && ((s+t)<(256)); t+=step_t)
	{
		px=face_x+((s*vx)>>8)+((t*wx)>>8);
		pz=face_z+((s*vz)>>8)+((t*wz)>>8);
		if((px>>8)!=prev_x||(pz>>8)!=prev_z)
		{
			py=face_y+((s*vy)>>8)+((t*wy)>>8);

			if((px>>8)>0&&(px>>8)<EDIT_MAP_WIDTH&&(pz>>8)>0&&(pz>>8)<EDIT_MAP_DEPTH)
			{
				LogText(" add walkable face to %d %d \n",px>>8,pz>>8);
				add_walk_face_to_map(face,px>>8,pz>>8);
			}
			prev_x=px>>8;
			prev_z=pz>>8;
		}

	}
}

//  0   1
//
//	2   3
void	add_quad_to_walkable_list(std::int16_t face)
{
	std::int32_t	x[4],y[4],z[4];
	std::int32_t	c0,p0;
	struct PrimFace4 *p_f4;
	p_f4=&prim_faces4[face];

	for(c0=0;c0<4;c0++)
	{
		p0=p_f4->Points[c0];
		x[c0]=prim_points[p0].X;
		y[c0]=prim_points[p0].Y;
		z[c0]=prim_points[p0].Z;
	}
	scan_walk_triangle(x[0],y[0],z[0],x[1],y[1],z[1],x[2],y[2],z[2],face);
	scan_walk_triangle(x[1],y[1],z[1],x[3],y[3],z[3],x[2],y[2],z[2],face);

}

void	add_tri_to_walkable_list(std::int16_t face)
{
	std::int32_t	x,z;
	std::int32_t	p0;
	struct PrimFace3 *p_f3;

	p_f3=&prim_faces3[face];
	//for now just take one corner and add it to map at that corner
	p0=p_f3->Points[0];
	x=prim_points[p0].X>>ELE_SHIFT;
	z=prim_points[p0].Z>>ELE_SHIFT;
	add_walk_face_to_map(-face,x,z);
}

std::int32_t	place_building_at(std::uint16_t prim,std::int32_t x,std::int32_t y,std::int32_t z)
{
	std::uint16_t	map_thing;
	struct	MapThing	*p_mthing;

	//y=0;
	//LogText(" place building prim %d x %d y %d z %d \n",prim,x,y,z);

	map_thing=find_empty_map_thing();
//	LogText(" map thing %d \n",map_thing);
	if(!map_thing)
		return(0);
	add_thing_to_edit_map(x>>ELE_SHIFT,z>>ELE_SHIFT,map_thing);
	p_mthing=TO_MTHING(map_thing);
	p_mthing->X=x;
	p_mthing->Y=y;
	p_mthing->Z=z;

	p_mthing->Type=MAP_THING_TYPE_BUILDING;
	p_mthing->IndexOther=prim;
	p_mthing->IndexOrig=prim;
//	apply_ambient_light_to_object(prim,edit_info.amb_dx,edit_info.amb_dy,edit_info.amb_dz,edit_info.amb_bright);

	return(map_thing);
}


void	add_point(std::int32_t x,std::int32_t y,std::int32_t z)
{
	prim_points[next_prim_point].X=x;
	prim_points[next_prim_point].Y=y;
	prim_points[next_prim_point++].Z=z;
}

std::int32_t	build_row_wall_points_at_y(std::int32_t y,std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t wall)
{
	std::int32_t	wcount,wwidth,wallwidth,dx,dz,dist;

	std::int32_t	start_point;

	start_point=next_prim_point;

	wwidth=BLOCK_SIZE;

	dx=abs(x2-x1);
	dz=abs(z2-z1);

	dist=sqrl(SDIST2(dx,dz));
	if(dist==0)
		return(0);

	if(wall_list[wall].WallFlags&FLAG_WALL_AUTO_WINDOWS)
	{
		wcount=dist/(BLOCK_SIZE*4);
		wwidth=dist/(wcount*2+1);
		wall_list[wall].WindowCount=wcount;
		
	}
	else
	{
		wcount=wall_list[wall].WindowCount;
		wwidth=BLOCK_SIZE;
	}

	dx=(x2-x1);
	dz=(z2-z1);

	if(wcount<0)
		return(0);

	wallwidth=(dist-(wcount*wwidth))/(wcount+1);

	dx=(dx<<10)/dist;
	dz=(dz<<10)/dist;


	add_point(x1,y,z1);

	while(wcount)
	{
		x1=x1+((dx*wallwidth)>>10);
		z1=z1+((dz*wallwidth)>>10);
		add_point(x1,y,z1);


		x1=x1+((dx*wwidth)>>10);
		z1=z1+((dz*wwidth)>>10);
		add_point(x1,y,z1);

		wcount--;

	}
	x1=x1+((dx*wallwidth)>>10);
	z1=z1+((dz*wallwidth)>>10);
	add_point(x1,y,z1);

	return(start_point);
	
}

std::int32_t	build_row_wall_only_points_at_y(std::int32_t y,std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t wall)
{
	std::int32_t	wcount,wwidth,dx,dz,dist;

	std::int32_t	start_point;

	start_point=next_prim_point;

	dx=abs(x2-x1);
	dz=abs(z2-z1);

	dist=sqrl(SDIST2(dx,dz));
	if(dist==0)
		return(0);

 	wcount=(dist/(BLOCK_SIZE*4));
	if(wcount==0)
		wcount=1;
	wwidth=dist/(wcount);
	wall_list[wall].WindowCount=wcount;

	dx=(x2-x1);
	dz=(z2-z1);


	dx=(dx<<10)/dist;
	dz=(dz<<10)/dist;

	add_point(x1,y,z1);
	wcount--;

	while(wcount)
	{

		x1=x1+((dx*wwidth)>>10);
		z1=z1+((dz*wwidth)>>10);
		add_point(x1,y,z1);

		wcount--;

	}
	add_point(x2,y,z2); //make sure last point is spot on.

	return(start_point);
	
}

std::int32_t	build_row_window_depth_points_at_y(std::int32_t y,std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t wall)
{

	std::int32_t	wcount,wwidth,wallwidth,dx,dz,dist;
	std::int32_t 	pdx,pdz;

	std::int32_t	start_point;

	start_point=next_prim_point;



	dx=abs(x2-x1);
	dz=abs(z2-z1);

	dist=sqrl(SDIST2(dx,dz));
	if(dist==0)
		return(0);

	if(wall_list[wall].WallFlags&FLAG_WALL_AUTO_WINDOWS)
	{
		wcount=dist/(BLOCK_SIZE*4);
		wwidth=dist/(wcount*2+1);
		wall_list[wall].WindowCount=wcount;
		
	}
	else
	{
		wcount=wall_list[wall].WindowCount;
		wwidth=BLOCK_SIZE;
	}

	dx=(x2-x1);
	dz=(z2-z1);

	if(wcount<0)
		return(0);

	wallwidth=(dist-(wcount*wwidth))/(wcount+1);

	dx=(dx<<10)/dist;
	dz=(dz<<10)/dist;

	pdx=-dz;
	pdz=dx;

	pdx=(pdx*20)>>10;
	pdz=(pdz*20)>>10;

	x1+=pdx;
	z1+=pdz;	  


//	add_point(x1,y,z1);

	while(wcount)
	{
		x1=x1+((dx*wallwidth)>>10);
		z1=z1+((dz*wallwidth)>>10);
		add_point(x1,y,z1);


		x1=x1+((dx*wwidth)>>10);
		z1=z1+((dz*wwidth)>>10);
		add_point(x1,y,z1);

		wcount--;

	}
	x1=x1+((dx*wallwidth)>>10);
	z1=z1+((dz*wallwidth)>>10);
//	add_point(x1,y,z1);

	return(start_point);
	
}

struct	Edge
{
	std::int16_t	X;
	std::int16_t	Type;
	std::int16_t	Next;
	std::int16_t	Prev;
};

struct	Edge	*edge_pool_ptr;
static	std::uint16_t	*edge_heads_ptr;
static	std::uint32_t	next_edge;
static	std::uint32_t	edge_min_z;
static	std::uint16_t	*flag_blocks;
static	std::uint16_t	*cut_blocks;
static	std::int32_t	global_y;
#define	MAX_BOUND_SIZE	(200)


void	insert_point(std::int32_t z,std::int32_t x,std::int16_t type)
{
	std::int32_t	edge;

	LogText(" insert point (%x,%x) \n",x,z);

	edge_pool_ptr[next_edge].X=x;
	edge_pool_ptr[next_edge].Type=type;

	edge=edge_heads_ptr[z];
	if(edge)
	{
		while(edge)
		{
			if(edge_pool_ptr[edge].X>x)
			{
				std::int32_t	prev;
				prev=edge_pool_ptr[edge].Prev;

				if(prev)
				{
				  // insert between current one and previous
					edge_pool_ptr[prev].Next=next_edge;
					edge_pool_ptr[edge].Prev=next_edge;
					edge_pool_ptr[next_edge].Next=edge;
					edge_pool_ptr[next_edge].Prev=prev;
					next_edge++;
				}
				else
				{
				  // insert before current one and head of list
					edge_heads_ptr[z]=next_edge;
					edge_pool_ptr[edge].Prev=next_edge;
					edge_pool_ptr[next_edge].Next=edge;
					edge_pool_ptr[next_edge].Prev=0;
					next_edge++;

				}
				return;
			}
			else
			if(edge_pool_ptr[edge].X==x)
			{
//				LogText(" allready exists cancel \n");
				return;
			}

			if(edge_pool_ptr[edge].Next==0)
			{
				//append after current
				edge_pool_ptr[edge].Next=next_edge;
				edge_pool_ptr[next_edge].Next=0;
				edge_pool_ptr[next_edge].Prev=edge;
				next_edge++;
				return;

			}
			edge=edge_pool_ptr[edge].Next;
		}
	}
	else
	{
		edge_heads_ptr[z]=next_edge;
		edge_pool_ptr[next_edge].Prev=0;
		edge_pool_ptr[next_edge].Next=0;
		next_edge++;
	}
}

#define	SIDEWAY_EDGE	(1)
#define	NORMAL_EDGE		(2)

#define	CUT_BLOCK_TOP		(0)
#define	CUT_BLOCK_BOTTOM	(1)
#define	CUT_BLOCK_LEFT		(2)
#define	CUT_BLOCK_RIGHT		(3)

void	set_cut_blocks(std::int32_t x,std::int32_t z)  // x is in pixels // zis in blocks
{

	LogText(" cut block [%d][%d] top    x %d x %x\n",x>>ELE_SHIFT,z+1,x,x);
	LogText(" cut block [%d][%d] bottom x %d x %x\n",x>>ELE_SHIFT,z,x,x);
//	z=z-(edge_min_z>>ELE_SHIFT);
		cut_blocks[(x>>ELE_SHIFT)*4+((z)*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP)]=x;
		cut_blocks[(x>>ELE_SHIFT)*4+((z-1)*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM)]=x;
}

void	set_cut_blocks_z(std::int32_t x,std::int32_t z) // x is in blocks z is in pixels
{

	LogText(" cut block [%d][%d] left   z %d z %x\n",x,z>>ELE_SHIFT,z,z);
	LogText(" cut block [%d][%d] right  z %d z %x\n",x-1,z>>ELE_SHIFT,z,z);
	cut_blocks[(x)*4+(((z>>ELE_SHIFT)-0)*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT)]=z;
	cut_blocks[(x-1)*4+(((z>>ELE_SHIFT)-0)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT)]=z;
}

void	scan_line_z(std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t flag)
{
	std::int32_t	dx,dz,count;
	std::int32_t	x,z;
	std::int16_t	type;
//	LogText(" scan line z x1 %d z1 %d x2 %d z2 %d  \n",x1,z1,x2,z2);
	LogText(" scan lineZ (%x,%x)->(%x,%x) \n",x1,(z1>>ELE_SHIFT),x2,(z2>>ELE_SHIFT));


	dz=z2-z1;
	dx=x2-x1;
//	LogText(" dx %d dz %d \n",dx,dz);
	{
		x1>>=ELE_SHIFT;
		x2>>=ELE_SHIFT;
		dx=x2-x1;

		z=z1<<16;
		count=dx;
//		LogText(" x1 %d x2 %d dx %d \n",x1,x2,dx);

		if(count<0)
		{
//			LogText(" neg count %d z1 %d z2 %d \n",count,z1,z2);
			dz=-(dz<<16)/count;
				x1--;
				z+=dz;
//			else
//				count--;

//			LogText("A x1 %d x2 %d dx %d dz %x count %d\n",x1,x2,dx,dz,count);
			while(count)
			{
				if(dz)
				{
//					LogText(" mid scan z %d z %x\n",(z>>16)&0xff,z);
					if((z>>16)&0xff)
					{
						set_cut_blocks_z(x1,z>>16);
					}

				}

				z+=dz;
				x1--;
				count++;
			}
		}

		if(count>0)
		{
			dz=(dz<<16)/count;
//			if(flag)
//				count++;

			//LogText("B x1 %d x2 %d dx %d count %d\n",x1,x2,dx,count);
			while(count)
			{
				if(dz)
				{
					//LogText(" mid scan z %d z %x\n",(z>>16)&0xff,z);
					if((z>>16)&0xff)
					{
						set_cut_blocks_z(x1,z>>16);
					}
				}
				z+=dz;
				x1++;
				count--;
			}
		}
	}
}

std::uint8_t	scan_line(std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t flag)
{
	std::int32_t	dx,dz,count;
	std::int32_t	x,z;
	std::int16_t	type;
	LogText(" scan line (%x,%x)->(%x,%x) \n",x1,(z1>>ELE_SHIFT),x2,(z2>>ELE_SHIFT));


	if(z1==z2)
		type=SIDEWAY_EDGE;
	else
		type=NORMAL_EDGE;

	dz=z2-z1;
	dx=x2-x1;
	//if(abs(dz)>abs(dx))
	{
		//LogText(" dz longest \n");
		z1>>=ELE_SHIFT;
		z2>>=ELE_SHIFT;
		dz=z2-z1;

		x=x1<<16;
		count=dz;

		if(count<0)
		{
//			LogText(" neg count %d z1 %d z2 %d \n",count,z1,z2);
			dx=-(dx<<16)/count;
			//z1--;
			//count++;
			//x+=dx;
//			if(!flag)
				z1--;
				x+=dx;
//			else
//				count--;

			while(count)
			{
				insert_point(z1,x>>16,type);
				if(dx)
				{
					LogText(" scan x %d \n",(x>>16)&0xff);
					if((x>>16)&0xff)
					{
						set_cut_blocks(x>>16,z1);
					}

				}

				x+=dx;
				z1--;
				count++;
			}
		}

		if(count>0)
		{
			dx=(dx<<16)/count;
//			if(flag)
//				count++;

			while(count)
			{
				insert_point(z1,x>>16,type);
				if(dx)
				{
					LogText(" scan x2 %d \n",(x>>16)&0xff);
					if((x>>16)&0xff)
					{
						set_cut_blocks(x>>16,z1);
					}
				}
				x+=dx;
				z1++;
				count--;
			}
		}
	}
/*
	else
	{
		LogText(" dx longest \n");
		x1>>=ELE_SHIFT;
		x2>>=ELE_SHIFT;
		dx=x2-x1;

		z=z1<<16;
		count=dx;

		if(count<0)
		{
			LogText(" neg count %d z1 %d z2 %d \n",count,z1,z2);
			dz=-(dz<<16)/count;
			while(count<0)
			{
				insert_point((z>>16)>>ELE_SHIFT,x1<<ELE_SHIFT,type);
				z+=dz;
				x1--;
				count++;
			}
		}

		if(count>0)
		{
			dz=(dz<<16)/count;

			while(count)
			{
				insert_point((z>>16)>>ELE_SHIFT,x1<<ELE_SHIFT,type);
				z+=dz;
				x1++;
				count--;
			}
		}
	}
*/
	if(dx&&dz)
		return(1);
	else
		return(0);
}



void	build_edge_list(std::int32_t storey,std::int32_t flag)
{
	std::uint16_t	wall;
	std::int32_t	px,pz;

	next_edge=1;
	edge_pool_ptr=(struct Edge*)MemAlloc(sizeof(struct Edge)*MAX_BOUND_SIZE*5);
	edge_heads_ptr=(std::uint16_t*)MemAlloc(sizeof(std::uint16_t)*MAX_BOUND_SIZE);
	flag_blocks=(std::uint16_t*)MemAlloc(sizeof(std::uint16_t)*MAX_BOUND_SIZE*MAX_BOUND_SIZE);
	cut_blocks=(std::uint16_t*)MemAlloc(sizeof(std::uint16_t)*MAX_BOUND_SIZE*MAX_BOUND_SIZE*4);

	memset(edge_heads_ptr,0,sizeof(std::uint16_t)*MAX_BOUND_SIZE);
	memset(flag_blocks,0,sizeof(std::uint16_t)*MAX_BOUND_SIZE*MAX_BOUND_SIZE);
	memset(cut_blocks,0,sizeof(std::uint16_t)*MAX_BOUND_SIZE*MAX_BOUND_SIZE*4);

	px=storey_list[storey].DX;
	pz=storey_list[storey].DZ-edge_min_z;
	wall=storey_list[storey].WallHead;

	while(wall)
	{
		if(scan_line(px,pz,wall_list[wall].DX,wall_list[wall].DZ-edge_min_z,flag))
		{
			scan_line_z(px,pz,wall_list[wall].DX,wall_list[wall].DZ-edge_min_z,flag);

		}
		px=wall_list[wall].DX;
		pz=wall_list[wall].DZ-edge_min_z;
		wall=wall_list[wall].Next;

	}
}



void	scan_bottom_line(std::int32_t x,std::int32_t z,std::int32_t x2,std::int32_t y)
{
	std::int32_t	dy;
	//LogText(" scan bottom line (%x,%x)->(%x) \n",x1,(z1>>ELE_SIZE),x2);

	//LogText(" scan bottom before x %d x2 %d z %d\n",x,x2,z);

	x2=x2>>ELE_SHIFT;
	x=(x>>ELE_SHIFT);
	if(x>x2)
	{
		std::int32_t temp;
		temp=x;
		x=x2;
		x2=temp;
	}
	//LogText(" scan bottom x %d x2 %d z %d\n",x,x2,z);

	for(;x<=x2;x++)
	{
		if(flag_blocks[(x)+(z>>ELE_SHIFT)*MAX_BOUND_SIZE]==0)
		{
			dy=edit_map[x][((z+edge_min_z)>>ELE_SHIFT)].Y<<FLOOR_HEIGHT_SHIFT;
			add_point(x<<ELE_SHIFT,y+dy,(z)+edge_min_z);
			//LogText(" scan bottom (%d,%d)\n",x,z>>ELE_SIZE);
			flag_blocks[(x)+(z>>ELE_SHIFT)*MAX_BOUND_SIZE]=next_prim_point-1;
		}
		//else
			//LogText(" x %d z %d contains %d \n",x,z>>ELE_SHIFT,flag_blocks[(x)+(z>>ELE_SHIFT)*MAX_BOUND_SIZE]);
	}
}


// scan horizontal edges
void	build_bottom_edge_list(std::int32_t storey,std::int32_t y)
{
	std::uint16_t	wall;
	std::int32_t	px,pz;

	px=storey_list[storey].DX;
	pz=storey_list[storey].DZ-edge_min_z;
	wall=storey_list[storey].WallHead;

	while(wall)
	{
		//add end points to the fray, incase we missed any
		if(flag_blocks[(px>>ELE_SHIFT)+(pz>>ELE_SHIFT)*MAX_BOUND_SIZE]==0)
		{
			std::int32_t	dy;
			dy=edit_map[px>>ELE_SHIFT][((pz+edge_min_z)>>ELE_SHIFT)].Y<<FLOOR_HEIGHT_SHIFT;
			add_point(px,y+dy,(pz)+edge_min_z);
			flag_blocks[(px>>ELE_SHIFT)+(pz>>ELE_SHIFT)*MAX_BOUND_SIZE]=next_prim_point-1;
		}
	//	LogText(" find bottom edge  wall %d \n",wall);
		if(pz==wall_list[wall].DZ-edge_min_z)
			scan_bottom_line(px,pz,wall_list[wall].DX,y);
		px=wall_list[wall].DX;
		pz=wall_list[wall].DZ-edge_min_z;
		wall=wall_list[wall].Next;
	}

}


void	bin_edge_list()
{
	MemFree((std::uint8_t*)edge_pool_ptr);
	MemFree((std::uint8_t*)edge_heads_ptr);
	MemFree((std::uint8_t*)flag_blocks);
	MemFree((std::uint8_t*)cut_blocks);
}



#define	BOUNDS(x,z)		if(x<min_x) min_x=x;if (x>max_x) max_x=x; if(z<min_z) min_z=z; if(z>max_z) max_z=z;

#define	INSIDE			(0)
#define	ON_EDGE			(1)
#define	OUTSIDE			(2)


void	dump_edge_list(std::uint16_t size)
{
	std::int32_t	c0;
	std::int32_t	edge;

	for(c0=0;c0<size;c0++)
	{
		edge=edge_heads_ptr[c0];
		LogText(" Z %d---",c0);
		while(edge)
		{
			LogText(" %x ",edge_pool_ptr[edge].X);
			edge=edge_pool_ptr[edge].Next;
		}
		LogText(" \n");
	}
}

#define MYX(x)	(((x-min_x)*box_width)+10)
#define MYY(x)	(((x)*box_depth)+10)

#define MYFX(x)	((  ((x-(min_x<<ELE_SHIFT))*(box_width>>3))>>(ELE_SHIFT-3))+10 )
#define MYFY(x)	( (((x)*(box_depth>>3))>>(ELE_SHIFT-3))+10)

void	show_grid(std::int32_t width,std::int32_t depth,std::int32_t min_x)
{
	std::int32_t	xt,xb,zl,zr;
	std::int32_t	x,z;
	std::int32_t	box_width,box_depth;
	SetWorkWindowBounds(0,0,800,600);
	DrawBox(0,0,800,600,0xfff);

	box_depth=(580/depth);
	box_width=(780/width);


	LogText(" show grid  width %d  depth %d min_x %d \n",width,depth,min_x);



	for(z=0;z<depth;z++)
	{
		for(x=min_x;x<min_x+width;x++)
		{
			std::int32_t	x1,z1;
			char	xt_s[10],xb_s[10],zl_s[10],zr_s[10];

			x1=((x-min_x)*box_width)+10;
			z1=(z*box_depth)+10;

			xt=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
			xb=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];
			zl=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
			zr=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];

			DrawLineC(x1,z1,x1+box_width,z1,0x8);
			DrawLineC(x1+box_width,z1,x1+box_width,z1+box_depth,0x8);
			DrawLineC(x1,z1+box_depth,x1+box_width,z1+box_depth,0x8);
			DrawLineC(x1,z1,x1,z1+box_depth,0x8);
			if(flag_blocks[x+z*MAX_BOUND_SIZE])
			{
				x1-=4;
				z1-=4;
				DrawLineC(x1,z1,x1+8,z1,0x8);
				DrawLineC(x1+8,z1,x1+8,z1+8,0x8);
				DrawLineC(x1,z1+8,x1+8,z1+8,0x8);
				DrawLineC(x1,z1,x1,z1+8,0x8);
				x1+=4;
				z1+=4;

			}

			sprintf(xt_s,"%d",xt);
			sprintf(xb_s,"%d",xb);
			sprintf(zl_s,"%d",zl);
			sprintf(zr_s,"%d",zr);

			QuickTextC(x1+((box_width)>>1)-7,z1+2,xt_s,0);
			QuickTextC(x1+((box_width)>>1)-7,z1+box_depth-12,xb_s,0);
			QuickTextC(x1+2,z1+(box_depth>>1)-6,zl_s,0);
			QuickTextC(x1+box_width-30,z1+(box_depth>>1)-6,zr_s,0);
#ifdef	POO

			if(xt)
				DrawLineC(x1+(box_width>>1),z1+(box_depth>>1),MYFX(xt),z1,0x1f);

				//DrawLineC(x1+(box_width>>1),z1+(box_depth>>1),MYFX(xt),z1,0x1f);
			if(xb)
				DrawLineC(x1+(box_width>>1),z1+(box_depth>>1),MYFX(xb),z1+box_depth,0x1f0);

			if(zl)
				DrawLineC(x1+(box_width>>1),z1+(box_depth>>1),x1,MYFY(zl),0x1f);

			if(zr)
				DrawLineC(x1+(box_width>>1),z1+(box_depth>>1),x1+box_width,MYFY(zr),0x1f0);
#endif

		}
	}
	ShowWorkScreen(0);
	while(!RightButton)
	{
	}
	while(RightButton);
}



#define	set_UV4(x0,y0,x1,y1,x2,y2,x3,y3) UV[0][0]=(x0);UV[0][1]=(y0);UV[1][0]=(x1);UV[1][1]=(y1);UV[2][0]=(x3);UV[2][1]=(y3);UV[3][0]=(x2);UV[3][1]=(y2);

// mx,mz are map co_ords 0..MAP_WIDTH
void	build_free_tri_texture_info(struct PrimFace3 *p_f3,std::int32_t mx,std::int32_t mz)
{
	std::uint8_t tx,ty,page;
	std::int32_t	tsize;
	std::int32_t	rot;
	std::uint8_t	UV[4][2];
	std::uint16_t	texture,p;

	std::int32_t	dtx_down,dty_down;
	std::int32_t	dtx_down_r,dty_down_r;
	//std::int32_t	dtx_across,dty_across;

	texture=edit_map[mx][mz].Texture;

	tx=((struct	MiniTextureBits*)(&texture))->X<<5;
	ty=((struct	MiniTextureBits*)(&texture))->Y<<5;
	page=(std::uint8_t)(((struct	MiniTextureBits*)(&texture))->Page);
	tsize=floor_texture_sizes[((struct	MiniTextureBits*)(&texture))->Size]-1;
	rot=((struct	MiniTextureBits*)(&texture))->Rot;
//	rot=(rot3)&3;
	switch(rot)
	{
		case	0:
			set_UV4(	tx,ty,tx+tsize,ty,tx,ty+tsize,tx+tsize,ty+tsize);
			break;
		case	1:
			set_UV4(	tx+tsize,ty,tx+tsize,ty+tsize,tx,ty,tx,ty+tsize);
			break;
		case	2:
			set_UV4(	tx+tsize,ty+tsize,tx,ty+tsize,tx+tsize,ty,tx,ty);
			break;
		case	3:
			set_UV4(	tx,ty+tsize,tx,ty,tx+tsize,ty+tsize,tx+tsize,ty);
			break;
	}

	dtx_down=UV[3][0]-UV[0][0];
	dty_down=UV[3][1]-UV[0][1];

	dtx_down_r=UV[2][0]-UV[1][0];
	dty_down_r=UV[2][1]-UV[1][1];

	p_f3->TexturePage=page;

		LogText(" U[0].X %d U[0].Y %d\n",UV[0][0],UV[0][1]);
		LogText(" U[1].X %d U[1].Y %d\n",UV[1][0],UV[1][1]);
		LogText(" U[2].X %d U[2].Y %d\n",UV[2][0],UV[2][1]);
		LogText(" U[3].X %d U[3].Y %d\n",UV[3][0],UV[3][1]);

	for(p=0;p<3;p++)
	{
		std::int32_t x1,z1;
		std::int32_t	lx,ly;
		std::int32_t	rx,ry;


		x1=prim_points[p_f3->Points[p]].X-(mx<<ELE_SHIFT);
		z1=prim_points[p_f3->Points[p]].Z-(mz<<ELE_SHIFT);


		LogText(" free tri p %d  x1 %d z1 %d \n",p,x1,z1);


		lx=(z1*dtx_down)>>8;
		ly=(z1*dty_down)>>8;
		lx+=UV[0][0];
		ly+=UV[0][1];

		rx=(z1*dtx_down_r)>>8;
		ry=(z1*dty_down_r)>>8;

		rx+=UV[1][0];
		ry+=UV[1][1];

		LogText("left (%d,%d) right (%d,%d)\n",lx,ly,rx,ry);

		p_f3->UV[p][0]=lx+(((rx-lx)*x1)>>8);
		p_f3->UV[p][1]=ly+(((ry-ly)*x1)>>8);

		LogText("result (%d,%d) \n",lx+(((rx-lx)*x1)>>8),ly+(((ry-ly)*x1)>>8));
	}
}

void	build_free_quad_texture_info(struct PrimFace4 *p_f4,std::int32_t mx,std::int32_t mz)
{
	std::uint8_t tx,ty,page;
	std::int32_t	tsize;
	std::int32_t	rot;
	std::uint8_t	UV[4][2];
	std::uint16_t	texture,p;

	std::int32_t	dtx_down,dty_down;
	std::int32_t	dtx_down_r,dty_down_r;
	std::int32_t	dtx_across,dty_across;

	texture=edit_map[mx][mz].Texture;

	tx=((struct	MiniTextureBits*)(&texture))->X<<5;
	ty=((struct	MiniTextureBits*)(&texture))->Y<<5;
	page=(std::uint8_t)(((struct	MiniTextureBits*)(&texture))->Page);
	tsize=floor_texture_sizes[((struct	MiniTextureBits*)(&texture))->Size]-1;
	rot=((struct	MiniTextureBits*)(&texture))->Rot;
//	rot=(rot3)&3;
	switch(rot)
	{
		case	0:
			set_UV4(	tx,ty,tx+tsize,ty,tx,ty+tsize,tx+tsize,ty+tsize);
			break;
		case	1:
			set_UV4(	tx+tsize,ty,tx+tsize,ty+tsize,tx,ty,tx,ty+tsize);
			break;
		case	2:
			set_UV4(	tx+tsize,ty+tsize,tx,ty+tsize,tx+tsize,ty,tx,ty);
			break;
		case	3:
			set_UV4(	tx,ty+tsize,tx,ty,tx+tsize,ty+tsize,tx+tsize,ty);
			break;
	}

	dtx_down=UV[3][0]-UV[0][0];
	dty_down=UV[3][1]-UV[0][1];

	dtx_down_r=UV[2][0]-UV[1][0];
	dty_down_r=UV[2][1]-UV[1][1];

	p_f4->TexturePage=page;

	LogText(" U[0].X %d U[0].Y %d\n",UV[0][0],UV[0][1]);
	LogText(" U[1].X %d U[1].Y %d\n",UV[1][0],UV[1][1]);
	LogText(" U[2].X %d U[2].Y %d\n",UV[2][0],UV[2][1]);
	LogText(" U[3].X %d U[3].Y %d\n",UV[3][0],UV[3][1]);

	for(p=0;p<4;p++)
	{
		std::int32_t x1,z1;
		std::int32_t	lx,ly;
		std::int32_t	rx,ry;


		x1=prim_points[p_f4->Points[p]].X-(mx<<ELE_SHIFT);
		z1=prim_points[p_f4->Points[p]].Z-(mz<<ELE_SHIFT);


		LogText(" free tri p %d  x1 %d z1 %d \n",p,x1,z1);


		lx=(z1*dtx_down)>>8;
		ly=(z1*dty_down)>>8;
		lx+=UV[0][0];
		ly+=UV[0][1];

		rx=(z1*dtx_down_r)>>8;
		ry=(z1*dty_down_r)>>8;

		rx+=UV[1][0];
		ry+=UV[1][1];

		LogText("	left (%d,%d) right (%d,%d)\n",lx,ly,rx,ry);

		p_f4->UV[p][0]=lx+(((rx-lx)*x1)>>8);
		p_f4->UV[p][1]=ly+(((ry-ly)*x1)>>8);

		LogText("		result (%d,%d) \n",lx+(((rx-lx)*x1)>>8),ly+(((ry-ly)*x1)>>8));
	}
}

//edge_min_z subtracted from z's
//x's are in world co-ords


void	scan_45(std::int32_t x1,std::int32_t z1,std::int32_t dx,std::int32_t dz)
{
	std::uint8_t	type=0;
	std::int32_t	count;
	std::int32_t	pp,p0,p1,p2,p3;
	struct	PrimFace3	*p_f3;

	count=abs(dx)>>ELE_SHIFT;
	x1=x1>>ELE_SHIFT;
	z1=z1>>ELE_SHIFT;
	
	if(dx<0)
	{
		dx=-1;
		type|=1;
	}
	else
	{
		dx=1;
	}

	if(dz<0)
	{
		type|=2;
		dz=-1;
	}
	else
	{
		dz=1;
	}

	pp=flag_blocks[(x1)+z1*MAX_BOUND_SIZE];
	while(count)
	{
		x1+=dx;
		z1+=dz;

		p1=flag_blocks[(x1)+z1*MAX_BOUND_SIZE];
		p2=flag_blocks[(x1-dx)+z1*MAX_BOUND_SIZE];
		p3=flag_blocks[(x1)+(z1-dz)*MAX_BOUND_SIZE];
		switch(type)
		{ //(vector goes from pp to p1)
				case	0:  //SE
					//  pp 	   p3
					//
					//	p2     p1
					p_f3=create_a_tri(p2,p1,pp,0,0);
					build_free_tri_texture_info(p_f3,x1-dx,z1-dz+(edge_min_z>>ELE_SHIFT));
					break;
				case	1:  //SW
					//  p3 	   pp
					//
					//	p1     p2
					p_f3=create_a_tri(p1,pp,p3,0,0);
					build_free_tri_texture_info(p_f3,x1,z1-dz+(edge_min_z>>ELE_SHIFT));
					break;
				case	2:  //NE
					//  p2 	   p1
					//
					//	pp     p3
					p_f3=create_a_tri(pp,p3,p1,0,0);
					build_free_tri_texture_info(p_f3,x1-dx,z1+(edge_min_z>>ELE_SHIFT));
					break;
				case	3:  //NW
					//  p1 	   p2
					//
					//	p3     pp
					p_f3=create_a_tri(p1,pp,p2,0,0);
					build_free_tri_texture_info(p_f3,x1,z1+(edge_min_z>>ELE_SHIFT));
					break;
		}

		pp=p1;
		count--;
	}
}

void	build_roof_grid(std::int32_t storey,std::int32_t y)
{
	std::int32_t	min_x=9999999,max_x=0,min_z=9999999,max_z=0;
	std::int32_t	width,depth;
	std::int32_t	x,z;

	std::int32_t	wall;
	struct	PrimFace4	*p_f4;
	struct	PrimFace3	*p_f3;
	std::int32_t	face_wall;
	//return;
	face_wall=-storey_list[storey].WallHead;

//	LogText(" build roof grid for storey %d at y %d \n",storey,y);

	global_y=edit_map[(storey_list[storey].DX>>ELE_SHIFT)][storey_list[storey].DZ>>ELE_SHIFT].Y<<FLOOR_HEIGHT_SHIFT;

	BOUNDS(storey_list[storey].DX,storey_list[storey].DZ);
	wall=storey_list[storey].WallHead;
	while(wall)
	{
		BOUNDS(wall_list[wall].DX,wall_list[wall].DZ);
		wall=wall_list[wall].Next;
	}
	min_x-=ELE_SIZE;
	min_z-=ELE_SIZE;
	max_x+=ELE_SIZE;
	max_z+=ELE_SIZE;

// bounds shound now be set, + bagginess
	width=(max_x-min_x)>>ELE_SHIFT;
	depth=(max_z-min_z)>>ELE_SHIFT;

	edge_min_z=min_z;

//now step over whole rect, flagging poins as either inside or outside or on the edge of the building
	build_edge_list(storey,0);
	dump_edge_list(depth);

	for(z=0;z<depth;z++)
	{
		std::int32_t	polarity=0;
		std::int32_t	edge;
		std::int32_t	dy;
		edge=edge_heads_ptr[z];
		for(x=min_x;x<max_x;x+=ELE_SIZE)
		{
			std::int32_t	done=0;
			while(!done&&edge)
			{
				if(x<edge_pool_ptr[edge].X)
				{
					if(polarity&1)
					{
						//struct	DepthStrip *me;

						//me=&edit_map[(x>>ELE_SHIFT)][(z+(edge_min_z>>ELE_SHIFT))];
						//me->Flags|=FLOOR_HIDDEN;
						dy=edit_map[(x>>ELE_SHIFT)][z+(edge_min_z>>ELE_SHIFT)].Y<<FLOOR_HEIGHT_SHIFT;
						add_point(x,y+dy,(z<<ELE_SHIFT)+edge_min_z);
						LogText(" ap %x %x ",x,(z<<ELE_SHIFT)+edge_min_z);

						//hidden
						flag_blocks[(x>>ELE_SHIFT)+z*MAX_BOUND_SIZE]=next_prim_point-1;
						LogText(" write flag %d at %d,%d \n",next_prim_point-1,x>>ELE_SHIFT,z);
					}
					done=1;
				}
				else
				if(x==edge_pool_ptr[edge].X)
				{
					//grid[][]			
					polarity++;
					//if(polarity&1)
					{
						struct	DepthStrip *me;

						//me=&edit_map[(x>>ELE_SHIFT)][(z+(edge_min_z>>ELE_SHIFT))];
						//me->Flags|=FLOOR_HIDDEN;

						dy=edit_map[(x>>ELE_SHIFT)][z+(edge_min_z>>ELE_SHIFT)].Y<<FLOOR_HEIGHT_SHIFT-global_y;
						add_point(x,y,(z<<ELE_SHIFT)+edge_min_z);
						LogText(" ap %x %x ",x,(z<<ELE_SHIFT)+edge_min_z);
						flag_blocks[(x>>ELE_SHIFT)+z*MAX_BOUND_SIZE]=next_prim_point-1;
						LogText(" write flag %d at %d,%d \n",next_prim_point-1,x>>ELE_SHIFT,z);

					}
					//flag_blocks[(x>>ELE_SHIFT)+z*MAX_BOUND_SIZE]=ON_EDGE;

					edge = edge_pool_ptr[ edge ].Next;
					done=1;
				}
				else
				if(x>edge_pool_ptr[edge].X)
				{
					polarity++;
					edge=edge_pool_ptr[edge].Next;
					if(edge==0)
					{
						//if(polarity&1)
							//flag_blocks[(x>>ELE_SHIFT)+z*MAX_BOUND_SIZE]=INSIDE;
						//else
							//flag_blocks[(x>>ELE_SHIFT)+z*MAX_BOUND_SIZE]=OUTSIDE;
					}
				}
			}
		}
		LogText(" \n");
	}
	build_bottom_edge_list(storey,y);

	{
		std::int32_t	wall;
		std::int32_t	px,pz;
		px=storey_list[storey].DX;
		pz=storey_list[storey].DZ-edge_min_z;
		wall=storey_list[storey].WallHead;
		while(wall)
		{
			std::int32_t	x,z;
			std::int32_t	dx,dz;
			x=wall_list[wall].DX;
			z=wall_list[wall].DZ-edge_min_z;

			dx=x-px;
			dz=z-pz;
			if(abs(dx)==abs(dz)&&dx)
			{	// 45 degree wall
				scan_45(px,pz,dx,dz);
			}
			px=x;
			pz=z;
			wall=wall_list[wall].Next;
		}

	}

//	show_grid((max_x>>ELE_SHIFT)-(min_x>>ELE_SHIFT),depth,min_x>>ELE_SHIFT);
	for(z=0;z<depth;z++)
	{
		std::int32_t	polarity=0;
		std::int32_t	edge;
		edge=edge_heads_ptr[z];
		for(x=min_x>>ELE_SHIFT;x<max_x>>ELE_SHIFT;x++)
		{
			std::int32_t	p0,p1,p2,p3;
			//
			// Build faces by finding quads with defined points
			//
			//  p0   p1
			//
			//	p3	 p2
			p0=flag_blocks[x+z*MAX_BOUND_SIZE];
			p1=flag_blocks[x+1+z*MAX_BOUND_SIZE];
			p2=flag_blocks[x+1+(z+1)*MAX_BOUND_SIZE];
			p3=flag_blocks[x+(z+1)*MAX_BOUND_SIZE];
			LogText(" x %d z %d p01234 %d %d %d %d \n",x,z,p0,p1,p2,p3);
			if(p0&&p1&&p2&&p3)
			{
				LogText(" use poly %d %d \n",x,z);
				//std::uint16_t p1,std::uint16_t p0,std::uint16_t p3,std::uint16_t p2,std::int16_t	texture_style,std::int16_t texture_piece)
				p_f4=create_a_quad(p0,p3,p1,p2,0,0);
				p_f4->ThingIndex=face_wall;
				LogText(" roof grid quad for map %d %d \n",x,z+(edge_min_z>>ELE_SHIFT));
				add_quad_to_walkable_list(next_prim_face4-1);
				build_face_texture_info(p_f4,edit_map[x][z+(edge_min_z>>ELE_SHIFT)].Texture);
				LogText(" add grid roof face to walkable \n");
			}
			else
			if(p0||p1||p2||p3)
			{ //the following code is shit, sorry
				std::uint8_t exist_flags=0;

#define	TL	(1)
#define	TR	(2)
#define	BL	(4)
#define	BR	(8)
				if(p0)
					exist_flags|=TL;
				if(p1)
					exist_flags|=TR;
				if(p2)
					exist_flags|=BR;
				if(p3)
					exist_flags|=BL;

				switch(exist_flags)
				{
					std::int32_t	xt,xb;
					std::int32_t	zl,zr;
					std::int32_t	pa,pb;

					case	(TR+BR):
						xt=cut_blocks[x*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
						xb=cut_blocks[x*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];

						if(xt&&xb)
						{
							//  pa  p1
							//
							//	pb	p2
							pa=next_prim_point;
							add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							p_f4=create_a_quad(pa,pb,p1,p2,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tr+br 1 at [%d][%d]\n",x,z);
						}
						else
						if(xt)
						{
							//   pa p1
							//
							//		p2
							pa=next_prim_point;
							add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
							p_f3=create_a_tri(p2,p1,pa,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tr+br 2 at [%d][%d]\n",x,z);
						}
						else
						if(xb)
						{
							//   	 p1
							//
							//	  pa p2
							pa=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							p_f3=create_a_tri(p2,p1,pa,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tr+br 3 at [%d][%d]\n",x,z);
						}
						else
							LogText(" pooerror1\n");

						break;
					case	(BL+BR):
							zl=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
							zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
							LogText(" zl %x zr %x \n",zl,zr);

							if(zl&&zr)
							{
								//  pa  pb
								//
								//	p3	p2
								pa=next_prim_point;
								add_point(x<<ELE_SHIFT,y,zl+edge_min_z);
								pb=next_prim_point;
								add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
								p_f4=create_a_quad(pa,p3,pb,p2,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done bl+br 1 at [%d][%d]\n",x,z);
							}
							else
							if(zl)
							{
								//   
								//	pa
								//	p3	 p2
								pa=next_prim_point;
								add_point(x<<ELE_SHIFT,y,zl+edge_min_z);
								p_f3=create_a_tri(p3,p2,pa,0,0);
								build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done bl+br 2 at [%d][%d]\n",x,z);
							}
							else
							if(zr)
							{
								//   
								//		 pa
								//	p3	 p2
								pa=next_prim_point;
								add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
								p_f3=create_a_tri(p3,p2,pa,0,0);
								build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done bl+br 3 at [%d][%d]\n",x,z);
							}
							else
								LogText(" pooerror2\n");
							break;
					case	(TL+BL):
						xt=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
						xb=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];

						if(xt&&xb)
						{
							//  p0  pa
							//
							//	p3	pb
							pa=next_prim_point;
							add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							p_f4=create_a_quad(p0,p3,pa,pb,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+bl 1 at [%d][%d]\n",x,z);
						}
						else
						if(xt)
						{
							//  p0 pa
							//
							//	p3	
							pa=next_prim_point;
							add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
							p_f3=create_a_tri(p3,pa,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+bl 2 at [%d][%d]\n",x,z);
						}
						else
						if(xb)
						{
							//  p0 
							//
							//	p3 pa	
							pa=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							p_f3=create_a_tri(p3,pa,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+bl 3 at [%d][%d]\n",x,z);
						}
/* can not be surely
						else
						{
							zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
							if(zr)
							{
								//  p0 
								//	    pa
								//	p3	
								pa=next_prim_point;
								add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
								create_a_tri(p3,pa,p0,0,0);

							}

						}
*/
							//LogText(" pooerror3\n");
						break;
					case	(TL+TR):
						zl=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
						zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
						LogText(" zl %x zr %x \n",zl,zr);

						if(zl&&zr)
						{
							//  p0  p1
							//
							//	pa	pb
							pa=next_prim_point;
							add_point(x<<ELE_SHIFT,y,zl+edge_min_z);
							pb=next_prim_point;
							add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
							p_f4=create_a_quad(p0,pa,p1,pb,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+tr 1 at [%d][%d]\n",x,z);
						}
						else
						if(zl)
						{
							//  p0   p1 
							//	pa
							//		 
							pa=next_prim_point;
							add_point(x<<ELE_SHIFT,y,zl+edge_min_z);
							p_f3=create_a_tri(pa,p1,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+tr 2 at [%d][%d]\n",x,z);
						}
						else
						if(zr)
						{
							//  p0   p1 
							//		 pa
							//	
							pa=next_prim_point;
							add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
							p_f3=create_a_tri(pa,p1,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+tr 3 at [%d][%d]\n",x,z);
						}
						else
							LogText(" pooerror4\n");
						break;

					case	(TR+BR+BL):
						xt=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
						zl=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
						LogText("POO1 xt %x zl %x \n",xt,zl);
						LogText(" [%d][%d] \n",x,z);
						if(xt&&zl)
						{
							//   pa p1
							//	pb
							//	p3	p2
							pa=next_prim_point;
							add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x)<<ELE_SHIFT,y,zl+edge_min_z);

							p_f4=create_a_quad(pa,pb,p1,p3,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							p_f3=create_a_tri(p3,p2,p1,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tr+br+bl 1 at [%d][%d]\n",x,z);
						}
						else
						if(xt||zl)
						{
							if(xt==(x+1)<<ELE_SHIFT||xt==0)
							{
								//      pa
								//	pb
								//	p3	p2
								LogText("special 1b \n");
								pb=next_prim_point;
								add_point((x)<<ELE_SHIFT,y,zl+edge_min_z);
								p_f4=create_a_quad(pb,p3,p1,p2,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tr+br+bl 2 at [%d][%d]\n",x,z);

							}
							else
							if(zl==(z+1)<<ELE_SHIFT||zl==0)
							{
								//   pa p1
								//	
								//	p3	p2
								LogText("special 1b \n");
								pa=next_prim_point;
								add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
								p_f4=create_a_quad(pa,p3,p1,p2,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tr+br+bl 3 at [%d][%d]\n",x,z);

							}
						}
//						else
//							create_a_tri(p3,p2,p1,0,0);



						break;
					case	(TL+BR+BL):
						xt=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
						zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
						LogText("POO2 xt %x zr %x \n",xt,zr);
						LogText(" [%d][%d] \n",x,z);
						LogText(" p0 (%d,%d,%d) p2 (%d,%d,%d) p3 (%d,%d,%d) \n",prim_points[p0].X,prim_points[p0].Y,prim_points[p0].Z,prim_points[p2].X,prim_points[p2].Y,prim_points[p2].Z,prim_points[p3].X,prim_points[p3].Y,prim_points[p3].Z);

						if(xt&&zr)
						{
							//  p0 pa
							//		 pb
							//	p3	 p2
							pa=next_prim_point;
							add_point(xt,y,(z<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);

							p_f4=create_a_quad(pb,pa,p2,p0,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							p_f3=create_a_tri(p3,p2,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+br+bl 1 at [%d][%d]\n",x,z);
						}
						else
						if(xt||zr)
						{
							if(xt==x<<ELE_SHIFT||xt==0)
							{
								//  pa  
								//		pb
								//	p3	p2
								LogText("special 2 \n");
								pb=next_prim_point;
								add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
								p_f4=create_a_quad(p0,p3,pb,p2,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tl+br+bl 2 at [%d][%d]\n",x,z);

							}
							else
							if(zr==((z+1)<<ELE_SHIFT)||zr==0)
							{
								//  p0 pa  
								//		
								//	p3	p2
								LogText("special 2b \n");
								pb=next_prim_point;
								add_point((xt),y,(z<<ELE_SHIFT)+edge_min_z);
								p_f4=create_a_quad(p0,p3,pa,p2,0,0);
								p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tl+br+bl 3 at [%d][%d]\n",x,z);

							}
						}
//						else
//							create_a_tri(p3,p2,p0,0,0);

						break;
					case	(TL+TR+BL):
						xb=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];
						zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
						LogText("POO3 xb %x zr %x \n",xb,zr);
						LogText(" [%d][%d] \n",x,z);
						LogText(" p0 (%d,%d,%d) p1 (%d,%d,%d) p3 (%d,%d,%d) \n",prim_points[p0].X,prim_points[p0].Y,prim_points[p0].Z,prim_points[p1].X,prim_points[p1].Y,prim_points[p1].Z,prim_points[p3].X,prim_points[p3].Y,prim_points[p3].Z);

						if(xb&&zr)
						{
							//  p0   p1
							//		 pb
							//	p3 pa
							pa=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);

							p_f4=create_a_quad(pa,pb,p3,p1,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							p_f3=create_a_tri(p3,p1,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+tr+bl 1 at [%d][%d]\n",x,z);
						}
						else
						if(xb||zr)
						{
							if(zr==((z+1)<<ELE_SHIFT)||zr==0)
							{
								//  p0  p1
								//		
								//	p3 pa	
								LogText("special 3 \n");
								pa=next_prim_point;
								add_point((xb),y,((z+1)<<ELE_SHIFT)+edge_min_z);
								p_f4=create_a_quad(p0,p3,p1,pa,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tl+tr+bl 2 at [%d][%d]\n",x,z);

							}
							else
							if(xb==(x<<ELE_SHIFT) || xb==0)
							{
								//  p0  p1
								//		pb
								//	pa3	
								LogText("special 3b \n");
								pb=next_prim_point;
								add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);
								p_f4=create_a_quad(p0,p3,p1,pb,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tl+tr+bl 3 at [%d][%d]\n",x,z);
							}
						}
//						else
//							create_a_tri(p3,p1,p0,0,0);

						break;

					case	(TL+TR+BR):
						xb=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];
						zl=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
						LogText("POO444 xb %x zl %x \n",xb,zl);
						LogText(" [%d][%d] \n",x,z);

						if(xb&&zl)
						{
							//  p0  p1
							//	pb
							//	 pa	p2
							pa=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x)<<ELE_SHIFT,y,zl+edge_min_z);
							LogText(" p0 (%d,%d,%d) p1 (%d,%d,%d) p2 (%d,%d,%d) \n",prim_points[p0].X,prim_points[p0].Y,prim_points[p0].Z,prim_points[p1].X,prim_points[p1].Y,prim_points[p1].Z,prim_points[p2].X,prim_points[p2].Y,prim_points[p2].Z);
							LogText(" pa (%d,%d,%d) pb (%d,%d,%d) \n",prim_points[pa].X,prim_points[pa].Y,prim_points[pa].Z,prim_points[pb].X,prim_points[pb].Y,prim_points[pb].Z);

							p_f4=create_a_quad(pb,pa,p0,p2,0,0);
							p_f4->ThingIndex=face_wall;
							build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
							p_f3=create_a_tri(p0,p2,p1,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl+tr+br 1 at [%d][%d]\n",x,z);
						}
						else
						if(xb||zl)
						{
							if(xb==(x+1)<<ELE_SHIFT||xb==0)
							{
								//  p0  p1
								//	pb
								//		p2a
								LogText("special 4 \n");
								pb=next_prim_point;
								add_point((x)<<ELE_SHIFT,y,zl+edge_min_z);
								p_f4=create_a_quad(p0,pb,p1,p2,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tl+tr+br 2 at [%d][%d]\n",x,z);

							}
							else
							if(zl==(z)<<ELE_SHIFT||zl==0)
							{
								//  pb   p1
								//	
								//	  pa p2
								LogText("special 4b \n");
								pa=next_prim_point;
								add_point((xb),y,((z+1)<<ELE_SHIFT)+edge_min_z);
								p_f4=create_a_quad(p0,pb,p1,p2,0,0);
								p_f4->ThingIndex=face_wall;
								build_free_quad_texture_info(p_f4,x,z+(edge_min_z>>ELE_SHIFT));
								LogText(" done tl+tr+br 3 at [%d][%d]\n",x,z);

							}
						}
//						else
//							create_a_tri(p2,p1,p0,0,0);
						break;
					case	(TL):
						xt=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
						zl=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
						LogText("SINGLE1 xt %x zl %x \n",xt,zl);
						LogText(" [%d][%d] \n",x,z);

						if(xt&&zl)
						{
							//  p0  pa
							//	pb

							pa=next_prim_point;
							add_point(xt,y,((z)<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x)<<ELE_SHIFT,y,zl+edge_min_z);

							p_f3=create_a_tri(pb,pa,p0,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tl 1 at [%d][%d]\n",x,z);
						}
						break;
					case	(TR):
						xt=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_TOP];
						zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
						LogText("SINGLE2 xt %x zr %x \n",xt,zr);
						LogText(" [%d][%d] \n",x,z);

						if(xt&&zr)
						{
							//  pa  p1
							//		pb

							pa=next_prim_point;
							add_point(xt,y,((z)<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);

							p_f3=create_a_tri(pa,pb,p1,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done tr 1 at [%d][%d]\n",x,z);
						}
						break;
					case	(BR):
						xb=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];
						zr=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_RIGHT];
						LogText("SINGLE3 xb %x zr %x \n",xb,zr);
						LogText(" [%d][%d] \n",x,z);

						if(xb&&zr)
						{
							//      pb
							//	 pa	p2

							pa=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x+1)<<ELE_SHIFT,y,zr+edge_min_z);

							p_f3=create_a_tri(pb,pa,p2,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done br 1 at [%d][%d]\n",x,z);
						}
						break;
					case	(BL):
						xb=cut_blocks[(x)*4+z*MAX_BOUND_SIZE*4+CUT_BLOCK_BOTTOM];
						zl=cut_blocks[(x)*4+(z)*MAX_BOUND_SIZE*4+CUT_BLOCK_LEFT];
						LogText("SINGLE4 xb %x zl %x \n",xb,zl);
						LogText(" [%d][%d] \n",x,z);

						if(xb&&zl)
						{
							//  pb  
							//	p3 pa

							pa=next_prim_point;
							add_point(xb,y,((z+1)<<ELE_SHIFT)+edge_min_z);
							pb=next_prim_point;
							add_point((x)<<ELE_SHIFT,y,zl+edge_min_z);

							p_f3=create_a_tri(p3,pa,pb,0,0);
							build_free_tri_texture_info(p_f3,x,z+(edge_min_z>>ELE_SHIFT));
							LogText(" done bl 1 at [%d][%d]\n",x,z);
						}
						break;




					default:
						LogText(" un supported p0...  (%d,%d,%d,%d)\n",p0,p1,p2,p3);
						break;




				}

			}

		}
	}

	bin_edge_list();
}




std::int32_t	is_storey_circular(std::int32_t storey)
{
	std::int32_t	sx,sz,wall;
	sx=storey_list[storey].DX;
	sz=storey_list[storey].DZ;
	wall=storey_list[storey].WallHead;

	while(wall)
	{
		if(sx==wall_list[wall].DX && sz==wall_list[wall].DZ)
		{
			return(1);
		}
		wall=wall_list[wall].Next;
	}
	return(0);
}

void	set_floor_hidden(std::int32_t storey)
{
	std::int32_t	min_x=9999999,max_x=0,min_z=9999999,max_z=0;
	std::int32_t	width,depth;
	std::int32_t	x,z;

	std::int32_t	wall;
//	return;
//	LogText(" set  floor hidden storey %d \n",storey);
	if(!is_storey_circular(storey))
	{
//		LogText(" not circular \n");
		return;
	}

	BOUNDS(storey_list[storey].DX,storey_list[storey].DZ);
	wall=storey_list[storey].WallHead;
	while(wall)
	{
		BOUNDS(wall_list[wall].DX,wall_list[wall].DZ);
		wall=wall_list[wall].Next;
	}
	min_x-=ELE_SIZE;
	min_z-=ELE_SIZE;
	max_x+=ELE_SIZE;
	max_z+=ELE_SIZE;

// bounds shound now be set, + bagginess
	width=(max_x-min_x)>>ELE_SHIFT;
	depth=(max_z-min_z)>>ELE_SHIFT;

	edge_min_z=min_z;

//now step over whole rect, flagging poins as either inside or outside or on the edge of the building
	build_edge_list(storey,0);
//	dump_edge_list(depth);

	for(z=0;z<depth;z++)
	{
		std::int32_t	polarity=0;
		std::int32_t	edge;
		edge=edge_heads_ptr[z];
		for(x=min_x;x<max_x;x+=ELE_SIZE)
		{
			std::int32_t	done=0;
			while(!done&&edge)
			{
				if(x<edge_pool_ptr[edge].X)
				{
					if(polarity&1)
					{
						struct	DepthStrip *me;

						me=&edit_map[(x>>ELE_SHIFT)][(z+(edge_min_z>>ELE_SHIFT))];
						me->Flags|=FLOOR_HIDDEN;
						//hidden
					}
					done=1;
				}
				else
				if(x==edge_pool_ptr[edge].X)
				{
					//grid[][]			
					polarity++;
					if(polarity&1)
					{
						struct	DepthStrip *me;

						me=&edit_map[(x>>ELE_SHIFT)][(z+(edge_min_z>>ELE_SHIFT))];
						me->Flags|=FLOOR_HIDDEN;

					}

					edge = edge_pool_ptr[ edge ].Next;
					done=1;
				}
				else
				if(x>edge_pool_ptr[edge].X)
				{
					polarity++;
					edge=edge_pool_ptr[edge].Next;
				}
			}
		}
	}
	bin_edge_list();
}

void	build_fe_mid_points(std::int32_t y,std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t	flag)
{
	std::int32_t	dx,dz,dist;

	dx=abs(x2-x1);
	dz=abs(z2-z1);

	dist=sqrl(SDIST2(dx,dz));

	if(dist==0)
		return;
	
	dx=(dx*BLOCK_SIZE)/dist;
	dz=(dz*BLOCK_SIZE)/dist;

	add_point(x1+dx,y,z1+dz);
	if(flag==0)
		add_point(x2-dx,y,z2-dz);
}

void	build_fire_escape_points(std::uint16_t	storey,std::int32_t	y,std::int32_t	flag)
{
	std::int32_t	walls[3],count=0,wall;
	std::int32_t	mx,mz,mx2,mz2;
	std::int32_t	p0=0;
	if(flag==0)
	{
		add_point(storey_list[storey].DX,y,storey_list[storey].DZ);
		//LogText(" point %d is storey \n",p0++);

	}
	wall=storey_list[storey].WallHead;
	while(wall&&count<3)
	{
		walls[count++]=wall;
		if(flag==0)
		{
			
			add_point(wall_list[wall].DX,y,wall_list[wall].DZ);
//			LogText(" point %d is rest of wall \n",p0++);
		}
		wall=wall_list[wall].Next;
	}

	mx=(storey_list[storey].DX+wall_list[walls[2]].DX)>>1;
	mz=(storey_list[storey].DZ+wall_list[walls[2]].DZ)>>1;

	mx2=(wall_list[walls[0]].DX+wall_list[walls[1]].DX)>>1;
	mz2=(wall_list[walls[0]].DZ+wall_list[walls[1]].DZ)>>1;

	if(flag==0)
	{
		add_point(mx,y,mz);
//			LogText(" point %d is middle left \n",p0++);
		add_point(mx2,y,mz2);
//			LogText(" point %d is middle right \n",p0++);
		
	}
	build_fe_mid_points(y,mx,mz,mx2,mz2,flag);
//			LogText(" point %d is middle mid left \n",p0++);
//			LogText(" point %d is middle mid right \n",p0++);
	build_fe_mid_points(y,wall_list[walls[2]].DX,wall_list[walls[2]].DZ,wall_list[walls[1]].DX,wall_list[walls[1]].DZ,flag);
//			LogText(" point %d is front mid left \n",p0++);
//			LogText(" point %d is front mid right \n",p0++);
}



#define	TEXTURE_PIECE_TOP_LEFT		(0)
#define	TEXTURE_PIECE_TOP_MIDDLE	(1)
#define	TEXTURE_PIECE_TOP_RIGHT		(2)
#define	TEXTURE_PIECE_LEFT			(3)
#define	TEXTURE_PIECE_MIDDLE		(4)
#define	TEXTURE_PIECE_RIGHT			(5)
#define	TEXTURE_PIECE_MIDDLE1		(6)
#define	TEXTURE_PIECE_MIDDLE2		(7)


struct	TXTY	texture_xy2[]=
{
	{0,0,0},				//0 
	{0,32,0},				//1 
	{0,64,0},				//2 
	{0,0,32},				//3 
	{0,32,32},			//4 
	{0,64,32},			//5 
	{0,96,32},			//6 
	{0,128,32},			//7 
	{0,0,64},				//8 
	{0,32,64},			//9 
	{0,64,64},			//10 
	{0,96,64},			//11  
	{0,128,64},			//12 
	{0,0,96},				//13  
	{0,32,96},			//14  
	{0,64,96},			//15  
	{0,96,96},			//16  
	{0,128,96},			//17  
	{0,0,128},			//18  
	{0,32,128},			//19 
	{0,64,128},			//20 
	{0,96,128},			//21  
	{0,128,128},			//22 
	{0,0,160},			//23  
	{0,32,160},			//24 
	{0,64,160},			//25 
	{0,96,160},			//26  
	{0,128,160},			//27 
	{3,4*32,6*32},			//28
	{0,0}

};

// textures[piece][style]
struct	TXTY	textures_xy[200][8]=
{//     TL      TM     TR       ML      MM      MR      MM1    MM2       
	{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
					   		     		   		     		   		     		   
	{{0,0,0,0},{0,1,0,0},{0,2,0,0},{0,0,1,0},{0,1,1,0},{0,2,1,0},{0,3,1,0},{0,4,1,0}},   //BROWN_BRICK1		
	{{0,0,2,0},{0,1,2,0},{0,2,2,0},{0,0,3,0},{0,1,3,0},{0,2,3,0},{0,3,3,0},{0,4,3,0}},   //BROWN_BRICK2
	{{0,6,5,0},{0,6,5,0},{0,6,5,0},{0,6,5,0},{0,6,5,0},{0,6,5,0},{0,6,5,0},{0,6,5,0}},	 //GREY_RIM2			
	{{0,0,4,0},{0,0,4,0},{0,0,4,0},{0,0,4,0},{0,0,4,0},{0,0,4,0},{0,1,4,0},{0,2,4,0}},	 //GREY_RIM1			
	{{0,3,7,0},{0,3,7,0},{0,3,7,0},{0,3,7,0},{0,3,7,0},{0,3,7,0},{0,3,7,0},{0,3,7,0}},	 //RED_WINDOW			
	{{0,4,7,0},{0,4,7,0},{0,4,7,0},{0,4,7,0},{0,4,7,0},{0,4,7,0},{0,5,7,0},{0,5,7,0}},	 //GREY_CORIGATED		
	{{0,7,7,0},{0,7,7,0},{0,7,7,0},{0,7,7,0},{0,7,7,0},{0,7,7,0},{0,7,7,0},{0,7,7,0}},	 //CRATES_SMALL_BROWN	
	{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},	 //GREY_POSH			
	{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},	 //HOTEL_SIGN1			
	{{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},	 //HOTEL_SIGN2			
	{{0,5,5,0},{0,5,5,0},{0,5,5,0},{0,5,5,0},{0,5,5,0},{0,5,5,0},{0,5,5,0},{0,5,5,0}},	 //Fence			

};

std::uint8_t	textures_flags[200][8];


struct	TextureInfo texture_info[]=
{ //  0     1     2     3     4     5     6     7
	{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},    //0
	{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},{1,0},	//1
	{2,0},{2,0},{2,0},{2,0},{2,0},{0,0},{9,0},{9,0},	//2
	{2,0},{2,0},{2,0},{2,0},{2,0},{0,0},{9,0},{9,0},	//3
	{4,0},{4,0},{4,0},{0,0},{0,0},{0,0},{10,0},{10,0},	//4
	{0,0},{0,0},{0,0},{0,0},{0,0},{11,0},{3,0},{8,0},    //5
	{0,0},{0,0},{0,0},{8,0},{8,0},{8,0},{8,0},{8,0},	//6
	{0,0},{0,0},{0,0},{5,0},{6,0},{6,0},{6,0},{7,0},	//7

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},

	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}

};

#define	PsetUV4(p_f4,x0,y0,x1,y1,x2,y2,x3,y3,page) p_f4->UV[0][0]=(x0);p_f4->UV[0][1]=(y0);p_f4->UV[1][0]=(x1);p_f4->UV[1][1]=(y1);p_f4->UV[2][0]=(x2);p_f4->UV[2][1]=(y2);p_f4->UV[3][0]=(x3);p_f4->UV[3][1]=(y3);p_f4->TexturePage=page;

void	build_face_texture_info(struct PrimFace4 *p_f4,std::uint16_t texture)
{
	std::uint8_t tx,ty,page;
	std::int32_t	tsize;
	std::int32_t	rot;

	tx=((struct	MiniTextureBits*)(&texture))->X<<5;
	ty=((struct	MiniTextureBits*)(&texture))->Y<<5;
	page=(std::uint8_t)(((struct	MiniTextureBits*)(&texture))->Page);
	tsize=floor_texture_sizes[((struct	MiniTextureBits*)(&texture))->Size]-1;
	rot=((struct	MiniTextureBits*)(&texture))->Rot;
	rot=(rot+3)&3;
	switch(rot)
	{
		case	0:		
			
			PsetUV4(	p_f4,tx,ty,tx+tsize,ty,tx,ty+tsize,tx+tsize,ty+tsize,page);
			break;
		case	1:		
			PsetUV4(	p_f4,tx+tsize,ty,tx+tsize,ty+tsize,tx,ty,tx,ty+tsize,page);
			break;
		case	2:		
			PsetUV4(	p_f4,tx+tsize,ty+tsize,tx,ty+tsize,tx+tsize,ty,tx,ty,page);
			break;
		case	3:		
			PsetUV4(	p_f4,tx,ty+tsize,tx,ty,tx+tsize,ty+tsize,tx+tsize,ty,page);
			break;
	}
}

struct	PrimFace4*	create_a_quad(std::uint16_t p1,std::uint16_t p0,std::uint16_t p3,std::uint16_t p2,std::int16_t	texture_style,std::int16_t texture_piece)
{
	struct	PrimFace4	*p4;
	std::int32_t	tx,ty;

	p4=&prim_faces4[next_prim_face4];
	next_prim_face4++;

	p4->Points[0]=p0;
	p4->Points[1]=p1;
	p4->Points[2]=p2;
	p4->Points[3]=p3;
/*
	p4->Bright[0]=128;
	p4->Bright[1]=128;
	p4->Bright[2]=128;
	p4->Bright[3]=128;
*/
	p4->DrawFlags=POLY_GT;
	p4->FaceFlags=0;

	if(texture_style)
	{
		if(texture_piece==TEXTURE_PIECE_MIDDLE)
		{
			if( (build_rand()&3)==0)
			{
				if(build_rand()&1)
					texture_piece=TEXTURE_PIECE_MIDDLE1;
				else
					texture_piece=TEXTURE_PIECE_MIDDLE2;
			}


		}
		tx=textures_xy[texture_style][texture_piece].Tx<<5;
		ty=textures_xy[texture_style][texture_piece].Ty<<5;
		p4->TexturePage=textures_xy[texture_style][texture_piece].Page;
//		LogText(" USE texture_style tx %d ty %d page %d \n",tx,ty,p4->TexturePage);
		p4->DrawFlags=textures_flags[texture_style][texture_piece];
	}
	else
	{
		tx=texture_xy2[texture_piece].Tx;
		ty=texture_xy2[texture_piece].Ty;
		p4->TexturePage=texture_xy2[texture_piece].Page;
	}

	switch(textures_xy[texture_style][texture_piece].Flip)
	{
		case	0:
			p4->UV[0][0]=tx;
			p4->UV[0][1]=ty;
			p4->UV[1][0]=tx+31;
			p4->UV[1][1]=ty;
			p4->UV[2][0]=tx;
			p4->UV[2][1]=ty+31;
			p4->UV[3][0]=tx+31;
			p4->UV[3][1]=ty+31;
			break;
		case	1: //flip x
			p4->UV[0][0]=tx+31;
			p4->UV[0][1]=ty;
			p4->UV[1][0]=tx;
			p4->UV[1][1]=ty;
			p4->UV[2][0]=tx+31;
			p4->UV[2][1]=ty+31;
			p4->UV[3][0]=tx;
			p4->UV[3][1]=ty+31;
			break;
		case	2: //flip y
			p4->UV[0][0]=tx;
			p4->UV[0][1]=ty+31;
			p4->UV[1][0]=tx+31;
			p4->UV[1][1]=ty+31;
			p4->UV[2][0]=tx;
			p4->UV[2][1]=ty;
			p4->UV[3][0]=tx+31;
			p4->UV[3][1]=ty;
			break;
		case	3: //flip x+y
			p4->UV[0][0]=tx+31;
			p4->UV[0][1]=ty+31;
			p4->UV[1][0]=tx;
			p4->UV[1][1]=ty+31;
			p4->UV[2][0]=tx+31;
			p4->UV[2][1]=ty;
			p4->UV[3][0]=tx;
			p4->UV[3][1]=ty;
			break;
	}

/*
	p4->UV[0][0]=tx;
	p4->UV[0][1]=ty;
	p4->UV[1][0]=tx+31;
	p4->UV[1][1]=ty;
	p4->UV[2][0]=tx;
	p4->UV[2][1]=ty+31;
	p4->UV[3][0]=tx+31;
	p4->UV[3][1]=ty+31;
*/
	return(p4);
}

struct	PrimFace3*	create_a_tri(std::uint16_t p2,std::uint16_t p1,std::uint16_t p0,std::int16_t	texture_id,std::int16_t texture_piece)
{
	struct	PrimFace3	*p3;
	std::int32_t	tx,ty;
	texture_id=texture_id;
	p3=&prim_faces3[next_prim_face3];
	next_prim_face3++;

	p3->Points[0]=p0;
	p3->Points[1]=p1;
	p3->Points[2]=p2;
/*
	p3->Bright[0]=128;
	p3->Bright[1]=128;
	p3->Bright[2]=128;
*/

	p3->DrawFlags=POLY_GT;

	tx=texture_xy2[texture_piece].Tx;
	ty=texture_xy2[texture_piece].Ty;
	p3->UV[0][0]=tx;
	p3->UV[0][1]=ty;
	p3->UV[1][0]=tx+31;
	p3->UV[1][1]=ty;
	p3->UV[2][0]=tx;
	p3->UV[2][1]=ty+31;

	p3->TexturePage=texture_xy2[texture_piece].Page;
	return(p3);
	
}

void	set_texture_fe(struct	PrimFace4 *p4,std::int32_t xw,std::int32_t xh,std::int32_t type)
{
	std::int32_t	tx,ty;
	switch(type)
	{
		case	0:
				tx=0;
				ty=6*32;
			break;
		case	1:
				tx=5*32;
				ty=4*32;
			break;
	}

	xw=1;
	xh=1;

	p4->UV[0][0]=tx;
	p4->UV[0][1]=ty;
	p4->UV[1][0]=tx+32*xw;
	p4->UV[1][1]=ty;
	p4->UV[2][0]=tx;
	p4->UV[2][1]=ty+32*xh;
	p4->UV[3][0]=tx+32*xw;
	p4->UV[3][1]=ty+32*xh;
	p4->TexturePage=1;
	
}

//     0--------------------------1 
//
//
//	   4		6		  7		  5 
//
//
//	   3	    8		  9		  2 
//

#ifdef	OLD_DOG_POO_OF_A_SYSTEM_OR_IS_IT
/*
std::uint16_t	next_face_type=1;
std::uint16_t	next_face_link=1;
std::uint16_t	next_face_connection=1;

struct	FaceLink
{
	std::uint16_t	Index;
	std::uint8_t	Count;
};

std::uint16_t	face_type_index[50];  // for face type %1  returns index into face_links which you add your ID to , to pull out an inde and number of faces connected to
struct	FaceLink	face_links[50*20]; // 
std::uint16_t	face_connection_pool[2000];



//each type has a variable number of ID's


void	add_connection_for_current_id(std::int32_t offset)
{
	face_connection_pool[next_face_connection++]=offset;
	face_links[next_face_link-1].Count++;
}

std::int32_t	advance_face_id_number()
{
	face_links[next_face_link].Count=0;
	face_links[next_face_link].Index=next_face_connection;

	next_face_link++;
	return(next_face_link-1);


}

std::int32_t	advance_face_type_number()
{
	face_type_index[next_face_type]=next_face_link;
	face_links[next_face_link].Count=0;
	next_face_type++;

	return(next_face_type-1);
}
*/
#endif



#define	FE_FIRST_SLOPE		1
#define	FE_PLINTH1			2
#define	FE_WALKWAY1			3
#define	FE_PLINTH2			4
#define	FE_SLOPE2			5

#define	FE_FIRST_SLOPE_RAIL	-6
#define	FE_PLINTH1_RAIL_A	-7
#define	FE_PLINTH1_RAIL_B	-8
#define	FE_WALKWAY1_RAIL	-9
#define	FE_PLINTH2_RAIL_A	-10
#define	FE_PLINTH2_RAIL_B	-11
#define	FE_SLOPE2_RAIL		-12


std::int16_t	face_offsets[]=
{
	0,
	-1,0,				//1
	1,-2,0,			//3
	2,1,0,				//6
	-1,12,0,				//9
	-12,-1,0				//12

};

std::uint16_t	id_offset[]=
{
	0,1,3,6,9,12
};

std::int32_t	next_connected_face(std::int32_t type,std::int32_t id,std::int32_t count)
{
	switch(type)
	{
		case	FACE_TYPE_FIRE_ESCAPE:
			std::int32_t	start;

			start=id_offset[id];
//			LogText(" id %d start %d count %d \n",id,start,count);
			return(face_offsets[start+count]);
			break;
	}
	return(0);
}
void	build_firescape(std::int32_t storey)
{
	std::int32_t	y=0;
	std::int32_t	count=0;
	std::int32_t	sp[20];
	struct	PrimFace4	*p4;
	std::int32_t	wall;


	wall=-storey_list[storey].WallHead;

	while(count<storey_list[storey].Height)
	{

		sp[count]=next_prim_point;		
		if(count==0)
		{
			build_fire_escape_points(storey,y,1);
			build_fire_escape_points(storey,y+BLOCK_SIZE,1);
		}
		else
		{
			build_fire_escape_points(storey,y,0);
			build_fire_escape_points(storey,y+BLOCK_SIZE,0);
		}


		if(count>0)
		{
			//banisters
			p4=create_a_quad(sp[count]+3+10,sp[count]+0+10,sp[count]+3,sp[count]+4,0,0);
			set_texture_fe(p4,1,1,0);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);

			p4=create_a_quad(sp[count]+8+10,sp[count]+3+10,sp[count]+8,sp[count]+3,0,0);
			set_texture_fe(p4,1,1,0);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);

			p4=create_a_quad(sp[count]+2+10,sp[count]+9+10,sp[count]+2,sp[count]+9,0,0);
			set_texture_fe(p4,1,1,0);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);

			p4=create_a_quad(sp[count]+1+10,sp[count]+2+10,sp[count]+1,sp[count]+2,0,0);
			set_texture_fe(p4,1,1,0);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);

			p4=create_a_quad(sp[count]+7+10,sp[count]+6+10,sp[count]+7,sp[count]+6,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);


			// floors			
			p4=create_a_quad(sp[count],sp[count]+1,sp[count]+4,sp[count]+5,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_WALKWAY1;
			add_quad_to_walkable_list(next_prim_face4-1);

			p4=create_a_quad(sp[count]+4,sp[count]+6,sp[count]+3,sp[count]+8,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_PLINTH2;
			add_quad_to_walkable_list(next_prim_face4-1);

			p4=create_a_quad(sp[count]+7,sp[count]+5,sp[count]+9,sp[count]+2,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_PLINTH1;
			add_quad_to_walkable_list(next_prim_face4-1);
		}

		if(count==1)
		{
			//first slope
			//floor
			insert_collision_vect(prim_points[sp[count-1]].X,prim_points[sp[count-1]].Y,prim_points[sp[count-1]].Z,
									prim_points[sp[count-1]+1].X,prim_points[sp[count-1]+1].Y,prim_points[sp[count-1]+1].Z,0,0,next_prim_face4);

			p4=create_a_quad(sp[count-1],sp[count]+7,sp[count-1]+1,sp[count]+9,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4->ThingIndex=wall;
			p4->FaceFlags|=FACE_FLAG_WALKABLE;
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_FIRST_SLOPE;
			add_quad_to_walkable_list(next_prim_face4-1);

			
			//bannister
			p4=create_a_quad(sp[count]+9+10,sp[count-1]+3,sp[count]+9,sp[count-1]+1,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_FIRST_SLOPE_RAIL;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
		}
		else
		if(count>1)
		{
			//continue slope
			p4=create_a_quad(sp[count-1]+6,sp[count]+7,sp[count-1]+8,sp[count]+9,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED; //|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_SLOPE2;
			add_quad_to_walkable_list(next_prim_face4-1);

			//rail
			p4=create_a_quad(sp[count]+9+10,sp[count-1]+8+10,sp[count]+9,sp[count-1]+8,0,0);
			set_texture_fe(p4,1,1,1);
			p4->DrawFlags=POLY_T|POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED;
			p4->ThingIndex=wall;
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4->Type=FACE_TYPE_FIRE_ESCAPE;
			p4->ID=FE_SLOPE2_RAIL;
			
		}

		count++;
		y+=BLOCK_SIZE*4;
	}
}


//   x1->x2->x3

//see diag 5.2 page 164 of van dam 1


std::int32_t	calc_sin_from_cos(std::int32_t sin)
{
	std::int32_t	cos;

	cos=(sqrl((1<<14)-((sin*sin)>>14)));
	cos=cos<<7;
	return(cos);

}

void	calc_new_corner_point(std::int32_t	x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t x3,std::int32_t z3,std::int32_t width,std::int32_t *res_x,std::int32_t *res_z)
{

	std::int32_t	vx,vz,dist;
	std::int32_t	wx,wz;
	std::int32_t	ax,az;
	std::int32_t	angle;

	std::int32_t	z;


//	LogText(" x1 %d z1 %d x2 %d z2 %d x3 %d z3 %d \n",x1,z1,x2,z2,x3,z3);

	vx=x2-x1;
	vz=z2-z1;

	wx=-(x3-x2);
	wz=-(z3-z2);



	z=vx*wz-vz*wx;
//	LogText(" z= %d \n",z);

	dist=sqrl(SDIST2(vx,vz));
//	LogText(" V (%d,%d) dist %d \n",vx,vz,dist);
	if(dist==0)
		return;

	vx=(vx<<14)/dist;
	vz=(vz<<14)/dist;

	dist=sqrl(SDIST2(wx,wz));
//	LogText(" W (%d,%d) dist %d \n",wx,wz,dist);
	if(dist==0)
		return;

	wx=(wx<<14)/dist;
	wz=(wz<<14)/dist;

	ax=(vx+wx);
	az=(vz+wz);

	dist=sqrl(SDIST2(ax,az));
//	LogText(" A (%d,%d) dist %d\n",ax,az,dist);
	if(dist==0)
	{
		//u & V cancel each other out
		*res_x = ((vz*width)>>14)+x2;
		*res_z = -((vx*width)>>14)+z2;

		return;
	}


	ax=(ax<<14)/dist;
	az=(az<<14)/dist;

//	LogText(" normalised vectors V(%d,%d) W(%d,%d) A(%d,%d)\n",vx,vz,wx,wz,ax,az);

	angle=(vx*ax+vz*az)>>14;
//	LogText(" cos angle = %d \n",angle);
//we now have the cos of the angle
	angle=calc_sin_from_cos(angle); //<<2;
	//LogText(" sin angle(<<14) = %d \n",angle);

//we now have the sin of the angle

// sin@=o/h

	if(angle==0)
		dist=width;
	else
	{
		dist=(width<<14)/angle;
	}
//	LogText(" dist= %d \n",dist);

	if(z>0)
	{
		dist=-dist;
		
	}


	*res_x=((ax*dist)>>14)+x2;
	*res_z=((az*dist)>>14)+z2;

//	LogText("  resx %d resz %d \n",(ax*dist)>>14,(az*dist)>>14);

}


void	build_ledge(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t wall,std::int32_t storey,std::int32_t height)
{
	std::int32_t	sp[10],count=0;
	std::int32_t	index,c0;
	std::int32_t	dx,dz;
	std::int32_t	px,pz,rx,rz;

	storey=storey;
	sp[0]=next_prim_point;
	add_point(x,y,z);
	index=wall;
	while(index)
	{
		dx=wall_list[index].DX;
		dz=wall_list[index].DZ;
		add_point(dx,y,dz);
		index=wall_list[index].Next;
		count++;
	}
	sp[1]=next_prim_point;
	add_point(x,y,z);

	px=x;
	pz=z;
	index=wall;
	while(index)
	{
		std::int32_t	next;
//		LogText(" calc corner %d \n",index);
		dx=wall_list[index].DX;
		dz=wall_list[index].DZ;
		next=wall_list[index].Next;
		if(next)
		{
			calc_new_corner_point(px,pz,dx,dz,wall_list[next].DX,wall_list[next].DZ,BLOCK_SIZE,&rx,&rz);
			add_point(rx,y,rz);
		}
		else
		{
			calc_new_corner_point(px,pz,dx,dz,wall_list[wall].DX,wall_list[wall].DZ,BLOCK_SIZE,&rx,&rz);
			add_point(rx,y,rz);
			prim_points[sp[1]].X=rx;
			prim_points[sp[1]].Z=rz;
		}

		px=dx;
		pz=dz;
		index=wall_list[index].Next;
	}
//	add_point(prim_points[sp[1]].X,y,prim_points[sp[1]].Z);

	y+=height;
	sp[2]=next_prim_point;
	for(c0=sp[1];c0<sp[2];c0++)
	{
		prim_points[next_prim_point].X=prim_points[c0].X;
		prim_points[next_prim_point].Y=y+2;
		prim_points[next_prim_point].Z=prim_points[c0].Z;
		next_prim_point++;
		
	}

	sp[3]=next_prim_point;
	add_point(x,y,z);
	index=wall;
	while(index)
	{
		dx=wall_list[index].DX;
		dz=wall_list[index].DZ;
		add_point(dx,y+2,dz);
		index=wall_list[index].Next;
	}

//	LogText("ledge sp %d %d %d %d next p point %d count %d \n",sp[0],sp[1],sp[2],sp[3],next_prim_point,count);
	for(c0=0;c0<=count;c0++)
	{
	 	create_a_quad(sp[1]+c0,sp[1]+c0+1,sp[0]+c0,sp[0]+c0+1,0,18);
//		LogText(" create a face with points %d %d %d %d \n",sp[1]+c0,sp[1]+c0+1,sp[0]+c0,sp[0]+c0+1);
	 	create_a_quad(sp[2]+c0,sp[2]+c0+1,sp[1]+c0,sp[1]+c0+1,0,18);
	 	create_a_quad(sp[3]+c0,sp[3]+c0+1,sp[2]+c0,sp[2]+c0+1,0,18);
	}
}

void	build_fence_points_and_faces(std::int32_t y1,std::int32_t y2,std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t wall,std::uint8_t posts)
{
	std::int32_t	wcount,wwidth,dx,dz,dist;
	std::int32_t	start_point;
	std::int32_t	texture,texture_style;
	struct	PrimFace4	*p_f4;
	std::int32_t	px,pz;

	texture_style=wall_list[wall].TextureStyle;
	texture=TEXTURE_PIECE_MIDDLE;

	start_point=next_prim_point;

	dx=abs(x2-x1);
	dz=abs(z2-z1);

	dist=sqrl(SDIST2(dx,dz));
	if(dist==0)
		return;

 	wcount=(dist/(BLOCK_SIZE*4));
	if(wcount==0)
		wcount=1;
	wwidth=dist/(wcount);

	dx=(x2-x1);
	dz=(z2-z1);


	dx=(dx<<10)/dist;
	dz=(dz<<10)/dist;

	px=((dz*(10))>>10);
	pz=-((dx*(10))>>10);

	while(wcount)
	{
		std::int32_t	p,p1,p2;
		p=next_prim_point;

		add_point(x1,y2,z1);
		add_point(x1,y1,z1);

		add_point(x1+px*5,y2+45,z1+pz*5);
		add_point(x1+px*1,y2+10,z1+pz*1);


		x1=x1+((dx*(wwidth-20))>>10);
		z1=z1+((dz*(wwidth-20))>>10);

		p1=next_prim_point;

		add_point(x1,y2,z1);
		add_point(x1,y1,z1);

		add_point(x1+px*4,y2+45,z1+pz*4);
		add_point(x1+px*1,y2+10,z1+pz*1);

		p_f4=create_a_quad(p,p1+0,p+1,p1+1,texture_style,texture);
		p_f4->ThingIndex=-wall;
		p_f4->DrawFlags|=(POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED);

		p_f4=create_a_quad(p+2,p1+2,p+3,p1+3,texture_style,texture);
		p_f4->ThingIndex=-wall;
		p_f4->DrawFlags|=(POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED);


		p=next_prim_point;
		add_point(x1+px,y2,z1+pz);
		add_point(x1+px,y1,z1+pz);
		add_point(x1-px,y2,z1-pz);
		add_point(x1-px,y1,z1-pz);

		add_point(x1+px*6,y2+50,z1+pz*6);
		add_point(x1+px*4,y2+50,z1+pz*4);

		x1=x1+((dx*(20))>>10);
		z1=z1+((dz*(20))>>10);

		p1=next_prim_point;
		add_point(x1+px,y2,z1+pz);
		add_point(x1+px,y1,z1+pz);
		add_point(x1-px,y2,z1-pz);
		add_point(x1-px,y1,z1-pz);

		add_point(x1+px*6,y2+50,z1+pz*6);
		add_point(x1+px*4,y2+50,z1+pz*4);

		p_f4=create_a_quad(p,p1,p+1,p1+1,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p1+2,p+2,p1+3,p+3,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p+2,p+0,p+3,p+1,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p1,p1+2,p1+1,p1+3,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;


		p_f4=create_a_quad(p+4,p1+4,p,p1,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p1+5,p+5,p1+2,p+2,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p1+4,p1+5,p1,p1+2,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p+5,p+4,p+2,p,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

		p_f4=create_a_quad(p+4,p+5,p1+4,p1+5,texture_style,TEXTURE_PIECE_TOP_LEFT);
		p_f4->ThingIndex=-wall;

//		p_f4=create_a_quad(p+0,p+2,p1,p1+2,texture_style,TEXTURE_PIECE_TOP_LEFT);
//		p_f4->ThingIndex=-wall;

		
		wcount--;

	}

	
}

void	build_fence(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t wall,std::int32_t storey,std::int32_t height)
{
		build_fence_points_and_faces(y,y+((height*3)>>2)+2,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall,1);
		wall_list[wall].WindowCount=0;

}

void	append_wall_prim(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t wall,std::int32_t storey,std::int32_t height)
{
	std::int32_t	c0;
	std::int32_t	sp[10];
//	std::int32_t	sf4[10];
	std::int32_t	texture,texture_style;
	struct	PrimFace4	*p_f4;

	set_build_seed(x*z*storey*wall+x+z+y);

	texture_style=wall_list[wall].TextureStyle;
	if(texture_style==0)
		texture_style=1;

//	LogText(" wall %d texture style %d \n",wall,texture_style);
	insert_collision_vect(x,y,z,wall_list[wall].DX,y,wall_list[wall].DZ,0,height,next_prim_face4);

	if(storey_list[storey].StoreyType==STOREY_TYPE_FENCE)
	{
		build_fence(x,y,z,wall,storey,height);
		return;
	}

	//if(texture_style)
		//LogText(" wall %d storey %d style %d \n",wall,storey,texture_style);

	//if(wall<0)
		//LogText(" poo \n");
	if(!(wall_list[wall].WallFlags&FLAG_WALL_AUTO_WINDOWS)&& wall_list[wall].WindowCount==0)
	{
		sp[0]=build_row_wall_only_points_at_y(y+height+2      ,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);
		sp[1]=build_row_wall_only_points_at_y(y             ,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);
		for(c0=0;c0<wall_list[wall].WindowCount;c0++)
		{
			texture=TEXTURE_PIECE_MIDDLE;
			if(c0==0)
			{
				if(storey_list[storey].Next==0)
					texture=TEXTURE_PIECE_TOP_RIGHT;
				else
					texture=TEXTURE_PIECE_RIGHT;
			}
			else
			if(c0==wall_list[wall].WindowCount-1)
			{
				
				if(storey_list[storey].Next==0)
					texture=TEXTURE_PIECE_TOP_LEFT;
				else
					texture=TEXTURE_PIECE_LEFT;
			}
			else
			{
				if(storey_list[storey].Next==0)
					texture=TEXTURE_PIECE_TOP_MIDDLE;
				else
					texture=TEXTURE_PIECE_MIDDLE;
				
			}
//			LogText(" face %d texture  %d \n",c0,texture);

			p_f4=create_a_quad(sp[0]+c0,sp[0]+c0+1,sp[1]+c0,sp[1]+c0+1,texture_style,texture);
//			LogText("HELLO face %d = wall %d ts %d t %d\n",next_prim_face4-1,wall,texture_style,texture);
			p_f4->ThingIndex=-wall;
//			if(wall==380)
//				LogText(" wall 380 face = %d \n",next_prim_face4);
			if(texture_style==11)
			{
				p_f4->DrawFlags|=(POLY_FLAG_DOUBLESIDED|POLY_FLAG_MASKED);
			}
		}
		wall_list[wall].WindowCount=0;
	}
	else
	{
		

		sp[0]=build_row_wall_points_at_y(y+height      ,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);
		sp[1]=build_row_wall_points_at_y(y+(height*2)/3,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);
		sp[2]=build_row_wall_points_at_y(y+(height*1)/3,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);
		sp[3]=build_row_wall_points_at_y(y             ,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);

		sp[4]=build_row_window_depth_points_at_y(y+(height*2)/3,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);
		sp[5]=build_row_window_depth_points_at_y(y+(height*1)/3,x,z,wall_list[wall].DX,wall_list[wall].DZ,wall);

		for(c0=0;c0<wall_list[wall].WindowCount*2+1;c0++)
		{
			p_f4=create_a_quad(sp[0]+c0,sp[0]+c0+1,sp[1]+c0,sp[1]+c0+1,0,0);
			p_f4->ThingIndex=-wall;
			p_f4=create_a_quad(sp[2]+c0,sp[2]+c0+1,sp[3]+c0,sp[3]+c0+1,0,0);
			p_f4->ThingIndex=-wall;
			if(!(c0&1))
			{
				p_f4=create_a_quad(sp[1]+c0,sp[1]+c0+1,sp[2]+c0,sp[2]+c0+1,0,0);
				p_f4->ThingIndex=-wall;
			}

		}
		for(c0=0;c0<wall_list[wall].WindowCount;c0++)
		{
			p_f4=create_a_quad(sp[1]+c0*2+1,sp[1]+c0*2+2,sp[4]+c0*2,sp[4]+c0*2+1,0,0);  //lid 
			p_f4->ThingIndex=-wall;
			p_f4=create_a_quad(sp[1]+c0*2+1,sp[4]+c0*2,sp[2]+c0*2+1,sp[5]+c0*2,0,0);  //side1
			p_f4->ThingIndex=-wall;
			p_f4=create_a_quad(sp[4]+c0*2+1,sp[1]+c0*2+2,sp[5]+c0*2+1,sp[2]+c0*2+2,0,0);  //side2
			p_f4->ThingIndex=-wall;
			p_f4=create_a_quad(sp[5]+c0*2,sp[5]+c0*2+1,sp[2]+c0*2+1,sp[2]+c0*2+2,0,0);  //base
			p_f4->ThingIndex=-wall;
		}
	}
}




std::int32_t	find_near_prim_point(std::int32_t x,std::int32_t z,std::int32_t sp,std::int32_t ep)
{
	std::int32_t	best,best_dist=0x7fffffff,dx,dz,dist,c0;

	for(c0=sp;c0<ep;c0++)
	{
		dx=(prim_points[c0].X-x);
		dz=(prim_points[c0].Z-z);
		dist=SDIST2(dx,dz);
		if(dist<best_dist)
		{
			best_dist=dist;
			best=c0;
		}
	}

	return(best);
}


void	create_recessed_storey_points(std::int32_t y,std::int32_t storey,std::int32_t count,std::int32_t size)
{

	std::int32_t	px,pz,index,dx,dz;
	std::int32_t	rx,rz;
	std::int32_t	wall;
	std::int32_t	sp;

	count=count;	
	sp=next_prim_point;

	px=storey_list[storey].DX;
	pz=storey_list[storey].DZ;
	add_point(px,y,pz); //this gets replaced later
	index=storey_list[storey].WallHead;
	wall=index;

	while(index)
	{
		std::int32_t	next;
//		LogText(" calc corner %d \n",index);
		dx=wall_list[index].DX;
		dz=wall_list[index].DZ;
		next=wall_list[index].Next;
		if(next)
		{
			calc_new_corner_point(px,pz,dx,dz,wall_list[next].DX,wall_list[next].DZ,size,&rx,&rz);
			add_point(rx,y,rz);
		}
		else
		{
			calc_new_corner_point(px,pz,dx,dz,wall_list[wall].DX,wall_list[wall].DZ,size,&rx,&rz);
			add_point(rx,y,rz);
			prim_points[sp].X=rx;
			prim_points[sp].Z=rz;
		}

		px=dx;
		pz=dz;
		index=wall_list[index].Next;
	}
}

void scan_triangle(std::int32_t x0, std::int32_t y0, std::int32_t z0,std::int32_t x1, std::int32_t y1, std::int32_t z1,std::int32_t x2, std::int32_t y2, std::int32_t z2,std::int32_t flag)
{

	std::int32_t	px,py,pz;
	std::int32_t	face_x,face_y,face_z;
	std::int32_t	c0;
	std::int32_t	s,t,step_s,step_t;
	std::int32_t	vx,vy,vz,wx,wy,wz;
	struct	DepthStrip *me;
	std::int32_t	quad;
	std::int32_t	len;
	char	str[100];
	std::uint8_t	info=0;

	face_x = x0;
	face_y = y0;
	face_z = z0;

	vx = x1 - x0;
	vy = y1 - y0;   //vector from point 0 to point 1
	vz = z1 - z0;

	wx = x2 - x0;   //vector from point 1 to point 2 
	wy = y2 - y0;
	wz = z2 - z0;
	
	len=(sqrl(vx*vx+vy*vy+vz*vz)>>7);
	if(len<2)
		len=2;
	step_s=(1<<7)/len;
	len=(sqrl(wx*wx+wy*wy+wz*wz)>>7);
	if(len<2)
		len=2;
	step_t=(1<<7)/len;

	if(step_s==0)
		step_s=256;
	if(step_t==0)
		step_t=256;

	for(s=5; s<(245)                 ; s+=step_s)
	for(t=5; t<(245) && ((s+t)<(245)); t+=step_t)
	{
		px=face_x+((s*vx)>>8)+((t*wx)>>8);
		py=face_y+((s*vy)>>8)+((t*wy)>>8);
		pz=face_z+((s*vz)>>8)+((t*wz)>>8);

		if((px>>8)>0&&(px>>8)<EDIT_MAP_WIDTH&&(pz>>8)>0&&(pz>>8)<EDIT_MAP_DEPTH)
		{
			me=&edit_map[(px>>ELE_SHIFT)][(pz>>ELE_SHIFT)];
			me->Flags|=FLOOR_HIDDEN;
			//LogText(" dx %d dz %d  hidden \n",px>>ELE_SHIFT,pz>>ELE_SHIFT);
//				me->Texture|=TS_TEXT_SORT;
		}
	}
}


void	flag_floor_tiles_for_quad(std::int32_t	p0,std::int32_t	p1,std::int32_t	p2,std::int32_t	p3)
{
	std::int32_t	x0,y0,z0,x1,y1,z1,x2,y2,z2,x3,y3,z3;


	x0=prim_points[p0].X;
	y0=prim_points[p0].Y;
	z0=prim_points[p0].Z;

	x1=prim_points[p1].X;
	y1=prim_points[p1].Y;
	z1=prim_points[p1].Z;

	x2=prim_points[p2].X;
	y2=prim_points[p2].Y;
	z2=prim_points[p2].Z;

	x3=prim_points[p3].X;
	y3=prim_points[p3].Y;
	z3=prim_points[p3].Z;


	scan_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,0);
	scan_triangle(x1,y1,z1,x3,y3,z3,x2,y2,z2,0);

}

void	flag_floor_tiles_for_tri(std::int32_t	p0,std::int32_t	p1,std::int32_t	p2)
{
	std::int32_t	x0,y0,z0,x1,y1,z1,x2,y2,z2;


	x0=prim_points[p0].X;
	y0=prim_points[p0].Y;
	z0=prim_points[p0].Z;

	x1=prim_points[p1].X;
	y1=prim_points[p1].Y;
	z1=prim_points[p1].Z;

	x2=prim_points[p2].X;
	y2=prim_points[p2].Y;
	z2=prim_points[p2].Z;



	scan_triangle(x0,y0,z0,x1,y1,z1,x2,y2,z2,0);

}


void	build_roof(std::uint16_t storey,std::int32_t y)
{
//	std::int32_t	x1,z1,x2,z2,x3,z3;
	std::int32_t	wall;
//	,prev_wall,prev_prev_wall;
	std::int32_t	sp[10];
	std::int32_t	roof;
	std::int32_t	p0,p1,p2,p3;
//	std::int32_t	rx,rz;
	std::int32_t	count;
	std::int32_t	roof_height=BLOCK_SIZE*3;
	std::int32_t	c0;
	std::int32_t	overlap;

	std::int32_t	roof_flags;
	std::int32_t	roof_rim;
	std::int32_t	overlap_height=BLOCK_SIZE>>1;

	std::int32_t	poox,pooz;

	//calc_new_corner_point(0,0,0,50,0,100,12,&poox,&pooz);
	overlap=BLOCK_SIZE>>1;
/*
	roof=storey_list[storey].Roof;
	roof_flags=storey_list[roof].StoreyFlags;

	if(roof_flags&FLAG_ROOF_WALLED)
		overlap_height=BLOCK_SIZE;

	if(roof_flags&FLAG_ROOF_FLAT)
		roof_height=0;

	overlap=BLOCK_SIZE>>1;

	if(roof_flags&FLAG_ROOF_OVERLAP_SMALL)
		overlap=BLOCK_SIZE>>1;
	else
	if(roof_flags&FLAG_ROOF_OVERLAP_MEDIUM)
		overlap=BLOCK_SIZE;
*/

	if((storey_list[storey].StoreyFlags&FLAG_STOREY_ROOF_RIM))
	{
		
		if(storey_list[storey].WallHead)// && storey_list[storey].Roof)
		{

			//
			// Roof points arround top of wall
			//

			sp[0]=next_prim_point;  
			add_point(storey_list[storey].DX,y+2,storey_list[storey].DZ);
			wall=storey_list[storey].WallHead;
			count=0;
			while(wall)
			{

				add_point(wall_list[wall].DX,y+2,wall_list[wall].DZ);
				wall=wall_list[wall].Next;
				count++;
			}

			//
			// Roof points jutting out at wall height
			//
			sp[1]=next_prim_point;
			create_recessed_storey_points(y,storey,count,overlap);

			//
			// Roof point jutting out at raised height (overlap_height)
			//

			sp[2]=next_prim_point;
			roof_rim=next_prim_point;
			for(c0=0;c0<sp[2]-sp[1];c0++)
			{
				std::int32_t	x,z;
				x=prim_points[c0+sp[1]].X;
				z=prim_points[c0+sp[1]].Z;

				add_point(x,y+(overlap_height),z);
				
			}

	//	BUILD RAISED/CENTER POINTS FOR ANGLED/FLAT ROOF
	//  now unused

	/*
			sp[3]=next_prim_point;
			wall=storey_list[roof].WallHead;
			add_point(storey_list[roof].DX,y+roof_height,storey_list[roof].DZ);
			while(wall)
			{
				add_point(wall_list[wall].DX,y+roof_height,wall_list[wall].DZ);
				wall=wall_list[wall].Next;
			}
	*/
			sp[4]=next_prim_point;
	/*
			for(c0=0;c0<=count;c0++)
			{
				p0=sp[0]+c0+1;
				p1=sp[0]+c0;

				p2=find_near_prim_point(prim_points[p1].X,prim_points[p1].Z,sp[3],sp[4]);
				p3=find_near_prim_point(prim_points[p0].X,prim_points[p0].Z,sp[3],sp[4]);
				if(p2&&p3)
				{
					if(p2!=p3)
					{
						flag_floor_tiles_for_quad(p0,p1,p3,p2);
					}
					else
					{
						flag_floor_tiles_for_tri(p0,p1,p3);
					}
				}
			}
	*/

			//cancel walled
	/*
			if(roof_flags&FLAG_ROOF_WALLED)
			{
				//
				// raised and overlap inwards
				//

				create_recessed_storey_points(y+overlap_height,storey,count,-overlap);
				sp[5]=next_prim_point;

				//
				// overlap inwards but back to wall height
				//
				roof_rim=next_prim_point;
				for(c0=0;c0<sp[5]-sp[4];c0++)
				{
					std::int32_t	x,z;
					x=prim_points[c0+sp[4]].X;
					z=prim_points[c0+sp[4]].Z;

					add_point(x,y,z);
					
				}
			}

			if( (roof_flags&(FLAG_ROOF_WALLED|FLAG_ROOF_FLAT)) ==(FLAG_ROOF_WALLED)) //ANGLED ROOF WITH WALL
			{
				//
				// overlap inwards even further but back to wall height //only for non flat walled buildings
				//
				sp[6]=next_prim_point;
				roof_rim=next_prim_point;
				create_recessed_storey_points(y,storey,count,-overlap*2);
				
			}
	*/

			
		//
		// Brind overlap back to storey position and a bit more
		//
			sp[5]=next_prim_point;
			roof_rim=next_prim_point;
			//
			// overlap inwards slightly and raised
			//
			create_recessed_storey_points(y+overlap_height,storey,count,-3);


	//	if(storey_list[storey].StoreyFlags&FLAG_ROOF_WALLED)

			for(c0=0;c0<=count;c0++)
			{
			 		create_a_quad(sp[1]+c0,sp[1]+c0+1,sp[0]+c0,sp[0]+c0+1,0,23);
			 		create_a_quad(sp[2]+c0,sp[2]+c0+1,sp[1]+c0,sp[1]+c0+1,0,23);
			 		create_a_quad(sp[5]+c0,sp[5]+c0+1,sp[2]+c0,sp[2]+c0+1,0,23);
			}


	/* //forget walled for now
			if(roof_flags&FLAG_ROOF_WALLED)
			{
				for(c0=0;c0<=count;c0++)
				{
			 		create_a_quad(sp[4]+c0,sp[4]+c0+1,sp[2]+c0,sp[2]+c0+1,0,23);
			 		create_a_quad(sp[5]+c0,sp[5]+c0+1,sp[4]+c0,sp[4]+c0+1,0,23);
				}
				
			}
			if( (roof_flags&(FLAG_ROOF_WALLED|FLAG_ROOF_FLAT)) ==(FLAG_ROOF_WALLED)) //ANGLED ROOF WITH WALL
			{
				for(c0=0;c0<=count;c0++)
			 		create_a_quad(sp[6]+c0,sp[6]+c0+1,sp[5]+c0,sp[5]+c0+1,0,23);
				
			}
	*/

	/* //old filling to roof points raised or flat
			wall=storey_list[storey].WallHead;
			count=0;
			while(wall)
			{
				std::int32_t	q0,q1;

				p0=roof_rim+count+1;
				p1=roof_rim+count;

				q0=sp[0]+count+1;
				q1=sp[0]+count;

				p2=find_near_prim_point(prim_points[p1].X,prim_points[p1].Z,sp[3],sp[4]);
				p3=find_near_prim_point(prim_points[p0].X,prim_points[p0].Z,sp[3],sp[4]);

				if(p2!=p3)
				{
					create_a_quad(p0,p1,p3,p2,0,23); //p2,p3,p1,p0);
					//no more flag_floor_tiles_for_quad(q0,q1,p3,p2);

				}
				else
				{
					create_a_tri(p0,p1,p2,0,23);
					//no more flag_floor_tiles_for_tri(q0,q1,p3);
				}
				wall=wall_list[wall].Next;
				count++;
			}
	*/
		}
	}
	else
		overlap_height=0;
	build_roof_grid(storey,y+overlap_height);

}

std::int32_t	area_of_quad(std::int32_t	p0,std::int32_t p1,std::int32_t p2,std::int32_t p3)
{

	std::int32_t dx,dz;
	dx=abs(prim_points[p0].X-prim_points[p1].X);
	dz=abs(prim_points[p0].Z-prim_points[p2].Z);
	return(dx*dz);
}


// p0         p01          p1


// p20        p03          p13


// p2        p32          p3


void	create_split_quad_into_4(std::int32_t	p0,std::int32_t p1,std::int32_t p2,std::int32_t p3,std::int32_t wall,std::int32_t y)
{

	std::int32_t	p01,p13,p32,p20,p03;
	std::int32_t	x,z;
	struct	PrimFace4	*p_f4;

	std::int16_t	texture_style;
	texture_style=wall_list[wall].TextureStyle;

	p01=next_prim_point;
	x=(prim_points[p1].X+prim_points[p0].X)>>1;
	z=(prim_points[p1].Z+prim_points[p0].Z)>>1;
	add_point(x,y,z);

	p13=next_prim_point;
	x=(prim_points[p1].X+prim_points[p3].X)>>1;
	z=(prim_points[p1].Z+prim_points[p3].Z)>>1;
	add_point(x,y,z);

	p32=next_prim_point;
	x=(prim_points[p3].X+prim_points[p2].X)>>1;
	z=(prim_points[p3].Z+prim_points[p2].Z)>>1;
	add_point(x,y,z);

	p20=next_prim_point;
	x=(prim_points[p0].X+prim_points[p2].X)>>1;
	z=(prim_points[p0].Z+prim_points[p2].Z)>>1;
	add_point(x,y,z);

	p03=next_prim_point;
	x=(prim_points[p0].X+prim_points[p1].X+prim_points[p2].X+prim_points[p3].X)>>2;
	z=(prim_points[p0].Z+prim_points[p1].Z+prim_points[p2].Z+prim_points[p3].Z)>>2;
	add_point(x,y,z);

	p_f4=create_a_quad(p0,p01,p20,p03,texture_style,0); //p2,p3,p1,p0);
	add_quad_to_walkable_list(next_prim_face4-1);
	p_f4->ThingIndex=-wall;

	p_f4=create_a_quad(p20,p03,p2,p32,texture_style,0); //p2,p3,p1,p0);
	add_quad_to_walkable_list(next_prim_face4-1);
	p_f4->ThingIndex=-wall;


	p_f4=create_a_quad(p01,p1,p03,p13,texture_style,0); //p2,p3,p1,p0);
	add_quad_to_walkable_list(next_prim_face4-1);
	p_f4->ThingIndex=-wall;

	p_f4=create_a_quad(p03,p13,p32,p3,texture_style,0); //p2,p3,p1,p0);
	add_quad_to_walkable_list(next_prim_face4-1);
	p_f4->ThingIndex=-wall;

}

void	create_split_quad_into_16(std::int32_t	p0,std::int32_t p1,std::int32_t p2,std::int32_t p3,std::int32_t wall,std::int32_t y)
{
	std::int32_t	p01,p13,p32,p20,p03;
	std::int32_t	x,z;
	struct	PrimFace4	*p_f4;


	p01=next_prim_point;
	x=(prim_points[p1].X+prim_points[p0].X)>>1;
	z=(prim_points[p1].Z+prim_points[p0].Z)>>1;
	add_point(x,y,z);


	p13=next_prim_point;
	x=(prim_points[p1].X+prim_points[p3].X)>>1;
	z=(prim_points[p1].Z+prim_points[p3].Z)>>1;
	add_point(x,y,z);

	p32=next_prim_point;
	x=(prim_points[p3].X+prim_points[p2].X)>>1;
	z=(prim_points[p3].Z+prim_points[p2].Z)>>1;
	add_point(x,y,z);

	p20=next_prim_point;
	x=(prim_points[p0].X+prim_points[p2].X)>>1;
	z=(prim_points[p0].Z+prim_points[p2].Z)>>1;
	add_point(x,y,z);

	p03=next_prim_point;
	x=(prim_points[p0].X+prim_points[p1].X+prim_points[p2].X+prim_points[p3].X)>>2;
	z=(prim_points[p0].Z+prim_points[p1].Z+prim_points[p2].Z+prim_points[p3].Z)>>2;
	add_point(x,y,z);

	create_split_quad_into_4(p0,p01,p20,p03,wall,y); //p2,p3,p1,p0);
	create_split_quad_into_4(p20,p03,p2,p32,wall,y); //p2,p3,p1,p0);
	create_split_quad_into_4(p01,p1,p03,p13,wall,y); //p2,p3,p1,p0);
	create_split_quad_into_4(p03,p13,p32,p3,wall,y); //p2,p3,p1,p0);

}

void	create_split_quad_into_48(std::int32_t	p0,std::int32_t p1,std::int32_t p2,std::int32_t p3,std::int32_t wall,std::int32_t y)
{
	std::int32_t	p01,p13,p32,p20,p03;
	std::int32_t	x,z;
	struct	PrimFace4	*p_f4;


	p01=next_prim_point;
	x=(prim_points[p1].X+prim_points[p0].X)>>1;
	z=(prim_points[p1].Z+prim_points[p0].Z)>>1;
	add_point(x,y,z);


	p13=next_prim_point;
	x=(prim_points[p1].X+prim_points[p3].X)>>1;
	z=(prim_points[p1].Z+prim_points[p3].Z)>>1;
	add_point(x,y,z);

	p32=next_prim_point;
	x=(prim_points[p3].X+prim_points[p2].X)>>1;
	z=(prim_points[p3].Z+prim_points[p2].Z)>>1;
	add_point(x,y,z);

	p20=next_prim_point;
	x=(prim_points[p0].X+prim_points[p2].X)>>1;
	z=(prim_points[p0].Z+prim_points[p2].Z)>>1;
	add_point(x,y,z);

	p03=next_prim_point;
	x=(prim_points[p0].X+prim_points[p1].X+prim_points[p2].X+prim_points[p3].X)>>2;
	z=(prim_points[p0].Z+prim_points[p1].Z+prim_points[p2].Z+prim_points[p3].Z)>>2;
	add_point(x,y,z);

	create_split_quad_into_16(p0,p01,p20,p03,wall,y); //p2,p3,p1,p0);
	create_split_quad_into_16(p20,p03,p2,p32,wall,y); //p2,p3,p1,p0);
	create_split_quad_into_16(p01,p1,p03,p13,wall,y); //p2,p3,p1,p0);
	create_split_quad_into_16(p03,p13,p32,p3,wall,y); //p2,p3,p1,p0);

}

void	build_roof_quad(std::uint16_t storey,std::int32_t y)
{
//	std::int32_t	x1,z1,x2,z2,x3,z3;
	std::int32_t	wall;
//	,prev_wall,prev_prev_wall;
	std::int32_t	sp[10];
	std::int32_t	roof;
	std::int32_t	p0,p1,p2,p3;
//	std::int32_t	rx,rz;
	std::int32_t	count=0;
	std::int32_t	roof_height=0; //BLOCK_SIZE*3;
	struct	PrimFace4	*p_f4;
	std::int16_t	texture_style;



	if(storey_list[storey].WallHead && storey_list[storey].Roof)
	{
		std::int32_t	area;

		roof=storey_list[storey].Roof;
		sp[0]=next_prim_point;
		wall=storey_list[roof].WallHead;
		if(wall_list[wall].WallFlags&FLAG_ROOF_FLAT)
			roof_height=0;

		add_point(storey_list[roof].DX,y-roof_height,storey_list[roof].DZ);
		while(wall&&count<3)
		{

			add_point(wall_list[wall].DX,y-roof_height,wall_list[wall].DZ);
			wall=wall_list[wall].Next;
			count++;
		}



		wall=storey_list[roof].WallHead;
		texture_style=wall_list[wall].TextureStyle;
		p0=sp[0]+3;
		p1=sp[0]+2;
		p2=sp[0]+0;
		p3=sp[0]+1;
		area=area_of_quad(p0,p1,p2,p3);
		if(area>8*256*8*256)
			create_split_quad_into_48(p0,p1,p2,p3,wall,y+roof_height);
		else
		if(area>4*256*4*256)
			create_split_quad_into_16(p0,p1,p2,p3,wall,y+roof_height);
		else
		if(area>2*256*2*256)
			create_split_quad_into_4(p0,p1,p2,p3,wall,y+roof_height);
		else
		{
			p_f4=create_a_quad(p0,p1,p2,p3,texture_style,0); //p2,p3,p1,p0);
			add_quad_to_walkable_list(next_prim_face4-1);
		//no more flag_floor_tiles_for_quad(p0,p1,p2,p3);
			p_f4->ThingIndex=-wall;
		}
	}
}

static std::int32_t	build_x,build_y,build_z;

void	center_object(std::int32_t sp,std::int32_t ep)
{
	std::int32_t	c0;
//	,count;
	std::int32_t	az=0,ax=0;
	if(ep-sp<0)
	{
		LogText(" sp %d ep %d \n",sp,ep);
		ERROR_MSG(0," center object has negative points");
		return;
	}
	if((ep-sp)==0)
	{
		LogText("CENTER OBJECT Error sp %d ep %d \n",sp,ep);
		return;
	}

	for(c0=sp;c0<ep;c0++)
	{
		ax+=prim_points[c0].X;
		az+=prim_points[c0].Z;
	}

	ax/=(ep-sp);
	az/=(ep-sp);
	build_x=ax;//+(64<<ELE_SHIFT);
	build_z=az;//+(64<<ELE_SHIFT);
	build_y=0;

	for(c0=sp;c0<ep;c0++)
	{
		prim_points[c0].X-=ax;
		prim_points[c0].Z-=az;
	}
	
}



std::int32_t	build_facet(std::int32_t sp,std::int32_t mp,std::int32_t sf3,std::int32_t sf4,std::int32_t mf4,std::int32_t prev_facet,std::uint16_t flags)
{
	struct	BuildingFacet	*p_obj;
	p_obj=&building_facets[next_building_facet];
	next_building_facet++;

	p_obj->StartPoint=sp;
	p_obj->MidPoint=mp;
	p_obj->EndPoint=next_prim_point;

	p_obj->StartFace4=sf4;
	p_obj->MidFace4=mf4;
	p_obj->EndFace4=next_prim_face4;
	p_obj->StartFace3=sf3;
	p_obj->EndFace3=next_prim_face3;
	p_obj->NextFacet=prev_facet;
	p_obj->FacetFlags=flags;

//	center_object(p_obj);
	return(next_building_facet-1);
}

std::int32_t	build_building(std::int32_t sp,std::int32_t sf3,std::int32_t sf4,std::int32_t prev_facet)
{
	struct	BuildingObject	*p_bobj;
	p_bobj=&building_objects[next_building_object];
	next_building_object++;

	p_bobj->StartPoint=sp;
	p_bobj->EndPoint=next_prim_point;

	p_bobj->StartFace4=sf4;
	p_bobj->EndFace4=next_prim_face4;

	p_bobj->StartFace3=sf3;
	p_bobj->EndFace3=next_prim_face3;
	p_bobj->FacetHead=prev_facet;
//	LogText(" center object %d->%d \n",p_bobj->StartPoint,p_bobj->EndPoint);
	center_object(p_bobj->StartPoint,p_bobj->EndPoint);
#ifdef	FUNNY_FANNY
	p_bobj->X=build_x;
	p_bobj->Y=build_y;
	p_bobj->Z=build_z;
#endif

//	center_object(p_bobj);
	return(next_building_object-1);
}

std::int32_t	build_prim_object(std::int32_t sp,std::int32_t sf3,std::int32_t sf4)
{
	struct	PrimObject	*p_obj;
	p_obj=&prim_objects[next_prim_object];
	next_prim_object++;

	p_obj->StartPoint=sp;
	p_obj->EndPoint=next_prim_point;

	p_obj->StartFace4=sf4;
	p_obj->EndFace4=next_prim_face4;

	p_obj->StartFace3=sf3;
	p_obj->EndFace3=next_prim_face3;
	center_object(p_obj->StartPoint,p_obj->EndPoint);
	return(next_prim_object-1);
}


void	find_next_last_coord(std::int16_t wall,std::int32_t *x,std::int32_t *z)
{
	std::int32_t	next_wall;
	LogText(" find next to last wall %d ",wall);
	while(wall)
	{
		next_wall=wall_list[wall].Next;
//		LogText(" wall %d ",wall);
		if(wall_list[next_wall].Next==0)
		{
//			LogText(" next to last wall is %d xz (%d,%d)",wall,wall_list[wall].DX,wall_list[wall].DZ);
			*x=wall_list[wall].DX;
			*z=wall_list[wall].DZ;
			return;
		}
		wall=next_wall;
	}
}

struct	LedgeInfo
{
	std::int16_t	Storey,Wall;
	std::int16_t	Y;
	std::int32_t   X1,Z1,X2,Z2,X3,Z3,X4,Z4;
	
};


void	build_single_ledge(struct LedgeInfo	*p_ledge)
{
	
	std::int32_t	sp[4],count=0;
	std::int32_t	rx,rz,rx2,rz2;

	std::int32_t	y,height;
	struct	PrimFace4	*p4;

//	LogText(" build ledge (%d,%d) (%d,%d) (%d,%d) (%d,%d)  storey %d wall %d \n",p_ledge->X1,p_ledge->Z1,p_ledge->X2,p_ledge->Z2,p_ledge->X3,p_ledge->Z3,p_ledge->X4,p_ledge->Z4,p_ledge->Storey,p_ledge->Wall);

	height=BLOCK_SIZE>>1;
	y=p_ledge->Y;

	sp[0]=next_prim_point;
	add_point(p_ledge->X2,y+2,p_ledge->Z2);
	add_point(p_ledge->X3,y+2,p_ledge->Z3);
	calc_new_corner_point(p_ledge->X1,p_ledge->Z1,p_ledge->X2,p_ledge->Z2,p_ledge->X3,p_ledge->Z3,BLOCK_SIZE,&rx,&rz);
	calc_new_corner_point(p_ledge->X2,p_ledge->Z2,p_ledge->X3,p_ledge->Z3,p_ledge->X4,p_ledge->Z4,BLOCK_SIZE,&rx2,&rz2);
	sp[1]=next_prim_point;
	add_point(rx,y+2,rz);
	add_point(rx2,y+2,rz2);

	y+=height;
	sp[2]=next_prim_point;
	add_point(rx,y+2,rz);
	add_point(rx2,y+2,rz2);

	sp[3]=next_prim_point;
	add_point(p_ledge->X2,y+2,p_ledge->Z2);
	add_point(p_ledge->X3,y+2,p_ledge->Z3);

	p4=create_a_quad(sp[1],sp[1]+1,sp[0],sp[0]+1,0,18);
	OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_LONG_LEDGE);
	p4=create_a_quad(sp[2],sp[2]+1,sp[1],sp[1]+1,0,18);
	OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_LONG_LEDGE);
	p4=create_a_quad(sp[3],sp[3]+1,sp[2],sp[2]+1,0,18);
	OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_LONG_LEDGE);

}

std::int32_t	find_wall_for_fe(std::int32_t fe_x,std::int32_t fe_y,std::int32_t storey)
{
	std::int32_t 	wall=0;
	std::int32_t	px,pz,x1,z1;
	std::int32_t	best_wall=-1,best_dist=0x7fffffff,dist;
	std::int32_t	wall_count=0;

	while(storey_list[storey].StoreyType==STOREY_TYPE_FIRE_ESCAPE||storey_list[storey].StoreyType==STOREY_TYPE_STAIRCASE||storey_list[storey].StoreyType==STOREY_TYPE_FENCE)
	{
		storey=storey_list[storey].Next;
	}

	wall=storey_list[storey].WallHead;
	px=storey_list[storey].DX;
	pz=storey_list[storey].DZ;
	while(wall)
	{
		x1=wall_list[wall].DX;
		z1=wall_list[wall].DZ;

		dist=dist_between_vertex_and_vector(px,pz,x1,z1,fe_x,fe_y);
		if(dist<best_dist)
		{
			best_wall=wall_count;
			best_dist=dist;
			
		}

		wall_count++;		
		wall=wall_list[wall].Next;
		px=x1;
		pz=z1;
	}
	return(best_wall);
	
}

void	build_staircase(std::int32_t	storey)
{
	std::int32_t	wall;
	std::int32_t	wall_count=0;
	std::int32_t	count;
	std::int32_t	step_count,step_size,step_height,len,step_y,step_pos,step_length;
	std::int32_t	sp[300];
	std::int32_t	row=0;
	std::int32_t	c0,c1;
	std::int32_t	y,start_y=0;
	struct	PrimFace4	*p4;
	std::int32_t	step_pos_old;
	std::int32_t	last;

	struct	 StairVect
	{
		std::int32_t	X1,Z1,X2,Z2;
		
	};

	struct	StairVect	s_vects[50];

	wall=storey_list[storey].WallHead;
	while(wall)
	{
		wall_count++;
		wall=wall_list[wall].Next;
	}

	if((wall_count<4)||(wall_count&1))
		return;

	count=1;
	wall=storey_list[storey].WallHead;
//	vects[0].X1=storey_list[storey].DX;
//	vects[0].Z1=storey_list[storey].DZ;
	while(wall)
	{
		if(count<=(wall_count>>1))
		{
			s_vects[count-1].X1 = wall_list[wall].DX;
			s_vects[count-1].Z1 = wall_list[wall].DZ;
			
		}
		else
		{
			std::int32_t	pos;
			pos=(wall_count)-(count)+1;

			s_vects[pos-1].X2=wall_list[wall].DX;
			s_vects[pos-1].Z2=wall_list[wall].DZ;
			
		}

		count++;
		wall=wall_list[wall].Next;
	}

	/* We now have a load of vects to travel along */


	len=QLEN(s_vects[0].X1,s_vects[0].Z1,s_vects[0].X2,s_vects[0].Z2);
	if(len==0)
		return;

	step_height=BLOCK_SIZE>>1;
	step_length=BLOCK_SIZE>>1;
	step_count=len/step_length;
	if(step_count==0)
		return;
	step_size=(len<<8)/step_count;
	step_y=storey_list[storey].DY;

	step_pos=len<<8;
	y=start_y;


	step_pos_old=step_pos;
	sp[99]=next_prim_point;
	while(step_pos>=0)
	{
		std::int32_t	x,z;
		if(storey_list[storey].Info1&&step_pos<storey_list[storey].Info1*step_size)
		{
			step_pos=0;
		}
		last=(wall_count>>1)-1;
		x=s_vects[0].X1+((s_vects[0].X2-s_vects[0].X1)*step_pos)/(len<<8);
		z=s_vects[0].Z1+((s_vects[0].Z2-s_vects[0].Z1)*step_pos)/(len<<8);
		add_point(x,start_y,z);
		x=s_vects[last].X1+((s_vects[last].X2-s_vects[last].X1)*step_pos)/(len<<8);
		z=s_vects[last].Z1+((s_vects[last].Z2-s_vects[last].Z1)*step_pos)/(len<<8);
		add_point(x,start_y,z);
		step_pos-=step_size;
	}
	step_pos=step_pos_old;


	while(step_pos>=0)
	{

		if(storey_list[storey].Info1&&step_pos<storey_list[storey].Info1*step_size)
		{
			step_pos=0;
/*
			sp[row]=next_prim_point;
			for(c0=0;c0<(wall_count>>1);c0++)
			{
				std::int32_t	x,z;

				x=s_vects[c0].X1;
				z=s_vects[c0].Z1;

				add_point(x,y,z);
			}
			row++;
			step_pos=-1;
*/

			
		}
//		else
		{
			
			sp[row]=next_prim_point;
			for(c0=0;c0<(wall_count>>1);c0++)
			{
				std::int32_t	x,z;
				x=s_vects[c0].X1+((s_vects[c0].X2-s_vects[c0].X1)*step_pos)/(len<<8);
				z=s_vects[c0].Z1+((s_vects[c0].Z2-s_vects[c0].Z1)*step_pos)/(len<<8);
				add_point(x,y,z);
			}
			sp[row+1]=next_prim_point;
			step_pos-=step_size;
			if(step_pos>=0)
			{
				//we dont need a riser on the last one
				for(c0=sp[row];c0<sp[row]+(wall_count>>1);c0++)
				{
					add_point(prim_points[c0].X,y+step_height,prim_points[c0].Z);
				}

				y+=step_height;
				row+=2;
			}
			else
			{
				
				y+=step_height;
				row+=1;
			}
		}

	}
	
	for(c1=0; c1<(row-1) ;c1++)
	{
		for(c0=0;c0 < ((wall_count>>1)-1);c0++)
		{
			p4=create_a_quad(sp[c1+1]+1+c0,sp[c1+1]+c0,sp[c1]+1+c0,sp[c1]+c0,0,18+((c1&1)?5:0));
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
		}
		if(c1&1)
		{

			p4=create_a_quad(sp[c1],sp[c1+1],sp[99]+((c1-1)&0xfffe),sp[99]+2+((c1-1)&0xfffe),0,23);
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
			p4=create_a_quad(sp[c1+1]+last,sp[c1]+last,sp[99]+3+((c1-1)&0xfffe),sp[99]+1+((c1-1)&0xfffe),0,23);
			OR_SORT_LEVEL(p4->FaceFlags,SORT_LEVEL_FIRE_ESCAPE);
		}
	}
}


#define	LIGHT_SIZE	BLOCK_SIZE
#define	CONE_MULT	5
std::int32_t	create_suspended_light(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t flags)
{
	std::int32_t	p1,p2;
	struct	PrimFace3 *p_f3;

	flags=flags;
	p1=next_prim_point;
	add_point(x,y,z);
	add_point(x-LIGHT_SIZE,y-LIGHT_SIZE,z-LIGHT_SIZE);
	add_point(x+LIGHT_SIZE,y-LIGHT_SIZE,z-LIGHT_SIZE);
	add_point(x+LIGHT_SIZE,y-LIGHT_SIZE,z+LIGHT_SIZE);
	add_point(x-LIGHT_SIZE,y-LIGHT_SIZE,z+LIGHT_SIZE);


/*
	p2=next_prim_point-1;
	add_point(x-LIGHT_SIZE*CONE_MULT,y-LIGHT_SIZE*6,z-LIGHT_SIZE*CONE_MULT);
	add_point(x+LIGHT_SIZE*CONE_MULT,y-LIGHT_SIZE*6,z-LIGHT_SIZE*CONE_MULT);
	add_point(x+LIGHT_SIZE*CONE_MULT,y-LIGHT_SIZE*6,z+LIGHT_SIZE*CONE_MULT);
	add_point(x-LIGHT_SIZE*CONE_MULT,y-LIGHT_SIZE*6,z+LIGHT_SIZE*CONE_MULT);
*/


	p_f3=create_a_tri(p1+2,p1+1,p1+0,0,0);
	p_f3->DrawFlags=0; //POLY_50T;
	p_f3=create_a_tri(p1+3,p1+2,p1+0,0,0);
	p_f3->DrawFlags=0; //POLY_50T;
	p_f3=create_a_tri(p1+4,p1+3,p1+0,0,0);
	p_f3->DrawFlags=0; //POLY_50T;
	p_f3=create_a_tri(p1+1,p1+4,p1+0,0,0);
	p_f3->DrawFlags=0; //POLY_50T;
/*
	p_f3=create_a_tri(p2+2,p2+1,p1+0,0,28);
	p_f3->DrawFlags=POLY_50T;
	p_f3=create_a_tri(p2+3,p2+2,p1+0,0,28);
	p_f3->DrawFlags=POLY_50T;
	p_f3=create_a_tri(p2+4,p2+3,p1+0,0,28);
	p_f3->DrawFlags=POLY_50T;
	p_f3=create_a_tri(p2+1,p2+4,p1+0,0,28);
	p_f3->DrawFlags=POLY_50T;
*/
	apply_light_to_map(x,50,z,150);

   return(0);	
}

void	build_cable(std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2,std::int16_t wall,std::int16_t type)
{
	std::int32_t	p1;
	std::uint16_t	start_point;
	std::uint16_t	start_face3,start_face4;
	struct	PrimFace4 *p_f4;
	std::int32_t	prim;
	std::int32_t	len,dx,dy,dz,count;
	std::int32_t	px,py,pz;
	std::int32_t	c0;
	std::int32_t	light_x,light_y,light_z;

	wall=wall;
	type=type;
	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;

	dx=abs(x2-x1);
	dy=abs(y2-y1);
	dz=abs(z2-z1);

	len=QDIST3(dx,dy,dz);
	count=(len<<1)/ELE_SIZE;
	dx=(x2-x1);
	dy=(y2-y1);
	dz=(z2-z1);


	px=x1;
	py=y1;
	pz=z1;
	add_point(px,py,pz);
	add_point(px,py+8,pz);
//	LogText(" cable len %d count %d dx %d dy %d dz %d \n",len,count,dx,dy,dz);

	for(c0=1;c0<=count;c0++)
	{
		std::int32_t	ex,ey,ez;
		std::int32_t	angle;

		ex=x1+(c0*dx)/count;
		ey=y1+(c0*dy)/count;

		angle=((c0-(count>>1))*1024)/count;
		angle=(angle+2048)&2047;
		ey-=COS(angle)>>9;
		ez=z1+(c0*dz)/count;
		if(c0==(count>>1))
		{
			light_x=ex;
			light_y=ey;
			light_z=ez;
			
		}
		p1=next_prim_point;
		add_point(ex,ey,ez);
		add_point(ex,ey+8,ez);
		p_f4=create_a_quad(p1-1,p1+1,p1-2,p1,0,0);
	 	p_f4->DrawFlags=POLY_FLAG_DOUBLESIDED;

		px=ex;
		py=ey;
		pz=ez;
		
	}
//	if(px&1)
		create_suspended_light(light_x,light_y,light_z,1);

/*
	p1=next_prim_point;
	add_point(x1,y1,z1);
	add_point(x2,y2,z2);
	add_point(x1,y1-10,z1);
	add_point(x2,y2-10,z2);

	p_f4=create_a_quad(p1+2,p1+3,p1,p1+1,0,0);

 	p_f4->DrawFlags|= POLY_FLAG_DOUBLESIDED;
*/
	//prim=build_prim_object(start_point,start_face3,start_face4);

	//place_prim_at(prim,build_x,0,build_z);
	
}

std::int32_t	build_cables(std::int16_t storey,std::int32_t prev_facet)
{
	std::int32_t	wall;
	std::int32_t	x1,y1,z1,x2,y2,z2;
	std::int32_t	start_point,start_face3,start_face4;
	std::int32_t	prim;


	wall=storey_list[storey].WallHead;
	x1=storey_list[storey].DX;
	y1=storey_list[storey].DY;
	y1=BLOCK_SIZE*8;
	z1=storey_list[storey].DZ;
	while(wall)
	{
		x2=wall_list[wall].DX;
		y2=y1; //wall_list[wall].Y;
		z2=wall_list[wall].DZ;

		start_point=next_prim_point;
		start_face3=next_prim_face3;
		start_face4=next_prim_face4;

		build_cable(x1,y1,z1,x2,y2,z2,wall,0);
   		prev_facet=build_facet(start_point,next_prim_point,start_face3,start_face4,next_prim_face4,0,FACET_FLAG_CABLE);

		prim=build_building(start_point,start_face3,start_face4,prev_facet);
		place_building_at(prim,build_x,0,build_z);

		wall=wall_list[wall].Next;

		x1=x2;
		y1=y2;
		z1=z2;
	}
	return(0);
}

std::int32_t	process_external_pieces(std::uint16_t building)
{
	std::int32_t	storey,c0=0;
	std::int32_t	prev_facet=0;

	storey=building_list[building].StoreyHead;

	while(storey&&c0<200)
	{
//		LogText(" pass %d storey %d \n",c0,storey);
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_CABLE:
				prev_facet=build_cables(storey,prev_facet);
				break;
		}
		storey=storey_list[storey].Next;
		c0++;
	}
	return(prev_facet);
}

/*   //pre store face  wall/storey links
struct	StoreyLink
{
	std::int16_t	Face;   //storey or wall
	std::uint16_t	Link;
};

struct	StoreyLink	storey_link_pool[2000];
std::uint16_t	next_storey_link=1;

std::uint16_t	storey_heads[100];


void	build_link_table(std::uint16_t building)
{
	std::uint16_t	storey;

	next_storey_link=1;

	storey=building_list[building].StoreyHead;
	while(storey)
	{
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
				wall=storey_list[storey].WallHead;
				while(wall)
				{
					find_
				}

				break;
		}
		storey=storey_list[storey].Next;
	}
}
*/


void	clear_storey_flags(std::uint16_t building)
{
	std::int32_t	wall,storey;

	storey=building_list[building].StoreyHead;
	while(storey)
	{
		storey_list[storey].StoreyFlags&=~FLAG_STOREY_FACET_LINKED;
		
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
			case	STOREY_TYPE_FENCE:
				wall=storey_list[storey].WallHead;
				while(wall)
				{
					wall_list[wall].WallFlags&=~FLAG_WALL_FACET_LINKED;
					wall=wall_list[wall].Next;
				}

				break;
		}
		storey=storey_list[storey].Next;
	}

}

std::int32_t	find_connect_wall(std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t *connect_storey,std::int32_t storey)
{
	std::int32_t	found=0;
	std::int32_t	wall;
	std::int32_t	fx1,fz1,fx2,fz2;

	storey=storey_list[storey].Next;

	while(storey&&!found)
	{
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
			case	STOREY_TYPE_FENCE:
				found=1;
				break;
			default:
				storey=storey_list[storey].Next;
		}
	}
	if(found)
	{
		fx1=storey_list[storey].DX;
		fz1=storey_list[storey].DZ;
		wall=storey_list[storey].WallHead;
		while(wall)
		{
			fx2=wall_list[wall].DX;
			fz2=wall_list[wall].DZ;
			if(fx1==x1&&fz1==z1&&fx2==x2&&fz2==z2)
			{
				*connect_storey=storey;
				return(wall);
			}
			fx1=fx2;
			fz1=fz2;
			wall=wall_list[wall].Next;
		}
	}
	else
		return(0);

	return(0);

}
/*

  A facet is used for sorting and quick back face culling (but not yet)
  For Each piece of wall on the bottom storey find vertically identical pieces to become 
  a facet.

	
*/
std::int32_t	create_building_prim(std::uint16_t building,std::int32_t	*small_y)
{
	std::uint8_t	pass2=0;
	std::int32_t	storey;
	std::int32_t	wall;

	std::int32_t	start_point,start_face3,start_face4;
	std::int32_t	mid_point,mid_face4;
	std::int32_t	y=0,offset_y=0;

	std::uint32_t	obj_start_point;
	std::uint32_t	obj_start_face3,obj_start_face4;
	std::uint32_t	prev_facet=0;
	std::int16_t	wall_for_fe[100];
	std::int32_t	wall_count=0;
	std::int16_t	fire_escape_count=0;
	std::int32_t	first=0;

//	LogText(" create building prim  next prim_point %d \n",next_prim_point);

	*small_y=99999;

	memset(wall_for_fe,0,200);

	prev_facet=process_external_pieces(building);
//	LogText(" create building prim2  next prim_point %d \n",next_prim_point);
	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;

	obj_start_point=start_point;
	obj_start_face3=start_face3;
	obj_start_face4=start_face4;


	clear_storey_flags(building);
	storey=building_list[building].StoreyHead;
	while(storey)
	{
		std::int32_t	x1,z1,x2,z2;

		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
			case	STOREY_TYPE_FENCE:
				if(first==0)
				{
					set_floor_hidden(storey);
					first=1;
				}

				x1=storey_list[storey].DX;
				z1=storey_list[storey].DZ;

				wall_count=0;
				wall=storey_list[storey].WallHead;
				while(wall)
				{
					std::int32_t	connect_wall,connect_storey;

					x2=wall_list[wall].DX;
					z2=wall_list[wall].DZ;
					if(!(wall_list[wall].WallFlags&FLAG_WALL_FACET_LINKED))
					{
						y=storey_list[storey].DY;

						if(y==0)
						{
							std::int32_t	temp_y;
							temp_y=calc_height_at(x1,z1);
							if(temp_y<*small_y)
								*small_y=temp_y;

						}

						wall_list[wall].WallFlags|=FLAG_WALL_FACET_LINKED;
						append_wall_prim(x1,y+offset_y,z1,wall,storey,BLOCK_SIZE*4);

						connect_wall=find_connect_wall(x1,z1,x2,z2,&connect_storey,storey);
						while(connect_wall)
						{
							y=storey_list[connect_storey].DY;
							wall_list[connect_wall].WallFlags|=FLAG_WALL_FACET_LINKED;
 							append_wall_prim(x1,y+offset_y,z1,connect_wall,connect_storey,BLOCK_SIZE*4);
							connect_wall=find_connect_wall(x1,z1,x2,z2,&connect_storey,connect_storey);
						}
						mid_point=next_prim_point;
//					LogText(" create building prim3  next prim_point %d \n",next_prim_point);
						mid_face4=next_prim_face4;
						if(wall_for_fe[wall_count]&&pass2==0)
						{
							build_firescape(wall_for_fe[wall_count]);
						}
   						prev_facet=build_facet(start_point,mid_point,start_face3,start_face4,mid_face4,prev_facet,0);

//	LogText(" create building prim4  next prim_point %d \n",next_prim_point);
						start_point=next_prim_point;
						start_face3=next_prim_face3;
						start_face4=next_prim_face4;

					}
					x1=x2;
					z1=z2;
					wall=wall_list[wall].Next;
					wall_count++;
				}
				pass2=1;

				break;
			case	STOREY_TYPE_FIRE_ESCAPE:
				wall=find_wall_for_fe(storey_list[storey].DX,storey_list[storey].DZ,building_list[building].StoreyHead);
				if(wall>=0)
					wall_for_fe[wall]=storey;
				fire_escape_count++;

			default:
				break;
		}
		storey=storey_list[storey].Next;
	}

	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;

	storey=building_list[building].StoreyHead;
	while(storey)
	{
		if(storey_list[storey].StoreyFlags&FLAG_STOREY_TILED_ROOF)
			build_roof(storey,storey_list[storey].DY+offset_y+4*BLOCK_SIZE);
		
		if(storey_list[storey].Roof)
		{
			switch(storey_list[storey_list[storey].Roof].StoreyType)
			{
				case	STOREY_TYPE_ROOF:
						build_roof(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
						break;
				case	STOREY_TYPE_ROOF_QUAD:
						build_roof_quad(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
				
			}
		}
		storey=storey_list[storey].Next;
	}

//	LogText(" create building prim5  next prim_point %d \n",next_prim_point);
   	prev_facet=build_facet(start_point,next_prim_point,start_face3,start_face4,next_prim_face4,prev_facet,FACET_FLAG_ROOF);
	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;

	return(build_building(obj_start_point,obj_start_face3,obj_start_face4,prev_facet));
}


std::int32_t	create_building_prim_old(std::uint16_t building)
{
	std::uint16_t	start_point;
	std::uint16_t	start_face3,start_face4;
	std::int32_t	storey;
	std::int32_t	wall,next_wall,next_next_wall;
	std::int32_t	prev_storey=0;
	std::int32_t	x,y,z;
	std::int32_t	offset_y=0;
	std::int16_t	wall_index[200]; //no building will have more than 200 storey's surely?
	std::int16_t	prev_x[200];
	std::int16_t	prev_z[200];
	std::int16_t	storey_index[200];
	std::int32_t	c0,c1,walls_left,max_storeys;
	std::int32_t	storey_count;

	std::uint32_t	obj_start_point;
	std::uint32_t	obj_start_face3,obj_start_face4;
	std::int32_t	prev_facet=0;
	std::int32_t	ledge_count=0;
	struct	LedgeInfo	ledge_info[200];
	std::int16_t	wall_for_fe[100];
	std::int16_t	fire_escape_count=0;

	std::int32_t	facet_count=0;

	memset(wall_for_fe,0,200);
	memset(ledge_info,0,sizeof(struct LedgeInfo)*100);


	process_external_pieces(building);


	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;

	obj_start_point=start_point;
	obj_start_face3=start_face3;
	obj_start_face4=start_face4;

	y=0; //storey_list[storey].DY;
	clear_storey_flags(building);

	storey=building_list[building].StoreyHead;
	c0=0;
	offset_y=0;
	while(storey&&c0<200)
	{
//		LogText(" pass %d storey %d \n",c0,storey);
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
					x=storey_list[storey].DX;
					y=storey_list[storey].DY;
					z=storey_list[storey].DZ;
					storey_index[c0]=storey;
					prev_x[c0]=x;
					prev_z[c0]=z;
					wall=storey_list[storey].WallHead;
					wall_index[c0]=wall;

					if(storey_list[storey].StoreyFlags&FLAG_STOREY_LEDGE)
					{
						struct	LedgeInfo	*p_ledge;
						p_ledge=&ledge_info[ledge_count];
						p_ledge->Storey=storey;
						p_ledge->Wall=0;
						p_ledge->Y=y+offset_y;
						p_ledge->X2=x;
						p_ledge->Z2=z;
						p_ledge->X3=wall_list[wall].DX;
						p_ledge->Z3=wall_list[wall].DZ;
						next_wall=wall_list[wall].Next;
						p_ledge->X4=wall_list[next_wall].DX;
						p_ledge->Z4=wall_list[next_wall].DZ;
						find_next_last_coord(wall,&(p_ledge->X1),&(p_ledge->Z1));
						ledge_count++;
						offset_y+=BLOCK_SIZE>>1;
					}
					if(wall)
					{
//						LogText(" HELLO2 append wall %d\n",wall);
						LogText(" append wall_start for building %d wall %d storey %d \n",building,wall,storey);
			 			append_wall_prim(x,y+offset_y,z,wall,storey,BLOCK_SIZE*4);
					}	

						

				break;

			case	STOREY_TYPE_FIRE_ESCAPE:
				wall=find_wall_for_fe(storey_list[storey].DX,storey_list[storey].DZ,building_list[building].StoreyHead);
				LogText(" find_wall_for_fe=%d \n",wall);
				if(wall>=0)
					wall_for_fe[wall]=storey;
				fire_escape_count++;
				
				break;
			case	STOREY_TYPE_STAIRCASE:
				wall=find_wall_for_fe(storey_list[storey].DX,storey_list[storey].DZ,building_list[building].StoreyHead);
				LogText(" find_wall_for_SC=%d \n",wall);
				if(wall>=0)
					wall_for_fe[wall]=storey;
				fire_escape_count++;
				
				break;


		}
		prev_storey=storey;
		storey=storey_list[storey].Next;
		c0++;
	}
	for(c1=0;c1<ledge_count;c1++)
	{
//		LogText("build ledge \n");
				build_single_ledge(&ledge_info[c1]);

	}
	ledge_count=0;

	if(wall_for_fe[facet_count])
	{
		switch(storey_list[wall_for_fe[facet_count]].StoreyType)
		{
			case	STOREY_TYPE_FIRE_ESCAPE:
				build_firescape(wall_for_fe[facet_count]);
				break;
			case	STOREY_TYPE_STAIRCASE:
				build_staircase(wall_for_fe[facet_count]);
				break;
		}
	}
//		LogText("build facet \n");
   	//oldprev_facet=build_facet(start_point,start_face3,start_face4,0,0);


	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;


	walls_left=1;
	max_storeys=c0;

	while(walls_left)
	{
		std::int32_t	temp_next;
		facet_count++;
		y=0;
		offset_y=0;
		walls_left=0;
		for(storey_count=0;storey_count<max_storeys;storey_count++)
		{
		//LogText("walls_left %d storey count %d  \n",walls_left,storey_count);
			wall=wall_index[storey_count];
			if(wall)
			{
				next_wall=wall_list[wall].Next;
				storey=storey_index[storey_count];

				x=wall_list[wall].DX;
				y=storey_list[storey].DY;
				z=wall_list[wall].DZ;



				if(next_wall)
				if(storey_list[storey].StoreyFlags&FLAG_STOREY_LEDGE)
				{
					struct	LedgeInfo	*p_ledge;
					p_ledge=&ledge_info[ledge_count];
					p_ledge->Storey=storey;
					p_ledge->Wall=wall;
					p_ledge->Y=y+offset_y;
					p_ledge->X2=x;
					p_ledge->Z2=z;
					temp_next=next_wall;
					offset_y+=BLOCK_SIZE>>1;

					//LogText(" Ledgeit wall %d next wall %d next next wall %d \n",wall,next_wall,wall_list[next_wall].Next);
					if(!next_wall)
					{
						next_wall=storey_list[storey].WallHead;
						//LogText(" next_wall is 0 so skip onto wall %d \n",next_wall);
					}
						
					p_ledge->X3=wall_list[next_wall].DX;
					p_ledge->Z3=wall_list[next_wall].DZ;
					next_next_wall=wall_list[next_wall].Next;
					if(!next_next_wall)
					{
						next_next_wall=storey_list[storey].WallHead;
						
					}
					p_ledge->X4=wall_list[next_next_wall].DX;
					p_ledge->Z4=wall_list[next_next_wall].DZ;

					p_ledge->X1=prev_x[storey_count];
					p_ledge->Z1=prev_z[storey_count];

					ledge_count++;
					next_wall=temp_next;
				}

				if(next_wall)
				{
					//LogText(" append wall \n");
					//LogText(" HELLO3 append next_wall %d\n",next_wall);
					LogText(" append wall for building %d wall %d storey %d \n",building,next_wall,storey);
		 			append_wall_prim(x,y+offset_y,z,next_wall,storey,BLOCK_SIZE*4);
					//LogText(" done append wall \n");
					walls_left=1;
				}


				prev_x[storey_count]=x;
				prev_z[storey_count]=z;
		   		wall_index[storey_count]=next_wall;
			}
		}
		//LogText(" exit loop \n");
		if(walls_left)
		for(c0=0;c0<ledge_count;c0++)
		{
			//LogText(" build ledge \n");
			build_single_ledge(&ledge_info[c0]);
		}
		ledge_count=0;

		//LogText(" walls_left %d \n",walls_left);
		if(walls_left)
		{
			
			if(wall_for_fe[facet_count])
			{
				switch(storey_list[wall_for_fe[facet_count]].StoreyType)
				{
					case	STOREY_TYPE_FIRE_ESCAPE:
						build_firescape(wall_for_fe[facet_count]);
						break;
					case	STOREY_TYPE_STAIRCASE:
						build_staircase(wall_for_fe[facet_count]);
						break;
					
				}
				
			}
		}
		//LogText(" build facet mid\n");
	   	//oldprev_facet=build_facet(start_point,start_face3,start_face4,prev_facet,0);
		//LogText(" DONE build facet mid\n");
		start_point=next_prim_point;
		start_face3=next_prim_face3;
		start_face4=next_prim_face4;
	}

	//LogText(" find roof\n");
	storey=building_list[building].StoreyHead;
	while(storey)
	{
		
		if(storey_list[storey].Roof)
		{
			switch(storey_list[storey_list[storey].Roof].StoreyType)
			{
				case	STOREY_TYPE_ROOF:
						build_roof(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
						break;
				case	STOREY_TYPE_ROOF_QUAD:
						build_roof_quad(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
				
			}
		}
		storey=storey_list[storey].Next;
	}
   //	LogText(" build facet \n");
   	//oldprev_facet=build_facet(start_point,start_face3,start_face4,prev_facet,FACET_FLAG_NEAR_SORT);
	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;


	//LogText(" build building \n");
	return(build_building(obj_start_point,obj_start_face3,obj_start_face4,prev_facet));
	//LogText(" DONE BUILDING \n");
#ifdef	OLD_STUFF
	while(storey)
	{
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
			
				x=storey_list[storey].DX;
				z=storey_list[storey].DZ;
				y=storey_list[storey].DY;
				wall=storey_list[storey].WallHead;

				if(storey_list[storey].StoreyFlags&FLAG_STOREY_LEDGE)
				{
					build_ledge(x,y,z,wall,storey,BLOCK_SIZE>>1);
//					y-=BLOCK_SIZE>>1;
					offset_y+=BLOCK_SIZE>>1;
					
				}

				while(wall)
				{
					append_wall_prim(x,y+offset_y,z,wall,storey,BLOCK_SIZE*4);
					x=wall_list[wall].DX;
					z=wall_list[wall].DZ;
					wall=wall_list[wall].Next;
				}
				prev_storey=storey;
				if(storey_list[storey].Roof)
				{
					switch(storey_list[storey_list[storey].Roof].StoreyType)
					{
						case	STOREY_TYPE_ROOF:
								build_roof(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
								break;
						case	STOREY_TYPE_ROOF_QUAD:
								build_roof_quad(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
						
					}
				}
	  //			LogText(" built a floor  next face 4 %d \n",next_prim_face4);
				break;
			case	STOREY_TYPE_FIRE_ESCAPE:
					build_firescape(storey);
				break;
		}
		storey=storey_list[storey].Next;
//		y-=BLOCK_SIZE*5;
	}
#endif


//	create_prim_object(start_point,next_prim_point,start_face4,next_prim_face4,start_face3,next_prim_face3,building);
	
}

void	create_building_prim_old2(std::uint16_t building)
{
	std::uint16_t	start_point;
	std::uint16_t	start_face3,start_face4;
	std::int32_t	storey,wall;
	std::int32_t	prev_storey;
	std::int32_t	x,y,z;
	std::int32_t	offset_y=0;

	start_point=next_prim_point;
	start_face3=next_prim_face3;
	start_face4=next_prim_face4;
	y=0; //storey_list[storey].DY;

	storey=building_list[building].StoreyHead;
	while(storey)
	{
		switch(storey_list[storey].StoreyType)
		{
			case	STOREY_TYPE_NORMAL:
			
				x=storey_list[storey].DX;
				z=storey_list[storey].DZ;
				y=storey_list[storey].DY;
				wall=storey_list[storey].WallHead;

				if(storey_list[storey].StoreyFlags&FLAG_STOREY_LEDGE)
				{
					build_ledge(x,y,z,wall,storey,BLOCK_SIZE>>1);
//					y-=BLOCK_SIZE>>1;
					offset_y+=-BLOCK_SIZE>>1;
					
				}

				while(wall)
				{
					append_wall_prim(x,y+offset_y,z,wall,storey,BLOCK_SIZE*4);
					x=wall_list[wall].DX;
					z=wall_list[wall].DZ;
					wall=wall_list[wall].Next;
				}
				prev_storey=storey;
				if(storey_list[storey].Roof)
				{
					switch(storey_list[storey_list[storey].Roof].StoreyType)
					{
						case	STOREY_TYPE_ROOF:
								build_roof(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
								break;
						case	STOREY_TYPE_ROOF_QUAD:
								build_roof_quad(storey,storey_list[storey_list[storey].Roof].DY+offset_y);
						
					}
				}
				LogText(" built a floor  next face 4 %d \n",next_prim_face4);
				break;
			case	STOREY_TYPE_FIRE_ESCAPE:
					build_firescape(storey);
				break;
		}
		storey=storey_list[storey].Next;
//		y-=BLOCK_SIZE*4;
	}


//	create_prim_object(start_point,next_prim_point,start_face4,next_prim_face4,start_face3,next_prim_face3,building);
	
}


void	clear_map2()
{
	std::int32_t	x,z,c0;
	for(x=0;x<EDIT_MAP_WIDTH;x++)
	for(z=0;z<EDIT_MAP_DEPTH;z++)
	{
		std::int32_t	index;
		index=edit_map[x][z].MapThingIndex;
		switch(map_things[index].Type)
		{
			case	MAP_THING_TYPE_PRIM:
			case	MAP_THING_TYPE_LIGHT:
				map_things[index].MapChild=0;
				break;
			default:
				edit_map[x][z].MapThingIndex=0;
				break;
		}

		edit_map[x][z].ColVectHead=0;
		edit_map[x][z].Walkable=0;
		edit_map[x][z].Flags&=~FLOOR_HIDDEN;
		
	}

//	memset(edit_map,0,sizeof(struct DepthStrip)*EDIT_MAP_WIDTH*EDIT_MAP_DEPTH);
	//memset((std::uint8_t*)&map_things[0],0,sizeof(struct MapThing)*MAX_MAP_THINGS);
	for(c0=0;c0<MAX_MAP_THINGS;c0++)
	{
		switch(map_things[c0].Type)
		{
			case	MAP_THING_TYPE_PRIM:
			case	MAP_THING_TYPE_LIGHT:
				break;
			default:
				memset((std::uint8_t*)&map_things[c0],0,sizeof(struct MapThing));
				break;
		}
	}

	clear_prims();


	next_building_object=1;
	next_building_facet=1;
	next_col_vect=1;
	next_col_vect_link=1;
	next_walk_link=1;
}


void	wibble_floor()
{
	std::int32_t	dx,dz;
	return;
	for(dx=0;dx<EDIT_MAP_WIDTH;dx++)
	for(dz=0;dz<EDIT_MAP_DEPTH;dz++)
	{
		edit_map[dx][dz].Y=(COS((dx*15)&2047)+SIN((dz*15)&2047))>>12;
	}
}



void	clip_building_prim(std::int32_t prim,std::int32_t x,std::int32_t y,std::int32_t z)
{
	std::int32_t	index;
	std::int32_t	best_z=-999999,az;
	struct	BuildingFacet	*p_facet;
	std::int32_t	sp,ep;
	struct	PrimFace4		*p_f4;
	struct	PrimFace3		*p_f3;
	std::int32_t	c0;

//	LogText(" draw a building %d at %d %d %d\n",building,x,y,z);
	index=building_objects[prim].FacetHead;
	while(index)
	{
		p_facet     =	&building_facets[index];

		if(p_facet->EndFace4)
		for(c0=p_facet->StartFace4;c0<p_facet->EndFace4;c0++)
		{
		}

		if(p_facet->EndFace3)
		for(c0=p_facet->StartFace3;c0<p_facet->EndFace3;c0++)
		{
		}

		sp=p_facet->StartPoint;
		ep=p_facet->EndPoint;
		for(c0=sp;c0<ep;c0++)
		{
			std::int32_t	px,py,pz;
			std::int32_t	fy;

			px=prim_points[c0].X+x;
			py=prim_points[c0].Y+y;
			pz=prim_points[c0].Z+z;

			fy=calc_height_at(px,pz);

			if(py<fy)
			{
				prim_points[c0].Y+=fy-py;
			}

		}

		index=building_facets[index].NextFacet;
	}

}


void	create_city()
{
	std::int32_t	c0;
	std::int32_t	temp_next_prim;
	std::int32_t	temp_next_face3;
	std::int32_t	temp_next_face4;
	std::int32_t	temp_next_point;
	std::int32_t	temp_next_building_object;
	std::int32_t	temp_next_building_facet;

//	save_all_prims("temp.sav");
	clear_map2();

	load_all_prims("allprim.sav");

	temp_next_prim			 =next_prim_object;
	temp_next_face3			 =next_prim_face3;
	temp_next_face4			 =next_prim_face4;
	temp_next_point			 =next_prim_point;
	temp_next_building_object=next_building_object;
	temp_next_building_facet =next_building_facet;
	

/*
	LogText("*************\n");
	LogText("*create city*\n");
	LogText("*************\n");
	LogText(" obj %d face3 %d face4 %d point %d build %d facet %d \n",next_prim_object,next_prim_face3,next_prim_face4,next_prim_point,next_building_object,next_building_facet);
	LogText(" col vect %d link %d \n",next_col_vect,next_col_vect_link);
*/


	wibble_floor();
	for(c0=1;c0<MAX_BUILDINGS;c0++)
	{
		std::int32_t	prim;
		if(building_list[c0].BuildingFlags)
		{
			std::int32_t	y;
			prim=create_building_prim(c0,&y);
			building_list[c0].X=build_x;
			building_list[c0].Y=y;
			building_list[c0].Z=build_z;
			//y=calc_height_at(build_x,build_z);
//			LogText(" place building y %d \n",y);
			place_building_at(prim,build_x,y,build_z);

extern void save_asc(std::uint16_t building,std::uint16_t version);
//			save_asc(c0,1);
			
//			clip_building_prim(prim,build_x,y,build_z);
		}
		
	}
extern void	apply_global_amb_to_map();
	apply_global_amb_to_map();
/*
	next_prim_object			 =temp_next_prim;
	next_prim_face3			 =temp_next_face3;
	next_prim_face4			 =temp_next_face4;
	next_prim_point			 =temp_next_point;
	next_building_object= temp_next_building_object;
	next_building_facet = temp_next_building_facet;
*/
	
}




//**************************************8

void	offset_buildings(std::int32_t x,std::int32_t y,std::int32_t z)
{
	std::int32_t	c0;
	for (c0=1;c0<MAX_STOREYS;c0++ )
	{
		if(storey_list[c0].StoreyFlags)
		{
			storey_list[c0].DX+=(std::int16_t)x;
			storey_list[c0].DY+=(std::int16_t)y;
			storey_list[c0].DY=-storey_list[c0].DY+(std::int16_t)y;
			storey_list[c0].DZ+=(std::int16_t)z;
		}
		
	}
	for (c0=1;c0<MAX_WALLS;c0++ )
	{
		if(wall_list[c0].WallFlags)
		{
			wall_list[c0].DX+=(std::int16_t)x;
			wall_list[c0].DY=-wall_list[c0].DY+(std::int16_t)y;
			wall_list[c0].DZ+=(std::int16_t)z;
		}
	}
/*
*/
}

/*
void	calc_buildings_screen_box(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,EdRect *rect)
{
	std::int32_t	c0,flags;
	struct	PrimObject	*p_obj;
	std::int32_t	sp,ep;
	std::int32_t	min_x=999999,max_x=-999999,min_y=999999,max_y=-999999;

	p_obj    =&prim_objects[prim];

	sp=p_obj->StartPoint;
	ep=p_obj->EndPoint;
	
	engine.X-=x<<8;
	engine.Y-=y<<8;
	engine.Z-=z<<8;
		
	for(c0=sp;c0<ep;c0++)
	{
		//transform all points for this Object
		flags=rotate_point_gte((struct SVECTOR*)&prim_points[c0],&global_res[c0-sp]);
//		if(!(flags & EF_CLIPFLAGS))
		{
			if(global_res[c0-sp].X<min_x)
				min_x=global_res[c0-sp].X;

			if(global_res[c0-sp].X>max_x)
				max_x=global_res[c0-sp].X;

			if(global_res[c0-sp].Y<min_y)
				min_y=global_res[c0-sp].Y;

			if(global_res[c0-sp].Y>max_y)
				max_y=global_res[c0-sp].Y;
		}
		
	}

	engine.X+=x<<8;
	engine.Y+=y<<8;
	engine.Z+=z<<8;
	if(min_x<0)
		min_x=0;
	if(min_y<0)
		min_y=0;

	rect->SetRect(min_x-2,min_y-2,max_x-min_x+4,max_y-min_y+4);
}

void	calc_buildings_world_box(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,EdRect *rect)
{
	std::int32_t	c0;
	struct	PrimObject	*p_obj;
	std::int32_t	sp,ep;
	std::int32_t	min_x=999999,max_x=-999999,min_y=999999,max_y=-999999;

	p_obj    =&prim_objects[prim];

	sp=p_obj->StartPoint;
	ep=p_obj->EndPoint;
	
		
	for(c0=sp;c0<ep;c0++)
	{
		global_res[c0-sp].X=prim_points[c0].X+x;
		global_res[c0-sp].Y=prim_points[c0].Y+y;
		global_res[c0-sp].Z=prim_points[c0].Z+z;
		{
			if(global_res[c0-sp].X<min_x)
				min_x=global_res[c0-sp].X;

			if(global_res[c0-sp].X>max_x)
				max_x=global_res[c0-sp].X;

			if(global_res[c0-sp].Y<min_y)
				min_y=global_res[c0-sp].Y;

			if(global_res[c0-sp].Y>max_y)
				max_y=global_res[c0-sp].Y;
		}
		
	}

	if(min_x<0)
		min_x=0;
	if(min_y<0)
		min_y=0;

	rect->SetRect(min_x,min_y,max_x-min_x,max_y-min_y);
}
*/
std::uint16_t	is_it_clockwise(std::int32_t p0,std::int32_t p1,std::int32_t p2)
{
	std::int32_t	z;
	std::int32_t	vx,vy,wx,wy;

	vx=global_res[p1].X-global_res[p0].X;
	wx=global_res[p2].X-global_res[p1].X;
	vy=global_res[p1].Y-global_res[p0].Y;
	wy=global_res[p2].Y-global_res[p1].Y;

	z=vx*wy-vy*wx;

	if(z>0)
		return	1;
	else
		return	0;
}

// problems getting the top face under the fires escape to be a facet member
extern std::int32_t	calc_shadow_co_ord(struct SVECTOR *input,struct SVECTOR *output,std::int32_t l_x,std::int32_t l_y,std::int32_t l_z);

std::int32_t	draw_a_facet_at(std::uint16_t	facet,std::int32_t x,std::int32_t y,std::int32_t z)
{
	struct	PrimFace4		*p_f4;
	struct	PrimFace3		*p_f3;
	std::uint32_t	flag_and,flag_or;
	std::int32_t	c0;
	struct	BuildingFacet	*p_facet;
	std::int32_t	sp,mp,ep;
	std::int32_t	az;
	std::int32_t	col=0,cor=0,cob=0,cot=0,total=0;
	std::int32_t	best_z=9999999;
	std::int32_t	min_z=9999999,max_z=-9999999;
	std::int32_t	first_face=1;

	std::int32_t	facet_flags;
	std::int32_t	offset_z=0;

	struct	SVECTOR			res_shadow[1560],temp_shadow; //max points per object?
	std::int32_t	flags_shadow[1560];
	std::int32_t	shadow=0;






	p_facet     =	&building_facets[facet];
	facet_flags =	p_facet->FacetFlags;
	p_f4        =	&prim_faces4[p_facet->StartFace4];
	p_f3        =	&prim_faces3[p_facet->StartFace3];
	if(facet_flags&FACET_FLAG_ROOF)
	{
		first_face=0;
		offset_z=-50;
	}

	sp=p_facet->StartPoint;
	mp=p_facet->MidPoint;
	ep=p_facet->EndPoint;
	
//	LogText(" draw a facet %d at %d %d %d, sp %d ep %d sf4 %d ef4 %d \n",facet,x,y,z,sp,ep,p_facet->StartFace4,p_facet->EndFace4);

	engine.X-=x<<8;
	engine.Y-=y<<8;
	engine.Z-=z<<8;
		
	for(c0=sp;c0<mp;c0++)
	{
		//transform all points for this Object

		if(shadow)
		{
			calc_shadow_co_ord((struct SVECTOR*)&prim_points[c0],&temp_shadow,9000*2,4000,8000*2);//light co_ord
			flags_shadow[c0-sp]=rotate_point_gte((struct SVECTOR*)&temp_shadow,&res_shadow[c0-sp]);
		}

		global_flags[c0-sp]=rotate_point_gte((struct SVECTOR*)&prim_points[c0],&global_res[c0-sp]);
		global_bright[c0-sp]=calc_lights(x,y,z,(struct SVECTOR*)&prim_points[c0]);
		if(ControlFlag)
		{
			char	str[100];
			sprintf(str,"%d",global_res[c0-sp].Z);
			QuickTextC(global_res[c0-sp].X+1,global_res[c0-sp].Y+1,str,0);
			QuickTextC(global_res[c0-sp].X,global_res[c0-sp].Y,str,1);
		}
		if(min_z>global_res[c0-sp].Z)
			min_z=global_res[c0-sp].Z;
		if(max_z<global_res[c0-sp].Z)
			max_z=global_res[c0-sp].Z;

		//if(best_z>global_res[c0-sp].Z)
		//	best_z=global_res[c0-sp].Z;

	}
	for(c0=mp;c0<ep;c0++)
	{
		//transform all points for this Object

		global_flags[c0-sp]=rotate_point_gte((struct SVECTOR*)&prim_points[c0],&global_res[c0-sp]);
		global_bright[c0-sp]=calc_lights(x,y,z,(struct SVECTOR*)&prim_points[c0]);

	}
	engine.X+=x<<8;
	engine.Y+=y<<8;
	engine.Z+=z<<8;
	best_z=min_z;



	if(p_facet->EndFace4)
	for(c0=p_facet->StartFace4;c0<p_facet->EndFace4;c0++)
	{
		std::int32_t	p0,p1,p2,p3;

		if(current_bucket_pool>=end_bucket_pool)
			goto	exit;

		p0=p_f4->Points[0]-sp;
		p1=p_f4->Points[1]-sp;
		p2=p_f4->Points[2]-sp;
		p3=p_f4->Points[3]-sp;

		if(shadow)
		{
			flag_and = flags_shadow[p0]&flags_shadow[p1]&flags_shadow[p2]&flags_shadow[p3];	
			flag_or = flags_shadow[p0]|flags_shadow[p1]|flags_shadow[p2]|flags_shadow[p3];	

			if((flag_or&EF_BEHIND_YOU)==0)
			if(!(flag_and & EF_CLIPFLAGS))
			{
				az=(res_shadow[p0].Z+res_shadow[p1].Z+res_shadow[p2].Z+res_shadow[p3].Z)>>2;
				az-=150;

				setPolyType4(
								current_bucket_pool,
								POLY_F
							);

				setCol4	(
							(struct BucketQuad*)current_bucket_pool,
							0
						);

				setXY4	(
							(struct BucketQuad*)current_bucket_pool,
							res_shadow[p0].X,res_shadow[p0].Y,
							res_shadow[p1].X,res_shadow[p1].Y,
							res_shadow[p2].X,res_shadow[p2].Y,
							res_shadow[p3].X,res_shadow[p3].Y
						);


				setZ4((struct BucketQuad*)current_bucket_pool,-res_shadow[p0].Z,-res_shadow[p1].Z,-res_shadow[p2].Z,-res_shadow[p3].Z);
				((struct BucketQuad*)current_bucket_pool)->DebugInfo=c0;
				((struct BucketQuad*)current_bucket_pool)->DebugFlags=0;
				
				add_bucket((void* )current_bucket_pool,az);

				current_bucket_pool+=sizeof(struct BucketQuad);

			}
		}


		{
			
	
			flag_and = global_flags[p0]&global_flags[p1]&global_flags[p2]&global_flags[p3];	
			flag_or = global_flags[p0]|global_flags[p1]|global_flags[p2]|global_flags[p3];	

			if((p_f4->FaceFlags&FACE_FLAG_SMOOTH)&&ShiftFlag)
			{
				
			}
			else
			{

				
				if( (!(flag_and & EF_CLIPFLAGS))&&((flag_or&EF_BEHIND_YOU)==0))
				{
					std::int32_t	wid,height;
					std::int32_t	sort_level;
/*
					if(first_face)
					{
						first_face=0;

						if(!(p_f4->DrawFlags&POLY_FLAG_DOUBLESIDED))
						if(!is_it_clockwise(p0,p1,p2))
						{
							c0=p_facet->MidFace4;
							p_f4     =	&prim_faces4[p_facet->MidFace4];
							best_z=max_z;
							goto	skip_wall;
						}
					}
*/
					total++;
/*
					if(!AltFlag)
					{
		
						az=global_res[p0].Z+1000;

						if(az<global_res[p1].Z)
							az=global_res[p1].Z+1000;

						if(az<global_res[p2].Z)
							az=global_res[p2].Z+1000;

						if(az<global_res[p3].Z)
							az=global_res[p3].Z+1000;
						az-=1000;

					}
					else
					{
						sort_level=GET_SORT_LEVEL(p_f4->FaceFlags);
						if(sort_level==0)
						{
							az=best_z;
						}
						else
						{
							az=best_z-(sort_level<<2);
							
						}

					}
*/
						sort_level=GET_SORT_LEVEL(p_f4->FaceFlags);
						if(sort_level==0)
						{
							//if(best_z>az)
							//	best_z=az;
							az=global_res[p0].Z;

							if(az>global_res[p1].Z)
								az=global_res[p1].Z;

							if(az>global_res[p2].Z)
								az=global_res[p2].Z;

							if(az>global_res[p3].Z)
								az=global_res[p3].Z;
						}
						else
						{
							az=best_z-(sort_level<<2);
							//return(best_z);
						}
						//LogText(" facet %d sort_level %d best_z %d az %d face %d \n",facet,sort_level,best_z,az,c0);

					wid=WorkWindowWidth;

					height=WorkWindowHeight;

					setPolyType4(
									current_bucket_pool,
									p_f4->DrawFlags
								);


					setCol4	(
								(struct BucketQuad*)current_bucket_pool,
								((std::uint8_t)p_f4->Col)
							);


					setXY4	(
								(struct BucketQuad*)current_bucket_pool,
								global_res[p0].X,global_res[p0].Y,
								global_res[p1].X,global_res[p1].Y,
								global_res[p2].X,global_res[p2].Y,
								global_res[p3].X,global_res[p3].Y
							);

					if(SelectFlag)
						do_quad_clip_list((std::int16_t)c0,p0,p1,p2,p3);

//RUD
					if(p_f4->DrawFlags&POLY_FLAG_TEXTURED)
					{
						setUV4	(
								(struct BucketQuad*)current_bucket_pool,
								p_f4->UV[0][0],p_f4->UV[0][1],
								p_f4->UV[1][0],p_f4->UV[1][1],
								p_f4->UV[2][0],p_f4->UV[2][1],
								p_f4->UV[3][0],p_f4->UV[3][1],
								(std::uint8_t)p_f4->TexturePage
								);
					}

					setZ4((struct BucketQuad*)current_bucket_pool,global_res[p0].Z,global_res[p1].Z,global_res[p2].Z,global_res[p3].Z);
					

					setShade4((struct BucketQuad*)current_bucket_pool,
					CLIP256(p_f4->Bright[0]+global_bright[p0]),
					CLIP256(p_f4->Bright[1]+global_bright[p1]),
					CLIP256(p_f4->Bright[2]+global_bright[p2]),
					CLIP256(p_f4->Bright[3]+global_bright[p3]));
					((struct BucketQuad*)current_bucket_pool)->DebugInfo=az; //c0;
					((struct BucketQuad*)current_bucket_pool)->DebugFlags=p_f4->FaceFlags;

					add_bucket((void* )current_bucket_pool,az);

					if(check_mouse_over_prim_quad(global_res,p0,p1,p2,p3,c0))
					{
						selected_prim_xyz.X	=	x;
						selected_prim_xyz.Y	=	y;
						selected_prim_xyz.Z	=	z;
					}

					current_bucket_pool	+=	sizeof(struct BucketQuad);
				}
				else
				{
					if(flag_and&EF_OFF_LEFT)
						col++;
					if(flag_and&EF_OFF_RIGHT)
						cor++;
					if(flag_and&EF_OFF_TOP)
						cot++;
					if(flag_and&EF_OFF_BOTTOM)
						cob++;


		
				}	
//				LogText(" clipped face %d \n",c0);
			}
		}

		p_f4++;
skip_wall:;
	}

	if(p_facet->EndFace3)
	for(c0=p_facet->StartFace3;c0<p_facet->EndFace3;c0++)
	{
		std::int32_t	p0,p1,p2;

		if(current_bucket_pool>=end_bucket_pool)
				goto	exit;


		p0=p_f3->Points[0]-sp;
		p1=p_f3->Points[1]-sp;
		p2=p_f3->Points[2]-sp;

		flag_and = global_flags[p0]&global_flags[p1]&global_flags[p2];	
		flag_or  = global_flags[p0]|global_flags[p1]|global_flags[p2];	

		if((flag_or&EF_BEHIND_YOU)==0)
		if(!(flag_and & EF_CLIPFLAGS))
		{
//			az=(global_res[p0].Z+global_res[p1].Z+global_res[p2].Z)/3;
			az=global_res[p0].Z;

			if(az>global_res[p1].Z)
				az=global_res[p1].Z;

			if(az>global_res[p2].Z)
				az=global_res[p2].Z;

			setPolyType3(
							current_bucket_pool,
							p_f3->DrawFlags
						);

			setCol3	(
						(struct BucketTri*)current_bucket_pool,
						p_f3->Col
					);

			setXY3	(
						(struct BucketTri*)current_bucket_pool,
						global_res[p0].X,global_res[p0].Y,
						global_res[p1].X,global_res[p1].Y,
						global_res[p2].X,global_res[p2].Y
					);

			if(SelectFlag)
				do_tri_clip_list(-c0,p0,p1,p2);

//RUD
			if(p_f3->DrawFlags&POLY_FLAG_TEXTURED)
			{
				setUV3	(
						(struct BucketTri*)current_bucket_pool,
						p_f3->UV[0][0],p_f3->UV[0][1],
						p_f3->UV[1][0],p_f3->UV[1][1],
						p_f3->UV[2][0],p_f3->UV[2][1],
						p_f3->TexturePage
					);
			}

			setShade3((struct BucketTri*)current_bucket_pool,
				CLIP256(p_f3->Bright[0]+global_bright[p0]),
				CLIP256(p_f3->Bright[1]+global_bright[p1]),
				CLIP256(p_f3->Bright[2]+global_bright[p2]));

			setZ3((struct BucketQuad*)current_bucket_pool,global_res[p0].Z,global_res[p1].Z,global_res[p2].Z);

			((struct BucketTri*)current_bucket_pool)->DebugInfo=c0;
			((struct BucketTri*)current_bucket_pool)->DebugFlags=p_f3->FaceFlags;

			add_bucket((void* )current_bucket_pool,az);

			if(check_mouse_over_prim_tri(global_res,p0,p1,p2,c0))
			{
				selected_prim_xyz.X	=	x;
				selected_prim_xyz.Y	=	y;
				selected_prim_xyz.Z	=	z;
			}

			current_bucket_pool	+=	sizeof(struct BucketQuad);
		}
		p_f3++;
	}
exit:;

//	LogText(" draw a prim  left %d right %d top %d bot %d  ok %d \n",col,cor,cot,cob,total);

	return(best_z);
}

void	draw_a_building_at(std::uint16_t building,std::int32_t x,std::int32_t y,std::int32_t z)
{
	std::uint16_t	index;
	std::int32_t	best_z=-999999,az;
	//LogText(" draw a building %d at %d %d %d\n",building,x,y,z);
	index=building_objects[building].FacetHead;
	while(index)
	{
		//LogText(" draw facet %d \n",index);
		az=draw_a_facet_at(index,x,y,z);
		if(best_z<az)
			best_z=az;
		index=building_facets[index].NextFacet;
	}
	
}
