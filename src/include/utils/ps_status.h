/*-------------------------------------------------------------------------
 *
 * ps_status.h
 *
 * Declarations for backend/utils/misc/ps_status.c
 *
 * src/include/utils/ps_status.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef PS_STATUS_H
#define PS_STATUS_H

extern PGDLLIMPORT bool update_process_title;

extern PGDLLIMPORT char **save_ps_display_args(int argc, char **argv);

extern PGDLLIMPORT void init_ps_display(const char *fixed_part);

extern PGDLLIMPORT void set_ps_display(const char *activity);

extern PGDLLIMPORT const char *get_ps_display(int *displen);

#endif							/* PS_STATUS_H */
