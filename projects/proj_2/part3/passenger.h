/*
  Andrue Peters
  3/6/19
*/

#ifndef _GROUP_15_MY_ELEV_PASSENGER_H_
#define _GROUP_15_MY_ELEV_PASSENGER_H_

#define LOAD_SCALE (2)


enum my_elev_pass_type {
  MY_ELEV_NONE,
  MY_ELEV_ADULT,
  MY_ELEV_CHILD,
  MY_ELEV_ROOMSERVICE,
  MY_ELEV_BELLHOP
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
struct my_elevator_passenger* my_elev_new_passenger(int pass_type, int dest_floor);


















#endif
