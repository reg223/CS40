/******************************************************************************
 * bitpack.c                                                                  *
 * Joshua Bernstein (jberns02)                                                *
 * Sam Hu (hku04)                                                             *
 *                                                                            *
 * HW4: arith                                                                 *
 *                                                                            *
 * Summary:                                                                   *
 * Provides functions for manipulating and working with bit-packed values.    *
 * It includes functions to check if unsigned and signed integers can be      * 
 * represented within a given width, extract values from bit-packed words,    * 
 * and create new bit-packed words by replacing portions of existing words    *
 * with either unsigned or signed integer values.                             *
 *                                                                            * 
 *                                                                            *
 * Notes:                                                                     *
 * The "Bitpack_Overflow" exception is raised when there's an attempt         *
 * to set or extract values that would result in an overflow.                 *
 *                                                                            *
 *****************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "except.h"
#include <assert.h>
#include <bitpack.h>

const unsigned long MAX = 0xffffffffffffffff;
const unsigned BITLEN = 64;

Except_T Bitpack_Overflow = { "Overflow packing bits" };


/**************** Bitpack_fitsu ****************
 *
 * Determine if an unsigned integer can be represented using a given width.
 *
 * Parameters:
 *      uint64_t n - The unsigned integer to be checked for representability.
 *      unsigned width - The width (number of bits) in which to represent 'n'.
 *
 * Return:
 *      Returns true if 'n' can be represented using 'width' bits; false 
 *      otherwise.
 *
 * Notes:
 *      - If 'width' is equal to 64, the function always returns true 
 *
 ************************/

bool Bitpack_fitsu(uint64_t n, unsigned width)
{     
        if (width == 64) {
                return true; /* all n can be represented with 64 */
        }
        
        uint64_t max_val = (uint64_t)((1 << width) - 1);
        return n <= max_val;
}

/**************** Bitpack_fitss ****************
 *
 * Determine if a signed integer can be represented using a given width.
 *
 * Parameters:
 *      int64_t n - The signed integer to be checked for representability.
 *      unsigned width - The width (number of bits) in which to represent 'n'.
 *
 * Return:
 *      Returns true if 'n' can be represented using 'width' bits; false 
 *  otherwise.
 *
 * Notes:
 *      - If 'width' is equal to 64, the function always returns true since all 
 *        64-bit signed integers can be represented.
 *     
 *************************************************/
bool Bitpack_fitss( int64_t n, unsigned width)
{
        if (width == 64) {
                return true; /* all n can be represented with 64 */
        }
        uint64_t mask = 1ULL << (width - 1);
        int64_t LB = -(int64_t)mask;
        int64_t UB = (int64_t)(mask - 1);

        return (n >= LB) && (n <= UB);
}

/**************** Bitpack_getu ****************
 *
 * Extract an unsigned integer value from a given bit-packed word.
 *
 * Parameters:
 *      uint64_t word - The bit-packed word from which to extract the value.
 *      unsigned width - The width (number of bits) of the value to extract.
 *      unsigned lsb - The position (least significant bit) of the
                       value within 'word'.
 *
 * Return:
 *      Returns the extracted unsigned integer value from 'word'.
 *
 * Notes:
 *      - If 'width' is 0, the function returns 0.
 *      - If 'width' is equal to the total number of bits in 'word' (BITLEN) 
          and 'lsb' is 0, the function returns the entire 'word'.
          Width should be between 0 and 64
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        if (width == 0) {
                return 0;
        } else if (width == BITLEN && lsb == 0) {
                return word;
        }
        assert (width <= BITLEN);
        assert ((width + lsb) <= BITLEN);
        /* Create mask and shift as needed */
        uint64_t mask = MAX << ((BITLEN - width) - lsb);
        mask = mask >> ((BITLEN - width) - lsb);
        /* Extract and shift the value to the right to remove any unwanted 
        bits. */
        uint64_t extracted_value = (word & mask) >> lsb;
        return extracted_value;
}


