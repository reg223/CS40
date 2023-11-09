#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "readaline.h"
#include "assert.h"


/*
read a line
returns a line from the given file stream,
if possible
see .h file for more detail
*/
size_t readaline(FILE *inputfd, char ** datapp) {
    assert(inputfd && datapp);/*null input verificaiton*/

    size_t length = 1000;/*initial length*/
    size_t count = 0;/*number of elements*/
    char *line = malloc(sizeof(char)* length);/*temporary storage for line*/
    assert(line);
    int element = fgetc(inputfd);/*temporary storage for each character*/
    
    if (feof(inputfd)) { /*empty string validaiton*/
        *datapp = NULL;
        free(line);
        return 0;
    }

    /*line iteration*/
    while ((element != '\n') && (element != EOF)) {
        /*-2 since might add null/ newline char after iteration*/
        if (count == length - 2) { 
            length *= 2;
            char *temp = realloc(line, sizeof(char)* length);
            assert(temp);
            line = temp;
        }
        line[count] = (char)element;
        count++;
        element = fgetc(inputfd);
    }

    if (element == '\n') {
        line[count] = element;
        count++;
    }
    
    line[count] = '\0';
    count++;

    /*resize*/
    line = realloc(line, sizeof(char)*count);
    assert(line);

    /*compile to datapp*/
    *datapp = malloc(sizeof(char)*(count));
    for (size_t i = 0; i < count; i++) {
        (*datapp)[i] = line[i];
    }

    free(line);
    return (count - 1);
}