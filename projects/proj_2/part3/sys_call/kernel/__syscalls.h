asmlinkage long sys_start_elevator(void);
asmlinkage long sys_issue_request(int pass_type, int start_floor, int dest_floor);
asmlinkage long sys_stop_elevator(void);
