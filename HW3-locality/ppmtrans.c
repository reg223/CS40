/*
 *     ppmtrans.c
 *     Stephen Burchfield (sburch01)
 *     Sam Hu (hku04)
 *
 *     HW3: locality
 * 
 *     Summary
 *
 *     This file is the interface and implementation for ppmtrans, a program
 *     designated to perform transformations on a given valid ppm file based
 *     on command line arguments. 
 *     Optionally, ppmtrans can also record the time it took for the operation
 *     done and the average time taken for each pixel.
 *
 *     These operations include:
 *      - A rotation of the image by 0,90,180, and 270 degrees (-rotate {x})
 *      - A flip across the vertical or horizontal axis (-flip {direction})
 *      - A transpose across the TL-LR diagonal axis (-transpose)
 *      - output of recorded time to a designated file (-time <file_name>)
 * 
 *     Notes
 *      
 *     Returns 1 upon incorrect usage, CRE for bad ppm format.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "cputiming.h"
#include "pnm.h"


#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>]"
                        " [-{row,col,block}-major]" 
                        " [-flip {horizontal, vertical}]" 
                        " [-transpose] [-time <output file>] [filename]\n",
                        progname);
        exit(1);
}

/********** new_blank ********
 *
 * Create and return a new Pnm_ppm instance with an empty A2Methods_UArray2
 *
 * Parameters:
 *      int width: The width of the new pixmap
 *      int height: The height of the new pixmap
 *      const Pnm_ppm image: The orginal image where the methods along with
 *      the denominator are copied
 *
 * Return: A new Pnm_ppm instance
 *
 * Expects
 *      const Pnm_ppm image to not be NULL
 * Notes:
 *      Will CRE if image is NULL
 *      Will CRE if malloc fails.
 ************************/
Pnm_ppm new_blank(int width, int height, const Pnm_ppm image){
        assert(image != NULL);
        /*substantiate the pixel map first*/
        A2Methods_UArray2 new_pixmap = image->methods->new(width, 
                                        height,
                                        image->methods->size(image->pixels)); 

        Pnm_ppm new_image = malloc(sizeof(*new_image));
        /* Assert the malloc worked */
        assert(new_image != NULL);
        /* Populate the struct */
        *new_image = (struct Pnm_ppm) {width, height, image->denominator, 
                                new_pixmap, image->methods};

        return new_image;
}

/********** print_timelog ********
 *
 * Print time data to a designated output file
 *
 * Parameters:
 *      double time: Calculated time used for a image transformation call
 *      char* fName: File name where to print the output to
 *      Pnm_ppm altered_image: The new Pnm_ppm after the transformation call
 *
 * Return: N/A
 *
 * Expects
 *      fName or altered_image to be NULL
 *      fName can be opened
 * Notes:
 *      Will CRE if fName or altered_image are NULL
 *      Will CRE if opening the file fails
 ************************/
void print_timelog(double time, char* fName, Pnm_ppm altered_image) {
        assert(fName != NULL);
        assert(altered_image != NULL);

        /* Calculate time per pixel */
        double total_area = altered_image->width * altered_image->height;
        double avrg_time_per_pixel = time / total_area;

        /* Open the file to read to */
        FILE *timeF = fopen(fName, "a");
        assert(timeF != NULL);

        /* Write the necessary text to the file */
        fprintf(timeF, "-----START-OF-RUN----\n");
        fprintf(timeF, "Total time used for this run: %.0f ns\n", time);
        fprintf(timeF, "Time per pixel: %.0f ns\n------END-OF-RUN-----\n", 
        avrg_time_per_pixel);

        fclose(timeF);
}

/********** rotate_90 ********
 *
 * Function to be used as the apply function in a mapping function that will
 * map pixels to where they should be after a 90 degree rotation
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos;
 *      A2Methods_UArray2 array: Original unaltered image
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The Pnm_ppm to write to 
 *
 * Return: N/A
 *
 * Expects
 *      cl, array, object not to be NULL
 * Notes:
 *      Will CRE if cl, array, or object is NULL
 ************************/
