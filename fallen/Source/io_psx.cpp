#ifdef PSX
//
// File I/O for game (M.C.Diskett)
//

#include "game.h"
#include "mav.h"
#include "ob.h"

//
// PSX include
//
#include "libsn.h"

#define MFFileHandle std::int32_t
#define FILE_OPEN_ERROR (-1)
#define SEEK_MODE_CURRENT (1)

#define FileOpen(x) PCopen(x, 0, 0)
#define FileClose(x) PCclose(x)
#define FileCreate(x, y) PCopen(x, 1, 0)
#define FileRead(h, a, s) PCread(h, (char *) a, s)
#define FileWrite(h, a, s) PCwrite(h, (char *) a, s)
#define FileSeek(h, m, o) PClseek(h, o, m)

// #include "math.h"
extern char texture_style_names[200][21];
extern void fix_style_names();
std::int32_t load_a_multi_prim(char *name);
void create_kline_bottle();

//
// This has been ripped from prpitab.cpp in the editor source directory
//
std::uint16_t local_next_prim_point;
std::uint16_t local_next_prim_face4;
std::uint16_t local_next_prim_face3;
std::uint16_t local_next_prim_object;
std::uint16_t local_next_prim_multi_object;

void record_prim_status() {
    local_next_prim_point = next_prim_point;
    local_next_prim_face4 = next_prim_face4;
    local_next_prim_face3 = next_prim_face3;
    local_next_prim_object = next_prim_object;
    local_next_prim_multi_object = next_prim_multi_object;
}

void revert_to_prim_status() {
    next_prim_point = local_next_prim_point;
    next_prim_face4 = local_next_prim_face4;
    next_prim_face3 = local_next_prim_face3;
    next_prim_object = local_next_prim_object;
    next_prim_multi_object = local_next_prim_multi_object;
}

//******************************************************************************************
//
// Bodge Section for PSX
//

//******************************************************************************************
volatile std::uint8_t AltFlag,
    ControlFlag,
    ShiftFlag;
volatile std::uint8_t Keys[256],
    LastKey;

SVector_F AENG_dx_prim_points[10];

/*
void DebugText(char* error, ...)
{
}

void MSG_add(char* fmt, ...)
{
}
*/
void NET_init() {
}

//******************************************************************************************

std::int32_t find_colour(std::uint8_t *the_palette, std::int32_t r, std::int32_t g, std::int32_t b) {
    std::int32_t found = -1;
    std::int32_t dist = 0x7fffffff,
                 c0,
                 dist2,
                 tr,
                 tg,
                 tb;

    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;

    for (c0 = 0; c0 < 256; c0++) {
        tr = *the_palette++;
        tg = *the_palette++;
        tb = *the_palette++;

        tr -= r;
        tg -= g;
        tb -= b;

        dist2 = abs(tr * tr) * 1 + abs(tg * tg) * 1 + abs(tb * tb) * 1; // we notice differences in red more than green more than blue1
        if (dist2 < dist) {
            found = c0;
            dist = dist2;
            if (dist < 8)
                return (c0);
        }
    }
    return (found);
}

void change_extension(char *name, char *add, char *new_name) {
    std::int32_t c0 = 0;
    while (name[c0]) {
        new_name[c0] = name[c0];
        if (name[c0] == '.') {
            new_name[c0 + 1] = add[0];
            new_name[c0 + 2] = add[1];
            new_name[c0 + 3] = add[2];
            new_name[c0 + 4] = 0;
            return;
        }
        c0++;
    }
    new_name[c0] = '.';
    new_name[c0 + 1] = add[0];
    new_name[c0 + 2] = add[1];
    new_name[c0 + 3] = add[2];
    new_name[c0 + 4] = 0;
}

