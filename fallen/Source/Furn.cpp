//
// Furniture.
//

#include "game.h"
#include "fmatrix.h"
#include "furn.h"
#include "statedef.h"
#include "dirt.h"
#include "mist.h"
#include "hm.h"
#include "interfac.h"
// #include "command.h"
#include "bang.h"
#include "Sound.h"
#include "pap.h"
#include "memory.h"

//
// The state functions and the state function table.
//

void FURN_process_normal(Thing *);
void FURN_process_moveing(Thing *);
void FURN_driving(Thing *);
void FURN_door(Thing *);

StateFunction FURN_statefunctions[] =
    {
        {STATE_INIT, nullptr},
        {STATE_NORMAL, FURN_process_normal},
        {STATE_COLLISION, nullptr},
        {STATE_ABOUT_TO_REMOVE, nullptr},
        {STATE_REMOVE_ME, nullptr},
        {STATE_MOVEING, FURN_process_moveing},
        {STATE_FDRIVING, nullptr},
        {STATE_FDOOR, FURN_door}};

//
// Initialises the furniture structures. If dx == FURN_NULL_DX then
// that furniture structure is unused.
//

#define FURN_NULL_DX (INFINITY)

void init_furniture() {
    std::int32_t i;

    for (i = 0; i < MAX_FURNITURE; i++) {
        TO_FURNITURE(i)->dx = FURN_NULL_DX;
    }
}

//
// Allocating and decallocating the furniture structues.
//

//
// Allocates a stationary furn structure.
//

Furniture *FURN_alloc_furniture() {
    std::int32_t i;

    for (i = 0; i < MAX_FURNITURE; i++) {
        if (TO_FURNITURE(i)->dx == FURN_NULL_DX) {
            Furniture *ans = TO_FURNITURE(i);

            ans->dx = 0;
            ans->dy = 0;
            ans->dz = 0;
            ans->dyaw = 0;
            ans->dpitch = 0;
            ans->droll = 0;

            return TO_FURNITURE(i);
        }
    }

    ASSERT(0);

    return nullptr;
}

void FURN_dealloc(Furniture *furn) {
    ASSERT(WITHIN(furn, TO_FURNITURE(0), TO_FURNITURE(MAX_FURNITURE - 1)));

    furn->dx = FURN_NULL_DX;
}

//
// Removes the furniture thing
//

void free_furniture(Thing *p_thing) {
    //
    // Free the furniture structure, the drawmesh structure and
    // finally the thing structure itself.
    //

    if (p_thing->Genus.Furniture) {
        FURN_dealloc(p_thing->Genus.Furniture);
    }

    free_draw_mesh(p_thing->Draw.Mesh);

    free_thing(p_thing);
}

THING_INDEX FURN_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t yaw,
    std::int32_t pitch,
    std::int32_t roll,
    std::int32_t prim) {
    DrawMesh *dm;
    THING_INDEX ans = nullptr;
    Thing *p_thing;

    //
    // Get a DrawMesh for this thing.
    //

    dm = alloc_draw_mesh();

    if (dm) {
        ans = alloc_primary_thing();

        if (ans) {
            p_thing = TO_THING(ans);

            //
            // Initialise the thing.
            //

            p_thing->Class = CLASS_FURNITURE;
            p_thing->State = 0;
            p_thing->SubState = 0;
            p_thing->DrawType = DT_MESH;
            p_thing->Flags = 0;
            p_thing->WorldPos.X = x;
            //			p_thing->WorldPos.Y      = calc_height_at(x>>8,z>>8)<<8;
            p_thing->WorldPos.Y = y;
            p_thing->WorldPos.Z = z;
            p_thing->Draw.Mesh = dm;
            p_thing->Genus.Furniture = nullptr;

            //
            // Initialise the drawmesh info.
            //

            dm->ObjectId = prim;
            dm->Angle = yaw;
            dm->Tilt = pitch;
            dm->Roll = roll;

            //
            // Place on the mapwho.
            //

            add_thing_to_map(p_thing);

            //
            // Put into the normal state.
            //

            set_state_function(p_thing, STATE_NORMAL);
        }
    }

    return ans;
}

