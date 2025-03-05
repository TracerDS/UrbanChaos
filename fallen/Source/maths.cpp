//
// Maths stuff...
//

#include "game.h"
#include "maths.h"

//
// Nowhere better to put these functions!
//

void matrix_mult33(struct Matrix33* result, struct Matrix33* mat1, struct Matrix33* mat2) {
    result->M[0][0] = ((mat1->M[0][0] * mat2->M[0][0]) + (mat1->M[0][1] * mat2->M[1][0]) + (mat1->M[0][2] * mat2->M[2][0])) >> 15;
    result->M[0][1] = ((mat1->M[0][0] * mat2->M[0][1]) + (mat1->M[0][1] * mat2->M[1][1]) + (mat1->M[0][2] * mat2->M[2][1])) >> 15;
    result->M[0][2] = ((mat1->M[0][0] * mat2->M[0][2]) + (mat1->M[0][1] * mat2->M[1][2]) + (mat1->M[0][2] * mat2->M[2][2])) >> 15;
    result->M[1][0] = ((mat1->M[1][0] * mat2->M[0][0]) + (mat1->M[1][1] * mat2->M[1][0]) + (mat1->M[1][2] * mat2->M[2][0])) >> 15;
    result->M[1][1] = ((mat1->M[1][0] * mat2->M[0][1]) + (mat1->M[1][1] * mat2->M[1][1]) + (mat1->M[1][2] * mat2->M[2][1])) >> 15;
    result->M[1][2] = ((mat1->M[1][0] * mat2->M[0][2]) + (mat1->M[1][1] * mat2->M[1][2]) + (mat1->M[1][2] * mat2->M[2][2])) >> 15;
    result->M[2][0] = ((mat1->M[2][0] * mat2->M[0][0]) + (mat1->M[2][1] * mat2->M[1][0]) + (mat1->M[2][2] * mat2->M[2][0])) >> 15;
    result->M[2][1] = ((mat1->M[2][0] * mat2->M[0][1]) + (mat1->M[2][1] * mat2->M[1][1]) + (mat1->M[2][2] * mat2->M[2][1])) >> 15;
    result->M[2][2] = ((mat1->M[2][0] * mat2->M[0][2]) + (mat1->M[2][1] * mat2->M[1][2]) + (mat1->M[2][2] * mat2->M[2][2])) >> 15;
}

void rotate_obj(std::int16_t xangle, std::int16_t yangle, std::int16_t zangle, Matrix33* r3) {
    std::int32_t sinx, cosx, siny, cosy, sinz, cosz;
    std::int32_t cxcz, sysz, sxsycz, sxsysz, sysx, cxczsy, sxsz, cxsysz, czsx, cxsy, sycz, cxsz;

    sinx = SIN(xangle & (2048 - 1)) >> 1; // 15's
    cosx = COS(xangle & (2048 - 1)) >> 1;
    siny = SIN(yangle & (2048 - 1)) >> 1;
    cosy = COS(yangle & (2048 - 1)) >> 1;
    sinz = SIN(zangle & (2048 - 1)) >> 1;
    cosz = COS(zangle & (2048 - 1)) >> 1;

    cxsy = (cosx * cosy); // 30's
    sycz = (cosy * cosz);
    cxcz = (cosx * cosz);
    cxsz = (cosx * sinz);
    czsx = (cosz * sinx);
    sysx = (cosy * sinx);
    sysz = (cosy * sinz);
    sxsz = (sinx * sinz);
    sxsysz = (sxsz >> 15) * siny;
    cxczsy = (cxcz >> 15) * siny;
    cxsysz = ((cosx * siny) >> 15) * sinz;
    sxsycz = (czsx >> 15) * siny;

    // Define rotation matrix r3.

    r3->M[0][0] = (sycz) >> 15; // 14's
    r3->M[0][1] = (-sysz) >> 15;
    r3->M[0][2] = siny;
    r3->M[1][0] = (sxsycz + cxsz) >> 15;
    r3->M[1][1] = (cxcz - sxsysz) >> 15;
    r3->M[1][2] = (-sysx) >> 15;
    r3->M[2][0] = (sxsz - cxczsy) >> 15;
    r3->M[2][1] = (cxsysz + czsx) >> 15;
    r3->M[2][2] = (cxsy) >> 15;
}