void load_texture_styles(std::uint8_t flag) {
    std::uint16_t temp, temp2;
    std::int32_t save_type = 1;
    MFFileHandle handle = FILE_OPEN_ERROR;

    handle = FileOpen("data/style.tma");
    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &save_type, 4);
        if (save_type > 1) {
            FileRead(handle, (std::uint8_t *) &temp, 2);
            FileRead(handle, (std::uint8_t *) &texture_info[0], sizeof(struct TextureInfo) * 8 * 8 * temp);

            FileRead(handle, (std::uint8_t *) &temp, 2);
            FileRead(handle, (std::uint8_t *) &temp2, 2);
            FileRead(handle, (std::uint8_t *) &textures_xy[0][0], sizeof(struct TXTY) * temp * temp2);

            FileRead(handle, (std::uint8_t *) &temp, 2);
            FileRead(handle, (std::uint8_t *) &temp2, 2);
            if (flag) {
#ifdef EDITOR
                FileRead(handle, (std::uint8_t *) &texture_style_names[0][0], temp * temp2);
                fix_style_names();
#endif
            } else
                FileSeek(handle, SEEK_MODE_CURRENT, temp * temp2);
            if (save_type > 2) {
                FileRead(handle, (std::uint8_t *) &temp, 2);
                FileRead(handle, (std::uint8_t *) &temp2, 2);
                LogText(" read flags x %d z %d\n", temp, temp2);
                FileRead(handle, (std::uint8_t *) &textures_flags[0][0], sizeof(std::uint8_t) * temp * temp2);
            } else {
                std::int32_t x, z;
                for (x = 0; x < 200; x++) {
                    for (z = 0; z < 8; z++) {
                        //						textures_flags[x][z]=POLY_GT;
                    }
                }
            }
        }
        FileClose(handle);
    }
}

void load_game_map(char *name) {
    std::uint16_t temp;
    std::int32_t save_type = 1;
    std::int32_t x, z;
    std::int32_t c0;
    MapElement me;
    Thing th;
    struct MapThing *t_mthing;

    std::uint16_t count1, count2, count3, count4;

    MFFileHandle handle = FILE_OPEN_ERROR;
    handle = FileOpen(name);
    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &save_type, 4);
        FileRead(handle, (std::uint8_t *) &PAP_2HI(0, 0), sizeof(PAP_Hi) * PAP_SIZE_HI * PAP_SIZE_HI);

        //
        // All this ob nonsense are the obkjects on the map (like lamposts)
        //
        FileRead(handle, (std::uint8_t *) &OB_ob_upto, sizeof(OB_ob_upto));
        FileRead(handle, (std::uint8_t *) &OB_ob[0], sizeof(OB_Ob) * OB_ob_upto);

        //
        // notice that strangely they have their very own mapwho
        //
        FileRead(handle, (std::uint8_t *) &OB_mapwho[0][0], sizeof(OB_Mapwho) * OB_SIZE * OB_SIZE);

        //
        // Clear the mapwho data in the low-res map.
        //

        for (x = 0; x < PAP_SIZE_LO; x++)
            for (z = 0; z < PAP_SIZE_LO; z++) {
                PAP_2LO(x, z).MapWho = 0;
            }

        void load_super_map(std::int32_t, std::int32_t st);
        load_super_map(handle, save_type);

        FileClose(handle);
    }
}

/*
void add_point(std::int32_t x,std::int32_t y,std::int32_t z)
{
        prim_points[next_prim_point].X=x;
        prim_points[next_prim_point].Y=y;
        prim_points[next_prim_point].Z=z;
        next_prim_point++;
}
#define	CHEIGHT1	80
#define	CHEIGHT2	160
void build_car_prim()
{
        std::int32_t sp[5];

        sp[0]=next_prim_point;

        add_point(-128,0,-30);
        add_point(-128,CHEIGHT,-30);

        add_point(-128,0,-30);
        add_point(-128,CHEIGHT,-30);



}
*/

