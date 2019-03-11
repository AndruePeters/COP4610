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

/*
  Returns the address of a new passenger.
  Does not do bounds checking. That should be done by the calling function.
*/
struct my_elev_passenger* my_elev_new_passenger(int pass_type, int dest_floor)
{
  struct my_elev_passenger* ep = kmalloc( sizeof(struct my_elev_passenger), GFP_KERNEL);
  if (!p) {
    printk(KERN_WARNING "my_elev_new_passenger kmalloc failed.\n");
  }
  return ep;
}

/*
  Prints information for passenger
*/
void my_elev_print_pass(const struct my_elevator_passenger * pass)
{
  if (pass) {
    printk(KERN_INFO "passenger type:%d\npassenger load:%d\npassenger floor:%d\n",
            pass->pass_type, my_elev_get_pass_load(pass->pass_type), pass->dest_floor);
  } else {
    printk(KERN_WARNING "my_elev_print_pass NULL POINTER");
  }
}
