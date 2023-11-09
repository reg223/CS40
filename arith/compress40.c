/*
 *     compress40.c
 *     Joshua Bernstein (jberns02)
 *     Sam Hu (hku04)
 *
 *     HW4: arith
 * 
 *     Summary
 *
 *     This file is the source code of compress40, the implementation for 
 *     40image, designated to compress or decompress a given valid ppm down to 
 *     approximately a third of its original size.
 *     
 * 
 *      Function hierarchy
 *      
 *      Compress40
 *          Trim
 *              new_blank
 *              apply_copy
 *          apply_cv
 *              convertToCV
 *          apply_pack
 *              make_pack
 *          apply_free_pack
 * 
 *          apply_calc_Word_Values
 *              Arith40_index_of_chroma
 *              trimBCD
 *          apply_bitpack
 *              Bitpack_newu
 *              Bitpack_news
 *          apply_print
 *          
 *
 *       Decompress40
 *          apply_read
 *          apply_bitUnpack
 *              Bitpack_getu
 *              Bitpack_gets
 *          apply_calc_CV_Values
 *               makepack
 *               Arith40_chroma_of_index
 *
 *          apply_unpack
 *          apply_free_pack
 *          apply_rgb
 *              convertToRGB
 *                  cap
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include "compress40.h"
#include <bitpack.h>
#include "a2methods.h"
#include "a2plain.h"
#include "arith40.h"
#include <uarray.h>
#include "pnm.h"
#include "cvpack.h"
#include "bitword.h"




   


#define methods uarray2_methods_plain
#define map methods->map_default
#define smap methods->small_map_col_major



/**************** Compress40 ****************
 *
 * compress the given ppm image into a third of its size,
 * print compressed result to stdout
 *
 * Parameters:
 *      File *input: file pointer pointed to the start of the Pnm_ppm object
 *
 * Return: N/A (void)
 *
 * Expects: input to not be null,
 * Notes:
 *      CRE if file is has 0 as dimension or input is NULL
 *      Exits early if either dimension is 1 (impossible to compress)
 *********************************************/
extern void compress40(FILE *input){

        /*************************************************
        *                   read input                   *
        *************************************************/
        /* Read in the image into pnm_ppm */
        Pnm_ppm image = trim(Pnm_ppmread(input, methods));
        unsigned width = image->width;
        unsigned height = image->height;
        

        /*************************************************
        *                 Convert to CV                  *
        *************************************************/
       /* Create a component video map */
        A2Methods_UArray2 cv_map = methods->new(width,height, 
                                   sizeof(struct cv_pixel));
                                   
        /* Applying each pixel cv values */
        map(cv_map, apply_cv, image);
        Pnm_ppmfree(&image);

        /*************************************************
        *                   Packing CV                   *
        *************************************************/
        
        A2Methods_UArray2 cv_pack = methods->new(width/2,height/2, 
                                    sizeof(struct packedCV));
        map(cv_map, apply_pack, cv_pack);
        methods->free(&cv_map);

        /*************************************************
        *                 Calculate Word                 *
        *************************************************/
        A2Methods_UArray2 word_val = methods->new(width/2,height/2, 
                                     sizeof(struct wordVal));
        map(word_val, apply_calc_Word_Values, cv_pack);
        smap(cv_pack, apply_free_pack, NULL);
        methods->free(&cv_pack);
        /*************************************************
        *                    Bitpack                     *
        *************************************************/
        A2Methods_UArray2 wordseq = methods->new(width/2,height/2, 
                                    sizeof(struct wordVal));
        map(word_val, apply_bitpack, wordseq);
        methods->free(&word_val);
        /*************************************************
        *                     Output                     *
        *************************************************/
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        smap(wordseq,apply_print,NULL);
        methods->free(&wordseq);

        
}

/**************** decompress40 ****************
 *
 * compress the given ppm image into a third of its size,
 * print compressed result to stdout
 *
 * Parameters:
 *      File *input: file pointer pointed to the start of the Pnm_ppm object
 *
 * Return: N/A (void)
 *
 * Expects: input to not be null,
 * Notes:
 *      CRE if file is has 0 as dimension or input is NULL
 *      Exits early if either dimension is 1 (impossible to compress)
 *********************************************/
extern void decompress40(FILE *input) 
{
        /*************************************************
        *                   Read Input                   *
        *************************************************/
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                          &width, &height); 
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        A2Methods_UArray2 wordseq = methods->new(width/2,height/2, 
                                       sizeof(uint32_t));
        smap(wordseq, apply_read, input);
        /*************************************************
        *                   Bit Unpack                   *
        *************************************************/

        A2Methods_UArray2 wordUnpack = methods->new(width/2,height/2,
                                       sizeof(struct wordVal));
        map(wordUnpack, apply_bitUnpack, wordseq);
        methods->free(&wordseq);
        /*************************************************
        *         Calculate CV values from Word          *
        *************************************************/
        A2Methods_UArray2 newcv_pack = methods->new(width/2,height/2, 
                                       sizeof(struct packedCV));
        map(wordUnpack, apply_calc_CV_Values, newcv_pack);
        methods->free(&wordUnpack);
        /*************************************************
        *                   Unpack CV                    *
        *************************************************/

        A2Methods_UArray2 newcv_map = methods->new(width,height, 
                                      sizeof(struct cv_pixel));
        map(newcv_map, apply_unpack, newcv_pack);
        smap(newcv_pack, apply_free_pack, NULL);
        methods->free(&newcv_pack);
        /*************************************************
        *                 Create RGB map                 *
        *************************************************/

        A2Methods_UArray2 rgb_map = methods->new(width,height, 
                                    sizeof(struct Pnm_rgb));
        map(rgb_map, apply_rgb, newcv_map);
        methods->free(&newcv_map);
        /*************************************************
        *                     Output                     *
        *************************************************/
        Pnm_ppm new_image = new_blank(width,height,methods,NULL);
        new_image->pixels = rgb_map;
        Pnm_ppmwrite(stdout, new_image);
        Pnm_ppmfree(&new_image);
}

