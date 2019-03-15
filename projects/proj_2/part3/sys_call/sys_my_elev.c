/*
	Andrue Peters
	3/10/19
	System call module for elevator scheduler.
*/
#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>

/* System call */
int (*start_elevator)(void) = NULL;
int (*stop_elevator)(void) = NULL;
int (*issue_request)(int pass_type, int start_floor, int dest_floor) = NULL;

EXPORT_SYMBOL(start_elevator);
EXPORT_SYMBOL(stop_elevator);
EXPORT_SYMBOL(issue_request);

asmlinkage long sys_start_elevator(void) 
{
	if (start_elevator != NULL) {
		return start_elevator();
	} else {
		return -ENOSYS;
	}
}

asmlinkage long sys_stop_elevator(void)
{
	if (stop_elevator != NULL) {
		return stop_elevator();
	} else {
		return -ENOSYS;
	}
}
 
asmlinkage long sys_issue_request(int pass_type, int start_floor, int dest_floor) 
{
	if (issue_request != NULL) {
		return issue_request(pass_type, start_floor, dest_floor);
	} else {
		return -ENOSYS;
	}
}
