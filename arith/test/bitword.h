/*
 *     bitword.h
 *     Joshua Bernstein (jberns02)
 *     Sam Hu (hku04)
 *
 *     HW4: arith
 * 
 *     Summary
 *      contains functions used by compress40, which specifically handles
 *      the conversion from packed CV arrays to coded words and finally the
 *      compressed file format and back.
 *
 * 
 *      
 */
#ifndef BITWORD_INCLUDED
#define BITWORD_INCLUDED


/*  -----WordVal-----
  contains the values of the codewords,
  prior to its compression
  --------------------*/
typedef struct wordVal{ 
        float a;
        float b;
        float c;
        float d;
        unsigned avg_Pb;
        unsigned avg_Pr;
} *wordVal;

void apply_calc_Word_Values(int row, int col, A2Methods_UArray2 array,
                            A2Methods_Object *object, void *cl);

void trimBCD(float *num);

void apply_bitpack(int row, int col, A2Methods_UArray2 array,
                   A2Methods_Object *object, void *cl);

void apply_print(A2Methods_Object *object, void *cl);

void apply_read(A2Methods_Object *object, void *cl);

void apply_bitUnpack(int row, int col, A2Methods_UArray2 array,
                     A2Methods_Object *object, void *cl);

void apply_calc_CV_Values(int row, int col, A2Methods_UArray2 array, 
                          A2Methods_Object *object, void *cl);




#endif