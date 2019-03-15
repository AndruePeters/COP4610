/*
  Andrue Peters
  3/1/19

  Elevator Scheduler
*/
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include "elevator.h"

extern struct task_struct *thread_elev_sched;



/*
  Initializes extern struct my_elevator elev to default values.
*/
int init_my_elevator(struct my_elevator *elev)
{
  elev->num_passengers = 0;
  elev->total_load = 0;
  elev->curr_floor = 1;
  elev->total_passengers = 0;
  elev->state = MY_ELEV_IDLE;
  init_floors(elev->floors);
  elev->init = 1;
  INIT_LIST_HEAD(&elev->pass_list);
  mutex_init(&(elev->mtx));
  mutex_unlock(&(elev->mtx));
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
long my_elev_start_elevator(void)
{
  long ret = 0;
  if (elev.init == 0) {
    init_my_elevator(&elev);
    thread_elev_sched = kthread_run(my_elev_scheduler, (void *)&elev, "elevator scheduler");
    printk(KERN_WARNING "global elev: %px\n", &elev);

    if (IS_ERR(thread_elev_sched)) {
      printk(KERN_WARNING "error spwaning thread\n");
      remove_proc_entry(ENTRY_NAME, NULL);
      return PTR_ERR(thread_elev_sched);
    }
  } else {
    ret = 1;
  }
  return ret;
}

/*
  Creates a passenger of type passenger_type at start_floor that wishes to go
  to destination_floor. This function returns 1 if the request is not valid
  (one of the variables is out of range), and 0 otherwise.
*/
long my_elev_issue_request(int passenger_type, int start_floor, int destination_floor)
{
  int ret = 1;
  if (mutex_lock_interruptible(&(elev.mtx)) == 0) {
    printk(KERN_INFO "calling add_passenger()\n");
    ret =  add_passenger(elev.floors, passenger_type, start_floor, destination_floor);

    // start elevator only if it's idle
    if (elev.state == MY_ELEV_IDLE) {
      if (elev.curr_floor > start_floor) {
        elev.state = MY_ELEV_DOWN;
      } else if (elev.curr_floor < start_floor) {
        elev.state = MY_ELEV_UP;
      }
    }
  } else {
    printk(KERN_WARNING "Can't aquare lock in issue_request\n");
  }
  mutex_unlock(&(elev.mtx));
  return ret;
}

/*
  Deactivates elevator. At this point, the elevator will process no more requests.
  However, it has to offload all of its current passengers.
*/
long my_elev_stop_elevator(void)
{
  long ret = 0;
  kthread_stop(thread_elev_sched);
  mutex_destroy(&elev.mtx);
}

/*
  Main thread that runs in order to actually schedule the elevator.
*/
int my_elev_scheduler(void *e)
{
  struct my_elevator *elev = e;
  static int curr_floor, floor;
  enum my_elev_state curr_state;
  void (*elev_move_func)(struct my_elevator *elev);
  curr_floor = 1;

  printk(KERN_WARNING "Threaded elev: %px\n", elev);
  while(!kthread_should_stop()) {
    if (my_elev_get_state(elev) == MY_ELEV_IDLE) {
      msleep(200);
      continue;
    }

    //printk(KERN_WARNING "IN THREAD\n");
    my_elev_unload(elev);
    my_elev_load(elev);

    if (my_elev_get_state(elev) == MY_ELEV_UP) {
      my_elev_up_floor(elev);
      ++curr_floor;
    } else if (my_elev_get_state(elev) == MY_ELEV_DOWN) {
      my_elev_down_floor(elev);
      --curr_floor;
    }

    if (curr_floor <= 1) {
      my_elev_set_state(elev, MY_ELEV_UP);
    } if (curr_floor >= 10) {
      my_elev_set_state(elev, MY_ELEV_DOWN);
    }

    msleep(100);
  }

}


/*
  Sleeps for a specified amount of time.
  To be used while loading and unloading passengers and moving floors.
  time is in seconds.
*/
void my_elev_sleep(int time)
{
  msleep(time * 1000);
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
void my_elev_up_floor(struct my_elevator *elev)
{
  if (mutex_lock_interruptible(&(elev->mtx)) == 0) {
    if (elev->curr_floor < MAX_FLOOR) {
      ++(elev->curr_floor);
      my_elev_sleep(TIME_BTW_FLOORS);
    }
  }
  mutex_unlock(&(elev->mtx));

}

/*
  Moves elevator down a single floor.
  Does nothing if elevator is already at MIN_FLOOR
*/
void my_elev_down_floor(struct my_elevator *elev)
{
  if (mutex_lock_interruptible(&(elev->mtx)) == 0) {
    if (elev->curr_floor > MIN_FLOOR) {
      --(elev->curr_floor);
      my_elev_sleep(TIME_BTW_FLOORS);
    }
  }
  mutex_unlock(&(elev->mtx));
}

/*
  Unloads elevator; essentially by deleting users in a list.
  Unloads all passengers it can.
  Works by deleting entries in pass_list;

  Unloads based upon elev.curr_floor;
*/
void my_elev_unload(struct my_elevator *elev)
{
  struct list_head *pos, *pos_next;
  struct my_elev_passenger *ep  = NULL;
  enum my_elev_state prev_state = MY_ELEV_OFFLINE;

  if (mutex_lock_interruptible(&(elev->mtx)) == 0) {
    prev_state = elev->state;
    elev->state = MY_ELEV_LOADING;
    list_for_each_safe(pos, pos_next, &elev->pass_list) {
      ep = list_entry(pos, struct my_elev_passenger, list);
      if (ep->dest_floor == elev->curr_floor) {
        elev->num_passengers -= my_elev_get_pass_units(ep->pass_type);
        elev->total_load -= my_elev_get_pass_load(ep->pass_type);
        list_del(pos);
        kfree(ep);
      }
    }
    elev->state = prev_state;
  }
  mutex_unlock(&(elev->mtx));
}

/*
  Loads as many passengers as possible.
  Works by moving entries from floors[] to pass_list;

  Loads based upon elev.curr_floor
*/
void my_elev_load(struct my_elevator *elev)
{
  struct list_head *pos, *pos_next;
  struct my_elev_passenger *ep = NULL;
  enum my_elev_state prev_state = MY_ELEV_OFFLINE;
  int pass_units, pass_load;

  if (mutex_lock_interruptible(&(elev->mtx)) == 0) {
    prev_state = elev->state;
    elev->state = MY_ELEV_LOADING;

    list_for_each_safe(pos, pos_next, &(elev->floors[elev->curr_floor - 1].pass_list)) {
      ep = list_entry(pos, struct my_elev_passenger, list);
      pass_units = my_elev_get_pass_units(ep->pass_type);
      pass_load = my_elev_get_pass_load(ep->pass_type);

      if ((pass_units + elev->num_passengers <= MAX_PASSENGERS) &&
          (pass_load + elev->total_load <= MAX_LOAD))             {
            list_move_tail(pos, &elev->pass_list);
            elev->num_passengers += pass_units;
            elev->total_load += pass_load;
            elev->floors[elev->curr_floor-1].num_pass_serviced += my_elev_get_pass_units(pass_units);
      }
    }
    elev->state = prev_state;
  }
  mutex_unlock(&(elev->mtx));
}


/*
  Loads and unloads the elevator at curr_floor.
*/
void my_elev_unload_load(struct my_elevator *elev)
{
  my_elev_unload(elev);
  my_elev_load(elev);
  my_elev_sleep(TIM_BTW_PASSENGER);
}
/*
  Returns string with information for elevator.
  To be written to proc. Make sure it is freed after use.

  Adding every entry comes out to under 550 characters.
  Exactly 534, but using a few extra bytes as a buffer.
*/
char* my_elev_dump_info(struct my_elevator *elev)
{
  char *msg, *floor_buff;
  int num_floor_pass[MAX_FLOOR], i;

  printk(KERN_INFO "in my_elev_dump_info()\n");
  msg = kmalloc(sizeof(char) * 550, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
  printk(KERN_WARNING "Got msg allocation\n");
  if (mutex_lock_interruptible(&(elev->mtx)) == 0) {
    printk(KERN_WARNING "Got lock\n");
    if (!msg) {
      printk(KERN_WARNING "Unable to allocate in my_elev_dump_info\n");
      msg = NULL;
    } else {
      // form first part of printed string
      printk(KERN_WARNING "Foramtting string\n");
      sprintf(msg, "Elevator State: %s\n"
                    "Current Floor: %d\n"
                    "Next Floor: %d\n"
                    "Curr Load: %d\n", my_elev_state_char(elev), elev->curr_floor, 0, elev->total_load);
      printk(KERN_INFO "msg before floor: %s", msg);
      // get the number of waiting passengers for each floor and create string and then append
      for (i=0; i < MAX_FLOOR; ++i) {
        num_floor_pass[i] = get_load_pass_floor(&(elev->floors[i]));
        floor_buff = kmalloc(sizeof(char) * 50, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

        if (!floor_buff) {
          printk(KERN_WARNING "floor_buff couldn't be allocated\n");
          kfree(msg);
          msg = NULL;
          break;
        } else {
          sprintf(floor_buff, "Floor %d\n\tLoad: %d\n\t#Pass Serviced: %d\n",
                  i+1, get_load_pass_floor(&(elev->floors[i])), elev->floors[i].num_pass_serviced);
          strcat(msg, floor_buff);
          kfree(floor_buff);
        }
      }
    }
  }
  mutex_unlock(&(elev->mtx));
  return msg;
}

/*
  Returns the state of the elevator.
  Returns a const pointer, so don't try freeing it.
*/
const char* my_elev_state_char(struct my_elevator *elev)
{
  char *state;
    switch(elev->state) {
      case MY_ELEV_OFFLINE: state = "OFFLINE"; break;
      case MY_ELEV_IDLE: state = "IDLE"; break;
      case MY_ELEV_LOADING: state = "LOADING"; break;
      case MY_ELEV_UP: state = "UP"; break;
      case MY_ELEV_DOWN: state = "DOWN"; break;
      default: state = NULL; break;
    }
  return state;
}

/*
  Returns the current floor of the elevator.
*/
int my_elev_curr_floor(struct my_elevator *elev)
{
  int cf = 1;
  if (mutex_lock_interruptible(&(elev->mtx)) == 0) {
    cf = elev->curr_floor;
  }
  mutex_unlock(&(elev->mtx));
  return cf;
}

/*
  Returns 1 if user at this floor.
  Returns 0 if no passenger to get off.
  This function can definitely be initialized in the future,
  but iterating through a list isn't too bad with 10 passengers.
*/
bool my_elev_stop_at_floor(struct my_elevator *elev)
{
  struct list_head *pos;
  struct my_elev_passenger *ep = NULL;
  bool ret = false;
  if (mutex_lock_interruptible(&elev->mtx) == 0) {
    list_for_each(pos, &(elev->floors[elev->curr_floor-1].pass_list)) {
      ep = list_entry(pos, struct my_elev_passenger, list);
      if (ep->dest_floor == elev->curr_floor) {
        ret = true;
        break;
      }
    }
  }
  mutex_unlock(&elev->mtx);
  return ret;
}

/*
  Returns the next floor the elevator should move to.
  Optimize in future for thoroughput.
*/
int my_elev_floor_to_move_to(struct my_elevator *elev)
{
  int floor = 0;
  if (mutex_lock_interruptible(&elev->mtx) == 0) {
    if (elev->curr_floor == 1) {
      floor = 2;
    } else if (elev->curr_floor == 10) {
      floor = 9;
    } else if (elev->state == MY_ELEV_UP) {
      floor = elev->curr_floor + 1;
    } else if (elev->state == MY_ELEV_DOWN) {
      floor = elev->curr_floor - 1;
    }
  }
  mutex_unlock(&elev->mtx);
}

/*
  Sets state of elevator.
  Don't use inside function with a lock.
*/
void my_elev_set_state(struct my_elevator *elev, enum my_elev_state s)
{
  if (s < MY_ELEV_OFFLINE || s > MY_ELEV_DOWN) {
    return;
  }

  if (mutex_lock_interruptible(&elev->mtx) == 0) {
    elev->state = s;
  }
  mutex_unlock(&elev->mtx);
}

/*
  Gets current state. Don't use inside a function with a lock.
*/
enum my_elev_state my_elev_get_state(struct my_elevator *elev)
{
  enum my_elev_state s;
  if (mutex_lock_interruptible(&elev->mtx) == 0) {
    s = elev->state;
  }
  mutex_unlock(&elev->mtx);
  return s;
}
