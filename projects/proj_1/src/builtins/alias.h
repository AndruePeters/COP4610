/*
  Andrue Peters
*/

#ifndef _GROUP_15_ALIAS_H_
#define _GROUP_15_ALIAS_H_

#include <stdbool.h>

/*
  Must be called before any other method is called.
*/
void init_alias();

/*
  Call to free all allocated memory concerning alias.
*/
void terminate_alias();

/*
  Accepts argument after alias.
*/
void alias(const char* c);

/*
  Removes an alias.
*/
void unalias(const char* c);

/*
  Checks to see if alias exists. If it does then it returns a pointer
  to the string.
*/
const char* expand_alias(const char* c);

/*
  Returns true if alias exists.
*/
bool exists_alias(const char* c);

/*
  Adds an alias.
*/
bool add_alias(const char* key, const char* data);

/*
  Removes an alias.
*/
bool rem_alias(const char* key);

/*
  Dumps all aliases in form "key: value"
*/
void dump_alias();






























#endif