std::int32_t load_all_prims(char *name) {
    std::int32_t c0, point;
    MFFileHandle handle;
    char file_name[64];
    std::uint16_t dummy[100];
    std::uint16_t check_it;

    sprintf(file_name, "data/%s", name);
    handle = FileOpen(file_name);
    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &next_prim_point, sizeof(std::uint16_t));
        FileRead(handle, (std::uint8_t *) &next_prim_face4, sizeof(std::uint16_t));
        FileRead(handle, (std::uint8_t *) &next_prim_face3, sizeof(std::uint16_t));
        FileRead(handle, (std::uint8_t *) &next_prim_object, sizeof(std::uint16_t));

        FileRead(handle, (std::uint8_t *) dummy, sizeof(std::uint16_t) * 10);

        FileRead(handle, (std::uint8_t *) prim_points, sizeof(struct PrimPoint) * next_prim_point);
        FileRead(handle, (std::uint8_t *) prim_faces4, sizeof(struct PrimFace4) * next_prim_face4);
        FileRead(handle, (std::uint8_t *) prim_faces3, sizeof(struct PrimFace3) * next_prim_face3);
        FileRead(handle, (std::uint8_t *) prim_objects, sizeof(struct PrimObject) * next_prim_object);
        FileClose(handle);
#ifdef EDITOR
        extern void record_prim_status();
        record_prim_status();
#endif
        return (1);
    }
    return (0);
}
//---------------------------------------------------------

//---------------------------------------------------------------
std::int32_t key_frame_count, current_element;
std::int32_t x_centre, y_centre, z_centre;

extern char *body_part_names[];

void set_default_people_types(struct KeyFrameChunk *the_chunk) {
    std::int32_t c0, c1;

    for (c0 = 0; c0 < 20; c0++) {
        strcpy(the_chunk->PeopleNames[c0], "Undefined");
        for (c1 = 0; c1 < MAX_BODY_BITS; c1++) {
            the_chunk->PeopleTypes[c0].BodyPart[c1] = c1;
        }
    }
}

void make_compress_matrix(struct KeyFrameElement *the_element, struct Matrix33 *matrix) {
    std::uint32_t encode;
    std::int32_t u, v, w;
    /*
            LogText(" compress>>6 %x %x %x \n",matrix->M[0][0]>>6,matrix->M[0][1]>>6,matrix->M[0][2]>>6);
            LogText(" compress>>6<<? %x %x %x \n",(((matrix->M[0][0]>>6))<<20),(((matrix->M[0][1]>>6))<<10),(((matrix->M[0][2]>>6))<<0));
            LogText(" compress>>6<<?&? %x %x %x \n",((matrix->M[0][0]>>6)<<20)&CMAT0_MASK,((matrix->M[0][1]>>6)<<10)&CMAT1_MASK,((matrix->M[0][2]>>6)<<0)&CMAT2_MASK);
            LogText(" compress %d %d %d ",matrix->M[0][0],matrix->M[0][1],matrix->M[0][2]);

            u=(((the_element->CMatrix.M[0]&CMAT0_MASK)<<2)>>22);
            v=(((the_element->CMatrix.M[0]&CMAT1_MASK)<<12)>>22);
            w=(((the_element->CMatrix.M[0]&CMAT2_MASK)<<22)>>22);

      LogText(" into %d %d %d all %x\n",u<<6,v<<6,w<<6,the_element->CMatrix.M[0]);

            LogText(" into %d \n",the_element->CMatrix.M[0]&CMAT1_MASK);
            LogText(" into<<2 %d \n",the_element->CMatrix.M[0]&CMAT1_MASK<<12);
            LogText(" into>>20 %d \n",((the_element->CMatrix.M[0]&CMAT1_MASK)<<12)>>22);
            LogText(" into>>20 %d \n",(((the_element->CMatrix.M[0]&CMAT1_MASK)<<12)>>22));
            LogText(" into>>20)<<6 %d \n",((((the_element->CMatrix.M[0]&CMAT1_MASK)<<12)>>22))<<6);
    */
    the_element->CMatrix.M[0] = ((((matrix->M[0][0] >> 6)) << 20) & CMAT0_MASK) + ((((matrix->M[0][1] >> 6)) << 10) & CMAT1_MASK) + ((((matrix->M[0][2] >> 6)) << 0) & CMAT2_MASK);
    the_element->CMatrix.M[1] = ((((matrix->M[1][0] >> 6)) << 20) & CMAT0_MASK) + ((((matrix->M[1][1] >> 6)) << 10) & CMAT1_MASK) + ((((matrix->M[1][2] >> 6)) << 0) & CMAT2_MASK);
    the_element->CMatrix.M[2] = ((((matrix->M[2][0] >> 6)) << 20) & CMAT0_MASK) + ((((matrix->M[2][1] >> 6)) << 10) & CMAT1_MASK) + ((((matrix->M[2][2] >> 6)) << 0) & CMAT2_MASK);
}

