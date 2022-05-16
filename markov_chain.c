#include "markov_chain.h"
#include <string.h>

int get_random_number (int max_number)
{
  return (rand () % max_number);
}

bool add_first_node_to_c_list (MarkovNode *first_node, MarkovNode *second_node)
{
  first_node->counter_list = malloc (sizeof (NextNodeCounter *));
  if (!first_node->counter_list)
    {
      return false;
    }
  NextNodeCounter *next_ptr = calloc (1, sizeof (NextNodeCounter));
  if (!next_ptr)
    {
      return false;
    }
  next_ptr->frequency = 1;
  next_ptr->markov_node = second_node;
  first_node->size = 1;
  first_node->counter_list[0] = next_ptr;
  return true;
}

bool reallocate_c_list (MarkovNode **first_ptr, MarkovNode *second_node,
                        MarkovChain *markov_chain)
{
  MarkovNode *first_node = *first_ptr;
  for (int i = 0; i < first_node->size; ++i)
    {
      NextNodeCounter *current = (first_node->counter_list)[i];
      if (markov_chain->comp_func (current->markov_node->data,
                                   second_node->data) == 0)
        {
          current->frequency++;
          return true;
        }
    }
  NextNodeCounter *next_ptr = malloc (sizeof (NextNodeCounter));
  if (!next_ptr)
    {
      return false;
    }
  next_ptr->frequency = 1;
  next_ptr->markov_node = second_node;

  first_node->counter_list = (NextNodeCounter **) realloc
      (first_node->counter_list, sizeof (NextNodeCounter *)
                                 * (first_node->size + 1));
  if (!first_node->counter_list)
    {
      free (next_ptr);
      return false;
    }
  first_node->size++;
  first_node->counter_list[first_node->size - 1] = next_ptr;
  return true;
}

void free_markov_node (MarkovNode **ptr_node, MarkovChain *markov_chain)
{
  if (*ptr_node)
    {
      if ((*ptr_node)->counter_list)
        {
          for (int i = 0; i < (*ptr_node)->size; ++i)
            {
              free ((*ptr_node)->counter_list[i]);
            }
          free ((*ptr_node)->counter_list);
        }
      markov_chain->free_data ((*ptr_node)->data);
      free (*ptr_node);
    }
}

MarkovNode *get_first_random_node (MarkovChain *Markov_chain)
{
  int random = get_random_number (Markov_chain->database->size);
  Node *current = Markov_chain->database->first;
  while (random)
    {
      current = current->next;
      random--;
    }
  return current->data;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  int sum = 0;
  for (int i = 0; i < state_struct_ptr->size; ++i)
    {
      sum += (state_struct_ptr->counter_list)[i]->frequency;
    }

  int random = get_random_number (sum);
  NextNodeCounter **next = state_struct_ptr->counter_list;

  int i = 0;
  while (random >= 0)
    {
      random -= next[i]->frequency;
      i++;
    }
  return (state_struct_ptr->counter_list[i - 1]->markov_node);
}

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if (!first_node)
    {
      first_node = get_first_random_node (markov_chain);
      while (markov_chain->is_last (first_node->data) == true)
        {
          first_node = get_first_random_node (markov_chain);
        }
    }
  MarkovNode *current = first_node;
  markov_chain->print_func (first_node->data);
  // we already have the first word
  int counter = 1;

  while (counter < max_length
         && markov_chain->is_last (current->data) == false)
    {
      current = get_next_random_node (current);
      markov_chain->print_func (current->data);
      counter++;
    }
  printf ("\n");
}

bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
  if (markov_chain->is_last (first_node->data) == true)
    {
      return true;
    }
  if (first_node->size == 0)
    {
      return add_first_node_to_c_list (first_node, second_node);
    }
  return reallocate_c_list (&first_node, second_node, markov_chain);
}

Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *current = markov_chain->database->first;
  while (current)
    {
      if (markov_chain->comp_func (current->data->data, data_ptr) == 0)
        {
          return current;
        }
      current = current->next;
    }
  return NULL;
}

Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *node = get_node_from_database (markov_chain, data_ptr);
  if (node == NULL)
    {
      MarkovNode *new_node = calloc (1, sizeof (MarkovNode));
      if (new_node == NULL)
        {
          return NULL;
        }
      void *new_data = markov_chain->copy_func (data_ptr);
      if (new_data == NULL)
        {
          free (new_node);
          return NULL;
        }
      new_node->data = new_data;
      if (add (markov_chain->database, new_node) == 1)
        {
          free (new_data);
          free (new_node);
          return NULL;
        }
      new_node->size = 0;
      return markov_chain->database->last;
    }
  return node;
}

void free_markov_chain (MarkovChain **ptr_chain)
{
  if (*ptr_chain)
    {
      Node *tmp, *cur = (*ptr_chain)->database->first;
      for (int i = 0; i < (*ptr_chain)->database->size; ++i)
        {
          tmp = cur->next;
          free_markov_node (&cur->data, *ptr_chain);
          free (cur);
          cur = tmp;
        }
      free ((*ptr_chain)->database);
      free (*ptr_chain);
    }
}