/*
  Andrue Peters

  This file contains functions responsible for handling matters concerning filepaths.
*/
#ifndef _GROUP15_UTILITY_PATH_H
#define _GROUP15_UTILITY_PATH_H

#include <stdbool.h>
#include <glib.h>

/*
  Pass in a const char* that is a file path and this returns the absolute path.
  If any error is encountered or the path passed has errors, then it returns null.

  A successful return does not mean that the filepath exists. You still much check it
  with is_file() or is_dir().

  Make sure to free the returned pointer once you're done to avoid memory leaks.
*/
char *get_full_path(const char* p);


/*
  Returns a string in form /home/$username/src or $HOME/src.
  Free returned pointer when finished.
*/
char* expand_home(const char* src);

/*
  Doesn't do anything yet.
  *WARNING
*/
char* expand_path(const char* file);

/*
  Returns a string in form of $PWD/src, where $PWD is expanded.
  Free returned pointer when finished.
*/
char* expand_pwd(const char* src);

/*
  Returns true if file/dir exists at path p.
  Returns false otherwise
*/
bool file_exists(const char* p);

/*
  Returns true if path p is a file.
  Returns false otherwise.
*/
bool is_file(const char* p);

/*
  Returns true if path p is a directory.
  Returns false otherwise.
*/
bool is_dir(const char* p);

/*
  Don't use this version unless you are really careful and *result has not been allocated.
  Kept only for reference.
*/
void concat_path(const char* first, const char* sec, char** result);

/*
  This version is safer to use to avoid memory leaks.
  Returns string in form "p1/p2".
  Free returned pointer when finished.
*/
char* concat_path_m2(const char* p1, const char* p2);

/*
  Returns true if path appears to be valid.
  Returns false otherwise.

  Does not check if file exists or not.
*/
bool is_valid_path(const char* path);

/*
  Returns a string with all environmental variables expanded.
*/
char* expand_env(const char *src);

/*
  Separates strtok_r with '/' as a delimiter.
  Stores tokens in a GQueue struct.
*/
void tokenize_path(const char* p, GQueue* q);

/*
  Converts a GQueue* object to a c string.
*/
char* queue_to_string(GQueue* q);

#endif
