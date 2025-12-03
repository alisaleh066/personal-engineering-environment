#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "colors.h"

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_header(const char *title) {
    clear_screen();
    printf("\n" COLOR_CYAN "==========================================\n");
    printf("  %s\n", title);
    printf("==========================================" COLOR_RESET "\n\n");
}

void flush_input(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void go_back(void) {
    char input[10];
    printf("\nPress Enter to continue (or 'q' to quit): ");
    flush_input();
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    
    if (input[0] == 'q' || input[0] == 'Q') {
        print_header("Exit");
        exit(0);
    }
}

void log_simulation(const char *suite, const char *sim_name, const char *params) {
    FILE *log = fopen("simulation_log.txt", "a");
    if (!log) return;
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    fprintf(log, "[%04d-%02d-%02d %02d:%02d] %s - %s: %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, suite, sim_name, params);
    fclose(log);
}