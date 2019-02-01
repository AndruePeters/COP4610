/*
  Andrue Peters
  2/1/19
*/

#include <builtins/alias.h>
#include <builtins/alias.c>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_add_alias();
void test_alias();

int main()
{
  init_alias();
  //test_add_alias();
  test_alias();
  dump_alias();
  terminate_alias();
  return 0;
}

void test_alias()
{
    char command[200];
    char alias_var[200];
    bool should_exit = false;

    do {
      printf("Enter your command: ");
      scanf("%s %s", command, alias_var);
      printf("You entered %s %s\n", command, alias_var);

      if (strcmp(command, "exit") == 0) break;
      else {
        alias(alias_var);
      }
    } while (!should_exit);
}

void test_add_alias()
{
  char key[200];
  char val[200];
  bool should_exit = false;

  do {
    printf("Enter a key and value: ");
    scanf("%s %s", key, val);
    printf("You entered\t\tkey: %s\t\tval: %s\n", key, val);

    if(strcmp(key, "0") == 0) should_exit = true;
    else {add_alias(key, val); printf("key: %s\n", key); }
  } while(!should_exit);

}
