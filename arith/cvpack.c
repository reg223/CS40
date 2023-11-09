/*
 *     cvpack.c
 *     Joshua Bernstein (jberns02)
 *     Sam Hu (hku04)
 *
 *     HW4: arith
 * 
 *     Summary
 *     Implementation of cvpack.h
 *        
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <bitpack.h>
#include "a2methods.h"
#include "a2plain.h"
#include <uarray.h>
#include "pnm.h"
#include "cvpack.h"


#define methods uarray2_methods_plain
#define map methods->map_default
const unsigned DENOM = 255;


/********** trim ********
 *
 * Trim an image which is not of even dimensions. Exit early if image 
 * has width or height of 1.
 *
 * Parameters:
 *      Pnm_ppm image - An image passed in from the command line via the user
 *
 * Return: A new, trimmed (if necessary) image, or EXIT_SUCCESS
 *
 * Expects
 *      Image must not be NULL
 * Notes:
 *      Exits early if width or height is less than 1
 *      CRE if image is NULL or 
 ************************/
Pnm_ppm trim(Pnm_ppm image)
{
        assert(image != NULL);
        int width = image->width;
        int height = image->height;
        
        /* Exit early if with or height is equal to 1 */
        if (width <= 1|| height <= 1) {
                /* IS THIS EXIT_SUCCESS OR EXIT EARLY */
                exit(EXIT_SUCCESS);
        }
        /* If height or width is not even, trim the image */
        if (height % 2 != 0) {
                height -=1;
        }
        if (width % 2 != 0) {
                width -=1;
        }
        /* Create a trimmed image */ 
        Pnm_ppm newImage = new_blank(width, height, methods, 
                                    (void *)image);
        map(newImage->pixels,apply_copy,image);
        Pnm_ppmfree(&image);
        return newImage;
}

/********** new_blank ********
 *
 * Create and return a new Pnm_ppm instance with an empty A2Methods_UArray2
 * Uses original denominator if provided an image
 * 
 *
 * Parameters:
 *      int width: The width of the new pixmap
 *      int height: The height of the new pixmap
 *      const A2Methods_T method: method for new Pnm_ppm
 *      Pnm_ppm image: The orginal image 
 *
 * Return: A new Pnm_ppm instance with empty pixmap
 *
 * Expects: 
 *      method to be not null
 *      
 * Notes:
 *      Will CRE if malloc fails or method to be null
 ************************/
Pnm_ppm new_blank(int width, int height, const A2Methods_T method,
                  void *image)
{
        assert(method != NULL);
        A2Methods_UArray2 new_pixmap;
        Pnm_ppm new_image = malloc(sizeof(*new_image));
        assert(new_image != NULL);

        /* If image is not NULL, use original denominator instead of 255 */
        
        if (image != NULL) {
                new_pixmap = method->new(width, height, sizeof(struct Pnm_rgb));
                Pnm_ppm og_image = image;
                *new_image = (struct Pnm_ppm) {width, height, 
                              og_image->denominator, new_pixmap, method};
        } else {      
                             
                *new_image = (struct Pnm_ppm) {width, height, 
                DENOM, NULL, method};
        }
        return new_image;

}

