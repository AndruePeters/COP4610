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
  Accepts and processes arguments after alias.

  Project description says command will only be given in the form "alias alias_name='alias'
*/
void alias(const char* c)
{
  if (!c) {

  }

  int name_start_pos=0, name_end_pos=0;
  int alias_start_pos=0, alias_end_pos=0;
  int key_size = 0, val_size=0;
  int len = strlen(c);
  int i = 0;
  char* key, *val;

  /* Find first alphanumeric character */
  for (i = 0; i < len; ++i) {
    if (isalnum(c[i])) {
      name_start_pos = i;
      break;
    }
  }

  /* Find last alaphanumeric character that's part of the alias name */
  for (i = name_start_pos; i < len; ++i) {
    if (c[i] == '=') {
      name_end_pos = i;
      break;
    }
  }

  /* Find the first character that's part of the actual alias */
  for (i = name_end_pos; i < len; ++i) {
    if (c[i] == '\'' || c[i] == '\"') {
      alias_start_pos = i+1;
      break;
    }
  }

  /* Find the last character that's part of the actual alias */
  for (i = len-1; i > alias_start_pos; ++i) {
    if (c[i] == '\'' || c[i] == '\"') {
      alias_end_pos = i;
      printf("alias_end_pos char: %c\n", c[i]);
      break;
    }
  }

  key_size = name_end_pos - name_start_pos;
  val_size = alias_end_pos - alias_start_pos;
  key = calloc(key_size + 1, sizeof(char));
  val = calloc(val_size + 1, sizeof(char));


  strncpy(key, c+name_start_pos, key_size);
  strncpy(val, c+alias_start_pos, val_size);
  key[key_size] = '\0';
  val[val_size] = '\0';


  add_alias(key, val);
  free(key);
  free(val);
}


void unalias(const char* c)
{
  rem_alias(c);
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
  if(c) {
    free(c);
  }
}
