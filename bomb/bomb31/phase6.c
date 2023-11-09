/******************************************************************************
 * phase6.c                                                                   *
 * David Chen (zchen18)                                                       *
 * Sam Hu (khu04)                                                             *
 *                                                                            *
 * HW5: bomb                                                                  *
 *                                                                            *
 * Summary:                                                                   *
 * Provides functionality of the assembly function "func6" and "phase 6"      *
 * written in c                                                               *
 *                                                                            * 
 *                                                                            *
 * Notes:                                                                     *
 *                                                                            *
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct List_Node {
        long val;
        struct List_Node *next;
} *List_Node;

const int LEN = 9;

const int list[9] = {800, 597, 338, 976, 709, 72, 677, 424, 694};

void explode_bomb();


/**************** construct_List ****************
 *
 * Construct a linked list of size LEN and return the head node
 *
 * Parameters:
 *      NONE
 *
 * Return:
 *      Returns the head node of the empty linked list
 *
 * Notes:
 *      - CRE if memory allocation fails
 *
 ************************/
List_Node construct_List()
{
        List_Node head, prev;
        for (int i = 0; i < LEN; i++) {
                List_Node cur = malloc(sizeof(*cur));
                assert(cur != NULL);
                cur->val = list[i];

                if (i == 0) {
                        head = cur;
                } else {
                        prev->next = cur;
                }
                prev = cur;
        }
        return head;

}

/**************** fun6 ****************
 *
 * Sort a linked list in non-increasing order
 *
 * Parameters:
 *      a List_Node object representing the head of the linked list
 *
 * Return:
 *      None, sorting in place
 *
 * Notes:
 *      - will terminate if the linked list is empty or of length 1
 *
 ************************/
void fun6(List_Node head)
{
        if (head == NULL || head->next == NULL) {
                return;
        }

        /* Left stores the sorted list of insertion */
        List_Node left = NULL;
        List_Node cur = head;
        List_Node nextNode = NULL;

        while (cur != NULL) {
                nextNode = cur->next;
                cur->next = NULL;
                /* Add the current node to the left of the sorted list */
                if (left == NULL || left->val <= cur->val) {
                        cur->next = left;
                        left = cur;
                /* Add the current node to its position in the sorted list */        
                } else {
                        List_Node temp = left;
                        while (temp->next != NULL 
                               && temp->next->val > cur->val) {
                                temp = temp->next;
                        }
                        cur->next = temp->next;
                        temp->next = cur;
                }
                cur = nextNode;
        }
        head = left;
}

/**************** phase6 ****************
 *
 * Check if the input integer (in the form of a char pointer) is 
 * equal to the third largest value of the linked list; explodes the 
 * bomb if it is not equal
 *
 * Parameters:
 *      a char pointer input representing the target long integer
 *
 * Return:
 *      None
 *
 ************************/
void phase6(char *input) 
{
        /* Convert input string to long */
        long input_long = strtol(input, NULL, 10);
        
        /* Construct the linked list and sort it */
        List_Node list_head = construct_List();
        fun6(list_head);

        /* Get the third biggest value and compare with input */
        list_head = list_head->next;
        list_head = list_head->next;
        if (list_head->val != input_long) {
                explode_bomb();
        }
}


