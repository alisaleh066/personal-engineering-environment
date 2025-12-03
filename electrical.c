#include <stdio.h>
#include <stdlib.h>  
#include <math.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "colors.h"
#include "file_import.h"

#define PI 3.14159265359

extern DataSeries global_series[3];
extern int global_count;
extern double get_double(char *p, int pos);
extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);
extern void plot_ascii(DataSeries *d);
extern void save_to_lab(DataSeries *d, char *msg);

typedef struct { char pair[10]; double pip_value; int pip_decimal; } CurrencyPair;
typedef struct { char name[20]; double value; char unit[10]; } Component;

Component r_presets[4] = {{"1kΩ", 1000, "Ω"}, {"10kΩ", 10000, "Ω"}, {"100kΩ", 100000, "Ω"}, {"1MΩ", 1000000, "Ω"}};
Component c_presets[4] = {{"1µF", 1e-6, "F"}, {"10µF", 1e-5, "F"}, {"100µF", 1e-4, "F"}, {"1mF", 1e-3, "F"}};
Component l_presets[4] = {{"1mH", 1e-3, "H"}, {"10mH", 1e-2, "H"}, {"100mH", 1e-1, "H"}, {"1H", 1, "H"}};

void save_to_file_electrical(DataSeries *d) {
    if (get_yn("Save to file? (y/n): ") != 'y') return;
    char fname[50]; 
    printf("Filename: "); 
    scanf("%s", fname);
    flush_input(); 
    strcat(fname, ".csv");
    
    FILE *f = fopen(fname, "w");
    if (!f) { printf("Error opening file\n"); return; }
    fprintf(f, "Index,%s\n", d->label);
    for (int i = 0; i < d->length; i++) fprintf(f, "%d,%.6f\n", i, d->data[i]);
    fclose(f); 
    printf("Saved to %s\n", fname);
    
    if (get_yn("Generate graph? (y/n): ") == 'y') {
        char cmd[200];
        sprintf(cmd, "python3 plot_electrical.py %s", fname);
        int result = system(cmd);
        if (result == 0) {
            printf("Graph generated\n");
        } else {
            printf("Failed to generate graph\n");
        }
    }
}

void rlc_transient(DataSeries *s, double zeta, double wn) {
    double dt = 0.0001; s->length = 2000;
    for (int i = 0; i < s->length; i++) {
        double t = i * dt, e = exp(-zeta * wn * t);
        if (zeta < 1) {
            double wd = wn * sqrt(1 - zeta*zeta);
            s->data[i] = 1 - e * (cos(wd * t) + zeta/sqrt(1 - zeta*zeta) * sin(wd * t));
        } else s->data[i] = 1 - e * (1 + zeta * wn * t);
    }
}

void rc_charge_curve(DataSeries *s, double tau) {
    s->length = 1000; double dt = 5 * tau / 999;
    for (int i = 0; i < s->length; i++) s->data[i] = 10 * (1 - exp(-i * dt / tau));
}

