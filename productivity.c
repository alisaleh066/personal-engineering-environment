#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "productivity.h"
#include "utils.h"
#include "colors.h"

static Assignment assignments[MAX_ASSIGNMENTS];
static int num_assignments = 0;
static StudyStats stats = {0, 0, 0.0, 0.0, 0, 0, 0};

extern double get_double(char *p, int pos);
extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);

static void load_assignments(void) {
    FILE *f = fopen("assignments.txt", "r");
    if (!f) return;
    num_assignments = 0;
    while (num_assignments < MAX_ASSIGNMENTS && 
           fscanf(f, "%49[^|]|%ld|%d|%d|%lf|%d\n", assignments[num_assignments].name,
                  &assignments[num_assignments].deadline, &assignments[num_assignments].priority,
                  &assignments[num_assignments].progress, &assignments[num_assignments].hours_logged,
                  &assignments[num_assignments].complete) == 6) {
        num_assignments++;
    }
    fclose(f);
}

static void save_assignments(void) {
    FILE *f = fopen("assignments.txt", "w");
    if (!f) return;
    for (int i = 0; i < num_assignments; i++) {
        fprintf(f, "%s|%ld|%d|%d|%.2f|%d\n", assignments[i].name, assignments[i].deadline,
                assignments[i].priority, assignments[i].progress, assignments[i].hours_logged,
                assignments[i].complete);
    }
    fclose(f);
}

static void load_stats(void) {
    FILE *f = fopen("study_stats.txt", "r");
    if (!f) return;
    fscanf(f, "%d|%d|%lf|%lf|%d|%d|%ld", &stats.sessions_today, &stats.total_sessions,
           &stats.hours_today, &stats.hours_this_week, &stats.current_streak,
           &stats.best_streak, &stats.last_session_date);
    fclose(f);
}

static void save_stats(void) {
    FILE *f = fopen("study_stats.txt", "w");
    if (!f) return;
    fprintf(f, "%d|%d|%.2f|%.2f|%d|%d|%ld", stats.sessions_today, stats.total_sessions,
            stats.hours_today, stats.hours_this_week, stats.current_streak,
            stats.best_streak, stats.last_session_date);
    fclose(f);
}

