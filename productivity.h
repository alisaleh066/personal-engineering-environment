#ifndef PRODUCTIVITY_H
#define PRODUCTIVITY_H

#include <time.h>

#define MAX_ASSIGNMENTS 20
#define MAX_NAME 50

typedef struct {
    char name[MAX_NAME];
    time_t deadline;
    int priority;
    int progress;
    double hours_logged;
    int complete;
} Assignment;

typedef struct {
    int sessions_today;
    int total_sessions;
    double hours_today;
    double hours_this_week;
    int current_streak;
    int best_streak;
    time_t last_session_date;
} StudyStats;

void productivity_suite(void);
void pomodoro_timer(void);
void assignment_tracker(void);
void grade_calculator(void);
void study_analytics(void);

#endif