/**************** Bitpack_gets ****************
 *
 * Extract a signed integer value from a given bit-packed word.
 *
 * Parameters:
 *      uint64_t word - The bit-packed word from which to extract the value.
 *      unsigned width - The width (number of bits) of the value to extract.
 *      unsigned lsb - The position (least significant bit) of the
                       value within 'word'.
 *
 * Return:
 *      Returns the extracted signed integer value from 'word'.
 *
 * Notes:
 *      - If 'width' is 0, the function returns 0.
 *      - If 'width' is equal to the total number of bits in 'word' (BITLEN) 
          and 'lsb' is 0, the function returns the entire 'word'.
          Width should be between 0 and 64
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        if (width == 0) {
                return 0;
        } else if (width == BITLEN && lsb == 0) {
                return word;
        }
        assert (width <= BITLEN);
        assert ((width + lsb) <= BITLEN);
        /* Create mask and shift as needed */
        uint64_t mask = MAX << (BITLEN - width - lsb);
        int64_t extracted_value = (word << (BITLEN - width - lsb)) & mask;
        /* Extract and shift the value to the right to remove any unwanted 
        bits. */
        extracted_value = extracted_value >> (BITLEN - width);
        return extracted_value;
}

/**************** Bitpack_newu ****************
 *
 * Create a new bit-packed word by replacing a portion with an unsigned
 * integer value.
 *
 * Parameters:
 *      uint64_t word - The original bit-packed word to modify.
 *      unsigned width - The width (number of bits) of the value to insert.
 *      unsigned lsb - The position (least significant bit) at which to insert 
 *      the new value.
 *      uint64_t value - The unsigned integer value to insert into 'word'.
 *
 * Return:
 *      Returns a new bit-packed word with the specified portion replaced by 
 *      'value'.
 *
 * Notes:
 *      - If 'width' is equal to the total number of bits in 'word' (BITLEN)
 *        and 'lsb' is 0, the function returns 'value' as the new word.
 *      - If 'width' or 'lsb' exceeds the size of 'word' (BITLEN),
 *         the function raises the Bitpack_Overflow exception.
 *      - If 'value' cannot be represented using 'width' bits, 
 *         the function raisesthe Bitpack_Overflow exception.
 *
 *********************************************/

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        if (width == BITLEN && lsb == 0) {
                return value;
        }
        if (width > BITLEN || lsb + width > BITLEN) {
                RAISE(Bitpack_Overflow);
        }
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        /* Create mask and shift as needed */
        uint64_t mask = ((1ULL << width) - 1) << lsb;  
        uint64_t inverted_mask = ~mask;
    
        word = (word & inverted_mask) | ((value << lsb) & mask);
        return word;
}


/**************** Bitpack_news ****************
 *
 * Create a new bit-packed word by replacing a portion with a signed 
 * integer value.
 *
 * Parameters:
 *      uint64_t word - The original bit-packed word to modify.
 *      unsigned width - The width (number of bits) of the value to insert.
 *      unsigned lsb - The position (least significant bit) at which to 
                       insert the new value.
 *      int64_t value - The signed integer value to insert into 'word'.
 *
 * Return:
 *      Returns a new bit-packed word with the specified portion replaced by 
 *      'value'.
 *
 * Errors:
 *      - If 'width' is equal to the total number of bits in 'word' (BITLEN)
 *        and 'lsb' is 0, the function returns 'value' as the new word.
 *      - If 'width' or 'lsb' exceeds the size of 'word' (BITLEN), 
 *        the function raises the Bitpack_Overflow exception.
 *      - If 'value' cannot be represented using 'width' bits as a 
 *        signed integer, the function raises the Bitpack_Overflow exception.
 *
 * Notes:
 *      - The function handles signed integers and ensures that sign-extension
 *        is applied correctly when inserting 'value' into 'word'.
 ********************************************/

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value)
{
        if (width == BITLEN && lsb == 0) {
                return value;
        }
        if (width > BITLEN || lsb + width > BITLEN) {
                RAISE(Bitpack_Overflow);
        }
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        /* Create mask and shift as needed */
        uint64_t mask = MAX << (width + lsb);
        uint64_t mask2 = (MAX >> (BITLEN - lsb));
        mask += mask2;
        mask2 = ~mask;
        word = word & mask;
        value = (value << lsb) & mask2;
        word = word | value;
        return word;
        
}