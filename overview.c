#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "overview.h"
#include "utils.h"
#include "colors.h"

static void get_study_stats(int *sessions, double *hours, int *streak) {
    *sessions = 0;
    *hours = 0.0;
    *streak = 0;
    
    FILE *f = fopen("study_stats.txt", "r");
    if (!f) return;
    
    int s_today, s_total, c_streak, b_streak;
    double h_today, h_week;
    long last_date;
    
    if (fscanf(f, "%d|%d|%lf|%lf|%d|%d|%ld", 
        &s_today, &s_total, &h_today, &h_week, &c_streak, &b_streak, &last_date) == 7) {
        *sessions = s_total;
        *hours = h_week;
        *streak = c_streak;
    }
    fclose(f);
}

static void get_next_deadline(char *name, int *days_left) {
    strcpy(name, "None");
    *days_left = 999;
    
    FILE *f = fopen("assignments.txt", "r");
    if (!f) return;
    
    time_t now = time(NULL);
    char aname[50];
    long deadline;
    int priority, progress, complete;
    double hours;
    
    while (fscanf(f, "%49[^|]|%ld|%d|%d|%lf|%d\n", 
           aname, &deadline, &priority, &progress, &hours, &complete) == 6) {
        if (complete) continue;
        int days = (deadline - now) / 86400;
        if (days >= 0 && days < *days_left) {
            *days_left = days;
            strcpy(name, aname);
        }
    }
    fclose(f);
}

static void get_forex_stats(double *total_pnl, int *total_trades, int *wins) {
    *total_pnl = 0.0;
    *total_trades = 0;
    *wins = 0;
    
    FILE *f = fopen("trade_journal.txt", "r");
    if (!f) return;
    
    char pair[10], dir[5];
    double entry, exit, lots, pnl;
    long date;
    
    while (fscanf(f, "%9[^|]|%4[^|]|%lf|%lf|%lf|%lf|%ld\n",
           pair, dir, &entry, &exit, &lots, &pnl, &date) == 7) {
        *total_pnl += pnl;
        (*total_trades)++;
        if (pnl > 0) (*wins)++;
    }
    fclose(f);
}

static void get_last_simulation(char *desc) {
    strcpy(desc, "None");
    
    FILE *f = fopen("simulation_log.txt", "r");
    if (!f) return;
    
    char line[200];
    char last_line[200] = "";
    
    while (fgets(line, sizeof(line), f)) {
        strcpy(last_line, line);
    }
    fclose(f);
    
    if (strlen(last_line) > 0) {
        char *start = strchr(last_line, ']');
        if (start) {
            start += 2;
            char *end = strchr(start, ':');
            if (end) {
                *end = '\0';
                strcpy(desc, start);
            }
        }
    }
}

static void print_bar(double value, double max, int width) {
    int filled = (int)((value / max) * width);
    if (filled > width) filled = width;
    if (filled < 0) filled = 0;
    
    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < filled) printf(COLOR_GREEN "#" COLOR_RESET);
        else printf("-");
    }
    printf("]");
}

void show_overview(void) {
    print_header("Dashboard");
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("Today: %04d-%02d-%02d %02d:%02d\n\n", 
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
    
    printf(COLOR_CYAN "=== Study ===" COLOR_RESET "\n");
    int sessions, streak;
    double hours;
    get_study_stats(&sessions, &hours, &streak);
    printf("This week:     %.1f hrs ", hours);
    print_bar(hours, 20.0, 15);
    printf("\n");
    printf("Total:         %d sessions\n", sessions);
    printf("Streak:        %d day%s ", streak, streak == 1 ? "" : "s");
    if (streak >= 7) printf(COLOR_GREEN "Great" COLOR_RESET);
    else if (streak >= 3) printf(COLOR_YELLOW "Good" COLOR_RESET);
    printf("\n\n");
    
    printf(COLOR_CYAN "=== Assignments ===" COLOR_RESET "\n");
    char next_name[50];
    int days_left;
    get_next_deadline(next_name, &days_left);
    if (days_left < 999) {
        printf("Next:          %s\n", next_name);
        printf("Due in:        ");
        if (days_left == 0) printf(COLOR_RED "Today" COLOR_RESET);
        else if (days_left == 1) printf(COLOR_YELLOW "Tomorrow" COLOR_RESET);
        else if (days_left <= 3) printf(COLOR_YELLOW "%d days" COLOR_RESET, days_left);
        else printf(COLOR_GREEN "%d days" COLOR_RESET, days_left);
        printf("\n");
    } else {
        printf("No upcoming deadlines\n");
    }
    printf("\n");
    
    printf(COLOR_CYAN "=== Forex ===" COLOR_RESET "\n");
    double total_pnl;
    int total_trades, wins;
    get_forex_stats(&total_pnl, &total_trades, &wins);
    if (total_trades > 0) {
        double win_rate = (double)wins / total_trades * 100;
        printf("Trades:        %d\n", total_trades);
        printf("Win rate:      %.1f%%\n", win_rate);
        printf("Total P/L:     ");
        if (total_pnl >= 0) printf(COLOR_GREEN "$%.2f" COLOR_RESET, total_pnl);
        else printf(COLOR_RED "$%.2f" COLOR_RESET, total_pnl);
        printf("\n");
    } else {
        printf("No trades logged yet\n");
    }
    printf("\n");
    
    printf(COLOR_CYAN "=== Last Simulation ===" COLOR_RESET "\n");
    char sim_desc[100];
    get_last_simulation(sim_desc);
    printf("%s\n\n", sim_desc);
    
    printf(COLOR_CYAN "=== Quick Actions ===" COLOR_RESET "\n");
    printf("[1] Start Pomodoro  [2] Log Trade  [3] Add Assignment  [0] Back\n\n");
    printf("Action: ");
    
    int c;
    if (scanf("%d", &c) != 1 || c < 0 || c > 3) {
        flush_input();
        printf("\n*** Invalid - choose 0-3 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 1) {
        extern void pomodoro_timer(void);
        pomodoro_timer();
    } else if (c == 2) {
        extern void trade_journal(void);
        trade_journal();
    } else if (c == 3) {
        extern void assignment_tracker(void);
        assignment_tracker();
    }
}