#ifndef PSX
std::int32_t MATHS_seg_intersect(
    std::int32_t vx1, std::int32_t vz1, std::int32_t vx2, std::int32_t vz2,
    std::int32_t wx1, std::int32_t wz1, std::int32_t wx2, std::int32_t wz2) {
    //
    // Work out all points relative to (vx1, vx2). This gives three
    // vectors, a, b, c where
    //
    //		a is v1 to v2
    //		b is v1 to w1
    //		c is v1 to w2
    //
    // Also define anther vector d from w1 to w2.
    //
    // The point P that intersects the two lines can be given
    // parametrically in t or q as
    //
    //		P = ta
    //		P = b + qd
    //
    // Solving the two equations in x and z, we get...
    //
    //		t = |d x b|  /  |d x a|
    //      q = |a x b|  /  |d x a|
    //
    // The line segments only intersect if both t and q are in the
    // range 0 to 1.  This is inclusive in our interpretation.  We
    // don't have to do the divide to establish whether or not t and
    // q are in those ranges.
    //
    //
    // p.s. We don't have to work out vector c...
    //

    std::int32_t ax, az;
    std::int32_t bx, bz;
    std::int32_t dx, dz;

    std::int32_t acrossb;
    std::int32_t dcrossa;
    std::int32_t dcrossb;

    //
    // Do a bounding box check in x.
    //

    if (vx1 < vx2) {
        if (wx1 < vx1 && wx2 < vx1) {
            return false;
        }
        if (wx1 > vx2 && wx2 > vx2) {
            return false;
        }
    } else {
        if (wx1 > vx1 && wx2 > vx1) {
            return false;
        }
        if (wx1 < vx2 && wx2 < vx2) {
            return false;
        }
    }

    //
    // Do a bounding box check in z.
    //

    if (vz1 < vz2) {
        if (wz1 < vz1 && wz2 < vz1) {
            return false;
        }
        if (wz1 > vz2 && wz2 > vz2) {
            return false;
        }
    } else {
        if (wz1 > vz1 && wz2 > vz1) {
            return false;
        }
        if (wz1 < vz2 && wz2 < vz2) {
            return false;
        }
    }

    //
    // Work out the vectors a, b and d.
    //

    ax = vx2 - vx1;
    az = vz2 - vz1;

    bx = wx1 - vx1;
    bz = wz1 - vz1;

    dx = wx2 - wx1;
    dz = wz2 - wz1;

#define MATH_DIFFERENT_SIGN(c1, c2) (((c1) & 0x80000000) ^ ((c2) & 0x80000000))

    //
    // Check that t is in range. We have to work out
    // dcrossa and dcrossb.  t = dcrossb / dcrossa
    //

    dcrossa = dx * az - dz * ax;
    dcrossb = dx * bz - dz * bx;

    if (dcrossa == 0) {
        //
        // The line segments are paralell.
        //
    }

    if (MATH_DIFFERENT_SIGN(dcrossb, dcrossa)) {
        //
        // t is negative, so the line segments don't cross.
        //

        return false;
    }

    if (abs(dcrossb) > abs(dcrossa)) {
        //
        // t = dcrossb / dcrossa and t > 0 so t must be more than one.
        //

        return false;
    }

    //
    // Do the same checks for q. q = acrossb / dcrossa
    //

    acrossb = ax * bz - az * bx;

    if (MATH_DIFFERENT_SIGN(acrossb, dcrossa)) {
        //
        // q is negative so the line segments don't cross.
        //

        return false;
    }

    if (abs(acrossb) > abs(dcrossa)) {
        //
        // q is more than one.
        //

        return false;
    }

    return true;

    //
    // What about if dcrossb and/or acrossb are zero?
    //
}
#endif
