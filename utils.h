#ifndef UTILS_H
#define UTILS_H

void clear_screen(void);
void print_header(const char *title);
void print_separator(void);
void go_back(void);
void flush_input(void);
void log_simulation(const char *suite, const char *sim_name, const char *params);

#endif