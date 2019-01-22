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
    sec_offset is used to start copying sec from either sec[0] or from sec[1]
      This is for the cases where first = "asdf/" and sec = "/asdf" and
                                  first = "asdf" and sec = "/asdf"; Simple way to remove extra '/'
      In the first case it combines to be "asdf/asdf" and the second case "asdf/asdf".
  */
  char* res;
  int res_size = 0;
  int sec_offset = 0;
  bool insert_slash = 0;

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
  } else if (first[strlen(first) -1] != '/' && sec[0] != '/') {
    res_size = strlen(first) + strlen(sec) + 1 + 1;
    sec_offset = 0;
    insert_slash = 1;
  }

  res = calloc(res_size, sizeof(char));

  /* This means we need to insert a '/' in between the strings */
  if (insert_slash) {
    snprintf(res, res_size, "%s/%s", first, sec);
  } else {
    snprintf(res, res_size, "%s%s", first, (sec + sec_offset));
  }

  *result = res;
}

void expand_prev(char** p)
{

}

void expand_home(char** p)
{

}

void expand_path(char** p)
{

}

void expand_pwd(char** p)
{
  /* getenv() returns a pointer to static data */
  char* pwd = getenv("PWD");
  char* exp = NULL;
  concat_path(pwd, *p, &exp);
  free(*p);
  *p = exp;
}

bool file_exists(const char* p)
{

}

bool is_file(const char* p)
{

}

bool is_dir(const char* p)
{

}