THING_INDEX VEHICLE_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t angle,
    std::int32_t prim) {
    DrawMesh *dm;
    THING_INDEX ans = nullptr;
    Thing *p_thing;

    //
    // Get a DrawMesh for this thing.
    //

    dm = alloc_draw_mesh();

    if (dm) {
        ans = alloc_primary_thing();

        if (ans) {
            p_thing = TO_THING(ans);

            //
            // Initialise the thing.
            //

            p_thing->Class = CLASS_VEHICLE;
            p_thing->State = 0;
            p_thing->SubState = 0;
            p_thing->DrawType = DT_MESH;
            p_thing->Flags = 0;
            p_thing->WorldPos.X = x;
            p_thing->WorldPos.Y = PAP_calc_height_at(x >> 8, z >> 8) << 8;
            p_thing->WorldPos.Z = z;
            p_thing->Draw.Mesh = dm;
            p_thing->Genus.Furniture = nullptr;

            //
            // Initialise the drawmesh info.
            //

            dm->ObjectId = prim;
            dm->Angle = angle;
            dm->Tilt = 0; // pitch;
            dm->Roll = 0; // roll;

            //
            // Place on the mapwho.
            //

            add_thing_to_map(p_thing);

            //
            // Put into the normal state.
            //

            //			set_state_function(p_thing, STATE_FDRIVING);
            p_thing->StateFn = nullptr;
            p_thing->Genus.Furniture = FURN_alloc_furniture();
        }
    }

    return ans;
}

extern void scan_walk_triangle(std::int32_t x0, std::int32_t y0, std::int32_t z0, std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t face);

void add_walkable_quad(THING_INDEX index, std::int32_t cx, std::int32_t cy, std::int32_t cz, std::int32_t face) {
    std::int32_t x[4], y[4], z[4];
    std::int32_t c0, p0;
    std::int32_t new_face;
    struct PrimFace4 *p_f4, *p_new_f4;

    new_face = next_prim_face4;

    p_new_f4 = &prim_faces4[new_face];

    p_f4 = &prim_faces4[face];

    p_new_f4->ThingIndex = index;

    //
    // The next four probably arent needed but what the hell
    //

    p_new_f4->DrawFlags = p_f4->DrawFlags;
    p_new_f4->FaceFlags = p_f4->FaceFlags;
    p_new_f4->Type = p_f4->Type;
    p_new_f4->ID = p_f4->ID;

    for (c0 = 0; c0 < 4; c0++) {
        p0 = p_f4->Points[c0];
        p_new_f4->Points[c0] = p0;
        x[c0] = cx + prim_points[p0].X;
        y[c0] = cy + prim_points[p0].Y;
        z[c0] = cz + prim_points[p0].Z;
    }

#ifndef PSX // scan_walk_triangle
    scan_walk_triangle(x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2], new_face);
    scan_walk_triangle(x[1], y[1], z[1], x[3], y[3], z[3], x[2], y[2], z[2], new_face);
#endif

    next_prim_face4++;
}

void place_walkable_faces_for_prim(THING_INDEX index, std::int32_t prim) {
    std::int32_t x, y, z;
    std::int32_t c0;
    std::int32_t sf, ef;
    struct PrimObject *p_obj;
    Thing *p_thing;

    p_thing = TO_THING(index);

    x = p_thing->WorldPos.X >> 8;
    y = p_thing->WorldPos.Y >> 8;
    z = p_thing->WorldPos.Z >> 8;

    p_obj = &prim_objects[prim];

    sf = p_obj->StartFace4;
    ef = p_obj->EndFace4;

    for (c0 = sf; c0 < ef; c0++) {
        if (prim_faces4[c0].DrawFlags & POLY_FLAG_WALKABLE) {
            //
            // In the game we use FaceFlags for the walkable flag,
            // not the drawflags...
            //

            prim_faces4[c0].FaceFlags |= FACE_FLAG_WALKABLE; // !!!

            add_walkable_quad(index, x, y, z, c0);
        }
    }
}

//
// for each furniture thing, scan for walkable faces, copy them, then index them to the thing
//

void FURN_add_walkable() {
    THING_INDEX current_thing;
    Thing *p_thing;

    current_thing = PRIMARY_USED;
    while (current_thing) {
        p_thing = TO_THING(current_thing);

        if (p_thing->Class == CLASS_FURNITURE) {
            place_walkable_faces_for_prim(current_thing, p_thing->Draw.Mesh->ObjectId);
        }

        current_thing = p_thing->LinkChild;
    }
}

