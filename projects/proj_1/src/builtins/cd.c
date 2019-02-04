#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utility/path.h>
#include <utility/path.c>

#include "cd.h"

void update_shell_data_pwd(struct shell_data* sd);

bool cd(const char* path, struct shell_data *sd)
{
  bool ret_stat = false;
  char *file_path = NULL;

  if ((file_path = get_full_path(path))) {
    if (is_dir(file_path) && (chdir(file_path) == 0)) {
      setenv("PWD", file_path, true);
      update_shell_data_pwd(sd);
    }
  }
  free(file_path);
  return ret_stat;
}

void update_shell_data_pwd(struct shell_data* sd)
{
  if (sd->pwd) {
    free(sd->pwd);
  }

  sd->pwd = strdup(getenv("PWD"));
}
