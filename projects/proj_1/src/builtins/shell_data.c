#include <glib.h>

#include "shell_data.h"

/*
  Internal function that returns the index of cpid in garr
*/
static int get_garray_index(GArray* garr, pid_t cpid);

/*
  Initializes sd.
  Must be the first function called before sd is used.
*/
void shell_data_init(struct shell_data *sd)
{
  sd->num_com = 0;
  sd->pwd = NULL;
  shell_data_update_pwd(sd);
  sd->bakgnd_proc = g_array_new(true, true, sizeof(pid_t));
}

/*
  Updates the pwd for sd
*/
void shell_data_update_pwd(struct shell_data *sd)
{
  if (sd->pwd) {
    free (sd->pwd);
  }

  sd->pwd = strdup(getenv("PWD"));
}

/*
  Adds a pid to the end of the array if it doesn't exist already.
*/
void shell_data_add_pid(struct shell_data *sd, pid_t cpid)
{
  /* Make sure no one tries to add elements twice. */
  if (get_garray_index(sd->bakgnd_proc, cpid) != -1)
    g_array_append_vals(sd->bakgnd_proc, &cpid, 1);
}

/*
  Removes cpid if it doesn't exist.
*/
void shell_data_rem_pid(struct shell_data *sd, pid_t cpid)
{
  int index = get_garray_index(sd->bakgnd_proc, cpid);
  if (index != -1) {
    g_array_remove_index_fast(sd->bakgnd_proc, index);
  }

}

void shell_data_clear(struct shell_data *sd)
{

}

/*
  Inefficient algorithm as its complexity is O(N), but I don't forsee
  thousands of elements for this homework assignment, so it should do just fine.

  Returns -1 if element not found.
*/
int get_garray_index(GArray* garr, pid_t cpid)
{
  int i;
  for (i = 0; i < garr->len; ++i) {
    if (g_array_index(garr, pid_t, i) == cpid) {
      return i;
    }
  }
  return -1;
}