void FURN_push(
    THING_INDEX thing,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2) {
    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t cx;
    std::int32_t cy;
    std::int32_t cz;

    std::int32_t qx;
    std::int32_t qy;
    std::int32_t qz;

    std::int32_t nx;
    std::int32_t ny;
    std::int32_t nz;

    std::int32_t dyaw;
    std::int32_t dpitch;
    std::int32_t droll;

    std::int32_t matrix[9];

    PrimInfo *inf;

    Thing *p_thing = TO_THING(thing);
    DrawMesh *dm = p_thing->Draw.Mesh;
    Furniture *furn = p_thing->Genus.Furniture;

    //
    // Make sure that this is a furniture thing.
    //

    ASSERT(p_thing->Class == CLASS_FURNITURE);

    if (!furn) {
        //
        // This furniture thing is currently stationary.
        //

        furn = p_thing->Genus.Furniture = FURN_alloc_furniture();

        //
        // Start it moving.
        //

        set_state_function(p_thing, STATE_MOVEING);
    }

    ASSERT(WITHIN(dm, TO_DRAW_MESH(0), TO_DRAW_MESH(MAX_DRAW_MESHES - 1)));
    ASSERT(WITHIN(furn, TO_FURNITURE(0), TO_FURNITURE(MAX_FURNITURE - 1)));

    //
    // Find the centre of gravity of the prim.
    //

    inf = get_prim_info(dm->ObjectId);

    //
    // Find the rotation matrix of the prim.
    //

    FMATRIX_calc(
        matrix,
        dm->Angle,
        dm->Tilt,
        dm->Roll);

    //
    // The weight and moment of inertia shifts.
    //

#define FURN_SHIFT_WEIGHT 12
#define FURN_SHIFT_YAW 10
#define FURN_SHIFT_PITCH 10
#define FURN_SHIFT_ROLL 10

    //
    // Add the force to the velocity of the object.
    //

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    furn->dx += dx << FURN_SHIFT_WEIGHT;
    furn->dy += dy << FURN_SHIFT_WEIGHT;
    furn->dz += dz << FURN_SHIFT_WEIGHT;

    //
    // Rotate the vector into prim space.
    //

    x1 -= p_thing->WorldPos.X >> 8;
    y1 -= p_thing->WorldPos.Y >> 8;
    z1 -= p_thing->WorldPos.Z >> 8;

    x2 -= p_thing->WorldPos.X >> 8;
    y2 -= p_thing->WorldPos.Y >> 8;
    z2 -= p_thing->WorldPos.Z >> 8;

    FMATRIX_MUL(matrix, x1, y1, z2);
    FMATRIX_MUL(matrix, x2, y2, z2);

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    //
    // Find the torque around the y, x and z axis.
    //

    //
    // The force on yaw is the torque around the y-axis.
    // Consider the (x,z) plane and find the perpendicular
    // distance of the 2d force line from the origin.  We
    // need the normal of the line, n, and a point on the
    // line q.
    //

    nx = dz;
    nz = -dx;

    qx = x1;
    qz = z1;

    dyaw = nx * qx + nz * qz;

    //
    // Pitch is about the x-axis.
    //

    nz = dy;
    ny = -dz;

    qz = z1;
    qy = y1;

    dpitch = nz * qz + ny * qy;

    //
    // Roll is about the z-axis.
    //

    dx = dy;
    dy = -dx;

    qx = x1;
    qz = y1;

    droll = nx * qx + ny * qy;

    furn->dyaw += dyaw << FURN_SHIFT_YAW;
    furn->dpitch += dpitch << FURN_SHIFT_PITCH;
    furn->droll += droll << FURN_SHIFT_ROLL;
}

