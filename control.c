#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "control.h"
#include "utils.h"
#include "colors.h"

#define PI 3.14159265359

static SystemParams sys = {0};
static PIDConfig pid = {0};

extern double get_double(char *p, int pos);
extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);

static void calc_response(double zeta, double wn, SystemParams *s) {
    s->zeta = zeta;
    s->omega_n = wn;
    
    if (zeta < 1.0) {
        double wd = wn * sqrt(1 - zeta * zeta);
        s->peak_time = PI / wd;
        s->overshoot = 100 * exp(-PI * zeta / sqrt(1 - zeta * zeta));
    } else {
        s->peak_time = 0;
        s->overshoot = 0;
    }
    
    s->rise_time = (1.8) / wn;
    s->settle_time = 4.0 / (zeta * wn);
}

static void display_response(SystemParams *s) {
    printf("\nSystem Response:\n\n");
    printf("Damping Ratio (zeta):    %.3f\n", s->zeta);
    printf("Natural Frequency (wn):  %.3f rad/s\n", s->omega_n);
    printf("Rise Time:               %.3f s\n", s->rise_time);
    printf("Peak Time:               %.3f s\n", s->peak_time);
    printf("Settling Time (2%%):      %.3f s\n", s->settle_time);
    printf("Overshoot:               %.1f%%\n\n", s->overshoot);
    
    if (s->zeta < 1.0) printf("Underdamped\n");
    else if (s->zeta == 1.0) printf("Critically damped\n");
    else printf("Overdamped\n");
}

static void export_step_csv(SystemParams *s) {
    FILE *f = fopen("step_response.csv", "w");
    if (!f) return;
    
    fprintf(f, "Time,Response\n");
    double dt = s->settle_time * 1.5 / MAX_POINTS;
    double wd = s->omega_n * sqrt(1 - s->zeta * s->zeta);
    
    for (int i = 0; i < MAX_POINTS; i++) {
        double t = i * dt;
        double y;
        
        if (s->zeta < 1.0) {
            double phi = atan(s->zeta / sqrt(1 - s->zeta * s->zeta));
            y = 1 - (exp(-s->zeta * s->omega_n * t) / sqrt(1 - s->zeta * s->zeta)) 
                * sin(wd * t + phi);
        } else {
            double r1 = -s->omega_n * (s->zeta + sqrt(s->zeta * s->zeta - 1));
            double r2 = -s->omega_n * (s->zeta - sqrt(s->zeta * s->zeta - 1));
            y = 1 + (r1 * exp(r2 * t) - r2 * exp(r1 * t)) / (r2 - r1);
        }
        fprintf(f, "%.4f,%.4f\n", t, y);
    }
    fclose(f);
}

void second_order_designer(void) {
    print_header("Second-Order System Designer");
    
    printf("Standard form: G(s) = wn^2 / (s^2 + 2*zeta*wn*s + wn^2)\n\n");
    printf("1. Enter zeta and wn directly\n");
    printf("2. Design from specs (settling time & overshoot)\n");
    printf("3. Back\nChoice: ");
    
    int c = get_int("", 1, 3);
    if (c == 3) return;
    
    if (c == 1) {
        double z = get_double("Damping ratio (zeta, 0.1-2.0): ", 0);
        double w = get_double("Natural frequency (wn, rad/s): ", 0);
        calc_response(z, w, &sys);
        
    } else {
        double ts = get_double("Desired settling time (s): ", 0);
        double mp = get_double("Desired overshoot (%%): ", 0);
        
        double z = sqrt(log(mp/100) * log(mp/100) / (PI*PI + log(mp/100) * log(mp/100)));
        double w = 4.0 / (z * ts);
        
        printf("\nCalculated parameters:\n");
        printf("zeta = %.3f, wn = %.3f rad/s\n", z, w);
        calc_response(z, w, &sys);
    }
    
    display_response(&sys);
    
    if (get_yn("Export step response? (y/n): ") == 'y') {
        export_step_csv(&sys);
        printf("Saved to step_response.csv\n");
        
        if (get_yn("Generate plot? (y/n): ") == 'y') {
            system("python3 plot_step.py step_response.csv");
        }
    }
    
    printf("\n[F] View Control Formulas  [Enter] Back\n");
    flush_input();
    char input[10];
    fgets(input, sizeof(input), stdin);
    if (input[0] == 'F' || input[0] == 'f') {
        extern void formula_library_filtered(const char *cat);
        formula_library_filtered("Control");
    }
}

