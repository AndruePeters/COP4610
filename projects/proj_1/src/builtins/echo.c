
#include <stdio.h>
#include <stdlib.h>
#include <utility/path.h>
#include "echo.h"

void echo(const char *line)
{
  if (!line) { return; }

  char *expanded = expand_env(line);
  if (expanded) {
    printf("%s\n", expanded);
    free(expanded);
  } else {
    printf("Error: environmental variable does not exist.\n");
  }
}
