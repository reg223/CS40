#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uarray2b.h>


void insert_block(int col, int row, UArray2b_T array2b,
                         void *elem, void *cl)
{
        (void) array2b;
        (void) elem;
        (void) col;
        (void) row;
        int *counter  = (int *)cl;
        *(int *)elem = (*counter)++;


}
void print_block(int col, int row, UArray2b_T array2b,
                         void *elem, void *cl)
{
        (void) array2b;
        (void) elem;
        (void)cl;
        (void) col;
        (void) row;
        printf("curr element ->%i\n", *(int *)elem);
        
        printf("currently at col:%i  and row:%i\n",col, row);
}

int main(){
        UArray2b_T big_test = UArray2b_new_64K_block(1, 1, 4);
        /* UArray2b_T test = UArray2b_new(3,3,sizeof(int),2);


        // UArray2b_T test;
        int *counter = malloc(sizeof(int));
        *counter = 1;
        
        UArray2b_map(test, insert_block, counter);
        free(counter);
        UArray2b_map(test, print_block, NULL);

        printf("THE WIDTH IS %i\n", UArray2b_width(test));
        printf("THE HEIGHT IS %i\n", UArray2b_height(test));
        printf("THE SIZE IS %i\n", UArray2b_size(test));
        printf("THE BLOCKSIZE IS %i\n", UArray2b_blocksize(test));

        assert(*(int *)UArray2b_at(test, 0, 2) == 7); */
        //UArray2b_at(test, 3, 3);


        UArray2b_free(&big_test);
        return 0;
}