/*
  Andrue Peters
  3/6/19
*/

#ifndef _GROUP_15_MY_ELEV_FLOOR_H_
#define _GROUP_15_MY_ELEV_FLOOR_H_


#define MAX_FLOOR (10)
/*
  Stores passengers.
*/

/*
  List of passengers on each floor.
*/
struct floor {
  struct list_head pass_list;
};



/*
  Initializes each list element in the floors array.
*/
int init_floors(void);

/*
  Cleans up memory for the floors.
*/
int cleanup_floors(void);


/*
  Creates a passenger of type passenger_type at start_floor that wishes to go
  to destination_floor. This function returns 1 if the request is not valid
  (one of the variables is out of range), and 0 otherwise.

  Called by issue_request in elev_mod.h as a system call.
*/
int add_passenger(int passenger_type, int start_floor, int destination_floor);

/*
  Unlinks and returns the first passenger of the floor.
*/
struct my_elev_passenger* floor_get_passenger(int floor);



#endif
