/*
  Andrue Peters
  2/20/19

  my_xtime
  Simple kernel module that keeps track of the time elapsed since the unix epoch.

*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/time.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Andrue Peters");
MODULE_DESCRIPTION("Keeps track of time since Unix EPOCH and last call");

#define ENTRY_NAME "timed"
#define ENTRY_SIZE 100
#define PERMS 0644
#define PARENT NULL

// points to proc file definitions
static struct file_operations fops;

// message to display in proc
static char *message;
static int read_p;

static struct timespec curr_time;
static struct timespec prev_time = {0,0};



int my_xtime_proc_open (struct inode *sp_inode, struct file *sp_file)
{
  long elapsed_sec;
  long elapsed_nsec;
  printk(KERN_INFO "proc called open\n");
  read_p = 1;
  message = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);

  if (!message) {
    printk(KERN_WARNING "my_xtime_open");
    return -ENOMEM;
  }
  curr_time = current_kernel_time();

  if (prev_time.tv_sec == 0) {
    sprintf(message, "current time: %ld.%ld\n", curr_time.tv_sec, curr_time.tv_nsec);
  } else {
    elapsed_sec = curr_time.tv_sec - prev_time.tv_sec;
    elapsed_nsec = curr_time.tv_nsec - prev_time.tv_nsec;
    sprintf(message, "current time: %ld.%ld\nelapsed time: %ld.%ld\n",
            curr_time.tv_sec, curr_time.tv_nsec, elapsed_sec,   elapsed_nsec);
  }


  return 0;
}

ssize_t my_xtime_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset)
{

  int len = strlen(message);

  read_p = !read_p;
  if (read_p) return 0;

  printk(KERN_INFO "proc called read\n");
  copy_to_user(buf, message, len);
  prev_time = curr_time;
  return len;
}

int my_xtime_proc_release(struct inode *sp_inode, struct file *sp_file)
{
  printk(KERN_INFO "proc called release\n");
  kfree(message);
  return 0;
}

static int my_xtime_init(void)
{
  printk(KERN_NOTICE "/proc/%s create\n", ENTRY_NAME);
  fops.open = my_xtime_proc_open;
  fops.read = my_xtime_proc_read;
  fops.release = my_xtime_proc_release;

  if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) {
    printk("ERROR! proc_create\n");
    remove_proc_entry(ENTRY_NAME, NULL);
    return -ENOMEM;
  }
  return 0;
}
module_init(my_xtime_init);

static void my_xtime_exit(void)
{
  remove_proc_entry(ENTRY_NAME, NULL);
  printk(KERN_NOTICE "Removing /proc/%s.\n", ENTRY_NAME);
}
module_exit(my_xtime_exit);
