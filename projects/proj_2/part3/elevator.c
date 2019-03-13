/*
  Andrue Peters
  3/1/19

  Elevator Scheduler
*/
#include <linux/delay.h>
#include <elevator.h>




int init_my_elevator(void)
{
  elev.num_passengers = 0;
  elev.total_load = 0;
  elev.curr_floor = 1;
  elev.total_passengers = 0;
  elev.state = MY_ELEV_IDLE;
  init_floors(elev.floors);
  INIT_LIST_HEAD(&elev.pass_list);
  return 0;
}

void my_elev_sleep(int time)
{
  ssleep(time);
}