static void unit_converter(void) {
    print_header("Unit Converter");
    printf("1. mA ↔ A\n2. mH ↔ H\n3. µF ↔ F\n4. kΩ ↔ Ω\n5. Back\nSelect: ");
    int c = get_int("", 1, 5); if (c == 5) return;
    double v = get_double("Value: ", 0);
    if (c == 1) printf("= %.4f %s\n", v * (v < 100 ? 0.001 : 1000), v < 100 ? "A" : "mA");
    else if (c == 2) printf("= %.6f %s\n", v * (v < 10 ? 0.001 : 1000), v < 10 ? "H" : "mH");
    else if (c == 3) printf("= %.9f %s\n", v * (v < 1000 ? 1e-6 : 1e6), v < 1000 ? "F" : "µF");
    else if (c == 4) printf("= %.2f %s\n", v * (v < 10000 ? 0.001 : 1000), v < 10000 ? "kΩ" : "Ω");
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void quick_calc(void) {
    print_header("Quick Reference");
    printf("1. Ohm's Law V=IR\n2. Power P=VI\n3. Reactance XL=2πfL\n4. Reactance XC=1/(2πfC)\n5. Back\nSelect: ");
    int c = get_int("", 1, 5);
    if (c == 5) return;
    
    if (c == 1) { double I = get_double("I (A): ", 0), R = get_double("R (Ω): ", 1); printf("V = %.2f V\n", I * R); }
    else if (c == 2) { double V = get_double("V (V): ", 0), I = get_double("I (A): ", 0); printf("P = %.2f W\n", V * I); }
    else if (c == 3) { double f = get_double("f (Hz): ", 1), L = get_double("L (H): ", 1); printf("XL = %.2f Ω\n", 2*PI*f*L); }
    else if (c == 4) { double f = get_double("f (Hz): ", 1), C = get_double("C (F): ", 1); printf("XC = %.2f Ω\n", 1/(2*PI*f*C)); }
    
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void rlc_circuit_sim(void) {
    print_header("RLC Transient Response");
    
    printf("1. Manual input\n2. Import from file (rlc_input.txt)\nSelect: ");
    int input_mode = get_int("", 1, 2);
    
    double R, L, C;
    
    if (input_mode == 2) {
        if (load_rlc_params(&R, &L, &C)) {
            printf("Parameters loaded from file\n");
            printf("R=%.0fΩ, L=%.3fH, C=%.6fF\n", R, L, C);
        } else {
            printf("Could not load file, using manual input\n");
            input_mode = 1;
        }
    }
    
    if (input_mode == 1) {
        printf("\nR presets:\n"); 
        for (int i = 0; i < 4; i++) printf("%d. %s\n", i+1, r_presets[i].name);
        printf("5. Custom\nSelect: "); 
        int rp = get_int("", 1, 5);
        R = (rp == 5) ? get_double("R (Ω): ", 1) : r_presets[rp-1].value;
        
        printf("\nL presets:\n"); 
        for (int i = 0; i < 4; i++) printf("%d. %s\n", i+1, l_presets[i].name);
        printf("5. Custom\nSelect: "); 
        int lp = get_int("", 1, 5);
        L = (lp == 5) ? get_double("L (H): ", 1) : l_presets[lp-1].value;
        
        printf("\nC presets:\n"); 
        for (int i = 0; i < 4; i++) printf("%d. %s\n", i+1, c_presets[i].name);
        printf("5. Custom\nSelect: "); 
        int cp = get_int("", 1, 5);
        C = (cp == 5) ? get_double("C (F): ", 1) : c_presets[cp-1].value;
    }

    double wn = 1/sqrt(L*C), zeta = R/2 * sqrt(C/L);
    printf("\nωₙ = %.0f rad/s | ζ = %.3f → %s\n", wn, zeta, 
           zeta < 0.95 ? "underdamped" : zeta < 1.05 ? "critical" : "overdamped");

    DataSeries s; strcpy(s.label, "Voltage V(t)"); rlc_transient(&s, zeta, wn);
    
    if (zeta < 0.95) printf("Underdamped (ringing present)\n");
    else if (zeta > 1.05) printf("Overdamped (slow response)\n");
    else printf("Critically damped\n");
    
    char log_msg[200];
    sprintf(log_msg, "R=%.0fΩ, L=%.3fH, C=%.6fF, ζ=%.3f", R, L, C, zeta);
    log_simulation("Electrical", "RLC Circuit", log_msg);
    
    plot_ascii(&s); 
    save_to_file_electrical(&s); 
    save_to_lab(&s, "RLC saved"); 
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void rc_circuit_sim(void) {
    print_header("RC Charge Simulator");
    printf("\nR presets:\n"); for (int i = 0; i < 4; i++) printf("%d. %s\n", i+1, r_presets[i].name);
    printf("5. Custom\nSelect: "); int rp = get_int("", 1, 5);
    double R = (rp == 5) ? get_double("R (Ω): ", 1) : r_presets[rp-1].value;
    
    printf("\nC presets:\n"); for (int i = 0; i < 4; i++) printf("%d. %s\n", i+1, c_presets[i].name);
    printf("5. Custom\nSelect: "); int cp = get_int("", 1, 5);
    double C = (cp == 5) ? get_double("C (F): ", 1) : c_presets[cp-1].value;

    double tau = R*C;
    printf("\nτ = %.6f s (%.2f ms)\n", tau, tau*1000);

    DataSeries s; strcpy(s.label, "Voltage V(t)"); rc_charge_curve(&s, tau);
    
    plot_ascii(&s); 
    save_to_file_electrical(&s); 
    save_to_lab(&s, "RC saved"); 
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void buck_converter_designer(void) {
    print_header("Buck Converter Designer");
    double Vin = get_double("Vin (V): ", 1);
    double Vout = get_double("Vout (V) < Vin: ", 1);
    while (Vout >= Vin) { printf("Must be < Vin: "); scanf("%lf", &Vout); }
    double Iload = get_double("Load current (A): ", 1);
    double f = get_double("fsw (kHz): ", 1); double fsw = f*1000;

    double D = Vout/Vin, Lmin = (Vin-Vout)*Vout/(0.3*Iload*fsw);

    printf("\nDuty cycle D = %.3f (%.1f%%)\n", D, D*100);
    printf("Lmin = %.1f µH\n", Lmin*1e6);
    printf("Peak current ≈ %.3f A\n", Iload*1.15);

    printf("\nSchematic:\n  Vin ──┬──SW──┬──L──┬── Vout\n        │      D     C──Load\n       GND    GND   GND\n");
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

void electrical_suite(void) {
    int c;
    while (1) {
        print_header("Electrical Calculator");
        printf("1. RLC Transient\n2. RC Charge\n3. Buck Converter\n4. Unit Converter\n5. Quick Calculator\n6. Back\n\nSelect: ");
        
        if (scanf("%d", &c) != 1 || c < 1 || c > 6) {
            flush_input();
            printf("\nInvalid input, try 1-6\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }
        flush_input();
        
        if (c == 1) rlc_circuit_sim();
        else if (c == 2) rc_circuit_sim();
        else if (c == 3) buck_converter_designer();
        else if (c == 4) unit_converter();
        else if (c == 5) quick_calc();
        else if (c == 6) return;
    }
}