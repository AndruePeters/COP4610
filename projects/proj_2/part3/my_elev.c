/*
  Andrue Peters
  3/6/19

  my_elev
  Elevator scheduler.
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/random.h>
#include <linux/kernel.h>



#include <passenger.h>
#include <floor.h>
#include <elevator.h>




#define ENTRY_NAME "my_elev"
#define ENTRY_SIZE 100
#define PERMS 0644
#define PARENT NULL

// points to proc file definitions
static struct file_operations fops;

// message to display in proc
static char *message;
static int read_p;

struct my_elevator elev;

int my_elev_proc_open (struct inode *sp_inode, struct file *sp_file)
{
  static int i = 0;
  printk(KERN_INFO "my_elev proc opened\n");
  read_p = 1;

  message = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
  if (!message) {
    printk(KERN_WARNING "my_elev_open\n");
    return -ENOMEM;
  }
  add_passenger(elev.floors, get_random_int() % 5, get_random_int() % 10 + 1, get_random_int() %10 + 1);
  sprintf(message, "%d\n", i);
  ++i;
  return 0;
}


ssize_t my_elev_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset)
{
  int len = strlen(message);
  read_p = !read_p;
  if (read_p) return 0;

  printk(KERN_INFO "my_elev proc called read\n");
  copy_to_user(buf, message, len);
  return len;
}

int my_elev_proc_release(struct inode *sp_inode, struct file *sp_file)
{
  printk(KERN_INFO "my_elev proc called release\n");
  kfree(message);
  return 0;
}

static int my_elev_init(void)
{
  printk(KERN_NOTICE "/proc/%s create\n", ENTRY_NAME);

  fops.open = my_elev_proc_open;
  fops.read = my_elev_proc_read;
  fops.release = my_elev_proc_release;

  if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) {
    printk("ERROR! proc_create\n");
    remove_proc_entry(ENTRY_NAME, NULL);
    return -ENOMEM;
  }

  /* General initialization for other components */
  init_my_elevator();
  return 0;
}
module_init(my_elev_init);

static void my_elev_exit(void)
{
  remove_proc_entry(ENTRY_NAME, NULL);
  printk(KERN_NOTICE "Removing /proc/%s.\n", ENTRY_NAME);
  print_floors(elev.floors);
}
module_exit(my_elev_exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrue Peters and Oscar Flores");
MODULE_DESCRIPTION("Elevator scheduler");
