/*
 *     uarray2b.c
 *     Stephen Burchfield (sburch01) 
 *     Sam Hu (khu04)
 *     HW3: locality
 *
 *     This is the implementation for the object UArray2b_T.
 *     Specifically, UArray2b_T uses a UArray2 of UArrays to simulate the
 *     blocked access. This is a simpler approach as opposed to using a single
 *     UArray which is easier to implement.
 *     The functionalities of UArray2b is effectively the same as UArray2,
 *     except for block related operations, most notably block major mapping,
 *     which performs row major operations block by block.
 * 
 */

#include <stdio.h>
#include <uarray2b.h>
#include "uarray2.h"
#include "uarray.h"
#include "mem.h"
#include <assert.h>
#include <math.h>


/********** struct UArray2b_T ********
 *
 *
 * Parameters:
 *      int width: actual width (number of cells horizontally)
 *      int height: actual height (number of cells vertically)
 *      int size: size of each cell, in bytes
 *      int blocksize: size of each block, sqrt-d
 *      UArray2_T blocks: The container for blocks

 ************************/
struct UArray2b_T {
        int width, height;
        int size, blocksize;
        UArray2_T blocks;
};  

/********** make_blocks ********
 *
 * Apply function that will create UArrays that will represent 
 * the individual blocks within the UArray2b
 *
 * Parameters:
 *      int i: Current col pos
 *      int j: Current row pos
 *      UArray2_T arr: The current UArray2_T which will hold the UArrays
 *      void *elem: The current UArray2 index
 *      void *cl: The blocksize and size of an element of the current UArray2b
 *
 * Return: N/A
 *
 * Expects
 *      cl to not be NULL
 *      UArray_new to create a valid instance of UArray_T
 * Notes:
 *      Will CRE if cl is NULL
 *      Will CRE if UArray_new fails
 ************************/
void make_blocks(int i, int j, UArray2_T arr, void *elem, void *cl)
{
        (void) i;
        (void) j;
        (void) arr;
        assert(cl != NULL);
        /* 
        * Note that uarray_info is a int array of size 2. Index 0 stores the 
        * blocksize, and index 1 stores the size of an element.
        * these calculations are used during the creation of a single block
        */
        int* uarray_info = cl;
        int size = uarray_info[0];
        int blocksize = uarray_info[1];
        
        /* Create a new UArray set it to be what is stored at the current 
        index of the UArray2 */
        UArray_T new_block = UArray_new(blocksize * blocksize, size);
        assert(new_block != NULL);

        UArray_T *curr_block = elem;
        *curr_block = new_block;

}


/********** UArray2b_new ********
 *
 * Create a new instance of UArray2b 
 *
 * Parameters:
 *      int width: The width of the UArray2b
 *      int height: The height of the UArray2b
 *      int size: The size of an element 
 *      int blocksize: The length of one side of a block
 *
 * Return: A new UArray2b_T instance
 *
 * Expects
 *      width and height to not be negative
 *      size and blocksize to be 1 or greater
 * Notes:
 *      Will CRE if width and height are negative
 *      Will CRE if size and blocksize are less than 1
 ************************/
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        assert((width >= 0) && (height  >= 0) && (size > 0) && (blocksize > 0));
        /* Create the number of blocks in the UArray2b, if the width/height is
        divisible by blocksize, then with its result create that many blocks,
        otherwise round up. */
        int blk_width, blk_height;
        if (width % blocksize == 0){
                blk_width = width / blocksize;
        }
        else {
                blk_width = (width / blocksize) + 1;
        }
        if (height % blocksize == 0){
                blk_height = height / blocksize;
        }
        else {
                blk_height = (height / blocksize) + 1;
        }
        UArray2_T blocks = UArray2_new(blk_width, blk_height, sizeof(UArray_T));
        /* Create UArrays in each index of the UArray2b */
        int* uarray_info = ALLOC(2 * sizeof(int));
        uarray_info[0] = size;
        uarray_info[1] = blocksize;
        UArray2_map_row_major(blocks, make_blocks, uarray_info);
        FREE(uarray_info);
        /* Populate the UArray2b_T struct */
        UArray2b_T arr_block;
        NEW(arr_block);
        arr_block->width = width;
        arr_block->height = height;
        arr_block->size = size; 
        arr_block->blocksize = blocksize;
        arr_block->blocks = blocks;
        return arr_block;
}

