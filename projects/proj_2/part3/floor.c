#include <linux/list.h>
#include <linux/kernel.h>
#include <floor.h>



/*
  Initialize each list head.
*/
void init_floors(struct floor *f)
{
  int i;
  for (i = 0; i < MAX_FLOOR; ++i) {
    INIT_LIST_HEAD(&(f[i].pass_list));
  }
  printk(KERN_INFO "Exited init_floors");
}

/*
  Cleanup memory for the floors.
*/
void cleanup_floors(struct floor *f)
{

}

/*
  Creates a passenger of type passenger_type at start_floor that wishes to go
  to destination_floor. This function returns 1 if the request is not valid
  (one of the variables is out of range), and 0 otherwise.

  Called by issue_request in elev_mod.h as a system call.
*/
int add_passenger(struct floor *f, int passenger_type, int start_floor, int destination_floor)
{
  struct my_elev_passenger *ep = NULL;
  if (passenger_type >= NUM_PASS_TYPES ||
      destination_floor < 1 || destination_floor > MAX_FLOOR ||
      start_floor < 1 || start_floor > MAX_FLOOR) {
        return 1;
      }

  printk(KERN_INFO "Adding passenger to floor: %d\n", start_floor);
  ep = my_elev_new_passenger(passenger_type, destination_floor);
  list_add_tail(&ep->list, &f[start_floor-1].pass_list);
  return 0;
}

/*
  Print passenger information waiting at each floor.
*/
void print_floors(struct floor *f)
{
  int i;
  struct my_elev_passenger *ep;
  struct list_head *temp;

  for (i = 0; i < MAX_FLOOR; ++i) {
    printk(KERN_INFO "Floor %d\n", i+1);
    list_for_each(temp, &f[i].pass_list) {
      my_elev_print_pass(list_entry(temp, struct my_elev_passenger, list));
    }
    printk(KERN_INFO "\n\n");
  }
}