void load_multi_vue(struct KeyFrameChunk *the_chunk) {
}

void load_key_frame_chunks(KeyFrameChunk *the_chunk, char *vue_name) {
}

void read_a_prim(std::int32_t prim, MFFileHandle handle) {
    std::int32_t c0;
    std::int32_t sf3, ef3, sf4, ef4, sp, ep;
    std::int32_t dp;

    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &prim_objects[next_prim_object].ObjectName, sizeof(prim_objects[0].ObjectName));

        LogText(" name %s becomes prim %d\n", prim_objects[next_prim_object].ObjectName, next_prim_object);

        FileRead(handle, (std::uint8_t *) &sp, sizeof(sp));
        FileRead(handle, (std::uint8_t *) &ep, sizeof(ep));

        LogText(" no points %d \n", ep - sp);
        //		for(c0=sp;c0<ep;c0++)
        FileRead(handle, (std::uint8_t *) &prim_points[next_prim_point], sizeof(struct PrimPoint) * (ep - sp));

        dp = next_prim_point - sp; // was 10 but is now 50 so need to add 40 to all point indexs

        FileRead(handle, (std::uint8_t *) &sf3, sizeof(sf3));
        FileRead(handle, (std::uint8_t *) &ef3, sizeof(ef3));
        LogText(" no face3 %d \n", ef3 - sf3);
        FileRead(handle, (std::uint8_t *) &prim_faces3[next_prim_face3], sizeof(struct PrimFace3) * (ef3 - sf3));
        for (c0 = sf3; c0 < ef3; c0++) {
            prim_faces3[next_prim_face3 + c0 - sf3].Points[0] += dp;
            prim_faces3[next_prim_face3 + c0 - sf3].Points[1] += dp;
            prim_faces3[next_prim_face3 + c0 - sf3].Points[2] += dp;
        }

        FileRead(handle, (std::uint8_t *) &sf4, sizeof(sf4));
        FileRead(handle, (std::uint8_t *) &ef4, sizeof(ef4));
        LogText(" no face4 %d \n", ef4 - sf4);
        FileRead(handle, (std::uint8_t *) &prim_faces4[next_prim_face4], sizeof(struct PrimFace4) * (ef4 - sf4));
        for (c0 = sf4; c0 < ef4; c0++) {
            prim_faces4[next_prim_face4 + c0 - sf4].Points[0] += dp;
            prim_faces4[next_prim_face4 + c0 - sf4].Points[1] += dp;
            prim_faces4[next_prim_face4 + c0 - sf4].Points[2] += dp;
            prim_faces4[next_prim_face4 + c0 - sf4].Points[3] += dp;
        }
        prim_objects[next_prim_object].StartPoint = next_prim_point;
        prim_objects[next_prim_object].EndPoint = next_prim_point + ep - sp;

        prim_objects[next_prim_object].StartFace3 = next_prim_face3;
        prim_objects[next_prim_object].EndFace3 = next_prim_face3 + ef3 - sf3;

        prim_objects[next_prim_object].StartFace4 = next_prim_face4;
        prim_objects[next_prim_object].EndFace4 = next_prim_face4 + ef4 - sf4;

        next_prim_point += ep - sp;
        next_prim_face3 += ef3 - sf3;
        next_prim_face4 += ef4 - sf4;

        next_prim_object++;
    }
}