void FURN_process_normal(Thing *p_thing) {
    std::int32_t x1;
    std::int32_t x2;
    std::int32_t z1;
    std::int32_t z2;

    ASSERT(WITHIN(p_thing, TO_THING(1), TO_THING(MAX_THINGS - 1)));

    DrawMesh *dm = p_thing->Draw.Mesh;

    //
    // Make sure that this is a furniture thing and everything is valid.
    //

    ASSERT(
        p_thing->Class == CLASS_FURNITURE ||
        p_thing->Class == CLASS_VEHICLE);
    ASSERT(WITHIN(dm, TO_DRAW_MESH(0), TO_DRAW_MESH(MAX_DRAW_MESHES - 1)));

    if (dm->Hm != 255) {
        std::int32_t dx;
        std::int32_t dz;

        std::int32_t mx;
        std::int32_t mz;

        std::int32_t mid_x;
        std::int32_t mid_z;

        std::int32_t v_list;
        std::int32_t i_vect;

        std::int32_t overndist;
        std::int32_t num_slides;
        std::int32_t last_slide;

        CollisionVect *p_vect;

        //
        // Move the object to the positon of the hypermatter object.
        //

        {
            std::int32_t mx;
            std::int32_t my;
            std::int32_t mz;
            std::int32_t myaw;
            std::int32_t mpitch;
            std::int32_t mroll;

            HM_find_mesh_pos(dm->Hm,
                             &mx,
                             &my,
                             &mz,
                             &myaw,
                             &mpitch,
                             &mroll);

            myaw &= 2047;
            mpitch &= 2047;
            mroll &= 2047;

            GameCoord new_pos;

            new_pos.X = mx << 8;
            new_pos.Y = my << 8;
            new_pos.Z = mz << 8;

            move_thing_on_map(p_thing, &new_pos);

            dm->Angle = myaw;
            dm->Tilt = mpitch;
            dm->Roll = mroll;
        }

        //
        // Sort out which colvects the hypermatter should collide with.
        //

        HM_colvect_clear(dm->Hm);

        //
        // Look for nearby colvects.
        //

#define FURN_COL_RADIUS (1024)

        x1 = (p_thing->WorldPos.X >> 8) - FURN_COL_RADIUS >> ELE_SHIFT;
        z1 = (p_thing->WorldPos.Z >> 8) - FURN_COL_RADIUS >> ELE_SHIFT;

        x2 = (p_thing->WorldPos.X >> 8) + FURN_COL_RADIUS >> ELE_SHIFT;
        z2 = (p_thing->WorldPos.Z >> 8) + FURN_COL_RADIUS >> ELE_SHIFT;

        SATURATE(x1, 0, MAP_WIDTH - 1);
        SATURATE(x2, 0, MAP_WIDTH - 1);

        SATURATE(z1, 0, MAP_HEIGHT - 1);
        SATURATE(z2, 0, MAP_HEIGHT - 1);

        for (mx = x1; mx <= x2; mx++)
            for (mz = z1; mz <= z2; mz++) {
                v_list = MAP2(mx, mz).ColVectHead;

                while (v_list) {
                    ASSERT(WITHIN(v_list, 0, next_col_vect_link - 1));
                    ASSERT(WITHIN(col_vects_links[v_list].VectIndex, 0, next_col_vect - 1));

                    i_vect = col_vects_links[v_list].VectIndex;
                    p_vect = &col_vects[i_vect];

                    if (p_vect->Y[0] != PAP_calc_height_at(p_vect->X[0], p_vect->Z[0])) {
                        //
                        // Only consider colvects on the ground.
                        //
                    } else {
                        HM_colvect_add(
                            dm->Hm,
                            p_vect->X[0], p_vect->Z[0],
                            p_vect->X[1], p_vect->Z[1]);
                    }

                    v_list = col_vects_links[v_list].Next;
                }
            }

        if (abs(dm->Tilt) < 256) {
            if (HM_stationary(dm->Hm)) {
                MSG_add("Hm is at a standstill");

                HM_destroy(dm->Hm);

                dm->Hm = 255;

                if (p_thing->Genus.Furniture) {
                    p_thing->Genus.Furniture->RAngle = dm->Angle;
                    p_thing->Genus.Furniture->Wheel = 0;
                    p_thing->Genus.Furniture->OverSteer = 0;
                    p_thing->Velocity = 0;

                    set_state_function(p_thing, STATE_FDRIVING);
                }
            }
        }
    }
}

