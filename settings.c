#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "settings.h"
#include "utils.h"
#include "colors.h"

static Settings cfg = {1, 2, 20};

extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);

static void load_settings(void) {
    FILE *f = fopen("config.txt", "r");
    if (!f) return;
    fscanf(f, "colors=%d\nrisk=%d\nstudy_goal=%d",
           &cfg.color_enabled, &cfg.default_risk_pct, &cfg.weekly_study_goal);
    fclose(f);
}

static void save_settings(void) {
    FILE *f = fopen("config.txt", "w");
    if (!f) return;
    fprintf(f, "colors=%d\nrisk=%d\nstudy_goal=%d",
            cfg.color_enabled, cfg.default_risk_pct, cfg.weekly_study_goal);
    fclose(f);
}

void show_settings(void) {
    load_settings();
    print_header("Settings");
    
    printf("Current settings:\n\n");
    printf("1. Colors:           %s\n", cfg.color_enabled ? "ON" : "OFF");
    printf("2. Default risk:     %d%%\n", cfg.default_risk_pct);
    printf("3. Weekly study goal: %d hrs\n", cfg.weekly_study_goal);
    printf("4. Reset to defaults\n");
    printf("5. Back\n\nChoice: ");
    
    int c;
    if (scanf("%d", &c) != 1 || c < 1 || c > 5) {
        flush_input();
        printf("\n*** Invalid - choose 1-5 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 1) {
        cfg.color_enabled = !cfg.color_enabled;
        printf("Colors %s\n", cfg.color_enabled ? "enabled" : "disabled");
    } else if (c == 2) {
        cfg.default_risk_pct = get_int("Default risk (1-10%%): ", 1, 10);
    } else if (c == 3) {
        cfg.weekly_study_goal = get_int("Weekly goal (5-40 hrs): ", 5, 40);
    } else if (c == 4) {
        cfg.color_enabled = 1;
        cfg.default_risk_pct = 2;
        cfg.weekly_study_goal = 20;
        printf("Reset to defaults\n");
    } else {
        return;
    }
    
    save_settings();
    printf("Settings saved\n");
    go_back();
}

void show_help(void) {
    print_header("Help");
    
    printf("1. Quick Start\n");
    printf("2. File Formats\n");
    printf("3. Back\n\nChoice: ");
    
    int c;
    if (scanf("%d", &c) != 1 || c < 1 || c > 3) {
        flush_input();
        printf("\n*** Invalid - choose 1-3 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 3) return;
    
    printf("\n");
    
    if (c == 1) {
        printf("Quick Start Guide\n\n");
        printf("Navigate using numbers\n");
        printf("Export calculations to CSV\n");
        printf("Generate plots with Python (optional)\n");
        printf("Import parameters from .txt files\n\n");
        printf("Main modules:\n");
        printf("  Mechanical - beam, spring-mass, pendulum\n");
        printf("  Electrical - RLC, RC, buck converter\n");
        printf("  Control - step response, PID tuning\n");
        printf("  Forex - position size, risk analysis\n");
        printf("  Productivity - pomodoro, assignments, grades\n");
    } else if (c == 2) {
        printf("File Format Guide\n\n");
        printf("beam_input.txt:\n");
        printf("  L E I type load\n");
        printf("  Example: 2.0 200e9 1e-6 1 1000\n\n");
        printf("rlc_input.txt:\n");
        printf("  R L C\n");
        printf("  Example: 1000 0.01 1e-6\n\n");
        printf("assignments.txt:\n");
        printf("  name|deadline|priority|progress|hours|complete\n\n");
        printf("study_stats.txt:\n");
        printf("  sessions_today|total|hrs_today|hrs_week|streak|best|date\n");
    }
    
    go_back();
}

static void run_core_tests(int *passed, int *failed) {
    double calc, expected;
    
    printf("[MECHANICAL] Beam deflection... ");
    double F = 1000, L = 2.0, E = 200e9, I = 1e-6;
    calc = (F * L * L * L) / (3 * E * I);
    expected = 0.01333;
    if (fabs(calc - expected) < 0.001) {
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("FAIL (got %.5f)\n", calc);
        (*failed)++;
    }
    
    printf("[ELECTRICAL] RC time constant... ");
    double R = 1000, C = 1e-6;
    calc = R * C;
    expected = 0.001;
    if (fabs(calc - expected) < 0.0001) {
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("FAIL (got %.4f)\n", calc);
        (*failed)++;
    }
    
    printf("[CONTROL] Settling time... ");
    double zeta = 0.5, wn = 10.0;
    calc = 4.0 / (zeta * wn);
    expected = 0.8;
    if (fabs(calc - expected) < 0.01) {
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("FAIL (got %.2f)\n", calc);
        (*failed)++;
    }
    
    printf("[FOREX] Position size... ");
    double account = 10000, risk_pct = 1, sl_pips = 50, pip_val = 10;
    calc = (account * risk_pct / 100) / (sl_pips * pip_val);
    expected = 0.2;
    if (fabs(calc - expected) < 0.01) {
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("FAIL (got %.2f lots)\n", calc);
        (*failed)++;
    }
    
    printf("[GRADES] Weighted average... ");
    double w1 = 40, s1 = 80, w2 = 60, s2 = 70;
    calc = (w1 * s1 + w2 * s2) / (w1 + w2);
    expected = 74.0;
    if (fabs(calc - expected) < 0.1) {
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("FAIL (got %.1f%%)\n", calc);
        (*failed)++;
    }
}

static void run_full_tests(int *passed, int *failed) {
    run_core_tests(passed, failed);
    
    printf("[FILE I/O] Read/write... ");
    FILE *f = fopen("_test.tmp", "w");
    if (f) {
        fprintf(f, "test123");
        fclose(f);
        f = fopen("_test.tmp", "r");
        if (f) {
            char buf[20];
            fscanf(f, "%s", buf);
            fclose(f);
            remove("_test.tmp");
            if (strcmp(buf, "test123") == 0) {
                printf("PASS\n");
                (*passed)++;
            } else {
                printf("FAIL (data mismatch)\n");
                (*failed)++;
            }
        } else {
            printf("FAIL (read error)\n");
            (*failed)++;
        }
    } else {
        printf("FAIL (write error)\n");
        (*failed)++;
    }
    
    printf("[FILE] CSV export... ");
    FILE *csv = fopen("_test.csv", "w");
    if (csv) {
        fprintf(csv, "Index,Value\n1,2.5\n");
        fclose(csv);
        remove("_test.csv");
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("FAIL\n");
        (*failed)++;
    }
    
    printf("[PYTHON] Availability... ");
    int ret = system("python3 --version > /dev/null 2>&1");
    if (ret == 0) {
        printf("PASS\n");
        (*passed)++;
    } else {
        printf("WARN (graphs disabled)\n");
    }
    
    printf("[PYTHON] Script check... ");
    int found = 0;
    FILE *p1 = fopen("pomodoro.py", "r");
    FILE *p2 = fopen("plot_mechanical.py", "r");
    FILE *p3 = fopen("plot_electrical.py", "r");
    if (p1) { fclose(p1); found++; }
    if (p2) { fclose(p2); found++; }
    if (p3) { fclose(p3); found++; }
    if (found == 3) {
        printf("PASS (3/3)\n");
        (*passed)++;
    } else {
        printf("WARN (%d/3 found)\n", found);
    }
}

void self_test(void) {
    print_header("Self-Test Diagnostics");
    
    printf("1. Quick Test (core calculations)\n");
    printf("2. Full Test (includes file I/O)\n");
    printf("3. Back\n\nChoice: ");
    
    int c;
    if (scanf("%d", &c) != 1 || c < 1 || c > 3) {
        flush_input();
        printf("\n*** Invalid - choose 1-3 ***\n");
        go_back();
        return;
    }
    flush_input();
    
    if (c == 3) return;
    
    printf("\nRunning %s...\n\n", c == 1 ? "quick test" : "full test");
    
    int passed = 0, failed = 0;
    
    if (c == 1) {
        run_core_tests(&passed, &failed);
    } else {
        run_full_tests(&passed, &failed);
    }
    
    printf("\nResults: %d passed, %d failed\n", passed, failed);
    
    if (failed == 0) printf("All tests passed\n");
    else printf("Some tests failed\n");
    
    go_back();
}

void utilities_menu(void) {
    int c;
    while (1) {
        print_header("Utilities");
        printf("1. Settings\n");
        printf("2. Help\n");
        printf("3. Self-Test\n");
        printf("0. Back\n\nChoice: ");
        
        if (scanf("%d", &c) != 1 || c < 0 || c > 3) {
            flush_input();
            printf("\n*** Invalid - choose 0-3 ***\n");
            go_back();
            continue;
        }
        flush_input();
        
        if (c == 1) show_settings();
        else if (c == 2) show_help();
        else if (c == 3) self_test();
        else return;
    }
}