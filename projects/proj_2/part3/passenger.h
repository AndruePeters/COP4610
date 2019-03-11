/*
  Andrue Peters
  3/6/19
*/

#ifndef _GROUP_15_MY_ELEV_PASSENGER_H_
#define _GROUP_15_MY_ELEV_PASSENGER_H_
#define LOAD_SCALE (2)

#include "floor.h"

enum my_elev_pass_type {
  MY_ELEV_NONE,
  MY_ELEV_ADULT,
  MY_ELEV_CHILD,
  MY_ELEV_ROOMSERVICE,
  MY_ELEV_BELLHOP,
  NUM_PASS_TYPES // automatically the number passenger types
};


struct my_elev_passenger {
  enum my_elev_pass_type pass_type;
  int dest_floor;
  struct list_head pass_list;
};




/*
  Calculates and returns the load of a passenger type.
*/
int my_elev_get_pass_load(int pass_type);

/*
  Returns the number of units for a passenger type.
*/
int my_elev_get_pass_units(int pass_type);

/*
  Returns the address of a new passenger.
  Does not do bounds checking. That should be done by the calling function.
*/
struct my_elev_passenger* my_elev_new_passenger(int pass_type, int dest_floor);

/*
  Prints debug information for passenger.
*/
void my_elev_print_pass(const struct my_elev_passenger * pass);

/*
  Does not type check.
*/
void my_elev_set_pass_type(struct my_elev_passenger *pass, int pass_type);

void my_elev_set_pass_load(struct my_elev_passenger *pass, int pass_type);

















#endif