void FURN_process_moveing(Thing *p_thing) {
    ASSERT(WITHIN(p_thing, TO_THING(1), TO_THING(MAX_THINGS - 1)));

    DrawMesh *dm = p_thing->Draw.Mesh;
    Furniture *furn = p_thing->Genus.Furniture;

    //
    // Make sure that this is a furniture thing and everything is valid.
    //

    ASSERT(p_thing->Class == CLASS_FURNITURE);
    ASSERT(WITHIN(dm, TO_DRAW_MESH(0), TO_DRAW_MESH(MAX_DRAW_MESHES - 1)));
    ASSERT(WITHIN(furn, TO_FURNITURE(0), TO_FURNITURE(MAX_FURNITURE - 1)));

    //
    // Move it.
    //

    GameCoord new_pos;

    new_pos.X = p_thing->WorldPos.X + (furn->dx >> 8);
    new_pos.Y = p_thing->WorldPos.Y + (furn->dy >> 8);
    new_pos.Z = p_thing->WorldPos.Z + (furn->dz >> 8);

    //
    // Rotate it.
    //

    dm->Angle += furn->dyaw >> 16;
    dm->Tilt += furn->dpitch >> 16;
    dm->Roll += furn->droll >> 16;

    move_thing_on_map(p_thing, &new_pos);
}

std::int32_t FURN_slide_along(
    THING_INDEX thing,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2,
    std::int32_t radius,
    std::int32_t dont_slide) {
    Thing *p_thing = TO_THING(thing);

    ASSERT(WITHIN(p_thing, TO_THING(1), TO_THING(MAX_THINGS - 1)));
    ASSERT(p_thing->Class == CLASS_FURNITURE || p_thing->Class == CLASS_VEHICLE);

    std::int32_t tx1, tz1;
    std::int32_t tx2, tz2;

    std::int32_t rx1, rz1;
    std::int32_t rx2, rz2;

    std::int32_t y_bot;
    std::int32_t y_top;

    std::int32_t sin_yaw;
    std::int32_t cos_yaw;

    std::int32_t minx;
    std::int32_t minz;
    std::int32_t maxx;
    std::int32_t maxz;

    std::int32_t matrix[4];

    PrimInfo *pi;
    DrawMesh *dm = p_thing->Draw.Mesh;
    Furniture *furn = p_thing->Genus.Furniture;

    x1 >>= 8;
    y1 >>= 8;
    z1 >>= 8;

    *x2 = (*x2) >> 8;
    *y2 = (*y2) >> 8;
    *z2 = (*z2) >> 8;

    //
    // The prim info...
    //

    pi = get_prim_info(dm->ObjectId);

    //
    // Is the 'y' range ok?
    //

#define FURN_UNDERNEATH 32

    y_bot = (p_thing->WorldPos.Y >> 8) + pi->miny - FURN_UNDERNEATH;
    y_top = (p_thing->WorldPos.Y >> 8) + pi->maxy;

    if (WITHIN(y1, y_bot, y_top)) {
        //
        // Rotates the movement vector into the space of the furniture.
        // We assume that the pitch of the furniture is negligible.
        //

        std::int32_t useangle;

        useangle = -dm->Angle;
        useangle &= 2047;

        sin_yaw = SIN(useangle);
        cos_yaw = COS(useangle);

        matrix[0] = cos_yaw;
        matrix[1] = sin_yaw;
        matrix[2] = -sin_yaw;
        matrix[3] = cos_yaw;

        //
        // Rotate the positions.
        //

        tx1 = x1 - (p_thing->WorldPos.X >> 8);
        tz1 = z1 - (p_thing->WorldPos.Z >> 8);

        tx2 = *x2 - (p_thing->WorldPos.X >> 8);
        tz2 = *z2 - (p_thing->WorldPos.Z >> 8);

        rx1 = MUL64(tx1, matrix[0]) + MUL64(tz1, matrix[1]);
        rz1 = MUL64(tx1, matrix[2]) + MUL64(tz1, matrix[3]);

        rx2 = MUL64(tx2, matrix[0]) + MUL64(tz2, matrix[1]);
        rz2 = MUL64(tx2, matrix[2]) + MUL64(tz2, matrix[3]);

        //
        // The bounding box.
        //

        minx = pi->minx - radius;
        minz = pi->minz - radius;

        maxx = pi->maxx + radius;
        maxz = pi->maxz + radius;

        //
        // Do we collide?
        //

        if (rx2 > maxx ||
            rx2 < minx ||
            rz2 > maxz ||
            rz2 < minz) {
            //
            // We don't collide with the bounding box.
            //

            *x2 <<= 8;
            *y2 <<= 8;
            *z2 <<= 8;

            return false; // false => Still moving.
        }

        if (p_thing->State == STATE_FDOOR) {
            //
            // Set the door rotating.
            //

            if (rz1 < 0) {
                furn->dyaw -= 3;
            } else {
                furn->dyaw += 3;
            }
        }

        if (dont_slide) {
            //
            // We aren't allowed to slide, and we can't go into the
            // furniture, so we have to stay still.
            //

            *x2 = x1 << 8;
            *y2 = y1 << 8;
            *z2 = z1 << 8;

            return true; // true => No more movement.
        } else {
            //
            // Slide to the nearest point to the edge of the box.
            //

            std::int32_t dminx;
            std::int32_t dminz;
            std::int32_t dmaxx;
            std::int32_t dmaxz;

            std::int32_t best;
            std::int32_t best_x;
            std::int32_t best_z;

            dminx = rx2 - minx;
            dmaxx = maxx - rx2;

            dminz = rz2 - minz;
            dmaxz = maxz - rz2;

            best = dminx;
            best_x = minx - 1;
            best_z = rz2;

            if (dmaxx < best) {
                best = dmaxx;
                best_x = maxx - 1;
                best_z = rz2;
            }

            if (dminz < best) {
                best = dminz;
                best_x = rx2;
                best_z = minz - 1;
            }

            if (dmaxz < best) {
                best = dmaxz;
                best_x = rx2;
                best_z = maxz + 1;
            }

            //
            // We have to un-rotate the points. The inverse of the
            // matrix is its transpose.
            //

            SWAP(matrix[1], matrix[2]);

            *x2 = MUL64(best_x, matrix[0]) + MUL64(best_z, matrix[1]);
            *z2 = MUL64(best_x, matrix[2]) + MUL64(best_z, matrix[3]);

            *x2 += p_thing->WorldPos.X >> 8;
            *z2 += p_thing->WorldPos.Z >> 8;

            *x2 <<= 8;
            *y2 <<= 8;
            *z2 <<= 8;

            return true; // bumped the car
        }
    }

    *x2 <<= 8;
    *y2 <<= 8;
    *z2 <<= 8;

    return false; // false => Still moving.	 //didnt bump car
}

