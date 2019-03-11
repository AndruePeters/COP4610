/*
  Andrue Peters
  3/1/19

  Elevator Scheduler
*/
#include <linux/delay.h>
#include <elevator.h>

void my_elev_sleep(int time)
{
  ssleep(time);
}
