#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <bitpack.h>

bool Bitpack_fitsu(uint64_t n, unsigned width);


int main() {
    printf("Testing Bitpack_fitsu function for unsigned variables:\n");
    bool fitsu = Bitpack_fitsu(4,8);
    fprintf(stderr, "(3,5): %d \n", fitsu);

    printf("Testing Bitpack_fitss function for unsigned variables:\n");
    bool fitss = Bitpack_fitss(-5,3);
    fprintf(stderr, "(5,3): %d \n", fitss);

    uint64_t temp = 1012;
    printf("Testing Bitpack_getu function for unsigned variables:\n");
    fprintf(stderr, "(1012, 6, 2): %lu \n", Bitpack_getu(temp, 6, 2));

    uint64_t temp1 = 1012;
    printf("Testing Bitpack_gets function for unsigned variables:\n");
    fprintf(stderr, "(1012, 6, 2): %ld \n", Bitpack_gets(temp1, 6, 2));
    
    /*********** BITPACK_NEWU************* */
    uint64_t word = 0xFFFFFFFFFFFFFFFF;
    
    printf("Testing Bitpack_newu function for unsigned variables:\n");
    uint64_t result = Bitpack_newu(word, 64, 0, 10);
    // Ensure the field was replaced with the new value
    uint64_t expected = 0xFFFFFFFFFFFFFAFF;
    if (result == expected) {
        printf("Test Passed: Bitpack_newu works as expected.\n");
    } else {
        printf("Test Failed: Bitpack_newu did not return the expected result.\n result was :%lu", result);
    }   
    word = 0xFFFFFFFFFFFFFFFF;
    /* BITPACK_NEWS */
    printf("Testing Bitpack_news function for unsigned variables:\n");
    result = Bitpack_news(word, 64, 0, -7);
    // Ensure the field was replaced with the new value
        expected = 0xFFFFFFFFFFFFFEFF;
    if (result == expected) {
        printf("Test Passed: Bitpack_news works as expected.\n");
    } else {
        printf("Test Failed: Bitpack_news did not return the expected result.\n result was :%lu", result);
    }

}
