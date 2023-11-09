/*
 *     bitword.c
 *     Joshua Bernstein (jberns02)
 *     Sam Hu (hku04)
 *
 *     HW4: arith
 * 
 *     Summary
 *     Implementation of bitword.h.
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "compress40.h"
#include <bitpack.h>
#include "a2methods.h"
#include "a2plain.h"
#include "arith40.h"
#include "bitword.h"
#include "cvpack.h"
#include "pnm.h"

#define methods uarray2_methods_plain
#define map methods->map_default

const unsigned A_L = 6, BCD_L = 6, A_MUL = 63, BCD_MUL = 103, CHR_L = 4;  /* bit length of indices */
const float TRIMSIZE = 0.3; 

/********** apply_calcWord_Values ********
 *
 * apply function that calculate word values from cv values and fill them into
 * the new array
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: empty array of word values to be populated
 *      A2Methods_Object *object: Element at current pos (a packedCV struct)
 *      void *cl:CVpack 2D array to read values from
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 ************************/
void apply_calc_Word_Values(int row, int col, A2Methods_UArray2 array,
                            A2Methods_Object *object, void *cl) 
{
        assert((cl != NULL) && (object != NULL));
        (void) array;
        A2Methods_UArray2 cv_pack = cl;
        wordVal curWord = object;
        packedCV curPack = methods->at(cv_pack,row,col);
        
        
        /* Computing values a,b,c,d, trim bcd as needed  */
        curWord->a = (curPack->UL->Y + curPack->UR->Y + curPack->LL->Y +
                           curPack->LR->Y)/4.0;
        curWord->b = (-curPack->UL->Y - curPack->UR->Y + curPack->LL->Y +
                           curPack->LR->Y)/4.0;
        trimBCD(&(curWord->b)); 
        curWord->c = (-curPack->UL->Y + curPack->UR->Y - curPack->LL->Y +
                           curPack->LR->Y)/4.0;
        trimBCD(&(curWord->c)); 
        curWord->d = (curPack->UL->Y - curPack->UR->Y - curPack->LL->Y +
                           curPack->LR->Y)/4.0;
        trimBCD(&(curWord->d)); 
        
        /* Calculating average PB and average Pr values */
        float avg_Pb = (curPack->UL->Pb + curPack->UR->Pb + curPack->LL->Pb +
                           curPack->LR->Pb)/4.0;
        float avg_Pr = (curPack->UL->Pr + curPack->UR->Pr + curPack->LL->Pr +
                           curPack->LR->Pr)/4.0;

        /* Updating currword's average Pb and Pr values */
        curWord->avg_Pb = Arith40_index_of_chroma(avg_Pb);
        curWord->avg_Pr = Arith40_index_of_chroma(avg_Pr);
} 


/********** trimBCD ********
 *
 * A function which trims b, c, and d to be within +.3 and -.3
 *
 * Parameters:
 *     A float value which will be trimmed
 *
 * Return: The trimmed float value
 *
 * Expects
 *      num should be a float 
 * Notes:
 *      malfunctions if non-float point numbers passed in
 ************************/
void trimBCD(float *num)
{
        if(*num > TRIMSIZE) {
                *num = TRIMSIZE;
        }
        if(*num < -TRIMSIZE) {
                *num = -TRIMSIZE;
        }
}

/********** apply_bitpack ********
 *
 * apply function that packs the wordVal values into the word 
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: array of word values, unused
 *      A2Methods_Object *object: Element at current pos (a wordVal struct)
 *      void *cl:coded word 2D array to be populated
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 ************************/
void apply_bitpack(int row, int col, A2Methods_UArray2 array,
                   A2Methods_Object *object, void *cl)
{
        assert(cl != NULL && object != NULL);
        (void) array;
        int  b, c, d;
        unsigned a, pb, pr;

        wordVal val = object;
        A2Methods_UArray2 wordarr = cl;
        uint32_t *word = methods->at(wordarr, row, col);
        
        *word = 0;

        /* Shift a, b, c, d as needed and cast as ints */
        a = (unsigned)(val->a * A_MUL);
        b = (int)(val->b * BCD_MUL);
        c = (int)(val->c * BCD_MUL);
        d = (int)(val->d * BCD_MUL);
        pb = val->avg_Pb; 
        pr = val->avg_Pr; 
        
        /* Calling bitpack.c methods on {a,b,c,d,pb,pr} to pack them */
        *word = Bitpack_newu(*word, A_L,  3 * BCD_L + 2 * CHR_L, a);
        *word = Bitpack_news(*word, BCD_L, 2 * BCD_L + 2 * CHR_L, b);
        *word = Bitpack_news(*word, BCD_L, BCD_L + 2 * CHR_L, c);
        *word = Bitpack_news(*word, BCD_L, 2 * CHR_L, d);
        *word = Bitpack_newu(*word, CHR_L, CHR_L, pb);
        *word = Bitpack_newu(*word, CHR_L, 0, pr);
         
}