void FURN_hypermatterise(THING_INDEX thing) {
    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    Thing *p_thing = TO_THING(thing);
    DrawMesh *dm = p_thing->Draw.Mesh;
    Furniture *furn = p_thing->Genus.Furniture;

    HM_Primgrid *hpg = HM_get_primgrid(dm->ObjectId);

    if (furn) {
        dx = -(SIN(furn->RAngle) * p_thing->Velocity) >> 8;
        dz = -(COS(furn->RAngle) * p_thing->Velocity) >> 8;

        dx = (dx * TICK_RATIO) >> (TICK_SHIFT);
        dz = (dz * TICK_RATIO) >> (TICK_SHIFT);

        dx >>= 16;
        dz >>= 16;

        dy = 2;
    } else {
        dx = 0;
        dy = 0;
        dz = 0;
    }

    std::int32_t pointx[5] = {0, 0x5300, 0xa600, 0x10000};
    std::int32_t pointy[5] = {0, 0x5300, 0xa600, 0x10000};
    std::int32_t pointz[5] = {0, 0x5300, 0xa600, 0x10000};

    std::uint8_t hm_index = HM_create(
        dm->ObjectId,
        p_thing->WorldPos.X >> 8,
        p_thing->WorldPos.Y >> 8,
        p_thing->WorldPos.Z >> 8,

        dm->Angle,
        0, // dm->Roll,
        0, // dm->Tilt,

        dx, // Velocity...
        dy,
        dz,

        hpg->x_res,
        hpg->y_res,
        hpg->z_res,
        hpg->x_point,
        hpg->y_point,
        hpg->z_point,

        /*

        4,//hpg->x_res,
        4,//hpg->y_res,
        4,//hpg->z_res,
        pointx,//hpg->x_point,
        pointy,//hpg->y_point,
        pointz,//hpg->z_point,

        */

        hpg->x_dgrav,
        hpg->y_dgrav,
        hpg->z_dgrav,

        0.010F,  // elasticity
        0.950F,  // bounciness
        0.850F,  // friction
        0.997F); // damping

    if (hm_index != HM_NO_MORE_OBJECTS) {
        dm->Hm = hm_index;

        //
        // Stop the car obeying commands and being driven...
        //

        if (furn) {
            furn->Flags &= ~FLAG_FURN_DRIVING;
            furn->Command = 0;
        }

        set_state_function(p_thing, STATE_NORMAL);
    }
}

