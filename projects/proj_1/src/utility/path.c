#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>


#include "path.h"

/* Dumps all data from a GQueue * */
static void print_queue(GQueue* q);

/* Checks to see if pointer is null before it is freed */
static void safe_free(void *ptr);

/*
  Handles expanding shortcuts like ~ and pwd and returns a concatenation of
  said shortcut and p.
*/
static char* expand_shortcuts(const char* p);

/*
  Separates strtok_r with '/' as a delimiter.
  Stores tokens in a GQueue struct.
*/
static void tokenize_path(const char* p, GQueue* q);

/*
  Converts a GQueue* object to a c string.
*/
static char* queue_to_string(GQueue* q);

/*
  Frees all allocated data in GQueue struct.
  Does not unlink nodes.
*/
static void free_queue_data(GQueue* q);


/*
  Returns absolute path from p.
*/
char *get_full_path(const char* p)
{
  if (!p) {
    printf("In function get_full_path, p is null.\n");
    return NULL;
  }

  if (!is_valid_path(p)) {
    printf("In get_full_path, %s is not a valid path.\n", p);
    return NULL;
  }

  char* path = strdup(p), *path_bak=path;
  GQueue* file_q = g_queue_new();
  path = expand_shortcuts(path);
  safe_free(path_bak);
  path_bak = path;
  tokenize_path(path, file_q);
  path = queue_to_string(file_q);
  free_queue_data(file_q);
  g_queue_free(file_q);

  safe_free(path_bak);
  return path;
}

/*
  Handles expanding shortcuts like ~ and pwd and returns a concatenation of
  said shortcut and p.
*/
char* expand_shortcuts(const char* p)
{
  if (!p) return NULL;

  char *exp=NULL;

  if (p[0] == '~') {
    /* no +1 because omitting first character */
    exp = expand_home(p);
  } else if (p[0] != '/') {
    /* Must be relative path at this point */
    exp = expand_pwd(p);
  } else {
    exp = strdup(p);
  }

  return exp;
}

/*
  Separates strtok_r with '/' as a delimiter.
  Stores tokens in a GQueue struct.
*/
void tokenize_path(const char* p, GQueue* q)
{
  char* token, *cpy, *saveptr = NULL, *tmp;

  printf("In tokenize_path, p = %s\n", p);
  /* Copy p into cpy */
  cpy = strdup(p);

  /* Tokenize and store each "filename" in fp */
  token = strtok_r(cpy, "/", &saveptr);
  while (token != NULL) {
    if (strcmp(token, "..") == 0) {
      free (g_queue_pop_tail(q));
    } else if (strcmp(token, ".") != 0) {
      tmp = strdup(token);
      g_queue_push_tail(q, tmp);
    }
    token = strtok_r(NULL, "/", &saveptr);
  }
  free(cpy);
}

/*
  Converts a GQueue* object to a c string.
*/
char* queue_to_string(GQueue* q)
{
  char* path = calloc(2, sizeof(char));
  char* path_bak = NULL;
  GList *walk = NULL;
  path[0] = '/'; path[1] = '\0';

  walk = g_queue_peek_head_link(q);
  while (walk) {
    path_bak = path;
    path = concat_path_m2(path, walk->data);
    safe_free(path_bak);
    walk = walk->next;
  }
  printf("q to string: %s\n", path);
  return path;
}

