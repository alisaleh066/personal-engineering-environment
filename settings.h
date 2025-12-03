#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct {
    int color_enabled;
    int default_risk_pct;
    int weekly_study_goal;
} Settings;

void show_settings(void);
void show_help(void);
void self_test(void);
void utilities_menu(void);

#endif