/*
  Andrue Peters
  Assumptions: Can free and malloc all char* arguments (aside from const char* obviously)
*/
#ifndef _GROUP15_UTILITY_PATH_H
#define _GROUP15_UTILITY_PATH_H

#include <stdbool.h>

enum Path_Type { PATH_ABS, PATH_HOME, PATH_CURR, PATH_PREV, PATH_PATH, PATH_ERR };

/*
  Returns the absolute path to a file.
  If the path doesn't exist then returns NULL.

  Allocation is called in this version, so make sure you free it.
*/
char *get_path(const char* p);

void expand_prev(char** p);

void expand_home(char** dest, const char* src);

void expand_path(char** p);

void expand_pwd(char** dest, const char* src);

bool file_exists(const char* p);

bool is_file(const char* p);

bool is_dir(const char* p);

void concat_path(const char* first, const char* sec, char** result);

bool is_valid_path(const char* path);

enum Path_Type get_path_type(const char* p);


































#endif