/********** UArray2b_new_64k_block ********
 *
 * Create a new instance of UArray2b with a block that is 64k bytes large
 *
 * Parameters:
 *      int width: The width of the UArray2b
 *      int height: The height of the UArray2b
 *      int size: The size of an element 
 *
 * Return: A new UArray2b_T instance
 *
 * Notes:
 *      UArray2b_new handles the CRE that would normally need to be caught
 ************************/
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        int blocksize = (64 * 1024) / size;
        blocksize = (int)sqrt((double)blocksize);
        return UArray2b_new(width, height, size, blocksize);
} 

/********** free_blocks ********
 *
 * Apply function that will free blocks (UArray) at indexes
 *
 * Parameters:
 *      int col: The current col index
 *      int row: The current row index
 *      UArray2_T array2: The UArray2_T which is being iterating on
 *      void* elem: The element at the current postion
 *      void* cl: The closure
 *
 * Return: N/A
 *
 * Expects:
 *      The current element is not NULL
 * Notes:
 *      Will CRE if the current element is NULL
 ************************/
void free_blocks(int col, int row, UArray2_T array2,
                 void *elem, void *cl)
{
        
        assert(elem != NULL);
        (void) cl;
        (void) col;
        (void) row;
        (void) array2;
        UArray_T* curr_block = elem;
        UArray_free(curr_block);
}

/********** UArray2b_free ********
 *
 * Frees memory associated with a UArray2b instance
 *
 * Parameters:
 *      UArray2b_t *array2b: Pointer to a UArray2b instance
 *
 * Return: N/A
 *
 * Expects:
 *      The pointer to UArray2b to not be NULL
 *      UArray2b to not be NULL
 * Notes:
 *      Will CRE if the pointer to UArray2b is NULL
 *      Will CRE if UArray2b is NULL
 ************************/
void UArray2b_free (UArray2b_T *array2b)
{
        assert(array2b != NULL && (*array2b != NULL));
        /* Free all the individual blocks */
        UArray2_map_row_major((*array2b)->blocks, free_blocks, NULL);
        /* Free the UArray2 storing the blocks */
        UArray2_free(&((*array2b)->blocks));
        /* Free the UArray2b struct */
        FREE(*array2b);
 }

/********** UArray2b_width ********
 *
 * Returns the width of the UArray2b
 *
 * Parameters:
 *      UArray2b_t array2b: The UArray2b_T to find the width of
 *
 * Return: The width of the current UArray2b_T
 *
 * Expects:
 *      array2b to not be NULL
 * Notes:
 *      Will CRE if UArray2b is NULL
 ************************/