// extern struct	PrimMultiObject	prim_multi_objects[];

std::int32_t load_a_multi_prim(char *name) {
    std::int32_t c0;
    MFFileHandle handle;
    std::int32_t save_type = 0;
    std::int32_t so, eo;
    char ext_name[80];

    change_extension(name, "moj", ext_name);
    //	sprintf(ext_name,"data/%s",ext_name);
    handle = FileOpen(ext_name);
    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &save_type, sizeof(save_type));

        FileRead(handle, (std::uint8_t *) &so, sizeof(so));
        FileRead(handle, (std::uint8_t *) &eo, sizeof(eo));

        prim_multi_objects[next_prim_multi_object].StartObject = next_prim_object;
        prim_multi_objects[next_prim_multi_object].EndObject = next_prim_object + (eo - so);
        LogText(" load multi prim  no object %d \n", eo - so);
        for (c0 = so; c0 < eo; c0++)
            read_a_prim(c0, handle);

        FileClose(handle);
        next_prim_multi_object++;
        return (next_prim_multi_object - 1);
    } else
        return (0);
}

std::int32_t find_matching_face(struct PrimPoint *p1, struct PrimPoint *p2, struct PrimPoint *p3, std::uint16_t prim) {
    std::int32_t c0, sf, ef, point;
    sf = prim_objects[prim].StartFace4;
    ef = prim_objects[prim].EndFace4;

    for (c0 = sf; c0 <= ef; c0++) {
        if (prim_points[prim_faces4[c0].Points[0]].X == p1->X &&
            prim_points[prim_faces4[c0].Points[0]].Y == p1->Y &&
            prim_points[prim_faces4[c0].Points[0]].Z == p1->Z &&
            prim_points[prim_faces4[c0].Points[1]].X == p2->X &&
            prim_points[prim_faces4[c0].Points[1]].Y == p2->Y &&
            prim_points[prim_faces4[c0].Points[1]].Z == p2->Z &&
            prim_points[prim_faces4[c0].Points[2]].X == p3->X &&
            prim_points[prim_faces4[c0].Points[2]].Y == p3->Y &&
            prim_points[prim_faces4[c0].Points[2]].Z == p3->Z) {
            return (c0);
        }
    }
    return (-1);
}

extern void add_point(std::int32_t x, std::int32_t y, std::int32_t z);
extern struct PrimFace4 *create_a_quad(std::uint16_t p1, std::uint16_t p0, std::uint16_t p3, std::uint16_t p2, std::int16_t texture_style, std::int16_t texture_piece);
extern std::int32_t build_prim_object(std::int32_t sp, std::int32_t sf3, std::int32_t sf4);
extern void save_prim_asc(std::uint16_t prim, std::uint16_t version);

