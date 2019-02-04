#ifndef _GROUP_15_SHELL_DATA_H_
#define _GROUP_15_SHELL_DATA_H_
#include <glib.h>

struct shell_data {
  unsigned num_com;
  char *pwd;
  GArray *bakgnd_proc;
};

void shell_data_init(struct shell_data *sd);

void shell_data_update_pwd(struct shell_data *sd);

void shell_data_add_pid(struct shell_data *sd, pid_t cpid);

void shell_data_rem_pid(struct shell_data *sd, pid_t cpid);

void shell_data_clear(struct shell_data *sd);



#endif