int UArray2b_width (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/********** UArray2b_height ********
 *
 * Returns the height of the UArray2b
 *
 * Parameters:
 *      UArray2b_t array2b: The UArray2b_T to find the height of
 *
 * Return: The height of the current UArray2b_T
 *
 * Expects:
 *      array2b to not be NULL
 * Notes:
 *      Will CRE if UArray2b is NULL
 ************************/
int UArray2b_height (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/********** UArray2b_size ********
 *
 * Returns the size of an element of the UArray2b
 *
 * Parameters:
 *      UArray2b_t array2b: The UArray2b_T to find the size of an element of
 *
 * Return: The size of an element of the current UArray2b_T
 *
 * Expects:
 *      array2b to not be NULL
 * Notes:
 *      Will CRE if UArray2b is NULL
 ************************/
int UArray2b_size (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->size;
}

/********** UArray2b_blocksize ********
 *
 * Returns the blocksize of the UArray2b
 *
 * Parameters:
 *      UArray2b_t array2b: The UArray2b_T to find the blocksize of
 *
 * Return: The blocksize of the current UArray2b_T
 *
 * Expects:
 *      array2b to not be NULL
 * Notes:
 *      Will CRE if UArray2b is NULL
 ************************/
int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/********** UArray2b_at ********
 *
 * Returns the element at a specific index of the UArray2b
 *
 * Parameters:
 *      UArray2b_t array2b: The UArray2b_T to find the blocksize of
 *      int column: The column index of the element 
 *      int row: The row index of the element
 *
he ind * Return: void* pointer to the element at 
 *
 * Expects:
 *      array2b to not be NULL
 *      column and row to be within range
 * Notes:
 *      Will CRE if UArray2b is NULL or index out of range
 ************************/
void *UArray2b_at(UArray2b_T array2b, int column, int row) 

{
        assert(array2b != NULL);
        assert((column < array2b->width) && (column >= 0));
        assert((row < array2b->height) && (row >= 0));
        int blocksize = array2b->blocksize;
        int block_col = column / (blocksize);
        int block_row = row / (blocksize);
        
        UArray_T block = *((UArray_T *)UArray2_at(array2b->blocks, block_col, 
                                block_row));
        int idx = (column % blocksize) * (blocksize) + (row % blocksize);
        void* curr_element = UArray_at(block, idx);
        return curr_element;
} 


/* Struct to be passed as closure which will hold the UArray2b_T instance,
the client given apply function, and the client provided closure */
typedef struct {
        UArray2b_T array2b;
        void (*apply)(int col, int row, UArray2b_T array2b,
                         void *elem, void *cl);
        void *cl;
} Map_Info;


/********** block_major ********
 *
 * Runs row-major within each block
 *
 * Parameters:
 *      int i: The col position of a block (within the UArray2)
 *      int j: The row position of a block (within the UArray2)
 *      UArray2_T a: The UArray2 that is holding all of the blocks
 *      void* p1: The current block
 *      void* p2: Map_Info that is passed so block major can be appropiately
 *      run
 *
 * Return: N/A
 *
 * Notes:
 *      Will only check elements that are in bounds
 ************************/
void block_major (int i, int j, UArray2_T a, void *p1, void *p2)
{
       
        Map_Info info = *(Map_Info *)p2;
        (void) a;
        int blksize = info.array2b->blocksize;
        (void) p1;
        /* Calculate the index of the top left element of a block
        based on blocksize and the postion of the block in the UArray2 */
        int col = i * blksize;
        int  row = j * blksize;
        /* Go row by row within a block */
        for (int x = 0; x < blksize; x++) {
                row = (j * blksize) + x;
                /* Each row iterate through the columns */
                for (int y = 0; y < blksize; y++) {
                        col = (i * blksize) + y;
                        /* Only run apply if the element is within bounds */
                        if (col < info.array2b->width &&
                        row < info.array2b->height) {
                                info.apply(col, row, info.array2b,
                                           UArray2b_at(info.array2b, col, row),
                                           info.cl);
                        }
                }
               
        }




}


/********** UArray2b_map ********
 *
 * Run block major on the given UArray2b
 *
 * Parameters:
 *      UArray2b_t array2b: The UArray2b_T to find the blocksize of
 *      void apply: The apply function to be run on each element
 *      void *cl: The closure that avaliable everytime a new element is gone to
 *
 * Return: N/A
 *
 * Expects:
 *      array2b to not be NULL
 *      apply to not be NULL
 *      
 * Notes:
 *      We go from one block to another in a row major format
 *      Will CRE if array2b is NULL
 *      Will CRE if apply is NULL
 ************************/
void UArray2b_map(UArray2b_T array2b,
                         void apply(int col, int row, UArray2b_T array2b,
                         void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
        assert(apply != NULL);
        /* Populate the Map_Info struct that will be passed as closure into
        the UArray2 map */
        Map_Info info = {array2b, apply, cl};
        UArray2_map_row_major(array2b->blocks, block_major, &info);
       
}
