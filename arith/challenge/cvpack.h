/*
 *     cvpack.h
 *     Joshua Bernstein (jberns02)
 *     Sam Hu (hku04)
 *
 *     HW4: arith
 * 
 *     Summary
 *     This file is part of the implementation for compress 40.
 *     This file helps our overall program by 
 *     performing image processing operations on PPM (Portable Pixmap) images.
 *     It includes various functions to manipulate and transform these images. 
 *     The program focuses on image processing operations, particularly 
 *     converting between RGB and CV color spaces, trimming images, 
 *     and working with pixel blocks. 
 */

#ifndef CVPACK_INCLUDED
#define CVPACK_INCLUDED

#include "pnm.h"
Pnm_ppm trim(Pnm_ppm image);


/*  -----CV Pixel-----
  contains three floats storing
  Y, Pb, Pr values respectively
  --------------------*/
typedef struct cv_pixel { 
        float Y;
        float Pb;
        float Pr;
} *cv_pixel;

/*  -----PackedCV-----
  A struct containing 4 cv_pixel structs,
        U/L refers to upper/lower
        L/R refers to left/right
  --------------------*/
typedef struct packedCV {
        cv_pixel UL;
        cv_pixel UR;
        cv_pixel LL;
        cv_pixel LR;
} *packedCV;

Pnm_ppm new_blank(int width, int height, const A2Methods_T method,
                  void *image);

void apply_copy(int row, int col, A2Methods_UArray2 array,
                A2Methods_Object *object, void *cl);

void apply_cv(int row, int col, A2Methods_UArray2 array, A2Methods_Object 
              *object, void *cl);

void convertToCV(Pnm_rgb rgb, cv_pixel pixel, unsigned denom);

void apply_pack(int row, int col, A2Methods_UArray2 array, A2Methods_Object 
                *object, void *cl);
                
void make_pack(packedCV pack);

void apply_free_pack(A2Methods_Object *object, void *cl);

void apply_unpack(int row, int col, A2Methods_UArray2 array,
                  A2Methods_Object *object, void *cl);

void apply_free_pack(A2Methods_Object *object, void *cl); 

void apply_rgb(int row, int col, A2Methods_UArray2 array, A2Methods_Object 
               *object, void *cl);

void convertToRGB(Pnm_rgb rgb, cv_pixel pixel);

float cap(float num);

#endif