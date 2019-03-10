/*
  Andrue Peters
  3/1/19

*/
#ifndef _GROUP_15_ELEV_MOD_H_
#define _GROUP_15_ELEV_MOD_H_
#include <linux/list.h>

#define MAX_PASSENGERS (10)
#define LOAD_SCALE (2)
#define MAX_LOAD       (15 * LOAD_SCALE)
#define BOTTOM_FLOOR (1)
#define TOP_FLOOR (10)
#define TIME_BTW_FLOORS (2)
#define TIM_BTW_PASSENGER (1)

/*
  Contains information for the current elevator.

  Rules:  my_elevtor.passengers cannot exceed MAX_PASSENGERS
          my_elevator.load cannot exceed MAX_LOAD

*/
struct my_elevator {
  int passengers;
  int load;
  enum my_elev_state state;
  struct my_elev_passenger;
};

struct my_elev_passenger {
  enum my_elev_pass_type pass_type;
  int dest_floor;
  struct list_head pass_list;
};


enum my_elev_pass_type {
  MY_ELEV_NONE,
  MY_ELEV_ADULT,
  MY_ELEV_CHILD,
  MY_ELEV_ROOMSERVICE,
  MY_ELEV_BELLHOP
};

enum my_elev_state {
  MY_ELEV_OFFLINE = 0x00,
  MY_ELEV_IDLE,
  MY_ELEV_LOADING,
  MY_ELEV_UP,
  MY_ELEV_DOWN
};


/*
  Public Methods
*/

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
int start_elevator();

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
int stop_elevator();

/*
  Private Methods
*/
void my_elev_sleep(int time);

/*
  Calculates and returns the load of a passenger type.
*/
int my_elev_get_load(int pass_type);

/*
  Returns the number of units for a passenger type.
*/
int my_elev_get_pass_units(int pass_type);

/*
  Returns the address of a new passenger.
  Does not do bounds checking. That should be done by the calling function.
*/
struct my_elevator_passenger* my_elev_new_passenger(int pass_type, int dest_floor);
#endif
