#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "ex2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = key;
  pair->value = value;
  pair->next = NULL;

  return pair;
}

// djb2 hash function
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char * u_str = (unsigned char *)str;

  while ((c = *u_str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL) free(pair);
}

HashTable *create_hash_table(int capacity)
{
  HashTable *ht = malloc(sizeof(HashTable));
  ht->capacity = capacity;
  ht->storage = calloc(capacity, sizeof(LinkedPair *));

  return ht;
}

void hash_table_insert(HashTable *ht, char *key, char *value)
{
  unsigned int index = hash(key, ht->capacity);

  LinkedPair *current_pair = ht->storage[index];
  LinkedPair *last_pair;

  while (current_pair != NULL && strcmp(current_pair->key, key) != 0) {
    last_pair = current_pair;
    current_pair = last_pair->next;
  }

  if (current_pair != NULL) {
    current_pair->value = value;
  } else {
    LinkedPair *new_pair = create_pair(key, value);
    new_pair->next = ht->storage[index];
    ht->storage[index] = new_pair;
  }
}

void hash_table_remove(HashTable *ht, char *key)
{
  unsigned int index = hash(key, ht->capacity);

  LinkedPair *current_pair = ht->storage[index];
  LinkedPair *previous_pair = NULL;

  while (current_pair != NULL && strcmp(current_pair->key, key) != 0) {
    previous_pair = current_pair;
    current_pair = current_pair->next;
  }

  if (current_pair == NULL) {

    fprintf(stderr, "Unable to remove entry with key: %s\n", key);

  } else {

    if (previous_pair == NULL) {  // Removing the first element in the Linked List
      ht->storage[index] = current_pair->next;
    } else {
      previous_pair->next = current_pair->next;
    }

    destroy_pair(current_pair);
  }
}

char *hash_table_retrieve(HashTable *ht, char *key)
{
  unsigned int index = hash(key, ht->capacity);

  LinkedPair *current_pair = ht->storage[index];

  while (current_pair != NULL) {
    if (strcmp(current_pair->key, key) == 0) {
      return current_pair->value;
    }
    current_pair = current_pair->next;
  }

  return NULL;
}

void destroy_hash_table(HashTable *ht)
{
  LinkedPair *current_pair;
  LinkedPair *pair_to_destroy;

  for (int i = 0 ; i < ht->capacity ; i++) {
    current_pair = ht->storage[i];
    while(current_pair != NULL) {
      pair_to_destroy = current_pair;
      current_pair = current_pair->next;
      destroy_pair(pair_to_destroy);
    }
  }

  free(ht->storage);
  free(ht);
}

HashTable *hash_table_resize(HashTable *ht)
{
  HashTable *new_ht = create_hash_table(2 * ht->capacity);

  LinkedPair *current_pair;
  for (int i = 0 ; i < ht->capacity ; i++) {
    current_pair = ht->storage[i];
    while (current_pair != NULL) {
      hash_table_insert(new_ht, current_pair->key, current_pair->value);
      current_pair = current_pair->next;
    }
  }
  destroy_hash_table(ht);

  return new_ht;
}

char **reconstruct_trip(Ticket **tickets, int length)
{
  HashTable *ht = create_hash_table(length);
  char **route = malloc(length * sizeof(char *));
  /* YOUR CODE HERE */
  // For each flight create a Linked Pair or source and destination. 
  for(int i = 0; i <length; i++){
    hash_table_insert(ht, tickets[i]->source, tickets[i]->destination);
  }
  // Init start 
  char *start = "NONE";
  // Find start and loop through 
  for(int i = 0; i <length; i++){
    start = hash_table_retrieve(ht, start);
    route[i] = start;
    }
  return route;
}

void print_route(char **route, int length)
{
  for (int i = 0; i < length; i++) {
    printf("%s\n", route[i]);
  }
}



#ifndef TESTING
int main(void)
{
  // Short test
  Ticket **tickets = malloc(3 * sizeof(Ticket *));

  Ticket *ticket_1 = malloc(sizeof(Ticket));
  ticket_1->source = "NONE";
  ticket_1->destination = "PDX";
  tickets[0] = ticket_1;

  Ticket *ticket_2 = malloc(sizeof(Ticket));
  ticket_2->source = "PDX";
  ticket_2->destination = "DCA";
  tickets[1] = ticket_2;

  Ticket *ticket_3 = malloc(sizeof(Ticket));
  ticket_3->source = "DCA";
  ticket_3->destination = "NONE";
  tickets[2] = ticket_3;

  print_route(reconstruct_trip(tickets, 3), 3); // PDX, DCA, NONE

  for (int i = 0; i < 3; i++) {
    free(tickets[i]);
  }

  free(tickets);

  return 0;
}
#endif
