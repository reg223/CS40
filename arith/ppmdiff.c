#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "assert.h"
#include "pnm.h"
#include "a2methods.h"
#include "compress40.h"
#include "a2plain.h"
#include "a2blocked.h"
#include <math.h>
#include <uarray2.h>

int min(int a, int b) {
        return ((a < b) ? a : b);
}

int max(int a, int b)  {
        return ((a > b) ? a : b);
}


int main(int argc, char* argv[]){

        int widthC, heightC;
        float denomC;
        int widthT, heightT;
        float denomT;
        int diffW, diffH;


        //set methods
        assert(argc == 3);
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);
        //open files and get pnm ppm
        FILE *fp = fopen(argv[1], "r");
        assert(fp != NULL);
        Pnm_ppm control = Pnm_ppmread(fp, methods);
        fclose(fp);

        fp = fopen(argv[2], "r");
        assert(fp != NULL);
        Pnm_ppm test = Pnm_ppmread(fp, methods);
        fclose(fp);


        A2Methods_UArray2 Carr = control->pixels;
        A2Methods_UArray2 Tarr = test->pixels;


        widthC =  control->width; 
        heightC = control->height;
        denomC = (float)control->denominator;
        widthT = test->width;
        heightT = test->height;
        denomT = (float)test->denominator;

        diffW = widthC;
        diffH = heightC;
        //compare width and  height
        if ((widthC != widthT) || (heightC != heightT)) {
                if (abs(widthC - widthT) > 1) {
                        fprintf(stderr, "Width difference is too big\n");
                        exit(1);
                }

                if (abs(heightC - heightT) > 1) {
                        fprintf(stderr, "Height difference is too big\n");
                        exit(1);
                }
                diffW = min(widthC, widthT);
                diffH = min(heightC, heightT);
        }
        //calculate E
        
        double val = 0.0;
        Pnm_rgb cPix = malloc(sizeof(*cPix));
        Pnm_rgb rPix = malloc(sizeof(*rPix));

        float redC, redT, blueC, blueT, greenC, greenT;
        
        for (int i = 0; i < diffH; i++) {
                for (int j = 0; j < diffW; j++) {
                        *cPix = *(Pnm_rgb)methods->at(Carr,j, i);
                        *rPix = *(Pnm_rgb)methods->at(Tarr,j,i);
        
                        redC = (float)cPix->red;
                        redC /= denomC;
                        redT = (float)rPix->red;
                        redT /= denomT;

                        greenC = (float)cPix->green;
                        greenC /= denomC;
                        greenT = (float)rPix->green;
                        greenT /= denomT;

                        blueC = (float)cPix->blue;
                        blueC /= denomC;
                        blueT = (float)rPix->blue;
                        blueT /= denomT;

                        val += (redC-redT)*(redC-redT);
                        
                        val += (greenC-greenT)*(greenC-greenT);

                        val += (blueC-blueT)*(blueC-blueT);

                }
        }
        printf("Val is at: %f \n", val);
        val /= (float)3*diffH*diffW;
        val = sqrtf(val);
        printf("the diff for this file is: %0.4f \n", val);
}