void pomodoro_timer(void) {
    print_header("Pomodoro Timer");
    load_stats();
    printf("Today: %d | Week: %.1fh | Streak: %d\n\n", 
           stats.sessions_today, stats.hours_this_week, stats.current_streak);
    
    printf("1. 25min  2. 15min  3. Custom  4. History  0. Back\nChoice: ");
    int c;
    if (scanf("%d", &c) != 1 || c < 0 || c > 4) {
        flush_input();
        printf("\n*** Invalid - choose 0-4 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 0) return;
    
    if (c == 4) {
        FILE *f = fopen("study_log.txt", "r");
        if (!f) { printf("\nNo sessions.\n"); go_back(); return; }
        printf("\n");
        char line[200];
        int cnt = 0;
        while (fgets(line, sizeof(line), f) && cnt++ < 15) printf("%s", line);
        fclose(f);
        go_back();
        return;
    }
    
    int mins = c == 1 ? 25 : c == 2 ? 15 : get_int("Minutes: ", 1, 120);
    char subj[50];
    printf("Subject: ");
    scanf(" %49[^\n]", subj);
    flush_input();
    
    char cmd[200];
    strcpy(cmd, "python3 pomodoro.py ");
    char tmp[20];
    int t = mins, idx = 0;
    if (t == 0) tmp[idx++] = '0';
    else {
        char rev[20];
        int r = 0;
        while (t > 0) { rev[r++] = '0' + (t % 10); t /= 10; }
        for (int i = r - 1; i >= 0; i--) tmp[idx++] = rev[i];
    }
    tmp[idx] = '\0';
    strcat(cmd, tmp);
    strcat(cmd, " \"");
    strcat(cmd, subj);
    strcat(cmd, "\"");
    printf("\nStarting timer(CTRL+C to stop early)\n");
    system(cmd);
    
    double hrs = mins / 60.0;
    stats.sessions_today++;
    stats.total_sessions++;
    stats.hours_today += hrs;
    stats.hours_this_week += hrs;
    
    time_t now = time(NULL);
    if (stats.last_session_date == 0) stats.current_streak = 1;
    else {
        int d = (now - stats.last_session_date) / 86400;
        if (d == 1) stats.current_streak++;
        else if (d > 1) stats.current_streak = 1;
    }
    
    if (stats.current_streak > stats.best_streak) {
        stats.best_streak = stats.current_streak;
        printf("Best streak: %d days\n", stats.best_streak);
    }
    stats.last_session_date = now;
    save_stats();
    
    FILE *log = fopen("study_log.txt", "a");
    if (log) {
        char *ts = ctime(&now);
        ts[24] = '\0';
        fprintf(log, "[%s] %d min - %s\n", ts, mins, subj);
        fclose(log);
    }
    
    printf("\nToday: %d sessions (%.1fh)\n", stats.sessions_today, stats.hours_today);
    if (stats.sessions_today % 4 == 0) printf("4 sessions done, take a break\n");
    go_back();
}

void assignment_tracker(void) {
    print_header("Assignment Tracker");
    load_assignments();
    time_t now = time(NULL);
    
    if (num_assignments > 0) {
        for (int i = 0; i < num_assignments; i++) {
            if (assignments[i].complete) continue;
            int d = (assignments[i].deadline - now) / 86400;
            char *col = d <= 2 ? COLOR_RED : d <= 7 ? COLOR_YELLOW : COLOR_GREEN;
            printf("%d. %s%s" COLOR_RESET " - %d%% - %.1fh - ", 
                   i+1, col, assignments[i].name, assignments[i].progress, 
                   assignments[i].hours_logged);
            if (d < 0) printf("overdue\n");
            else if (d == 0) printf("due today\n");
            else printf("%d days\n", d);
        }
    } else printf("No assignments.\n");
    
    printf("\n1. Add  2. Update  3. Complete  4. Delete  0. Back\nChoice: ");
    int c;
    if (scanf("%d", &c) != 1 || c < 0 || c > 4) {
        flush_input();
        printf("\n*** Invalid - choose 0-4 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 0) return;
    
    if (c == 1 && num_assignments < MAX_ASSIGNMENTS) {
        Assignment *a = &assignments[num_assignments];
        printf("Name: ");
        scanf(" %49[^\n]", a->name);
        flush_input();
        a->deadline = now + (get_int("Days: ", 0, 365) * 86400);
        a->priority = get_int("Priority (1-3): ", 1, 3);
        a->progress = 0;
        a->hours_logged = 0;
        a->complete = 0;
        num_assignments++;
        save_assignments();
        printf("Added\n");
    } else if (c == 2 && num_assignments > 0) {
        int i = get_int("Which? ", 1, num_assignments) - 1;
        assignments[i].progress = get_int("Progress: ", 0, 100);
        save_assignments();
        printf("Updated\n");
    } else if (c == 3 && num_assignments > 0) {
        assignments[get_int("Which? ", 1, num_assignments) - 1].complete = 1;
        save_assignments();
        printf("Marked complete\n");
    } else if (c == 4 && num_assignments > 0) {
        int i = get_int("Which? ", 1, num_assignments) - 1;
        for (int j = i; j < num_assignments - 1; j++) assignments[j] = assignments[j + 1];
        num_assignments--;
        save_assignments();
        printf("Deleted\n");
    }
    go_back();
}

void grade_calculator(void) {
    print_header("Grade Calculator");
    printf("1. Module  2. Semester  3. What I need  0. Back\nChoice: ");
    int c;
    if (scanf("%d", &c) != 1 || c < 0 || c > 3) {
        flush_input();
        printf("\n*** Invalid - choose 0-3 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 0) return;
    
    if (c == 1) {
        int n = get_int("Components: ", 1, 10);
        double tw = 0, sum = 0;
        for (int i = 0; i < n; i++) {
            double w = get_double("Weight: ", 1);
            double s = get_double("Score: ", 0);
            tw += w;
            sum += w * s;
        }
        double g = sum / tw;
        printf("\nGrade: %.1f%%\n", g);
        if (g >= 70) printf("First\n");
        else if (g >= 60) printf("2:1\n");
        else if (g >= 50) printf("2:2\n");
        else if (g >= 40) printf("Third\n");
        else printf("Fail\n");
    } else if (c == 2) {
        int n = get_int("Modules: ", 1, 10);
        double tc = 0, sum = 0;
        for (int i = 0; i < n; i++) {
            double cr = get_double("Credits: ", 1);
            double gr = get_double("Grade: ", 0);
            tc += cr;
            sum += cr * gr;
        }
        printf("\nAverage: %.1f%%\n", sum / tc);
    } else {
        double cur = get_double("Current: ", 0);
        double don = get_double("Weight done: ", 1);
        double tar = get_double("Target: ", 1);
        double ned = (tar * 100 - cur * don) / (100 - don);
        printf("\nNeed: %.1f%%\n", ned);
        if (ned > 100) printf("Not achievable\n");
        else if (ned < 0) printf("Already met\n");
        else printf("Achievable\n");
    }
    go_back();
}

void study_analytics(void) {
    print_header("Study Analytics");
    load_stats();
    
    printf("Total:  %d sessions\n", stats.total_sessions);
    printf("Week:   %.1f hrs\n", stats.hours_this_week);
    printf("Today:  %d sessions (%.1f hrs)\n", stats.sessions_today, stats.hours_today);
    printf("Streak: %d (Best: %d)\n\n", stats.current_streak, stats.best_streak);
    
    double p = (stats.hours_this_week / 20.0) * 100;
    if (p > 100) p = 100;
    printf("Goal (20h): %.0f%% [", p);
    for (int i = 0; i < 20; i++) printf(i < (int)(p/5) ? "#" : "-");
    printf("]\n\n");
    
    printf("1. Log  2. Reset day  3. Reset week  4. CSV  0. Back\nChoice: ");
    int c;
    if (scanf("%d", &c) != 1 || c < 0 || c > 4) {
        flush_input();
        printf("\n*** Invalid - choose 0-4 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 0) return;
    
    if (c == 1) {
        FILE *f = fopen("study_log.txt", "r");
        if (f) {
            printf("\n");
            char line[200];
            while (fgets(line, sizeof(line), f)) printf("%s", line);
            fclose(f);
        } else printf("No log.\n");
    } else if (c == 2) {
        stats.sessions_today = 0;
        stats.hours_today = 0;
        save_stats();
        printf("Daily stats reset\n");
    } else if (c == 3) {
        stats.hours_this_week = 0;
        save_stats();
        printf("Weekly stats reset\n");
    } else if (c == 4) {
        FILE *f = fopen("study_stats.csv", "w");
        if (f) {
            fprintf(f, "Metric,Value\nTotal,%d\nWeek,%.2f\nStreak,%d\n",
                    stats.total_sessions, stats.hours_this_week, stats.current_streak);
            fclose(f);
            printf("Exported to study_stats.csv\n");
        }
    }
    go_back();
}

void productivity_suite(void) {
    int c;
    while (1) {
        print_header("Productivity Suite");
        printf("1. Pomodoro  2. Assignments  3. Grades  4. Analytics  0. Back\nChoice: ");
        
        if (scanf("%d", &c) != 1 || c < 0 || c > 4) {
            flush_input();
            printf("\n*** Invalid - choose 0-4 ***\n");
            go_back();
            continue;
        }
        flush_input();
        
        if (c == 1) pomodoro_timer();
        else if (c == 2) assignment_tracker();
        else if (c == 3) grade_calculator();
        else if (c == 4) study_analytics();
        else return;
    }
}