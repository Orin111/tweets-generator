#ifndef markov_chain_h
#define MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE "Allocation failure: \
Failed to allocate new memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/

typedef void *element;
typedef void (*print_func) (element);
typedef int (*comp_func) (element, element);
typedef void (*free_data) (element);
typedef element (*copy_func) (element);
typedef bool (*is_last) (element);

/***************************/



/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNode
{
    void *data;
    struct NextNodeCounter **counter_list;
    int size;
} MarkovNode;

typedef struct NextNodeCounter
{
    MarkovNode *markov_node;
    int frequency;
} NextNodeCounter;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain
{
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    print_func print_func;

    // pointer to a func that gets 2 pointers of generic data
    // type(same one) and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    comp_func comp_func;

    // a pointer to a function that gets a pointer of generic data
    // type and frees it.
    // returns void.
    free_data free_data;

    // a pointer to a function that  gets a pointer of generic data
    // type and returns a newly allocated copy of it
    // returns a generic pointer.
    copy_func copy_func;

    //  a pointer to function that gets a pointer of generic data
    //  type and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    is_last is_last;
} MarkovChain;
/**
 *
 * @param max_number
 * @return a random number between 0 to max_number
 */
int get_random_number (int max_number);
/**
 * first allocate of first_node's counter_list and add second_node to the
 * counter_list of first_node
 * @param first_node
 * @param second_node
 * @return true upon success, otherwise false
 */
bool
add_first_node_to_c_list (MarkovNode *first_node, MarkovNode *second_node);

/**
 * reallocate the counter list of the first node and add the second node
 * @param first_ptr
 * @param second_node
 * @param markov_chain
 * @return true upon success, otherwise false
 */
bool reallocate_c_list (MarkovNode **first_ptr, MarkovNode *second_node,
                        MarkovChain *markov_chain);
/**
 * free a single markov_node
 * @param ptr_node
 * @param markov_chain
 */
void
free_markov_node (MarkovNode **ptr_node, MarkovChain *markov_chain);
/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random
 * markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain (MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it
 * in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's markov_node. Otherwise, create new
 * markov_node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database
 */
Node *add_to_database (MarkovChain *markov_chain, void *data_ptr);

#endif /* markov_chain_h */
