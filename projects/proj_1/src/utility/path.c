#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "path.h"

/*
  PATH_ABS is an absolute path, relative to root.
      Starts with '/'
  PATH_HOME is a path relative to $HOME directory.
      Starts with '~'
  PATH_CURR is a path relative to $PWD
      Starts with '.' or nothing.
  PATH_PREV is a path relative to previous directory.
      Starts with '..'
  PATH_PATH is a path relative to $PATH
      If this is a command, then check this last.
*/




char *get_path(const char* p)
{

}


/*
  Returns the Path_Type of p.
*/
enum Path_Type get_path_type(const char* p)
{
  if (p == NULL) return PATH_ERR;

  enum Path_Type pt;

  if (p[0] == '/') {
    pt = PATH_ABS;
  } else if (p[0] == '~') {
    pt = PATH_HOME;
  } else if ((strlen(p) > 2) && (p[0] == '.') && (p[1] == '.')) {
    pt = PATH_PREV;
  } else if (*p == '\0') {
    pt = PATH_ERR;
  } else {
    pt = PATH_CURR;
  }

  return pt;
}

/*
  Combines two paths and stores result in result.
  Note that memory is allocated in result, so make sure it is freed when done
*/
void concat_path(const char* first, const char* sec, char** result)
{
  /* Check for bad conditions */
  if (first == NULL || sec == NULL) return;
  int s1 = strlen(first);
  int s2 = strlen(sec);
  if (s1 < 1 || s2 < 1) return;

  /* Result stored in res
    res_size is the size needed to store res including null
    sec_offset is used to determine if the program shoudl start from the origin
      or one past for the case the strings appear with first = "..../" and
      sec = "/.../". It removes the duplicate '/' that would be included by
      copying both.
  */
  char* res;
  int res_size = 0;
  int sec_offset = 0;

  /* Now we should be good to go */
  /* Check for different combinations of '/' */
  if (first[strlen(first) - 1] == '/' && sec[0] != '/') {
    res_size = strlen(first) + strlen(sec) + 1;
    sec_offset = 0;
  } else if (first[strlen(first) - 1] == '/' && sec[0] == '/') {
    res_size = strlen(first) + strlen(sec) -1 + 1;
    sec_offset = 1;
  } else if (first[strlen(first) - 1] != '/' && sec[0] == '/') {
    res_size = strlen(first) + strlen(sec) + 1;
    sec_offset = 0;
  }

  res = calloc(res_size, sizeof(char));
  snprintf(res, res_size, "%s%s", first, sec + sec_offset);
  *result = res;
}
