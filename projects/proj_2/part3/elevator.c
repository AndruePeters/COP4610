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
  mutex_unlock(&elev.mtx);
  return 0;
}

/*
  Activates elevator service. From that point onward, the elevator exists
  and will begin to service requests. This system call will return 1 if the
  elevator is already active, 0 for a successful elevator start, and -ERRORNUM
  if it could not initialize (e.g. -ENOMEM if it couldn't allocate memory.)
  Initialize elvator as follows:
      State: idle
      current floor: 1
      current load 0 units, 0 weight
*/
int start_elevator(void)
{

}

/*
  Creates a passenger of type passenger_type at start_floor that wishes to go
  to destination_floor. This function returns 1 if the request is not valid
  (one of the variables is out of range), and 0 otherwise.
*/
int issue_request(int passenger_type, int start_floor, int destination_floor)
{
  int ret;
  if (mutex_lock_interruptible(&elev.mtx) == 0) {
    printk(KERN_INFO "calling add_passenger()\n");
    ret =  add_passenger(elev.floors, passenger_type, start_floor, destination_floor);
  } else {
    printk(KERN_WARNING "Can't aquare lock in issue_request\n");
  }
  mutex_unlock(&elev.mtx);
  return ret;
}

/*
  Deactivates elevator. At this point, the elevator will process no more requests.
  However, it has to offload all of its current passengers.
*/
int stop_elevator(void)
{

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
  if (mutex_lock_interruptible(&elev.mtx) == 0) {
    if (elev.curr_floor == MAX_FLOOR) {
      return;
    }

    my_elev_sleep(TIME_BTW_FLOORS);
    ++elev.curr_floor;
  }
  mutex_unlock(&elev.mtx);
}

/*
  Moves elevator down a single floor.
  Does nothing if elevator is already at MIN_FLOOR
*/
void my_elev_down_floor(void)
{
  if (mutex_lock_interruptible(&elev.mtx) == 0) {
    if (elev.curr_floor == MIN_FLOOR) {
      return;
    }

    my_elev_sleep(TIME_BTW_FLOORS);
    --elev.curr_floor;
  }
  mutex_unlock(&elev.mtx);
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

  if (mutex_lock_interruptible(&elev.mtx) == 0) {
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
  mutex_unlock(&elev.mtx);
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

  if (mutex_lock_interruptible(&elev.mtx) == 0) {
    list_for_each_safe(pos, pos_next, &(elev.floors[elev.curr_floor - 1].pass_list)) {
      ep = list_entry(pos, struct my_elev_passenger, list);
      pass_units = my_elev_get_pass_units(ep->pass_type);
      pass_load = my_elev_get_pass_load(ep->pass_type);

      if ((pass_units + elev.num_passengers <= MAX_PASSENGERS) &&
          (pass_load + elev.total_load <= MAX_LOAD))             {
            list_move_tail(pos, &elev.pass_list);
            elev.num_passengers += pass_units;
            elev.total_load += pass_load;
            elev.floors[elev.curr_floor-1].num_pass_serviced += my_elev_get_pass_units(pass_units);
      }
    }
  }
  mutex_unlock(&elev.mtx);
}

/*
  Returns string with information for elevator.
  To be written to proc. Make sure it is freed after use.

  Adding every entry comes out to under 550 characters.
  Exactly 534, but using a few extra bytes as a buffer.
*/
char* my_elev_dump_info(void)
{
  printk(KERN_INFO "in my_elev_dump_info()\n");
  char *msg, *floor_buff;
  int num_floor_pass[MAX_FLOOR], i;

  msg = kmalloc(sizeof(char) * 550, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

  if (mutex_lock_interruptible(&elev.mtx) == 0) {
    if (!msg) {
      printk(KERN_WARNING "Unable to allocate in my_elev_dump_info\n");
      msg = NULL;
    } else {
      // form first part of printed string
      sprintf(msg, "Elevator State: %s\n"
                    "Current Floor: %d\n"
                    "Next Floor: %d\n"
                    "Curr Load: %d\n", my_elev_state(), elev.curr_floor, 0, elev.total_load);
      printk(KERN_INFO "msg before floor: %s", msg);
      // get the number of waiting passengers for each floor and create string and then append
      for (i=0; i < MAX_FLOOR; ++i) {
        num_floor_pass[i] = get_load_pass_floor(&elev.floors[i]);
        floor_buff = kmalloc(sizeof(char) * 50, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

        if (!floor_buff) {
          printk(KERN_WARNING "floor_buff couldn't be allocated\n");
          kfree(msg);
          msg = NULL;
          break;
        } else {
          sprintf(floor_buff, "Floor %d\n\tLoad: %d\n\t#Pass Serviced: %d\n",
                  i+1, get_load_pass_floor(&elev.floors[i]), elev.floors[i].num_pass_serviced);
          strcat(msg, floor_buff);
          kfree(floor_buff);
        }
      }
    }
  }
  mutex_unlock(&elev.mtx);
  return msg;
}

/*
  Returns the state of the elevator.
  Returns a const pointer, so don't try freeing it.
*/
const char* my_elev_state(void)
{
  char *state;
  if (mutex_lock_interruptible(&elev.mtx) == 0) {
    switch(elev.state) {
      case MY_ELEV_OFFLINE: state = "OFFLINE"; break;
      case MY_ELEV_IDLE: state = "IDLE"; break;
      case MY_ELEV_LOADING: state = "LOADING"; break;
      case MY_ELEV_UP: state = "UP"; break;
      case MY_ELEV_DOWN: state = "DOWN"; break;
      default: state = NULL; break;
    }
  }
  mutex_unlock(&elev.mtx);
  return state;
}
