#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "path.h"


struct fpath {
  struct list_head list;
  char[255] file;
}

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

static int get_num_slashes(int* buffer, int buff_size, char* str);
static int filter_dots(char** str);
static void rem_space(char* des, const char* src);
void expand_shortcuts(char** p);
void strsl(char** des, int start, int num);
void update_slash_pos(int* slash_pos, int size, int offset, int start);

void resolve_dots(const char* p, struct fpath* fp);
void tokenize_path(const char* p, struct fpath* fp);

char *get_path(const char* p)
{
  if (!p) {
    printf("In function get_path, p is null.\n");
    return;
  }

  if (!is_valid_path(p)) {
    printf("In get_path, %s is not a valid path.\n", p);
  }

  struct fpath fp;
  INIT_LIST_HEAD(&fp.list);


  char* path = NULL;
  enum Path_Type pt;
  if (p && is_valid_path(p)) {
    strcpy(path, p);
    pt = get_path_type(path);
    expand_shortcuts(&path);
  }

  return path;
}

void resolve_dots(const char* p, struct fpath* fp)
{
  struct fpath* tmp;
}


void tokenize_path(const char* p, struct fpath* fp)
{
  char* token, *cpy, *saveptr = NULL;
  struct fpath* tmp;

  /* Copy p into cpy */
  cpy = calloc( (strlen(p)+1), sizeof(char));
  strcpy(cpy, line);

  /* Tokenize and store each "filename" in fp */
  token = strtok_r(cpy, " ", &saveptr);
  while (token != NULL) {
    tmp = malloc(sizeof(struct fpath));
    strcpy(tmp->file, token);
    list_add(&(tmp->list), &(fp->list));
    token = strtok_r(NULL, " ", &saveptr);
  }
}
bool is_valid_path(const char* path)
{
  bool valid_path = true;

  if (path == NULL) {
    printf("Invalid. path is null.\n");
    valid_path = false;
  }

  int len = strlen(path);
  int i;

  /* Check for /.. where root is proceeded by a previous directory shortcut */
  if (path[0] == '/' && path[1] == '.' && path[2] == '.'
      && ( path[3] == '/' || isspace(path[3]))) {
    printf("Invalid path. No parent of root directory.\n");
    valid_path = false;
  }

  /* Make sure ~ only appears at the beginning */
  for (i = 1; i < len; ++i) {
    if (path[i] == '~') {
      printf("Invalid path. ~ can only appear once at beginning of path.\n");
      valid_path = false;
    }

    /* Make sure there are no double /'s */
    if (path[i] == '/' && path[i-1] == '/') {
      printf("Invalid path. Only only one / at a time.\n");
      valid_path = false;
    }
  }

  return valid_path;
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
  if (first == NULL) return;

  if (sec == NULL) {
    sec = calloc(strlen(first) + 1, sizeof(char));
  }

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

  printf("Original:\t%s\n", *p);

  filter_dots(p);
  printf("After filter_dots: %s\n", *p);
  //path = calloc(size, sizeof(char));
  //rem_space(path, *p);
  free(*p);
  *p = path;
  printf("Expanded:\t%s\n", path);
}

/*
  Forms string in the form $HOME/src
  Stores in *dest
*/
void expand_home(char** dest, const char* src)
{
  char* home = getenv("HOME");
  char* exp = NULL;
  concat_path(home, src, &exp);
  *dest = exp;
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


/*
  Returns true if file exists.
  False otherwise or if p is NULL
*/
bool file_exists(const char* p)
{
  bool fl_exists = true;
  if (!p) {
    return false;
  } else {
    struct stat s;
    if (stat(p, &s) != 0) fl_exists = false;
    if (errno == ENOENT) fl_exists = false;
  }
  return fl_exists;
}

bool is_file(const char* p)
{
  if (!p) return false;

  struct stat s;
  if (stat(p, &s) != 0)  return 0;
  return ((s.st_mode & S_IFMT) == S_IFREG) || ((s.st_mode & S_IFMT) == S_IFDIR);
}

bool is_dir(const char* p)
{
  if (!p) return false;

  struct stat s;
  if (stat(p, &s) != 0) return 0;
  return (s.st_mode & S_IFMT) == S_IFDIR;
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

  *WARNING Currently does not support paths like /User/druepeters/./../
*/
int filter_dots(char** str)
{
  int slash_pos[50] = {0};
  int num_slash = get_num_slashes(slash_pos, 50, *str);
  int i = 0;

  for (i = 2; i < num_slash; ++i) {
    if (*str[slash_pos[i]-1] == '.' && *str[slash_pos[i]-2] == '/') {
      strsl(str, slash_pos[i-1], slash_pos[i] - slash_pos[i-1]);
      update_slash_pos(slash_pos, num_slash, slash_pos[i] - slash_pos[i-1], i);
    }
  }

  printf("%s\n", *str);


  /* Works for double dot */
  /*for (i = 2; i < num_slash; ++i) {
    /* /......./dir/../...../ erase /dir/.. /
    if (str[slash_pos[i] -1] == '.' && str[slash_pos[i] -2] == '.') {
      memset(str + slash_pos[i-2], ' ', slash_pos[i] - slash_pos[i-2]);
      new_size = new_size - (slash_pos[i] - slash_pos[i-2]);
    }
  } */

  /* Works for single dot *
  for (i = 2; i < num_slash; ++i) {
    if (str[slash_pos[i] - 2] == '/' && str[slash_pos[i]-1] == '.') {
      memset(str + slash_pos[i-1], ' ', slash_pos[i] - slash_pos[i-1]);
      new_size = new_size - (slash_pos[i] - slash_pos[i-1]);
    }
  } */
  /*int j;
  for (i = 2; i < num_slash; ++i) {
    if (str[slash_pos[i]-1] == '.' && str[slash_pos[i]-2] == '/') {
      strsl(&str, slash_pos[i-1], slash_pos[i] - slash_pos[i-1]);
      update_slash_pos(slash_pos, num_slash, slash_pos[i] - slash_pos[i-1], i);
    }

    if (str[slash_pos[i]-1] == '.' && str[slash_pos[i]-2] == '.') {
      strsl(&str, slash_pos[i-2], slash_pos[i] - slash_pos[i-2]);
      update_slash_pos(slash_pos, num_slash, slash_pos[i] - slash_pos[i-2], i);
    } */
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

void expand_shortcuts(char** p)
{
  if (*p[0] == '~' ) {

  }
}

void strsl(char** des, int start, int num)
{
  if (!des || !(*des) || strlen(*des) < start + num) return;

  int i, size = strlen(*des);
  for (i = start; i < size - num; ++i) {
    (*des)[i] = (*des)[i+num];
    (*des)[i + num] = '\0';
  }
  (*des)[i+num+1] = '\0';
  printf("%s\n", *des);
}

void update_slash_pos(int* slash_pos, int size, int offset, int start)
{
  int i;
  for (i = 0; i < size; ++i) {
    printf("%d\t", slash_pos[i]);
  }
  printf("\n");
  for (i = start; i < size; ++i) {
    slash_pos[i] -= offset;
  }
  for (i = 0; i < size; ++i) {
    printf("%d\t", slash_pos[i]);
  }
  printf("\n");
}
