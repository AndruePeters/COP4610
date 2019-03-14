/*
  Andrue Peters
  3/1/19

  Elevator Scheduler
*/
#include <linux/delay.h>
#include <linux/slab.h>
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
  mutex_init(&elev.mtx);
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
  if (mutex_lock_interruptible(&elev.mtx)) {
    if (elev.curr_floor == MAX_FLOOR) {
      return;
    }

    my_elev_sleep(TIME_BTW_FLOORS);
    ++elev.curr_floor;
  }
}

/*
  Moves elevator down a single floor.
  Does nothing if elevator is already at MIN_FLOOR
*/
void my_elev_down_floor(void)
{
  if (mutex_lock_interruptible(&elev.mtx)) {
    if (elev.curr_floor == MIN_FLOOR) {
      return;
    }

    my_elev_sleep(TIME_BTW_FLOORS);
    --elev.curr_floor;
  }
}

/*
  Unloads elevator; essentially by deleting users in a list.
  Unloads all passengers it can.
  Works by deleting entries in pass_list;

  Unloads based upon elev.curr_floor;
*/
void my_elev_unload(void)
{
  struct list_head *pos, *pos_next;
  struct my_elev_passenger *ep  = NULL;

  if (mutex_lock_interruptible(&elev.mtx)) {
    list_for_each_safe(pos, pos_next, &elev.pass_list) {
      ep = list_entry(pos, struct my_elev_passenger, list);
      if (ep->dest_floor == elev.curr_floor) {
        elev.num_passengers -= my_elev_get_pass_units(ep->pass_type);
        elev.total_load -= my_elev_get_pass_load(ep->pass_type);
        list_del(pos);
        kfree(ep);
      }
    }
  }
}

/*
  Loads as many passengers as possible.
  Works by moving entries from floors[] to pass_list;

  Loads based upon elev.curr_floor
*/
void my_elev_load(void)
{
  struct list_head *pos, *pos_next;
  struct my_elev_passenger *ep = NULL;
  int pass_units, pass_load;

  if (mutex_lock_interruptible(&elev.mtx)) {
    list_for_each_safe(pos, pos_next, &(elev.floors[elev.curr_floor - 1].pass_list)) {
      ep = list_entry(pos, struct my_elev_passenger, list);
      pass_units = my_elev_get_pass_units(ep->pass_type);
      pass_load = my_elev_get_pass_load(ep->pass_type);

      if ((pass_units + elev.num_passengers <= MAX_PASSENGERS) &&
          (pass_load + elev.total_load <= MAX_LOAD))             {
            list_move_tail(pos, &elev.pass_list);
            elev.num_passengers += pass_units;
            elev.total_load += pass_load;
      }
    }
  }
}