void FURN_turn_into_door(
    THING_INDEX furniture_thing,
    std::uint16_t closed_angle,
    std::uint16_t ajar,
    std::uint8_t am_i_locked) {
    Thing *p_thing = TO_THING(furniture_thing);
    DrawMesh *dm = p_thing->Draw.Mesh;
    Furniture *furn;

    //
    // Allocate the furniture structure.
    //

    furn = p_thing->Genus.Furniture = FURN_alloc_furniture();

    furn->dyaw = 0;
    furn->closed_angle = closed_angle;
    furn->ajar = ajar;

    if (am_i_locked) {
        p_thing->Flags |= FLAGS_LOCKED;
    } else {
        p_thing->Flags &= ~FLAGS_LOCKED;
    }

    set_state_function(p_thing, STATE_FDOOR);
}

void FURN_door(Thing *p_thing) {
    DrawMesh *dm = p_thing->Draw.Mesh;
    Furniture *furn = p_thing->Genus.Furniture;

    std::int32_t min_angle;
    std::int32_t max_angle;

    dm->Angle += furn->dyaw;
    dm->Angle &= 2047;

    furn->dyaw -= SIGN(furn->dyaw);
}

std::int32_t FURN_avoid(
    THING_INDEX thing,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2) {
    Thing *p_thing = TO_THING(thing);

    std::int32_t x;
    std::int32_t y;
    std::int32_t z;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t da;
    std::int32_t db;

    std::int32_t dist;
    std::int32_t dist1;
    std::int32_t dist2;

    std::int32_t fx;
    std::int32_t fz;

    std::int32_t px1;
    std::int32_t pz1;
    std::int32_t px2;
    std::int32_t pz2;

    std::int32_t furn_radius;

    PrimInfo *pi = get_prim_info(p_thing->Draw.Mesh->ObjectId);

    x1 >>= 8;
    y1 >>= 8;
    z1 >>= 8;

    x2 >>= 8;
    y2 >>= 8;
    z2 >>= 8;

    //
    // Lengthen the movement vector...
    //

    dx = x2 - x1 << 3;
    dy = y2 - y1 << 3;
    dz = z2 - z1 << 3;

    x2 = x1 + dx;
    y2 = y1 + dy;
    z2 = z1 + dz;

#define FURN_AVOID_LOOK_EXTRA 0x40

    if (p_thing->Draw.Mesh->ObjectId < 5 ||
        p_thing->Draw.Mesh->ObjectId == 55) {
        //
        // A lampost.
        //

        furn_radius = 0x40 + FURN_AVOID_LOOK_EXTRA;
    } else {
        furn_radius = pi->radius + FURN_AVOID_LOOK_EXTRA;
    }

    fx = p_thing->WorldPos.X >> 8;
    fz = p_thing->WorldPos.Z >> 8;

    dx = fx - x1;
    dz = fz - z1;

    //
    // Normalise dx,dz to the radius of the furniture.
    //

    dist = QDIST2(abs(dx), abs(dz));

    if (dist == 0) {
        dist = 1;
    }

    dx = dx * furn_radius / dist;
    dz = dz * furn_radius / dist;

    px1 = fx + (-dz);
    pz1 = fz + (+dx);

    px2 = fx - (-dz);
    pz2 = fz - (+dx);

    std::int32_t roomy1 = there_is_a_los(fx, y1, fz, px1, y2, pz1);
    std::int32_t roomy2 = there_is_a_los(fx, y1, fz, px2, y2, pz2);

    if (roomy1 ^ roomy2) {
        //
        // Turn towards the side of the prim we can go around.
        //

        if (roomy1) {
            return -1;
        } else {
            return +1;
        }
    } else {
        //
        // Turn towards the nearest side of the prim to where you are going.
        //

        dx = abs(px1 - x2);
        dz = abs(pz1 - z2);

        dist1 = QDIST2(dx, dz);

        dx = abs(px2 - x2);
        dz = abs(pz2 - z2);

        dist2 = QDIST2(dx, dz);

        if (dist1 < dist2) {
            return -1;
        } else {
            return +1;
        }
    }
}
