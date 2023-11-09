#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>


/*convert to ASCII
  takes in the "pixel" in the form of a string,
  validates its length and value, convert into
  the corresponding ASCII character
  e.g. "123" -> '{' 
  arg: char *pixel,
       size_t length (of the character)
  return: char
*/
int convert_to_ascii(char *pixel, size_t length){
    /*validation*/
    assert(pixel);
    assert(!(length > 3 || length < 1 ));
    for (size_t i = 0; i < length; i++) {
        assert(isdigit(pixel[i])) ;
    }

    /*conversion*/
    int cur = 0;
    for (size_t i = 0; i < length; i++) {
        cur *= 10;
        cur += (int)pixel[i] - 48;
    }

    /*final validation*/
    assert(!(cur > 255) || (cur < 0));
    return cur;
}


/*parse line
  split the string into pixels and the corruption sequence
  arg: char *original (the line from readaline()),
       char **digit (returned pixel sequence by ref),
       char **non_digit (returned corrupt sequence ny ref)
       int *nd_length (length of non_digit by reference)
  return: size_t length of pixels
*/
size_t parse_line(char *original, char **digit, char **non_digit, size_t length ,int *nd_length) {
    /*validation*/
    assert(original);

    /*Variables*/
    *non_digit = malloc(sizeof(char) * length);
    assert(non_digit);
    *digit = malloc(sizeof(char) * length);
    assert(digit);

    size_t count_nd = 0;/*counter for non digit length*/
    size_t count_d = 0;/*counter for digit length*/
    size_t count_t = 0;/*counter for individual pixel holder*/

    char *temp_digit = malloc(sizeof(char) * 4);/*temporary storage for pixel*/
    assert(temp_digit);

    /*iterate through original by char,
    add char to temp if is digit,
    add to non_d otherwise and call
    convert_to_ascii for temp*/
    for (size_t i = 0; i < length; i++) {
        if (isdigit(original[i]) && count_t < 3) {
            temp_digit[count_t] = original[i];
            count_t++;
        } else {
            (*non_digit)[count_nd] = original[i];
            count_nd++;

            if (count_t > 0){
                temp_digit[count_t] = '\0';
                int res = convert_to_ascii(temp_digit, count_t);
                if (res != -1) {
                    (*digit)[count_d] = (char)res;
                    count_d++;
                }
                count_t = 0;/*reset value*/
                free(temp_digit);
                temp_digit = malloc(sizeof(char) * 4);
                assert(temp_digit);
            }
        }
    }

    /*check for sequences ending with pixels,
    since convert triggered by proceeding char*/
    if (count_t > 0){
        temp_digit[count_t] = '\0';
        int res = (int)convert_to_ascii(temp_digit, count_t);
        if (res != -1) {
            (*digit)[count_d] = (char)res;
            count_d++;
        }
    }

    /*set values and free members*/
    *non_digit = realloc(*non_digit, sizeof(char) * (count_nd + 1));
    assert(non_digit);
    (*non_digit)[count_nd] = '\0';
    *nd_length = count_nd;
    *digit = realloc(*digit, sizeof(char) * (count_d + 1));
    assert(digit);
    (*digit)[count_d] = '\0';
    free(temp_digit);

    return count_d;
}
