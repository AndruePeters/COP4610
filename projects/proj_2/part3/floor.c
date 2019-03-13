#include <linux/list.h>
#include <linux/kernel.h>
#include <floor.h>
/*
  Global array to hold passengers at each floor.
*/
static struct floor floors[MAX_FLOOR];


/*
  Initialize each list head.
*/
void init_floors(void)
{
  int i;
  for (i = 0; i < MAX_FLOOR; ++i) {
    INIT_LIST_HEAD(&(floors[i].pass_list));
  }
  printk(KERN_INFO "Exited init_floors");
}

/*
  Cleanup memory for the floors.
*/
void cleanup_floors(void)
{

}

/*
  Creates a passenger of type passenger_type at start_floor that wishes to go
  to destination_floor. This function returns 1 if the request is not valid
  (one of the variables is out of range), and 0 otherwise.

  Called by issue_request in elev_mod.h as a system call.
*/
int add_passenger(int passenger_type, int start_floor, int destination_floor)
{
  struct my_elev_passenger *ep = NULL;
  if (passenger_type >= NUM_PASS_TYPES ||
      destination_floor < 1 || destination_floor > MAX_FLOOR ||
      start_floor < 1 || start_floor > MAX_FLOOR) {
        return 1;
      }

  ep = my_elev_new_passenger(passenger_type, destination_floor);
  printk(KERN_INFO "Attempting to call list_add_tail.\n");
  list_add_tail(&ep->list, &floors[start_floor].pass_list);
  return 0;
}

/*
  Print passenger information waiting at each floor.
*/
void print_floors(void)
{
  int i;
  struct my_elev_passenger *ep;
  struct list_head *temp;

  for (i = 0; i < MAX_FLOOR; ++i) {
    list_for_each(temp, &floors[i].pass_list) {
      my_elev_print_pass(list_entry(temp, struct my_elev_passenger, list));
    }
  }
}
