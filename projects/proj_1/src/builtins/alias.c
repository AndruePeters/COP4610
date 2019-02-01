#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>

#include "alias.h"

/*
  Alias keys are stored here in this table.
  Not sure if this is best practice, but the scope
  is limited to this file.

  Must call init_alias() to allocate hash table.
  Must call terminate_alias() before closing program.
*/
static GHashTable* alias_table;

/*
  Both key and val in hashtable are char*, so the same destroy function can be used.
*/
static void destory_key_val(gpointer c);

void init_alias()
{
  alias_table = g_hash_table_new_full(g_str_hash, g_str_equal, destory_key_val, destory_key_val);
}

/*
  Call this method to free allocated memory to alias.
*/
void terminate_alias()
{
  g_hash_table_destroy (alias_table);
}

/*
  Returns true if key is found in hashtable. False otherwise.
  This relies on glib 2.32.
*/
bool exists_alias(const char* key)
{
  return g_hash_table_contains (alias_table, key);
}

/*
  Adds alias.
  Returns true if item did not exist and was added.
  Returns false if key or data == NULL or if key already existed.
*/
bool add_alias(const char* key, const char* data)
{
  bool alias_added;
  if (!key || !data) {
    printf("Null is an invalid value as an alias. Alias cannot be empty.\n");
    alias_added = false;
    return alias_added;
  }

  gpointer k_cpy, d_cpy;
  k_cpy = strdup(key);
  d_cpy = strdup(data);

  alias_added = (bool)g_hash_table_replace (alias_table, k_cpy, d_cpy);
  return alias_added;
}

/*
  Returns true if key was found and removed.
*/
bool rem_alias(const char* key)
{
  return (bool)g_hash_table_remove(alias_table, key);
}



/*
  Prints all aliases that are stored and their values.
*/
void dump_alias()
{
  GHashTableIter it;
  gpointer key, value;
  g_hash_table_iter_init(&it, alias_table);
  while (g_hash_table_iter_next(&it, &key, &value)) {
    printf("key: %s\t\tvalue: %s\n", (char*)key, (char*)value);
  }
}

void destory_key_val(gpointer c)
{
  if(c) free(c);
}
