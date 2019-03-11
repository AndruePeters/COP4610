#include <linux/list.h>
#include "floor.h"

/*
  Global array to hold passengers at each floor.
*/
static struct floor floors[MAX_FLOOR];


/*
  Initialize each list head.
*/
int init_floors(void)
{
  int i;
  for (i = 0; i < MAX_FLOOR) {
    INIT_LIST_HEAD(&(floors->pass_list));
  }
}