void create_kline_bottle() {
    float x, y, z, u, v;
    float sqrt_2, a = 1.0; // what the fuck should a be
    struct PrimFace4 *p_f4;

    float step = PI / 10.0; // low poly version

    return; // switch it off for now

    /*
            std::int32_t	sp[10000],count=0,inner_count=0;
            std::int32_t	c0,c1;
            std::int32_t	sf3,sf4,stp;

            stp=next_prim_point;
            sf3=next_prim_face3;
            sf4=next_prim_face4;

            sqrt_2=sqrt(2.0);


            for(u=-1.0*PI;u<(1.0*PI)+(step/2.0);u+=step,count++)
            {
                    sp[count]=next_prim_point;
                    for(v=-1.0*PI;v<(1.0*PI)+(step/2.0);v+=step)
                    {
    //			x = cos(u)*(cos(u/2.0)*(sqrt_2+cos(v))+(sin(u/2.0)*sin(v)*cos(v)));
    //			y = sin(u)*(cos(u/2.0)*(sqrt_2+cos(v))+(sin(u/2.0)*sin(v)*cos(v)));
    //			z = -1.0*sin(u/2.0)*(sqrt_2+cos(v))+cos(u/2.0)*sin(v)*cos(v);

                              x = (a+cos(u/2.0)*sin(v)-sin(u/2.0)*sin(2.0*v))*cos(u);

                              y = (a+cos(u/2.0)*sin(v)-sin(u/2.0)*sin(2.0*v))*sin(u);

                              z = sin(u/2.0)*sin(v)+cos(u/2.0)*sin(2.0*v);


                            add_point((std::int32_t)(x*200.0),(std::int32_t)(y*200.0),(std::int32_t)(z*200.0));
                    }
            }

            for(c0=0;c0<count-1;c0++)
            for(c1=0;c1<count-1;c1++)
            {
    //		if((c0+c1)&1)
                    {
                            p_f4=create_a_quad(sp[c0]+c1,sp[c0]+c1+1,sp[c0+1]+c1,sp[c0+1]+c1+1,0,0);
                            p_f4->DrawFlags=1+(1<<6); //POLY_G; gourad
                            p_f4->Col=((c0+c1)&1)*50+50;
                    }
            }

            build_prim_object(stp,sf3,sf4);
            save_prim_asc(next_prim_object-1,0);
            */
}

void load_palette(char *palette) {
}

//
// no meshes
//

// mesh

//

extern void write_a_prim(std::int32_t prim, MFFileHandle handle);

std::int32_t save_insert_a_multi_prim(MFFileHandle handle, std::int32_t multi) {
    std::int32_t c0, point;
    char file_name[64];
    std::int32_t save_type = 0;
    std::int32_t so, eo;
    char ext_name[80];
#ifdef EDITOR
    if (handle != FILE_OPEN_ERROR) {
        FileWrite(handle, (std::uint8_t *) &save_type, sizeof(save_type));

        so = prim_multi_objects[multi].StartObject;
        eo = prim_multi_objects[multi].EndObject;

        FileWrite(handle, (std::uint8_t *) &so, sizeof(so));
        FileWrite(handle, (std::uint8_t *) &eo, sizeof(eo));

        for (c0 = so; c0 < eo; c0++)
            write_a_prim(c0, handle);

        return (1);
    }
#endif
    return (0);
}

std::int32_t save_insert_game_chunk(MFFileHandle handle, struct GameKeyFrameChunk *game_chunk) {
    std::int32_t save_type = 0;
    std::int32_t temp;
#ifdef EDITOR

    FileWrite(handle, (std::uint8_t *) &save_type, sizeof(save_type));

    FileWrite(handle, (std::uint8_t *) &game_chunk->ElementCount, sizeof(game_chunk->ElementCount));
    FileWrite(handle, (std::uint8_t *) &game_chunk->MaxElements, sizeof(game_chunk->MaxElements));
    FileWrite(handle, (std::uint8_t *) &game_chunk->MaxKeyFrames, sizeof(game_chunk->MaxKeyFrames));

    //
    // save the people types
    //
    temp = MAX_PEOPLE_TYPES;
    FileWrite(handle, (std::uint8_t *) &temp, sizeof(temp));
    FileWrite(handle, (std::uint8_t *) &game_chunk->PeopleTypes[0], sizeof(struct BodyDef) * temp);

    //
    // save the keyframe linked lists
    //
    temp = (std::int32_t) &game_chunk->AnimKeyFrames[0];
    FileWrite(handle, (std::uint8_t *) &temp, sizeof(temp));
    FileWrite(handle, (std::uint8_t *) &game_chunk->AnimKeyFrames[0], sizeof(struct GameKeyFrame) * game_chunk->MaxKeyFrames);

    //
    // save the anim elements
    //
    temp = (std::int32_t) &game_chunk->TheElements[0];
    FileWrite(handle, (std::uint8_t *) &temp, sizeof(temp));
    FileWrite(handle, (std::uint8_t *) &game_chunk->TheElements[0], sizeof(struct GameKeyFrameElement) * game_chunk->MaxElements);

    //
    // save the animlist
    //
    temp = 200;
    FileWrite(handle, (std::uint8_t *) &temp, sizeof(temp));
    FileWrite(handle, (std::uint8_t *) &game_chunk->AnimList[0], sizeof(struct GameKeyFrame) * temp);
#endif
    return (1);
}

