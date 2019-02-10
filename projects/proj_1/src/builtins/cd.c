#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utility/path.h>

#include "cd.h"

#define _CD_MAX_BUFFER_ (4096)

void update_shell_data_pwd(struct shell_data* sd);

void cd(int argc, char **argv)
{
  if (argc < 2) {
    printf("Invalid syntax for cd.\n");
    return;
  }
  char *file_path = NULL;
  char *path = argv[1];
  printf("argv[1]:%s\n\n", argv[1]);
  char *cwd = calloc(_CD_MAX_BUFFER_, sizeof(char));

  if ((file_path = get_full_path(path))) {
    if (is_dir(file_path) && (chdir(file_path) == 0)) {
      getcwd(cwd, _CD_MAX_BUFFER_);
      setenv("PWD", cwd, true);
    }
  }
  free(cwd);
  free(file_path);
}

void update_shell_data_pwd(struct shell_data* sd)
{
  if (sd->pwd) {
    free(sd->pwd);
  }

  sd->pwd = strdup(getenv("PWD"));
}
