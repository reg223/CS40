/*
 *               parseline (& convert to ascii)
 *
 *   Purpose:
 *  
 *     parse the line given by readaline, spliting it into
 *     digit and non digit components
 *
 *   Parameters:
 * 
 *     see bellow or the .c file
 *
 *   Errors:
 * 
 *     raise checked runtime error for:
 *      convert_t_ ascii: incorrect strings
 *      parse: malloc failure or null input
 * 
 */

#ifndef PARSELINE_H
#define PARSELINE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>


/*convert to ASCII
  takes in the "pixel" in the form of a string,
  validates its length and value, convert into
  the corresponding ASCII character
  e.g. "123" -> '{' 
  arg: char *pixel,
       size_t length (of the character)
  return: char
*/
int convert_to_ascii(char *pixel, size_t length);

/*parse line
  split the string into pixels and the corruption sequence
  arg: char *original (the line from readaline()),
       char **digit (returned pixel sequence by ref),
       char **non_digit (returned corrupt sequence ny ref)
       int *nd_length (length of non_digit by reference)
  return: size_t length of pixels
*/
size_t parse_line(char *original, char **digit, char **non_digit, size_t length ,int *nd_length);

#endif


