/*
  Andrue Peters
  2/1/19
*/

#include <builtins/alias.h>
#include <builtins/alias.c>

#include <stdio.h>
#include <stdlib.h>

void test_add_alias();

int main()
{
  init_alias();
  test_add_alias();
  dump_alias();
  return 0;
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