std::int32_t save_anim_system(struct GameKeyFrameChunk *game_chunk, char *name) {
    std::int32_t c0, point;
    MFFileHandle handle;
    char file_name[64];
    std::int32_t save_type = 0;
    std::int32_t so, eo;
    char ext_name[80];
#ifdef EDITOR

    change_extension(name, "all", ext_name);
    handle = FileCreate(ext_name, 1);
    if (handle != FILE_OPEN_ERROR) {
        FileWrite(handle, (std::uint8_t *) &save_type, sizeof(save_type));

        save_insert_a_multi_prim(handle, game_chunk->MultiObject[0]);
        save_insert_game_chunk(handle, game_chunk);

        FileClose(handle);
        return (1);
    }
#endif
    return (0);
}

std::int32_t load_insert_game_chunk(MFFileHandle handle, struct GameKeyFrameChunk *game_chunk) {
    std::int32_t save_type = 0, c0;
    std::int32_t temp;
    std::uint32_t addr1, addr2, addr3;
    std::int32_t a_off, ae_off;
    std::int32_t af_off;

    FileRead(handle, (std::uint8_t *) &save_type, sizeof(save_type));

    FileRead(handle, (std::uint8_t *) &game_chunk->ElementCount, sizeof(game_chunk->ElementCount));
    FileRead(handle, (std::uint8_t *) &game_chunk->MaxElements, sizeof(game_chunk->MaxElements));
    FileRead(handle, (std::uint8_t *) &game_chunk->MaxKeyFrames, sizeof(game_chunk->MaxKeyFrames));

    //
    // Load the people types
    //
    FileRead(handle, (std::uint8_t *) &temp, sizeof(temp));
    FileRead(handle, (std::uint8_t *) &game_chunk->PeopleTypes[0], sizeof(struct BodyDef) * temp);

    //
    // Load the keyframe linked lists
    //
    FileRead(handle, (std::uint8_t *) &addr1, sizeof(addr1));
    FileRead(handle, (std::uint8_t *) &game_chunk->AnimKeyFrames[0], sizeof(struct GameKeyFrame) * game_chunk->MaxKeyFrames);

    //
    // Load the anim elements
    //
    FileRead(handle, (std::uint8_t *) &addr2, sizeof(addr2));
    FileRead(handle, (std::uint8_t *) &game_chunk->TheElements[0], sizeof(struct GameKeyFrameElement) * game_chunk->MaxElements);

    //
    // Load the animlist
    //
    FileRead(handle, (std::uint8_t *) &temp, sizeof(temp));
    FileRead(handle, (std::uint8_t *) &game_chunk->AnimList[0], sizeof(struct GameKeyFrame) * temp);

    if (save_type >= 1) {
        std::int32_t temp_fight;

        FileRead(handle, (std::uint8_t *) &addr3, sizeof(addr3));

        FileRead(handle, (std::uint8_t *) &temp_fight, sizeof(temp_fight));

        game_chunk->MaxFightCols = temp_fight;

        FileRead(handle, (std::uint8_t *) &game_chunk->FightCols[0], sizeof(struct GameFightCol) * temp_fight);
    }

    LogText("PSX1 game chunk  max animkeyframes %d max theelements %d max animlist %d\n", game_chunk->MaxKeyFrames, game_chunk->MaxElements, temp);

    // was at 100 now at 10, a_off =-90 so we take 90 off each stored address

    a_off = ((std::uint32_t) &game_chunk->AnimKeyFrames[0]) - addr1;
    ae_off = ((std::uint32_t) &game_chunk->TheElements[0]) - addr2;
    af_off = ((std::uint32_t) &game_chunk->FightCols[0]) - addr3;
    for (c0 = 0; c0 < game_chunk->MaxKeyFrames; c0++) {
        std::uint32_t a;

        a = (std::uint32_t) game_chunk->AnimKeyFrames[c0].NextFrame;
        a += a_off;
        if (game_chunk->AnimKeyFrames[c0].NextFrame)
            game_chunk->AnimKeyFrames[c0].NextFrame = (struct GameKeyFrame *) a;

        a = (std::uint32_t) game_chunk->AnimKeyFrames[c0].PrevFrame;
        a += a_off;
        if (game_chunk->AnimKeyFrames[c0].PrevFrame)
            game_chunk->AnimKeyFrames[c0].PrevFrame = (struct GameKeyFrame *) a;

        //		game_chunk->AnimKeyFrames[c0].Fight=0;

        a = (std::uint32_t) game_chunk->AnimKeyFrames[c0].FirstElement;
        a += ae_off;
        game_chunk->AnimKeyFrames[c0].FirstElement = (struct GameKeyFrameElement *) a;

        a = (std::uint32_t) game_chunk->AnimKeyFrames[c0].Fight;
        a += af_off;

        if (game_chunk->AnimKeyFrames[c0].Fight) {
            struct GameFightCol *p_fight;

            game_chunk->AnimKeyFrames[c0].Fight = (struct GameFightCol *) a;

            p_fight = (struct GameFightCol *) a;

            while (p_fight->Next) {
                a = (std::uint32_t) p_fight->Next;
                a += af_off;
                p_fight->Next = (struct GameFightCol *) a;
                p_fight = p_fight->Next;
            }
        }
    }

    a_off = ((std::uint32_t) &game_chunk->AnimKeyFrames[0]) - addr1;
    for (c0 = 0; c0 < temp; c0++) {
        std::uint32_t a;

        a = (std::uint32_t) game_chunk->AnimList[c0];
        a += a_off;
        game_chunk->AnimList[c0] = (struct GameKeyFrame *) a;
    }

    return (1);
}

