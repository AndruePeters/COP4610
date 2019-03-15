/*
  Stores information related to configuration of elevator module.
*/

#ifndef _GROUP_15_MY_ELEV_CONFIG_H_
#define _GROUP_15_MY_ELEV_CONFIG_H_

// Pasenger information
#define MAX_PASSENGERS    (10)
#define LOAD_SCALE        (2)

// Floor information
#define MIN_FLOOR      (1)
#define MAX_FLOOR         (10)

// Elevator Information
#define MAX_LOAD          (15 * LOAD_SCALE)
#define TIME_BTW_FLOORS   (2)
#define TIM_BTW_PASSENGER (1)



#define ENTRY_NAME "my_elev"
#define ENTRY_SIZE 100
#define PERMS 0644
#define PARENT NULL

#endif
