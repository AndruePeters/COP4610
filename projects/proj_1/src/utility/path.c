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

int get_num_slashes(int* buffer, int buff_size, char* str);
int filter_prev(int* slash_pos, int num_slash,  char* str);
void rem_space(char* des, const char* src);

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

/*
  Takes in a cstring, evaluates the parent directory symbol, and stores forms new string.
  Works lexigraphically through removing the directory before /../.

  Assumes *p has been dymanically allocated.
  Assumes *p is the absolute path.
*/
void expand_prev(char** p)
{
  if (p == NULL || *p == NULL || *(*p) == '\0') return;
  char* path = NULL;
  int num_slashes = 0;
  int slash_pos[50] = {0};

  //int get_num_slashes(int* buffer, int buff_size, char* str)
  num_slashes = get_num_slashes(slash_pos, 50, *p);

  printf("Original:\t%s\n", *p);

  int size = filter_prev(slash_pos, num_slashes, *p);
  path = calloc(size, sizeof(char));
  rem_space(path, *p);
  free(*p);
  *p = path;
  printf("Expanded:\t%s\n", path);
}

void expand_home(char** p)
{

}

void expand_path(char** p)
{

}


/*
  Forms string in the form $PWD/src.
  Stored in *dest.
*/
void expand_pwd(char** dest, const char* src)
{
  char* pwd = getenv("PWD");
  char* exp = NULL;
  concat_path(pwd, src, &exp);
  *dest = exp;
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

/*
  Accepts an int buffer to store position of slashes and size of buffer, and string.
  Returns the number of slashes in a path and stores positions in buffer
*/
int get_num_slashes(int* buffer, int buff_size, char* str)
{
  int str_size = strlen(str);
  int num_slashes = 0, i;
  for (i = 0; i < str_size && num_slashes < buff_size; ++i) {
    if (str[i] == '/') {
      buffer[num_slashes] = i;
      ++num_slashes;
    }
  }
  return num_slashes;
}

/*
  Accepts int array for position of slashes, number of slashes, and the original string.
  Returns the number of non-white spaces in cstring + 1.
  Replaces paths that shouldn't be there with whitespace.
*/
int filter_prev(int* slash_pos, int num_slash,  char* str)
{
  int new_size = strlen(str);
  int i = 0;
  for (i = 2; i < num_slash && i < num_slash; ++i) {
    if (str[slash_pos[i] -1] == '.' && str[slash_pos[i] -2] == '.') {
      memset(str + slash_pos[i-2], ' ', slash_pos[i] - slash_pos[i-2]);
      new_size = new_size - (slash_pos[i] - slash_pos[i-2]);
    }
  }
  return new_size + 1;
}

/*
  Copies src to des with whitespace removed.
  des must be large enough to accomodate a null character at the end.
*/
void rem_space(char* des, const char* src)
{
  int i, j;
  int src_sz = strlen(src);
  for (i = j = 0; i < src_sz; ++i) {
    if (src[i] != ' ') {
      des[j] = src[i];
      ++j;
    }
  }
  des[i] = '\0';
}
