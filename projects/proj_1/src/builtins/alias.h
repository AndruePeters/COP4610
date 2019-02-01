/*
  Andrue Peters
*/

#ifndef _GROUP_15_ALIAS_H_
#define _GROUP_15_ALIAS_H_

#include <stdbool.h>

void init_alias();
void terminate_alias();

void alias(const char* c);
char* expand_alias(const char* c);
bool exists_alias(const char* c);
bool add_alias(const char* key, const char* data);
bool rem_alias(const char* key);
void dump_alias();






























#endif