void step_response_calc(void) {
    print_header("Step Response Calculator");
    
    printf("Enter two known values:\n\n");
    printf("1. From overshoot & peak time\n");
    printf("2. From settling time & overshoot\n");
    printf("3. From rise time & overshoot\n");
    printf("4. Back\nChoice: ");
    
    int c = get_int("", 1, 4);
    if (c == 4) return;
    
    double z, w;
    
    if (c == 1) {
        double mp = get_double("Overshoot (%%): ", 0);
        double tp = get_double("Peak time (s): ", 0);
        
        z = sqrt(log(mp/100) * log(mp/100) / (PI*PI + log(mp/100) * log(mp/100)));
        double wd = PI / tp;
        w = wd / sqrt(1 - z * z);
        
    } else if (c == 2) {
        double ts = get_double("Settling time (s): ", 0);
        double mp = get_double("Overshoot (%%): ", 0);
        
        z = sqrt(log(mp/100) * log(mp/100) / (PI*PI + log(mp/100) * log(mp/100)));
        w = 4.0 / (z * ts);
        
    } else {
        double tr = get_double("Rise time (s): ", 0);
        double mp = get_double("Overshoot (%%): ", 0);
        
        z = sqrt(log(mp/100) * log(mp/100) / (PI*PI + log(mp/100) * log(mp/100)));
        w = 1.8 / tr;
    }
    
    printf("\nCalculated system parameters:\n");
    printf("zeta = %.3f\n", z);
    printf("wn = %.3f rad/s\n", w);
    
    calc_response(z, w, &sys);
    display_response(&sys);
    
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

void pid_tuner(void) {
    print_header("PID Tuner (Ziegler-Nichols)");
    
    printf("Ultimate Gain Method:\n");
    printf("1. Set Ki=0, Kd=0\n");
    printf("2. Increase Kp until sustained oscillation\n");
    printf("3. Record Ku and Tu\n\n");
    
    double Ku = get_double("Ultimate Gain (Ku): ", 0);
    double Tu = get_double("Ultimate Period (Tu, seconds): ", 0);
    
    printf("\nZiegler-Nichols Tuning:\n\n");
    
    printf("P Controller:\n");
    printf("  Kp = %.3f\n\n", 0.5 * Ku);
    
    printf("PI Controller:\n");
    printf("  Kp = %.3f\n", 0.45 * Ku);
    printf("  Ki = %.3f\n\n", 0.54 * Ku / Tu);
    
    printf("PID Controller:\n");
    pid.Kp = 0.6 * Ku;
    pid.Ki = 1.2 * Ku / Tu;
    pid.Kd = 0.075 * Ku * Tu;
    printf("  Kp = %.3f\n", pid.Kp);
    printf("  Ki = %.3f\n", pid.Ki);
    printf("  Kd = %.3f\n", pid.Kd);
    
    if (get_yn("\nSave PID config? (y/n): ") == 'y') {
        printf("Config name: ");
        scanf(" %29[^\n]", pid.name);
        flush_input();
        
        FILE *f = fopen("pid_configs.txt", "a");
        if (f) {
            fprintf(f, "%s: Kp=%.3f Ki=%.3f Kd=%.3f (Ku=%.3f Tu=%.3f)\n", 
                    pid.name, pid.Kp, pid.Ki, pid.Kd, Ku, Tu);
            fclose(f);
            printf("Saved to pid_configs.txt\n");
        }
    }
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

void control_suite(void) {
    int c;
    while (1) {
        print_header("Control Systems Suite");
        printf("1. Second-Order System Designer\n");
        printf("2. Step Response Calculator\n");
        printf("3. PID Tuner\n");
        printf("0. Back\n\nChoice: ");
        
        if (scanf("%d", &c) != 1 || c < 0 || c > 3) {
            flush_input();
            printf("\nInvalid input, try 0-3\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }
        flush_input();
        
        if (c == 1) second_order_designer();
        else if (c == 2) step_response_calc();
        else if (c == 3) pid_tuner();
        else return;
    }
}