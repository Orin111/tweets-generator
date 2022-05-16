#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define BASE 10
#define INVALID 0
#define VALID 1
#define NUM_OF_PARAM 3
#define ERR_NUM_OF_PARAM "Usage: the number of parameters should be 3"

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in
    // case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case
    // there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain);

/**
 *
 * @param error_msg Error message to print
 * @param database Pointer to pointer to MarkovChain
 * @return EXIT_FAILURE
 */
static int handle_error (char *error_msg, MarkovChain **database);

/**
 * create a bord in a given size
 * @param cells
 * @return return EXIT_SUCCESS upon success failre otherwise
 */
static int create_board (Cell *cells[BOARD_SIZE]);

/**
 * a function that fill the database
 * @param markov_chain
 * @return 1 upon success 0 otherwise
 */
static int fill_database (MarkovChain *markov_chain);

/**
 *
 * @param source
 * @return a copy of the cell
 */
static element copy_sl (element source);

/**
 * this func print a single cell
 * @param data a cell
 */
static void print_sl (element data);

/**
 * this function compare the data in both cells
 * @param data1
 * @param data2
 * @return 0 if data1->number = data2->number
 */
static int comp_sl (element data1, element data2);

/**
 * this func free a single cell
 * @param data
 */
static void free_sl (element data);

/**
 *
 * @param data
 * @return 1 if this is the last cell, 0 otherwise
 */
static bool is_last_sl (element data);

/**
 * this func set all markov chain fields
 * @param markov_chain
 */
static void set_markov_chain_sl (MarkovChain **markov_chain);

/**
 * this function generate and prints num_of_routes routes from the markov_chain
 * @param markov_chain
 * @param num_of_routes
 * @return  1 upon success 0 otherwise
 */
static int generate_all_routes (MarkovChain *markov_chain, int num_of_routes);

/**
 * this func run the program
 * @param argv
 * @return  1 upon success 0 otherwise
 */
static int run (char *argv[]);

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
    {
      free_markov_chain (database);
    }
  return EXIT_FAILURE;
}

static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
    {
      cells[i] = malloc (sizeof (Cell));
      if (cells[i] == NULL)
        {
          for (int j = 0; j < i; j++)
            {
              free (cells[j]);
            }
          handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
          return EXIT_FAILURE;
        }
      *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
      int from = transitions[i][0];
      int to = transitions[i][1];
      if (from < to)
        {
          cells[from - 1]->ladder_to = to;
        }
      else
        {
          cells[from - 1]->snake_to = to;
        }
    }
  return EXIT_SUCCESS;
}

static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
    {
      return EXIT_FAILURE;
    }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
    {
      add_to_database (markov_chain, cells[i]);
    }

  for (size_t i = 0; i < BOARD_SIZE; i++)
    {
      from_node = get_node_from_database (markov_chain, cells[i])->data;

      if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
          index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
          to_node = get_node_from_database (markov_chain, cells[index_to])
              ->data;
          add_node_to_counter_list (from_node, to_node, markov_chain);
        }
      else
        {
          for (int j = 1; j <= DICE_MAX; j++)
            {
              index_to = ((Cell *) (from_node->data))->number + j - 1;
              if (index_to >= BOARD_SIZE)
                {
                  break;
                }
              to_node = get_node_from_database (markov_chain, cells[index_to])
                  ->data;
              add_node_to_counter_list (from_node, to_node, markov_chain);
            }
        }
    }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
    {
      free (cells[i]);
    }
  return EXIT_SUCCESS;
}

static element copy_sl (element source)
{
  Cell *source_cell = source;
  Cell *target_cell = malloc (sizeof (Cell));
  if (target_cell != NULL)
    {
      target_cell->number = source_cell->number;
      target_cell->ladder_to = source_cell->ladder_to;
      target_cell->snake_to = source_cell->snake_to;
    }
  return target_cell;
}

static void print_sl (element data)
{
  Cell *data_c = data;
  int num = data_c->number, snake = data_c->snake_to,
      ladder = data_c->ladder_to;
  if (snake != -1)
    {
      printf ("[%d]-snake to %d -> ", num, snake);
      return;
    }
  if (ladder != -1)
    {
      printf ("[%d]-ladder to %d -> ", num, ladder);
      return;
    }
  if (num == BOARD_SIZE)
    {
      printf ("[%d]", BOARD_SIZE);
      return;
    }
  printf ("[%d] -> ", num);
}

static int comp_sl (element data1, element data2)
{
  Cell *data1_c = data1, *data2_c = data2;
  return (data1_c->number - data2_c->number);
}

static void free_sl (element data)
{
  free (data);
}

static bool is_last_sl (element data)
{
  Cell *data_c = data;
  return (data_c->number == BOARD_SIZE);
}

static void set_markov_chain_sl (MarkovChain **markov_chain)
{
  (*markov_chain)->copy_func = copy_sl;
  (*markov_chain)->comp_func = comp_sl;
  (*markov_chain)->is_last = is_last_sl;
  (*markov_chain)->print_func = print_sl;
  (*markov_chain)->free_data = free_sl;
}

static int generate_all_routes (MarkovChain *markov_chain, int num_of_routes)
{
  //creating the database
  if (fill_database (markov_chain) == EXIT_FAILURE)
    {
      free_markov_chain (&markov_chain);
      return INVALID;
    }
  for (int i = 0; i < num_of_routes; ++i)
    {
      printf ("Random Walk %d: ", i + 1);
      //generate and print a route
      MarkovNode *first_node = markov_chain->database->first->data;
      generate_random_sequence (markov_chain, first_node,
                                MAX_GENERATION_LENGTH);
    }
  return VALID;
}

static int run (char *argv[])
{
  // save all arguments
  unsigned int seed = (unsigned int) strtol (argv[1], NULL, BASE);
  srand (seed);
  int num_of_routes = (int) strtol (argv[2], NULL, BASE);
  //allocating memory
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (markov_chain == NULL)
    {
      return INVALID;
    }
  LinkedList *database = calloc (1, sizeof (LinkedList));
  if (database == NULL)
    {
      free (markov_chain);
      return INVALID;
    }
  markov_chain->database = database;
  set_markov_chain_sl (&markov_chain);
  if (!generate_all_routes (markov_chain, num_of_routes))
    {
      return INVALID;
    }
  free_markov_chain (&markov_chain);
  return VALID;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  {
    //check that the num of arg is correct
    if (argc != NUM_OF_PARAM)
      {
        fprintf (stdout, ERR_NUM_OF_PARAM);
        return EXIT_FAILURE;
      }
    if (!run (argv))
      {
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }
}
