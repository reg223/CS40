/*
 *               Restoration
 *
 *   Purpose:
 *  
 *     restore the corrupted plain pgm file and convert it into a raw pgm file
 *
 *   Non-library Includes:
 *     readaline.h - reads a single line from file source
 *          readaline();
 *     parseline.h - parses the line into digit and non digit sequences
 *          parse_line();
 *          convert_to_ascii();
 *
 *   Other Functions:
 *     vfree() - helper function to free table members
 *     remove_last_char() - helper function to remove last character from
 *     line
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <atom.h>
#include <assert.h>
#include "readaline.h"
#include "parseline.h"
#include <table.h>
#include <seq.h>


/* vfree
function for freeing table members*/
static void vfree(const void *key, void **value, void *cl) {
    (void)key;
    (void)cl;
    Seq_T seq = *(Seq_T *)value; /* Dereference the void** to get Seq_T */
    for (int i = 0; i < Seq_length(seq); i++) {
        if (Seq_get(seq, i) != NULL) {
            free(Seq_get(seq, i));
        } 
         /* Free each character in the sequence*/
    }
    Seq_free(&seq); /* Free the sequence itself */
    *value = NULL;
}

/*helper function used to remove
newline character from lines from datapp
since it's of no use for restoration*/
void remove_last_char(char *str, size_t length) {
    if (str && length > 0) {
        str[length - 1] = '\0';
    }
}

int main(int argv, char *argc[]) {
    const int HINT = 1000;/*assumed size for hanson structures*/
    /*table structure for storing lines*/
    Table_T Hash_Table = Table_new(HINT, NULL, NULL);
    /*sequence structure for storing pixels*/
    Seq_T original_seq = Seq_new(HINT);

    size_t digit_len;/*length / number of pixels*/
    const char *correct_seq; /* semi permanent storage for corrupt 
                                sequence of original lines*/
    const char *imm_non_digit;/*storage for all corrupt sequences*/
    char *digit;/*storage for parse line returns*/
    char *non_digit;
    Seq_T temp;/*temp storage for retrieved correct sequence*/

    assert(argv == 2);

    /*file handling*/
    FILE *fp = fopen(argc[1], "r");
    assert(fp);

    char *datapp; /*storage for datapp returns*/
    size_t length;

    /*iteration for reading line*/
    while(((length = readaline(fp, &datapp)) > 0)) {
        if (datapp[length - 1] == '\n') {
            remove_last_char(datapp, length);
            length -=1;
        }

        int found = 0; /*true after original sequence found*/

        /*line is handled here*/
        int nd_length; /*placed here since not pointer*/
        digit_len = parse_line(datapp, &digit, &non_digit, length, &nd_length);
        free(datapp);

        /*store digit into sequence*/
        Seq_T d_seq = Seq_new((digit_len)); 
        for (size_t i = 0; i < digit_len; i++) {
            char *new_c = malloc(sizeof(char));
            assert(new_c);
            *new_c = digit[i];
            Seq_addhi(d_seq, (void *)new_c);
        }
        /*store corrupt sequence (key) into atom*/
        imm_non_digit = Atom_new(non_digit,nd_length);

        /*using table to find original sequence,
        i.e. non unique sequence is retrieved
        while infused lines stays in table*/
        if ((Table_get(Hash_Table, (void *)imm_non_digit)) == NULL){
            Table_put(Hash_Table, (void *)imm_non_digit, (void *)d_seq);
        } else {
            if (!found) {
                found = 1;
                correct_seq = imm_non_digit;
            }

            /*put retrieved old stored member and stores new one in*/
            temp = (Table_put(Hash_Table,(void *)imm_non_digit,(void *)d_seq));

            /*add to original sequence*/
            for (int i = 0; i < Seq_length(temp); i++) {
                char *cur_c = Seq_get(temp, i);
                Seq_addhi(original_seq, cur_c);
            }

            /*free temp for next iteration*/
            Seq_free(&temp);
        }
        free(digit);
        free(non_digit);
    }

    /*take out the original line that was infused last*/
    Seq_T end = Table_remove(Hash_Table, (void *)correct_seq);
    digit_len = Seq_length(end);
    for (int i = 0; i < Seq_length(end); i++) {
            char *cur_c = Seq_get(end, i);
            
            Seq_addhi(original_seq, cur_c);
    } 
    Seq_free(&end);
    
    /*return result*/
    int full_length = Seq_length(original_seq);/*full length of sequence*/
    size_t width = full_length/digit_len;
    assert(!((digit_len < 2 )||( width < 2)));
    
    fprintf(stdout, "P5 %lu %lu 255\n",digit_len,width);
    for (int i = 0; i < full_length; i++) {
        /*print char by char since %s uses strlen()*/
        fprintf(stdout,"%c",*(char *)(Seq_get(original_seq, i)));
    }
    for (int i = 0; i < full_length; i++) {
        free(Seq_get(original_seq, i));
    }
    /*clean up*/
    Table_map(Hash_Table, vfree, NULL);
    Table_free(&Hash_Table);
    Seq_free(&original_seq);
    fclose(fp);

    exit(EXIT_SUCCESS);
}


