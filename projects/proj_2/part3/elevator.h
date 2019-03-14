/*
  Andrue Peters
  3/1/19

*/
#ifndef _GROUP_15_ELEV_MOD_H_
#define _GROUP_15_ELEV_MOD_H_
#include <linux/list.h>
#include <linux/mutex.h>

#include <passenger.h>
#include <floor.h>
#include <my_elev_config.h>




enum my_elev_state {
  MY_ELEV_OFFLINE = 0x00,
  MY_ELEV_IDLE,
  MY_ELEV_LOADING,
  MY_ELEV_UP,
  MY_ELEV_DOWN
};

/*
  Contains information for the current elevator.

  Rules:  my_elevtor.passengers cannot exceed MAX_PASSENGERS
          my_elevator.load cannot exceed MAX_LOAD

*/
struct my_elevator {
  int num_passengers;
  int total_load;
  int curr_floor;
  int total_passengers;
  enum my_elev_state state;
  struct floor floors[MAX_FLOOR];
  struct list_head pass_list;
  struct mutex mtx;
};


extern struct my_elevator elev;

/*
  Public Methods
*/

/*
  Initializes all values within an my_elevator struct.
*/
int init_my_elevator(void);

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
int start_elevator(void);

/*
  Creates a passenger of type passenger_type at start_floor that wishes to go
  to destination_floor. This function returns 1 if the request is not valid
  (one of the variables is out of range), and 0 otherwise.
*/
int issue_request(int passenger_type, int start_floor, int destination_fllor);

/*
  Deactivates elevator. At this point, the elevator will process no more requests.
  However, it has to offload all of its current passengers.
*/
int stop_elevator(void);

/*
  Private Methods
  Placed here only for easy documentation.
*/

/*
  Sleeps for the specified amount of time.
*/
void my_elev_sleep(int time);

/*
  Moves to specified floor.
*/
void my_elev_move_to_floor(int floor);

/*
  Goes up a single floor.
  Does nothing if floor == MAX_FLOOR
*/
void my_elev_up_floor(void);

/*
  Goes down a single floor.
  Does nothing if floor == 1
*/
void my_elev_down_floor(void);

/*
  Unloads all available people at current floor.
*/
void my_elev_unload(void);

/*
  Loads all available people at current floor.
*/
void my_elev_load(void);

/*
  Returns string with information for elevator.
  To be written to proc. Make sure it is freed after use.
*/
char* my_elev_dump_info(void);

/*
  Returns the state of the elevator.
  Returns a const pointer, so don't try freeing it.
*/
const char* my_elev_state(void);
#endif
