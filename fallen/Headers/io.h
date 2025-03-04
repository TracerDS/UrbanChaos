#ifndef		IO_H
#define		IO_H

//
// Defines
//



//
// Structs
//

struct	LoadGameThing
{
	std::uint16_t	Type;
	std::uint16_t	SubStype;

	std::int32_t	X;
	std::int32_t	Y;
	std::int32_t	Z;
	std::uint32_t	Flags;

	std::uint16_t	IndexOther;
	std::uint16_t	AngleX;

	std::uint16_t	AngleY;
	std::uint16_t	AngleZ;

	std::uint32_t	Dummy[4];



};

//
// Data
//

extern char	DATA_DIR[];
extern char	LEVELS_DIR[];
extern char	TEXTURE_WORLD_DIR[];

//
// Functions
//


extern void	change_extension(char	*name,char* add,char* new_name);
extern void	load_game_map(char     *name);
extern std::int32_t	load_all_prims(char	*name);
extern std::int32_t	load_a_multi_prim(char* name);
extern void	load_palette(char* palette);
extern void	load_key_frame_chunks(KeyFrameChunk *the_chunk,char* vue_name,float shrink=1.0);
extern std::int32_t	save_anim_system(struct GameKeyFrameChunk *game_chunk,char	*name);


//
// Loads the textures styles from the given world. Only set (load_editor_names) if
// you are calling this function from the editor.
//

extern void load_texture_styles(std::uint8_t load_editor_names, std::uint8_t world);

//
// Loads in the given prim object if it is not already loaded.
// Returns false on failure.
//

std::int32_t load_prim_object(std::int32_t prim);

//
// Loads in all the individual prim objects.
//

void load_all_individual_prims();


#endif