/**************** apply_print ****************
 *
 * Apply function that prints an object to the standard output.
 *
 * Parameters:
 *      A2Methods_Object *object - The object to be printed.
 *      void *cl - An unused context pointer.
 *
 * Return: N/A (void)
 *
 * Notes:
 *      - This function prints the given object to the standard output
          (stdout) using fwrite.
 *********************************************/
void apply_print(A2Methods_Object *object, void *cl)
{
        (void)cl;
        uint32_t word = *(uint32_t*)(object);
        for (int w = 24; w >= 0; w -= 8) {
                        unsigned char byte = (word >> w);
                        putchar(byte);
        }
}

/**************** apply_read ****************
 *
 * Apply function that reads an object from a stream and stores it in 'object'
 *
 * Parameters:
 *      A2Methods_Object *object - The object where data will be
                                   read and stored.
 *      void *cl - A pointer to the input stream from which to read data.
 *
 * Return: N/A (void)
 *
 * Notes:
 *      This function reads data from the input stream pointed to by 'cl'
 *      and stores it in 'object'.
 *      The function asserts that the 'cl' pointer is not NULL before reading
 *********************************************/
void apply_read(A2Methods_Object *object, void *cl) 
{
        assert(cl != NULL);
        uint64_t temp_word = 0;
        uint64_t byte;

        /*Bytes are written to the disk in big-endian order.*/
        for (int w = 24; w >= 0; w -= 8) {
                byte = getc(cl);
                byte = byte << w;
                temp_word = temp_word | byte;
        }
        *(uint32_t*)object = temp_word;
}

/********** apply_bitUnpack ********
 *
 * apply function that unpacks the wordVal values from the word 
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: empty array of word values, unused
 *      A2Methods_Object *object: empty element at current pos (wordVal struct)
 *      void *cl:coded word 2D array 
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 ************************/
void apply_bitUnpack(int row, int col, A2Methods_UArray2 array,
                     A2Methods_Object *object, void *cl)
{
        assert(cl != NULL);
        (void) array;

        wordVal val = object;
        A2Methods_UArray2 wordarr = cl;
        uint32_t *word = methods->at(wordarr, row, col);
        
        /* Compute a,b,c,d,pb,pr using bitpack methods (the get methods) */
        val->a = (float)(Bitpack_getu(*word, A_L, 3 * BCD_L + 2 * CHR_L))
                                                        / A_MUL;
        val->b = (float)(Bitpack_gets(*word, BCD_L, 2 * BCD_L + 2 * CHR_L))
                                                        / BCD_MUL;
        val->c = (float)(Bitpack_gets(*word, BCD_L, BCD_L + 2 * CHR_L)) 
                                                        / BCD_MUL;
        val->d = (float)(Bitpack_gets(*word, BCD_L, 2 * CHR_L)) 
                                                        / BCD_MUL;
        val->avg_Pb = (unsigned)Bitpack_getu(*word, CHR_L, CHR_L);
        val->avg_Pr = (unsigned)Bitpack_getu(*word, CHR_L, 0);
}

/********** apply_calc_CV_Values ********
 *
 * apply function that calculate cv values from word values and 
 * fill them into the new array
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: array of word values
 *      A2Methods_Object *object: Element at current pos (a wordVal struct)
 *      void *cl:empty CVpack 2D array to be populated
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 ************************/
void apply_calc_CV_Values(int row, int col, A2Methods_UArray2 array, 
                          A2Methods_Object *object, void *cl) 
{
        assert((cl != NULL) && (object != NULL));
        (void) array;
        A2Methods_UArray2 cv_pack = cl;
        wordVal curWord = object;
        packedCV curPack = methods->at(cv_pack,row,col);
        
        /* Malloc the four corners of a our box to store each pixel */
        make_pack(curPack);
        
        /* Calculating the Y value for each corner of the pixel box */
        curPack->UL->Y = curWord->a - curWord->b - curWord->c + curWord->d;
        curPack->UR->Y = curWord->a - curWord->b + curWord->c - curWord->d;
        curPack->LL->Y = curWord->a + curWord->b - curWord->c - curWord->d;
        curPack->LR->Y = curWord->a + curWord->b + curWord->c + curWord->d;

        /* Computer avg PB, avg Pr */
        float avgPb = Arith40_chroma_of_index(curWord->avg_Pb);
        float avgPr = Arith40_chroma_of_index(curWord->avg_Pr);

        /* Updating each pixels Pb */
        curPack->UL->Pb = avgPb;
        curPack->UR->Pb = avgPb;
        curPack->LL->Pb = avgPb;
        curPack->LR->Pb = avgPb;

        /* Updating each pixels Pr */
        curPack->UL->Pr = avgPr;
        curPack->UR->Pr = avgPr;
        curPack->LL->Pr = avgPr;
        curPack->LR->Pr = avgPr;
} 