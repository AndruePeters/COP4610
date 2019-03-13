/*
  Andrue Peters
  3/1/19

  Elevator Scheduler
*/
#include <linux/delay.h>
#include <linux/types.h>
#include <elevator.h>



/*
  Initializes extern struct my_elevator elev to default values.
*/
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

/*
  Sleeps for a specified amount of time.
  To be used while loading and unloading passengers and moving floors.
  time is in seconds.
*/
void my_elev_sleep(int time)
{
  ssleep(time);
}

/*
  Moves to elevator to a specific floor.
  TODO: Finish this.
*/
void my_elev_move_to_floor(int floor)
{

}

/*
  Moves elevator up a single floor.
  Does nothing if elevator already at MAX_FLOOR
*/
void my_elev_up_floor(void)
{
  if (elev.curr_floor == MAX_FLOOR) {
    return;
  }

  my_elev_sleep(TIME_BTW_FLOORS);
  ++elev.curr_floor;
}

/*
  Moves elevator down a single floor.
  Does nothing if elevator is already at MIN_FLOOR
*/
void my_elev_down_floor(void)
{
  if (elev.curr_floor == MIN_FLOOR) {
    return;
  }

  my_elev_sleep(TIME_BTW_FLOORS);
  --elev.curr_floor;
}

/*
  Unloads elevator; essentially by deleting users in a list.
  Unloads all passengers it can.
  Works by deleting entries in pass_list;
*/
void my_elev_unload(void)
{
  struct list_head *pos, *pos_next;
  list_for_each_safe(pos, pos_next, &elev.pass_list) {

  }
}

/*
  Loads as many passengers as possible.
  Works by moving entries from floors[] to pass_list;
*/
void my_elev_load(void)
{

}