/*
  Returns true if the path is valid.
*/
bool is_valid_path(const char* path)
{
  bool valid_path = true;

  if (path == NULL) {
    printf("Invalid. path is null.\n");
    valid_path = false;
    return valid_path;
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
  concat_path_ method 2.
  This method was written because the first easily introduced memory leaks into the program.
  concats p1 and p2 together, accounting for position of slashes in string.
*/
char* concat_path_m2(const char* p1, const char* p2)
{
  if (!p1 && !p2) return NULL;
  int s1 = strlen(p1);
  int s2 = strlen(p2);

  char *res = NULL;
  int res_size = 0;
  int sec_offset = 0;
  bool insert_slash = 0;


    /* Now we should be good to go */
    /* Check for different combinations of '/' */
    if (p1[s1 - 1] == '/' && p2[0] != '/') {
      res_size = s1 + s2 + 1;
      sec_offset = 0;
    } else if (p1[s1 - 1] == '/' && p2[0] == '/') {
      res_size = s1+ s2-1 + 1;
      sec_offset = 1;
    } else if (p1[s1- 1] != '/' && p2[0] == '/') {
      res_size = s1+ s2 + 1;
      sec_offset = 0;
    } else if (p1[s1 -1] != '/' && p2[0] != '/') {
      res_size = s1+ s2+ 1 + 1;
      sec_offset = 0;
      insert_slash = 1;
    }

    res = calloc(res_size, sizeof(char));

    /* This means we need to insert a '/' in between the strings */
    if (insert_slash) {
      snprintf(res, res_size, "%s/%s", p1, p2);
    } else {
      snprintf(res, res_size, "%s%s", p1, (p2 + sec_offset));
    }

    return res;
}

/*
  Combines two paths and stores result in result.
  Note that memory is allocated in result, so make sure it is freed when done.
  Only used for reference. Easily causes memory leaks.
*/
void concat_path(const char* first, const char* sec, char** result)
{
  /* Check for bad conditions */
  if (!first && !sec) return;

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
  char* res = NULL;
  int res_size = 0;
  int sec_offset = 0;
  bool insert_slash = 0;

  /* Now we should be good to go */
  /* Check for different combinations of '/' */
  if (first[s1 - 1] == '/' && sec[0] != '/') {
    res_size = s1 + s2 + 1;
    sec_offset = 0;
  } else if (first[s1 - 1] == '/' && sec[0] == '/') {
    res_size = s1+ s2-1 + 1;
    sec_offset = 1;
  } else if (first[s1- 1] != '/' && sec[0] == '/') {
    res_size = s1+ s2 + 1;
    sec_offset = 0;
  } else if (first[s1 -1] != '/' && sec[0] != '/') {
    res_size = s1+ s2+ 1 + 1;
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
  if (!(*result)) free((*result));
  (*result) = res;
}



/*
  Returns pointer to string with $HOME expanded to form
  string in form of $Home/src ... /User/username/src
*/
char* expand_home(const char* src)
{
  char* home = getenv("HOME");
  char* exp = NULL;
  exp = concat_path_m2(home, src);
  return exp;
}

/*
  Doesn't currently do anything.
*/
void expand_path(char** p)
{

}


/*
  Returns pointer to string with $PWD expanded to form
  string in the form of $PWD/src ... /User/username/Documents/project1/src

  Free returned pointer when done.
*/
char* expand_pwd(const char* src)
{
  char* pwd = getenv("PWD");
  char* exp = NULL;
  exp = concat_path_m2(pwd, src);
  return exp;
}


/*
  Returns true if file exists.
  False otherwise or if p is NULL
*/
bool file_exists(const char* p)
{
  bool fl_exists = true;
  if (!p) {
    fl_exists = false;
  } else {
    struct stat s;
    if (stat(p, &s) != 0) fl_exists = false;
    if (errno == ENOENT) fl_exists = false;
  }
  return fl_exists;
}

/*
  Returns true if path is a file. Returns false otherwise.
*/
bool is_file(const char* p)
{
  if (!p) return false;

  struct stat s;
  if (stat(p, &s) != 0)  return 0;
  return ((s.st_mode & S_IFMT) == S_IFREG) || ((s.st_mode & S_IFMT) == S_IFDIR);
}

/*
  Returns true if path is directory. Returns false otherwise.
*/
bool is_dir(const char* p)
{
  if (!p) return false;

  struct stat s;
  if (stat(p, &s) != 0) return 0;
  return (s.st_mode & S_IFMT) == S_IFDIR;
}

/*
  Only calls free on ptr if ptr != NULL
*/
void safe_free(void *ptr)
{
  if(ptr) free(ptr);
}

/*
  Dumps all data in GQueue data structure.
*/
void print_queue(GQueue* q) {
  GList* walk = g_queue_peek_head_link(q);
  while(walk) {
    printf("%s ", walk->data);
    walk = walk->next;
  }
  printf("\n");
}

/*
  Frees all allocated memory in GQueue*.
  Does not unlink elements and free the node itself.
*/
void free_queue_data(GQueue* q)
{
  GList* walk;
  walk = g_queue_peek_head_link(q);
  while (walk) {
    if(walk->data) free(walk->data);
    walk = walk->next;
  }
}

/*
  Returns true if there were no environmental variables or all found were present.
  If all environmental variables exist, then the expanded path is stored in dest.
  If no environmental variables exist, then dest is a copy of src, but must be freed.

  Returns false if src is NULL, or an incorrect environmental variable is found.

  If an incorrect environmental variable was found, then dest == NULL.
*/
bool expand_env(char *dest, const char *src)
{
  if (!src) return false;


}
