/******************************************************************************
 * phase5.c                                                                   *
 * David Chen (zchen18)                                                       *
 * Sam Hu (khu04)                                                             *
 *                                                                            *
 * HW5: bomb                                                                  *
 *                                                                            *
 * Summary:                                                                   *
 * Provides functionality of the assembly function  "phase_5"  written in c   *
 *                                                                            * 
 *                                                                            *
 * Notes:                                                                     *
 *                                                                            *
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int ARR[16] = {2,10,6,1,12,16,9,3,4,7,14,5,11,8,15,13};
const int LENGTH = 6, VAL = 21;

void explode_bomb();

/**************** phase_5 ****************
 *
 * hash 6 characters by 16 and use as index to add number to accumulator
 *
 * Parameters:
 *      char* input: contains the input string
 *
 * Return:
 *      none
 *
 * Notes:
 *      Bomb explodes if string is not length 6 or value for accumulator does
 *      not match
 *
 ************************/
void phase_5(char* input) 
{
        if(strlen(input) != 6){
                explode_bomb();
                exit(1);
        }

        int val = 0;
        for (int i = 0; i < LENGTH; i++){
                int idx = (int)input[i] & 0xf; /*hash with last 4 bits*/
                val += ARR[idx];/*add to accumulator*/
        }

        if (val != VAL ) {
                explode_bomb();
        }
}

