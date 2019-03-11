#include "passenger.h"

int my_elev_get_pass_load(int pass_type)
{
  int ret_load;
  switch(pass_type) {
    my_elev_adult:
      ret_load = 1 * LOAD_SCALE;
      break;

    my_elev_child:
      ret_load = 0.5 * LOAD_SCALE;
      break;
    my_elev_roomservice:
      ret_load = 2 * LOAD_SCALE;
      break;
    my_elev_bellhop:
      ret_load = 4 * LOAD_SCALE;
      break;
    default:
      ret_load = 0 * LOAD_SCALE;
      break;
  }
  return ret_load * LOAD_SCALE;
}

int my_elev_get_pass_units(int pass_type)
{
  int pass_unit;
  switch(pass_type) {
    my_elev_adult:
      pass_unit = 1;
      break;
    my_elev_child:
      pass_unit = 1;
      break;
    my_elev_roomservice:
      pass_unit = 2;
      break;
    my_elev_bellhop:
      pass_unit = 2;
      break;
  }

  return pass_unit;
}