/********** apply_copy ********
 *
 * apply function that copy the pixels from the original image to the 
 * new image
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: New pixmap, unused
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The original image as Pnm_ppm 
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 ************************/
void apply_copy(int row, int col, A2Methods_UArray2 array,
                A2Methods_Object *object, void *cl) 
{
        assert(cl != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm og_img = cl;
        *(Pnm_rgb)object = *(Pnm_rgb)(methods->at(og_img->pixels, row, col));

}

/********** apply_CV ********
 *
 * apply function that converts rgb pixmap into cv_pixel pixmap
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: array of cv pixels, unused
 *      A2Methods_Object *object: Element at current pos (a cv_pixel struct)
 *      void *cl:pixmap containing Pnm_rgb structs
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 *      Will CRE if malloc fails
 ************************/
void apply_cv(int row, int col, A2Methods_UArray2 array, A2Methods_Object *object, 
              void *cl) 
{
        assert(cl != NULL && object != NULL);
        (void)array;
        Pnm_ppm og_img = cl;

        cv_pixel newpix = malloc(sizeof(*newpix));
        assert(newpix != NULL);
        convertToCV(methods->at(og_img->pixels,row,col), newpix,
                    og_img->denominator);
        *(cv_pixel)object = *newpix;

        /* Free newpix */
        free(newpix);
}

/********** convertToCV ********
 *
 * Calculate CV values using RGB from the pnm object and given formulas
 *
 * Parameters:
 *      The rgb values form Pnm_rgb
 *      cv_pixel struct 
 *      denom : denominator of image
 *
 * Return: void
 *
 * Expects
 *      rgb and pixel not to be NULL
 * Notes:
 *      CRE if rgb or pixel is NULL
 ************************/
void convertToCV(Pnm_rgb rgb, cv_pixel pixel, unsigned denom)
{
        assert(rgb != NULL && pixel != NULL);
        /* Cast each RGB value to a float */
        float reddy = (float)rgb->red;
        reddy /= (float) denom;
        float greeny = (float)rgb->green;
        greeny /= (float) denom;
        float bluey = (float)rgb->blue;
        bluey /= (float) denom;
        
        /* Set each Y,Pb,Pr value equal to their respective equation */
        pixel->Y = (0.299 * reddy) + (0.587 * greeny) + (0.114 * 
                             bluey);
        pixel->Pb =  - (0.168736 * reddy) - (0.331264 * greeny) + (0.5  
                             * bluey);
        pixel->Pr = (0.5 * reddy) - (0.418688 * greeny) - (0.081312 * 
                             bluey);
}



/********** apply_pack ********
 *
 * apply function that pack cv pixels into cv pack
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: image array to read cv_pixel
 *      A2Methods_Object *object: Element at current pos (a cv_pixel struct)
 *      void *cl:empty CVpack 2D array to be populated
 *
 * Return: N/A
 *
 * Expects
 *      cl is not to be NULL
 * Notes:
 *      Will CRE if cl is NULL
 ************************/
void apply_pack(int row, int col, A2Methods_UArray2 array, A2Methods_Object *object,
                void *cl) 
{
        assert(cl != NULL);
        (void) object;
        if((row % 2 ==0) && (col % 2 ==0)){  /*only fill in if at top left*/
                A2Methods_UArray2 cv_pack = cl;
                struct packedCV pack;
                /* Malloc the four corners of a our box to store each pixel */
                make_pack(&pack);
                
                /* fill in the pixels*/
                *pack.UL = *(cv_pixel)methods->at(array, row, col);
                *pack.UR = *(cv_pixel)methods->at(array, row+1, col);
                *pack.LL = *(cv_pixel)methods->at(array, row, col+1);
                *pack.LR = *(cv_pixel)methods->at(array, row+1, col+1);
                
                /*assign to the position on pack*/
                packedCV newpack =  methods->at(cv_pack, row / 2, col / 2);
                *newpack = pack;
        }
}

/********** make_pack ********
 *
 * helper function that allocates space for packedCV structs
 *
 * Parameters:
 *      A2Methods_Object *object: Element at current pos (a cv_pixel struct)
 *      void *cl: empty, unused
 *
 * Return: N/A
 *
 * Expects
 *      cv_pixels not allocated already
 * Notes:
 *      Will CRE if malloc fails
 ************************/
void make_pack(packedCV pack) 
{
        pack->UL = malloc(sizeof(struct cv_pixel));
        assert(pack->UL != NULL);
        pack->UR = malloc(sizeof(struct cv_pixel));
        assert(pack->UR != NULL);
        pack->LL = malloc(sizeof(struct cv_pixel));
        assert(pack->LL != NULL);
        pack->LR = malloc(sizeof(struct cv_pixel));
        assert(pack->LR != NULL);
}

/********** apply_free_pack ********
 *
 * small apply function that frees pixels in cv pack, as uarray only frees the
 * cv pack
 *
 * Parameters:
 *      A2Methods_Object *object: Element at current pos (a cv_pixel struct)
 *      void *cl: empty, unused
 *
 * Return: N/A
 *
 * Expects
 *      cv_pixels not freed already
 * Notes:
 *      Will CRE if cv_pixels were NULL before freeing
 ************************/
void apply_free_pack(A2Methods_Object *object, void *cl) 
{
        
        (void) cl;
        free(((packedCV)object)->UL);
        free(((packedCV)object)->UR);
        free(((packedCV)object)->LL);
        free(((packedCV)object)->LR);

}

/********** apply_unpack ********
 *
 * apply function that unpack the cv pixel blocks and put into a new array
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: empty image array to be populated
 *      A2Methods_Object *object: Element at current pos (a cv_pixel struct)
 *      void *cl:CVpack 2D array to read values from
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 ************************/
void apply_unpack(int row, int col, A2Methods_UArray2 array, A2Methods_Object         
                  *object, void *cl) 
{
        assert((cl != NULL) && (object != NULL));
        (void) array;
        A2Methods_UArray2 cv_pack = cl;
        packedCV pack = methods->at(cv_pack, row/2,col/2);
        cv_pixel pix = object;
        
        /*read the pixel from the corresponding position*/
        if ((row%2 == 0) && (col%2 == 0)) {
                *pix = *(*pack).UL;
        } else if ((row%2 == 1) && (col%2 == 0)){
                *pix = *(*pack).UR;
        } else if ((row%2 == 0) && (col%2 == 1)){
                *pix = *(*pack).LL;
        } else { 
                *pix = *(*pack).LR;
        }
}

/********** apply_rgb ********
 *
 * apply function that converts cv values to RGB values, and put in new array
 *
 * Parameters:
 *      int row: Current col pos
 *      int col: Current row pos;
 *      A2Methods_UArray2 array: image array to read cv_pixel
 *      A2Methods_Object *object: Element at current pos (an empty Pnm_rgb struct)
 *      void *cl:CV pixel 2D array to read cv pixels from
 *
 * Return: N/A
 *
 * Expects
 *      cl, object not to be NULL
 * Notes:
 *      Will CRE if cl or object is NULL
 *      Will CRE if malloc fails
 ************************/
void apply_rgb(int row, int col, A2Methods_UArray2 array, A2Methods_Object *object, 
               void *cl) 
{
        assert(cl != NULL && object != NULL);
        (void)array;
        A2Methods_UArray2 og_img = (A2Methods_UArray2)cl;

        Pnm_rgb newpix = malloc(sizeof(struct Pnm_rgb));
        assert(newpix != NULL);

        convertToRGB(newpix, methods->at(og_img,row,col));
        *(Pnm_rgb)object = *newpix;
        free(newpix);
}


/********** convertToRGB ********
 *
 * Calculate CV values using RGB from the pnm object and given formulas
 *
 * Parameters:
 *      rgb: The rgb values form Pnm_rgb
 *      pixel: cv_pixel struct 
 *
 * Return: void
 *
 * Expects
 *      rgb and pixel not to be NULL
 * Notes:
 *      CRE if rgb or pixel is NULL
 ************************/
void convertToRGB(Pnm_rgb rgb, cv_pixel pixel)
{
        assert(rgb != NULL && pixel != NULL);
        /* Cast each RGB value to a float */
        float Y = (pixel)->Y;
        float Pb = (pixel)->Pb;
        float Pr = (pixel)->Pr;

        /* Set each R,G,B value equal to their respective equation */
        rgb->red = cap((Y + (1.402 * Pr) )) * DENOM;
        rgb->green = cap((Y - (0.344136 * Pb) - (0.714136 * Pr))) * DENOM;
        rgb->blue = cap((Y + (1.772 * Pb))) * DENOM; 
}

/********** cap ********
 *
 * cap the bcd values to [0,1]
 *
 * Parameters:
 *      num: bcd value as float
 *
 * Return: capped bcd values
 *           
 ************************/
float cap(float num)
{
        if (num < 0) return 0.0;
        if (num > 1) return 1.0;
        return num;
}
