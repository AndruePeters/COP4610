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

void init_alias()
{
  alias_table = g_hash_table_new(g_str_hash, g_str_equal);
}

void terminate_alias()
{
  
}