std::int32_t load_insert_a_multi_prim(MFFileHandle handle) {
    std::int32_t c0;
    std::int32_t save_type = 0;
    std::int32_t so, eo;
    char ext_name[80];

    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &save_type, sizeof(save_type));

        FileRead(handle, (std::uint8_t *) &so, sizeof(so));
        FileRead(handle, (std::uint8_t *) &eo, sizeof(eo));

        prim_multi_objects[next_prim_multi_object].StartObject = next_prim_object;
        prim_multi_objects[next_prim_multi_object].EndObject = next_prim_object + (eo - so);
        LogText(" load multi prim  no object %d \n", eo - so);
        for (c0 = so; c0 < eo; c0++)
            read_a_prim(c0, handle);

        next_prim_multi_object++;
        return (next_prim_multi_object - 1);
    } else
        return (0);
}

std::int32_t load_anim_system(struct GameKeyFrameChunk *game_chunk, char *name) {
    std::int32_t c0, point;
    MFFileHandle handle;
    char file_name[64];
    std::int32_t save_type = 0;
    std::int32_t so, eo;
    char ext_name[80];

    change_extension(name, "all", ext_name);
    handle = FileOpen(ext_name);
    if (handle != FILE_OPEN_ERROR) {
        FileRead(handle, (std::uint8_t *) &save_type, sizeof(save_type));

        game_chunk->MultiObject[0] = load_insert_a_multi_prim(handle);
        load_insert_game_chunk(handle, game_chunk);

        FileClose(handle);
        return (1);
    }
    return (0);
}

#endif