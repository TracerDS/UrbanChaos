#ifndef	SCAN_H
#define	SCAN_H			1

extern void	(*scan_function)(std::int32_t face,std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t extra);
extern void	scan_map();
extern void	scan_map_thing(std::int32_t	map_thing);
extern void	scan_a_prim_at(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z);

#endif