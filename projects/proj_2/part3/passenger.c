#include <linux/slab.h>
#include <linux/kernel.h>
#include <passenger.h>


int my_elev_get_pass_load(int pass_type)
{
  int ret_load;
  switch(pass_type) {
    case MY_ELEV_ADULT:
      ret_load = 1 * LOAD_SCALE;
      break;

    case MY_ELEV_CHILD:
      ret_load = 0.5 * LOAD_SCALE;
      break;
    case MY_ELEV_ROOMSERVICE:
      ret_load = 2 * LOAD_SCALE;
      break;
    case MY_ELEV_BELLHOP:
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
    case MY_ELEV_ADULT:
      pass_unit = 1;
      break;
    case MY_ELEV_CHILD:
      pass_unit = 1;
      break;
    case MY_ELEV_ROOMSERVICE:
      pass_unit = 2;
      break;
    case MY_ELEV_BELLHOP:
      pass_unit = 2;
      break;
    default:
      pass_unit = 0;
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
  struct my_elev_passenger* ep = NULL;

  if (pass_type >= NUM_PASS_TYPES || pass_type < 0 || dest_floor <= 0 || dest_floor > MAX_FLOOR) {
    printk(KERN_WARNING "my_elev_new_passenger invalid pass_type or dest_fllor");
    printk(KERN_WARNING "pass_type:%d\ndest_floor:%d", pass_type, dest_floor);
  }

  ep = kmalloc( sizeof(struct my_elev_passenger), __GFP_RECLAIM);
  if (ep) {
    ep->pass_type = pass_type;
    ep->dest_floor = dest_floor;
    INIT_LIST_HEAD(&ep->list);
  } else {
    printk(KERN_WARNING "my_elev_new_passenger kmalloc failed.\n");
  }

  my_elev_print_pass(ep);
  return ep;
}



/*
  Prints information for passenger
*/
void my_elev_print_pass(const struct my_elev_passenger * pass)
{
  if (pass) {
    printk(KERN_INFO "passenger type:%d\npassenger load:%d\npassenger floor:%d\n",
            pass->pass_type, my_elev_get_pass_load(pass->pass_type), pass->dest_floor);
  } else {
    printk(KERN_WARNING "my_elev_print_pass NULL POINTER");
  }
}