void rotate_90(int i, int j, A2Methods_UArray2 array, A2Methods_Object *object, 
void *cl)
{
        assert(cl != NULL);
        assert(array != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm new_img = cl;
        /* The height of the original unaltered image is the width of the
        rotated image */
        int ogHeight = new_img->width;
        *(Pnm_rgb)(new_img->methods->at(new_img->pixels, 
                                (ogHeight - j - 1), i)) = *(Pnm_rgb)object;
}

/********** rotate_180 ********
 *
 * Function to be used as the apply function in a mapping function that will
 * map pixels to where they should be after a 180 degree rotation
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos;
 *      A2Methods_UArray2 array: Original unaltered image
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The Pnm_ppm to write to 
 *
 * Return: N/A
 *
 * Expects
 *      cl, array, object not to be NULL
 * Notes:
 *      Will CRE if cl, array, or object is NULL
 ************************/
void rotate_180(int i, int j, A2Methods_UArray2 array, 
A2Methods_Object *object, void *cl)
{
        assert(cl != NULL);
        assert(array != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm new_img = cl;
        /* The height and width of the original unaltered image 
        are the height and width of the rotated image respectively */
        int ogHeight = new_img->height;
        int ogWidth = new_img->width;
        *(Pnm_rgb)(new_img->methods->at(new_img->pixels,(ogWidth - i - 1), 
                                       (ogHeight - j - 1))) = *(Pnm_rgb)object;
}

/********** rotate_270 ********
 *
 * Function to be used as the apply function in a mapping function that will
 * map pixels to where they should be after a 270 degree rotation
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos;
 *      A2Methods_UArray2 array: Original unaltered image
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The Pnm_ppm to write to 
 *
 * Return: N/A
 *
 * Expects
 *      cl, array, object not to be NULL
 * Notes:
 *      Will CRE if cl, array, or object is NULL
 ************************/
void rotate_270(int i, int j, A2Methods_UArray2 array, 
A2Methods_Object *object, void *cl)
{
        assert(cl != NULL);
        assert(array != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm new_img = cl;
        /* The height of the original unaltered image is the width of the
        rotated image */
        int ogWidth = new_img->height;
        *(Pnm_rgb)(new_img->methods->at(new_img->pixels, j,
                                        (ogWidth - i - 1))) = *(Pnm_rgb)object;
}

/********** flip_v ********
 *
 * Function to be used as the apply function in a mapping function that will
 * map pixels to where they should be after a vertical flip
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos;
 *      A2Methods_UArray2 array: Original unaltered image
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The Pnm_ppm to write to 
 *
 * Return: N/A
 *
 * Expects
 *      cl, array, object not to be NULL
 * Notes:
 *      Will CRE if cl, array, or object is NULL
 ************************/
void flip_v(int i, int j, A2Methods_UArray2 array, A2Methods_Object *object, 
void *cl)
{
        assert(cl != NULL);
        assert(array != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm new_img = cl;
        int ogWidth = new_img->width;
        *(Pnm_rgb)(new_img->methods->at(new_img->pixels,(ogWidth - i -1), 
                                       j)) = *(Pnm_rgb)object;
}


/********** flip_h ********
 *
 * Function to be used as the apply function in a mapping function that will
 * map pixels to where they should be after a horizontal flip
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos;
 *      A2Methods_UArray2 array: Original unaltered image
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The Pnm_ppm to write to 
 *
 * Return: N/A
 *
 * Expects
 *      cl, array, object not to be NULL
 * Notes:
 *      Will CRE if cl, array, or object is NULL
 ************************/
void flip_h(int i, int j, A2Methods_UArray2 array, A2Methods_Object *object, 
void *cl)
{
        assert(cl != NULL);
        assert(array != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm new_img = cl;
        int ogHeight = new_img->height;
        *(Pnm_rgb)(new_img->methods->at(new_img->pixels, i, 
                                       (ogHeight - j - 1))) = *(Pnm_rgb)object;
}

/********** transpose_img ********
 *
 * Function to be used as the apply function in a mapping function that will
 * map pixels to where they should be after the image is transposed
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos;
 *      A2Methods_UArray2 array: Original unaltered image
 *      A2Methods_Object *object: Element at current pos (a pixel as Pnm_rgb)
 *      void *cl: The Pnm_ppm to write to 
 *
 * Return: N/A
 *
 * Expects
 *      cl, array, object not to be NULL
 * Notes:
 *      Will CRE if cl, array, or object is NULL
 ************************/
void transpose_img(int i, int j, A2Methods_UArray2 array, 
A2Methods_Object *object, void *cl)
{
        assert(cl != NULL);
        assert(array != NULL);
        assert(object != NULL);
        (void)array;
        Pnm_ppm new_img = cl;
        *(Pnm_rgb)(new_img->methods->at(new_img->pixels, j, 
                                       i)) = *(Pnm_rgb)object;
}

/********** flip ********
 *
 * Function to call a new empty Pnm_ppm pixmap into the mapping 
 * function with the corresponding flip function
 *
 * Parameters:
 *      bool flipDir: Bool to signify if the command line specified a 
 *      horizontal or vertical flip
 *      Pnm_ppm image: Original image that the mapping function will be called
 *      on
 *      A2Methods_mapfun *map: Mapping function to use
 *
 * Return: Altered Pnm_ppm after the flip
 *
 * Expects
 *      image and *map to not be NULL
 * Notes:
 *      Will CRE if image and *map are NULL
 ************************/
Pnm_ppm flip(bool flipDir, Pnm_ppm image, A2Methods_mapfun *map)
{
        assert(image != NULL);
        assert(map != NULL);
        /* Create new Pnm_ppm */
        Pnm_ppm new_image = new_blank(image->width, image->height, image);
        
        if (flipDir) { /* Horizontal flip if true */
                map(image->pixels, flip_h, new_image);
        } else { /* Otherwise vertical flip  */
                map(image->pixels, flip_v, new_image);
        }
        
        /* Free original image since deep copy*/
        Pnm_ppmfree(&image);

        return new_image;
        
}
/********** transpose ********
 *
 * Function to call a new empty Pnm_ppm pixmap into the mapping 
 * function with the transpose_img function
 *
 * Parameters:
 *      Pnm_ppm image: Original image that the mapping function will be called
 *      on
 *      A2Methods_mapfun *map: Mapping function to use
 *
 * Return: Altered Pnm_ppm after the transposing
 *
 * Expects
 *      image and *map to not be NULL
 * Notes:
 *      Will CRE if image and *map are NULL
 ************************/
Pnm_ppm transpose(Pnm_ppm image, A2Methods_mapfun *map) 
{
        assert(image != NULL);
        assert(map != NULL);
        /* Create new Pnm_ppm with inverted dimensions */
        Pnm_ppm new_image = new_blank(image->height, image->width, image);
        
        map(image->pixels, transpose_img, new_image);

        /* Free original image since deep copy*/
        Pnm_ppmfree(&image);

        return new_image;
}

/********** rotate ********
 *
 * Function to call a new empty Pnm_ppm pixmap into the mapping 
 * function with the corresponding rotate function
 *
 * Parameters:
 *      int rotation: The degrees to rotate by 
 *      Pnm_ppm image: Original image that the mapping function will be called
 *      on
 *      A2Methods_mapfun *map: Mapping function to use
 *
 * Return: Altered Pnm_ppm after the rotation
 *
 * Expects
 *      image and *map to not be NULL
 * Notes:
 *      Rotation will either be 0, 90, 180, or 270
 *      Will CRE if image and *map are NULL
 ************************/
Pnm_ppm rotate(int rotation, Pnm_ppm image, A2Methods_mapfun *map)
{
        assert(image != NULL); 
        assert(map != NULL);
        if (rotation == 0) {
                return image;
        } else {
                int new_width, new_height;
                /* dimensions inverted for 90 and 270 degree rotations */
                if (rotation == 90 || rotation == 270) {
                        new_width = image->height;
                        new_height = image->width;
                }
                /* dimensions not inverted for 180 degree rotations */
                else { 
                        new_width = image->width;
                        new_height = image->height;
                }
                /* Create new Pnm_ppm instance */
                Pnm_ppm new_image = new_blank(new_width, new_height, image);
                /* Apply correct rotate function */
                if (rotation == 90) {
                        map(image->pixels, rotate_90, new_image);
                } else if (rotation == 180) {
                        map(image->pixels, rotate_180, new_image);
                } else if (rotation == 270) {
                        map(image->pixels, rotate_270, new_image);
                }
                /* Free original image since deep copy*/
                Pnm_ppmfree(&image);
                return new_image;
        }
}


int main(int argc, char *argv[]) 
{
        /*************************************************
        *             Variable Declarations              *
        *************************************************/
        char *time_file_name = NULL;
        char *flip_op        = NULL;
        int   rotation       = 0;
        int   i;
        
        CPUTime_T timer;

        /* Bools to keep track which image editing option is chosen */
        bool time            = false;
        bool isrot           = false;
        bool isflip          = false;
        bool flipDir; /* Keeps track if the flip is horizontal or vertical */
        bool istran          = false;

        double time_used;
        FILE *input          = NULL;

        /*************************************************
        *                Setting Methods                 *
        *************************************************/

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);

        /*Take Command line arguments*/
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if ((strcmp(argv[i], "-rotate") == 0)) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        isrot = true; /*operations kept exclusive OR */
                        isflip = false;
                        istran = false;
                        
                } else if (strcmp(argv[i], "-flip") == 0) {
                        
                        if (!(i + 1 < argc)) {      /* no flip direction */
                                usage(argv[0]);
                        }
                        flip_op = argv[++i];
                        assert(flip_op != NULL);
                        if (!((strcmp(flip_op,"horizontal")) == 0 || 
                        strcmp(flip_op,"vertical") == 0)) {
                                fprintf(stderr, 
                                        "Flip must be horizontal"
                                        " or vertical\n");
                                usage(argv[0]);
                        }
                        isrot = false; /*operations kept exclusive OR */
                        isflip = true;
                        istran = false;
                        flipDir = strcmp(flip_op, "horizontal");
                }  else if (strcmp(argv[i], "-transpose") == 0) {
                        isrot = false; /*operations kept exclusive OR */
                        isflip = false;
                        istran = true;
                } else if (strcmp(argv[i], "-time") == 0) {
                        if (!(i + 1 < argc)) {      /* no time file */
                                usage(argv[0]);
                        }
                        time_file_name = argv[++i];
                        time = true;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        
        /*************************************************
        *                  File Read In                  *
        *************************************************/
        /* Read from stdin or file provided */
        if (i == argc){
                input = stdin;
        } else if (i == (argc - 1)) {
                input = fopen(argv[i], "r");
        } 
        assert (input != NULL);
        /* Passed in image */
        Pnm_ppm image = Pnm_ppmread(input, methods);

        Pnm_ppm altered_image;

        /*************************************************
        *                 Transformation                 *
        *************************************************/
        
        if (time){/* start timer if -time called */
                timer = CPUTime_New();
                CPUTime_Start(timer);      
        } 
        
        if (isrot) {
                altered_image = rotate(rotation, image, map);
        } else if (isflip) {
                altered_image = flip(flipDir, image, map);
        } else if (istran){
                altered_image = transpose(image, map);
        } else { /* when no operation args provided*/
                altered_image = image;
        }

        /* If the time command line argument was provided stop the CPU timer, 
        free memory associated, and print timing data. */
        if (time) {
                time_used = CPUTime_Stop(timer);  
                CPUTime_Free(&timer);
                print_timelog(time_used, time_file_name, altered_image);
        }

        /*************************************************
        *                Output & Cleanup                *
        *************************************************/
        Pnm_ppmwrite(stdout, altered_image);
        Pnm_ppmfree(&altered_image);
        fclose(input);
        
        return 0;
}


