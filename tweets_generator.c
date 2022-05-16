#include "markov_chain.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define INVALID 0
#define VALID 1
#define DOT '.'
#define DELI " \r\n"
#define MAX_L_TEXT 1001
#define BASE 10
#define NUM_OF_PARAM "Usage: the number of parameters should be 3 or 4"
#define INVALID_FILE "Error: The given file is invalid.\n"
#define MAX_LENGTH 20
#define WITH_NUM_LINES 5
#define WITHOUT_NO_NUM_LINES 4

/**
 * this func set all markov chain fields
 * @param markov_chain
 */
static void set_markov_chain (MarkovChain **markov_chain);

/**
 *
 * @param source
 * @return a copy of the source element
 */
static element copy_str (element source);

/**
 * this func print a given string
 * @param data
 */
static void print_str (element data);

/**
 *
 * @param data
 */
static void free_str (element data);

/**
 *
 * @param data1
 * @param data2
 * @return 0 if data1=data2 else otherwise
 */
static int comp_str (element data1, element data2);

/**
 *
 * @param node
 * @return 0 if the data of the nod has "." in the end, 1 else
 */
static bool end_with_dot (element data);

/**
 *
 * @param path file location
 * @return an open file if valid, else NULL
 */
static FILE *check_file_validation (char path[]);

/**
 *
 * @param fp an open file of the tweets
 * @param words_to_read number of words to read from the file
 * @param markov_chain an empty markov chain
 */
static int
fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain);

/**
 *
 * @param open_file an open file of the tweets
 * @param words_to_read number of words to read from the file
 * @param markov_chain  a markov chain with all the words from the file
 * @param num_of_tweets number of tweets we should generate
 * @return 1 for success 0 otherwise
 */
static int generate_tweets (FILE *open_file, int words_to_read,
                            MarkovChain *markov_chain, int num_of_tweets);
/**
 *
 * @param num_of_tweets number of tweets we should generate
 * @param fp a path of file
 * @param words_to_read number of words to read from the file
 * @return 1 for success 0 otherwise
 */
static int run (int num_of_tweets, char *fp, int words_to_read);

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  //check that the num of arg is correct
  if (argc > WITH_NUM_LINES || argc < WITHOUT_NO_NUM_LINES)
    {
      fprintf (stdout, NUM_OF_PARAM);
      return EXIT_FAILURE;
    }
  // save all arguments
  unsigned int seed = (unsigned int) strtol (argv[1], NULL, BASE);
  srand (seed);
  int num_of_tweets = (int) strtol (argv[2], NULL, BASE);
  char *fp = argv[3];
  int words_to_read = -1;
  if (argc == WITH_NUM_LINES)
    {
      words_to_read = (int) strtol (argv[4], NULL, BASE);
    }
  if (!run (num_of_tweets, fp, words_to_read))
    {
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static int run (int num_of_tweets, char *fp, int words_to_read)
{
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

  // checking the file
  FILE *open_file = check_file_validation (fp);
  if (open_file == NULL)
    {
      free (markov_chain);
      free (database);
      return INVALID;
    }
  set_markov_chain (&markov_chain);
  if (!generate_tweets (open_file, words_to_read, markov_chain, num_of_tweets))
    {
      fclose (open_file);
      return INVALID;
    }
  free_markov_chain (&markov_chain);
  fclose (open_file);
  return VALID;
}

static void set_markov_chain (MarkovChain **markov_chain)
{
  (*markov_chain)->copy_func = copy_str;
  (*markov_chain)->comp_func = comp_str;
  (*markov_chain)->is_last = end_with_dot;
  (*markov_chain)->print_func = print_str;
  (*markov_chain)->free_data = free_str;
}

static element copy_str (element source)
{
  char *source_str = source;
  char *target_str = malloc ((strlen (source_str) + 1) * sizeof (char));
  if (target_str != NULL)
    {
      strcpy (target_str, source_str);
    }
  return target_str;
}

static void print_str (element data)
{
  char *data_c = data;
  if (end_with_dot (data) == true)
    {
      printf ("%s", data_c);
    }
  else
    {
      printf ("%s ", data_c);
    }
}

static int comp_str (element data1, element data2)
{
  char *data1_c = data1;
  char *data2_c = data2;
  return strcmp (data1_c, data2_c);
}

static void free_str (element data)
{
  free (data);
}

static bool end_with_dot (element data)
{
  char *data_c = data;
  size_t len = strlen (data_c);
  if ((data_c[len - 1]) == DOT)
    {
      return true;
    }
  return false;
}

static int generate_tweets (FILE *open_file, int words_to_read,
                            MarkovChain *markov_chain, int num_of_tweets)
{
  //creating the database
  if (fill_database (open_file, words_to_read, markov_chain) == INVALID)
    {
      free_markov_chain (&markov_chain);
      return INVALID;
    }
  for (int i = 0; i < num_of_tweets; ++i)
    {
      printf ("Tweet %d: ", i + 1);
      //generate and print a sequence
      generate_random_sequence (markov_chain, NULL, MAX_LENGTH);
    }
  return VALID;
}

static int
fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char str[MAX_L_TEXT];
  Node *prev_node = NULL, *cur_node = NULL;
  while (fgets (str, MAX_L_TEXT, fp) != NULL)
    {
      char *token = strtok (str, DELI);
      while (token != NULL)
        {
          if (words_to_read != -1)
            {
              words_to_read--;
            }
          cur_node = add_to_database (markov_chain, token);
          if (cur_node == NULL)
            {
              return INVALID;
            }
          //handling counter list
          if (prev_node != NULL)
            {
              if (add_node_to_counter_list
                      (prev_node->data, cur_node->data, markov_chain) == false)
                {
                  return INVALID;
                }
            }
          if (words_to_read == 0)
            {
              return VALID;
            }
          token = strtok (NULL, DELI);
          prev_node = cur_node;
        }
    }
  return VALID;
}

static FILE *check_file_validation (char path[])
{
  FILE *source = fopen (path, "r");
  if (source == NULL)
    {
      fprintf (stdout, INVALID_FILE);
      return NULL;
    }
  else
    {
      return source;
    }
}
