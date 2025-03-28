#include <MFStdLib.h>
#include <ddlib.h>
#include "poly.h"
#include "qeng.h"
#include "vertexbuffer.h"

//
// Once at the start of the whole program.
//

void QENG_init() {
    POLY_init();
}

std::int32_t QENG_cam_x;
std::int32_t QENG_cam_y;
std::int32_t QENG_cam_z;

//
// Where everything is drawn from.
//

void QENG_set_camera(
    float x,
    float y,
    float z,
    float yaw,
    float pitch,
    float roll) {
    QENG_cam_x = std::int32_t(x);
    QENG_cam_y = std::int32_t(y);
    QENG_cam_z = std::int32_t(z);

    POLY_camera_set(x, y, z, yaw, pitch, roll, 128.0F * 256.0F, 8.0F);
    POLY_frame_init(false, false);
}

//
// Draws a line in the world. Sets QENG_mouse_over and QENG_mouse_pos if the
// mouse is over the line.
//

std::int32_t QENG_mouse_over;
float QENG_mouse_pos_x; // Position in the world
float QENG_mouse_pos_y;
float QENG_mouse_pos_z;

void QENG_world_line(
    std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t width1, std::uint32_t colour1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t width2, std::uint32_t colour2,
    bool sort_to_front) {
    POLY_Point pp1;
    POLY_Point pp2;

    QENG_mouse_over = false;

    POLY_transform(
        float(x1),
        float(y1),
        float(z1),
        &pp1);

    if (pp1.IsValid()) {
        POLY_transform(
            float(x2),
            float(y2),
            float(z2),
            &pp2);

        if (pp2.IsValid()) {
            if (POLY_valid_line(&pp1, &pp2)) {
                pp1.colour = colour1;
                pp2.colour = colour2;

                pp1.specular = 0x00000000;
                pp2.specular = 0x00000000;

                pp1.u = 0.0F;
                pp1.v = 0.0F;
                pp2.u = 0.0F;
                pp2.v = 0.0F;

                POLY_add_line(&pp1, &pp2, float(width1), float(width2), POLY_PAGE_COLOUR, sort_to_front);

                //
                // Is the mouse over the line?
                //
            }
        }
    }
}

void QENG_draw(QMAP_Draw *qd) {
    std::int32_t i;

    std::uint16_t point;
    std::uint16_t face;

    std::int32_t mx;
    std::int32_t mz;

    QMAP_Point *qp;
    QMAP_Face *qf;

    POLY_Point *pp;
    POLY_Point *quad[4];

    ASSERT(QMAP_MAX_POINTS <= POLY_BUFFER_SIZE);

    //
    // The origin of the points.
    //

    mx = qd->map_x << 13;
    mz = qd->map_z << 13;

    //
    // The counter for which points have already been transformed.
    //

    qd->trans += 1;

    if (qd->trans == 0) {
        qd->trans = 1;

        //
        // Clear the trans field in all this squares points.
        //

        for (point = qd->next_point; point; point = qp->next) {
            ASSERT(WITHIN(point, 1, QMAP_MAX_POINTS - 1));

            qp = &QMAP_point[point];

            qp->trans = 0;
        }
    }

    //
    // Create all the faces.
    //

    for (face = qd->next_face; face; face = qf->next) {
        ASSERT(WITHIN(face, 1, QMAP_MAX_FACES - 1));

        qf = &QMAP_face[face];

        //
        // Reject the face because of its normal?
        //

        //
        // Make sure all the points are transformed.
        //

        for (i = 0; i < 4; i++) {
            ASSERT(WITHIN(qf->point[i], 1, QMAP_MAX_POINTS - 1));

            qp = &QMAP_point[qf->point[i]];
            pp = &POLY_buffer[qf->point[i]];

            if (qp->trans == qd->trans) {
                //
                // This point is already transformed.
                //
            } else {
                //
                // Transform the point.
                //

                POLY_transform(
                    float(mx + qp->x),
                    float(qp->y),
                    float(mz + qp->z),
                    pp);

                qp->trans = qd->trans;
            }

            if (!pp->MaybeValid()) {
                goto abandon_this_face;
            }

            pp->colour = qf->texture * 111;
            pp->specular = 0x00000000;
            pp->u = 0.0F;
            pp->v = 0.0F;

            quad[i] = pp;
        }

        if (POLY_valid_quad(quad)) {
            POLY_add_quad(quad, POLY_PAGE_COLOUR, false);
        }

    abandon_this_face:;
    }
}

void QENG_render() {
    POLY_frame_draw(true, true);
}

void QENG_flip() {
    FLIP(nullptr, DDFLIP_WAIT);
}

void QENG_clear_screen() {
    SET_BLACK_BACKGROUND;
    CLEAR_VIEWPORT;
    TheVPool->ReclaimBuffers();
}

void QENG_fini() {
}
