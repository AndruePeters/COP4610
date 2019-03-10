/*
  Andrue Peters
  3/1/19

*/
#ifndef _GROUP_15_ELEV_MOD_H_
#define _GROUP_15_ELEV_MOD_H_

#define MAX_PASSENGERS (10)
#define LOAD_SCALE (2)
#define MAX_LOAD       (15 * LOAD_SCALE)
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
};

struct my_elev_passenger {
  enum my_elev_pass_type pass_type;
  int dest_floor;
};

enum my_elev_pass_type {
  my_elev_none = 0,
  my_elev_adult,
  my_elev_child,
  my_elev_roomservice,
  my_elev_bellhop
};

enum my_elev_state {
  OFFLINE = 0x00,
  IDLE,
  LOADING,
  UP,
  DOWN
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
int start_elevator();

int issue_request(int passenger_type, int start_floor, int destination_fllor);

int stop_elevator();

/*
  Private Methods
*/
void my_elev_sleep(int time);
int my_elev_get_load(int pass_type);
int my_elev_get_pass_units(int pass_type);
#endif
