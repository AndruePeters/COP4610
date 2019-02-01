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
char *get_full_path(const char* p);

char* expand_shortcuts(const char* p);

char* expand_home(const char* src);

void expand_path(char** p);

char* expand_pwd(const char* src);

bool file_exists(const char* p);

bool is_file(const char* p);

bool is_dir(const char* p);

void concat_path(const char* first, const char* sec, char** result);

char* concat_path_m2(const char* p1, const char* p2);

bool is_valid_path(const char* path);




































#endif
