#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "colors.h"
#include "file_import.h"

#define PI 3.14159265359

DataSeries global_series[3];
int global_count = 0;

typedef struct { char name[30]; double E_GPa; } Material;
Material presets[5] = {{"Steel", 200.0}, {"Aluminum", 70.0}, {"Concrete", 30.0}, {"Wood", 11.0}, {"Titanium", 110.0}};

double get_double(char *p, int pos) {
    double v; printf("%s", p);
    while (scanf("%lf", &v) != 1 || (pos && v <= 0)) { printf("Invalid - positive number: "); flush_input(); }
    return v;
}

int get_int(char *p, int mn, int mx) {
    int v; printf("%s", p);
    while (scanf("%d", &v) != 1 || v < mn || v > mx) { printf("Invalid - choose %d-%d: ", mn, mx); flush_input(); }
    return v;
}

char get_yn(char *p) {
    char c;
    do {
        printf("%s", p);
        if (scanf(" %c", &c) != 1) { flush_input(); c = 0; }
        if (c != 'y' && c != 'Y' && c != 'n' && c != 'N') printf("Invalid - type y or n\n");
    } while (c != 'y' && c != 'Y' && c != 'n' && c != 'N');
    return c;
}

void plot_ascii(DataSeries *d) {
    double max = d->data[0], min = d->data[0];
    for (int i = 1; i < d->length; i++) { if (d->data[i] > max) max = d->data[i]; if (d->data[i] < min) min = d->data[i]; }
    printf("\n%s\n", d->label);
    for (int row = 15; row >= 0; row--) {
        double level = min + (max - min) * row / 15;
        printf("%7.2f |", level);
        for (int i = 0; i < 50; i++) {
            int idx = i * d->length / 50;
            printf(fabs(d->data[idx] - level) < (max - min) / 15 ? "*" : " ");
        }
        printf("\n");
    }
    printf("        +"); for (int i = 0; i < 50; i++) printf("-"); printf("\n");
}

void save_to_file(DataSeries *d) {
    if (get_yn("Save to file? (y/n): ") != 'y') return;
    char fname[50]; 
    printf("Filename (no spaces): "); 
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
        sprintf(cmd, "python3 plot_mechanical.py %s", fname);
        int result = system(cmd);
        if (result == 0) {
            printf("Graph generated\n");
        } else {
            printf("Failed to generate graph\n");
        }
    }
}

void save_to_lab(DataSeries *d, char *msg) {
    if (get_yn("\nSend to Analytics Lab? (y/n): ") == 'y') {
        if (global_count < 3) { global_series[global_count++] = *d; printf("%s (slot %d/3)\n", msg, global_count); }
        else printf("Lab full - clear first\n");
    }
}

void compare_lab(void) {
    print_header("Cross-Domain Analytics Lab");
    if (global_count == 0) { printf("No data stored\n"); go_back(); return; }
    printf("Stored series: %d/3\n\n", global_count);
    for (int i = 0; i < global_count; i++) printf("%d. %s (max: %.4f)\n", i+1, global_series[i].label, global_series[i].data[global_series[i].length-1]);
    printf("\n1. Plot  2. Export  3. Clear  4. Back\nSelect: ");
    int c = get_int("", 1, 4);
    if (c == 1) { int s = get_int("Plot which? ", 1, global_count); plot_ascii(&global_series[s-1]); go_back(); }
    else if (c == 2) { int s = get_int("Export which? ", 1, global_count); save_to_file(&global_series[s-1]); go_back(); }
    else if (c == 3) { global_count = 0; printf("Lab cleared\n"); go_back(); }
}

static void unit_converter(void) {
    print_header("Unit Converter");
    printf("1. mm ↔ inches\n2. GPa ↔ PSI\n3. kN ↔ lbf\n4. Back\nSelect: ");
    int c = get_int("", 1, 4); if (c == 4) return;
    double v = get_double("Value: ", 0);
    if (c == 1) printf("= %.4f %s\n", v * (v < 100 ? 0.03937 : 25.4), v < 100 ? "inches" : "mm");
    else if (c == 2) printf("= %.2f %s\n", v * (v < 1000 ? 145037.7 : 0.00000689476), v < 1000 ? "PSI" : "GPa");
    else if (c == 3) printf("= %.2f %s\n", v * (v < 10000 ? 224.809 : 0.00444822), v < 10000 ? "lbf" : "kN");
    go_back();
}

static void quick_calc(void) {
    print_header("Quick Reference");
    printf("1. Stress σ = F/A\n2. Strain ε = ΔL/L\n3. Moment of Inertia (rectangle)\n4. Back\nSelect: ");
    int c = get_int("", 1, 4);
    if (c == 1) { double F = get_double("Force F (N): ", 0), A = get_double("Area A (m²): ", 1); printf("Stress = %.2f MPa\n", F / A / 1e6); }
    else if (c == 2) { double dL = get_double("ΔL (mm): ", 0), L = get_double("L (mm): ", 1); printf("Strain = %.6f\n", dL / L); }
    else if (c == 3) { double b = get_double("Width b (mm): ", 1), h = get_double("Height h (mm): ", 1); printf("I = %.2e mm⁴\n", b * h * h * h / 12); }
    if (c != 4) go_back();
}

static void cantilever_beam_deflection(void) {
    print_header("Cantilever Beam Deflection");
    
    printf("1. Manual input\n2. Import from file (beam_input.txt)\nSelect: ");
    int input_mode = get_int("", 1, 2);
    
    double L, E, I, load;
    int t, mp;
    
    if (input_mode == 2) {
        if (load_beam_params(&L, &E, &I, &t, &load)) {
            printf("Parameters loaded from file\n");
            printf("L=%.2fm, E=%.0fGPa, I=%.4fm⁴, Type=%d, Load=%.2f\n", L, E/1e9, I, t, load);
            E *= 1e9;
            if (t == 1) load *= 1000;
            else load *= 1000;
        } else {
            printf("Could not load file, using manual input\n");
            input_mode = 1;
        }
    }
    
    if (input_mode == 1) {
        L = get_double("L (m): ", 1);
        printf("\nMaterial presets:\n");
        for (int i = 0; i < 5; i++) printf("%d. %s (%.0f GPa)\n", i+1, presets[i].name, presets[i].E_GPa);
        printf("6. Custom\nSelect: "); 
        mp = get_int("", 1, 6);
        E = (mp == 6 ? get_double("E (GPa): ", 1) : presets[mp-1].E_GPa) * 1e9;
        I = get_double("I (m⁴): ", 1);
        t = get_int("\n1. Point load  2. Uniform load\nChoose: ", 1, 2);
        load = (t == 1 ? get_double("P (kN): ", 1) : get_double("w (kN/m): ", 1)) * 1000;
    }
    
    DataSeries d; strcpy(d.label, "Deflection (mm)"); d.length = 100;
    
    if (t == 1) {
        printf("\nδ(x) = Px²(3L-x)/(6EI)\n");
        for (int i = 0; i < 100; i++) { 
            double x = L * i / 99.0; 
            d.data[i] = load * x * x * (3*L - x) / (6 * E * I) * 1000; 
        }
    } else {
        printf("\nδ(x) = wx²(6L²-4Lx+x²)/(24EI)\n");
        for (int i = 0; i < 100; i++) { 
            double x = L * i / 99.0; 
            d.data[i] = load * x * x * (6*L*L - 4*L*x + x*x) / (24 * E * I) * 1000; 
        }
    }
    
    printf("\nMax deflection = %.4f mm\n", d.data[99]);
    if (d.data[99] > 10) {
        printf("Large deflection - increase I or E\n");
    }
    
    char log_msg[200];
    sprintf(log_msg, "L=%.2fm, E=%.0fGPa, Max=%.4fmm", L, E/1e9, d.data[99]);
    log_simulation("Mechanical", "Beam Deflection", log_msg);
    
    plot_ascii(&d); 
    save_to_file(&d); 
    save_to_lab(&d, "Beam data saved"); 
    go_back();
}

void spring_mass_damper(DataSeries *s, double zeta, double wn) {
    double dt = 0.01; s->length = 1000;
    for (int i = 0; i < s->length; i++) s->data[i] = exp(-zeta * wn * i * dt) * cos(wn * sqrt(1 - zeta*zeta) * i * dt);
}

static void spring_mass_damper_sim(void) {
    print_header("Spring-Mass-Damper");
    double m = get_double("Mass m (kg): ", 1), k = get_double("Spring k (N/m): ", 1), c = get_double("Damping c (Ns/m): ", 0);
    double wn = sqrt(k/m), zeta = c/(2*sqrt(m*k));
    printf("\nωₙ = %.3f rad/s | ζ = %.3f → %s\n", wn, zeta, zeta < 0.95 ? "underdamped" : zeta < 1.05 ? "critical" : "overdamped");
    if (zeta < 0.7) printf("Underdamped (oscillatory)\n");
    DataSeries s; strcpy(s.label, "Displacement x(t)"); spring_mass_damper(&s, zeta, wn);
    plot_ascii(&s); save_to_file(&s); save_to_lab(&s, "Spring data saved"); go_back();
}

static void simple_pendulum_sim(void) {
    print_header("Simple Pendulum");
    double L = get_double("Length L (m): ", 1), th0 = get_double("Initial angle θ₀ (degrees): ", 0), b = get_double("Damping b (1/s, 0=none): ", 0);
    double th = th0 * PI / 180, g = 9.81, w0 = sqrt(g/L);
    printf("\nPeriod ≈ %.3f s\n", 2*PI/w0);
    DataSeries s; strcpy(s.label, "Angle θ(t) (rad)"); s.length = 1000; double dt = 0.01;
    if (th0 <= 15 && b == 0) { for (int i = 0; i < s.length; i++) s.data[i] = th * cos(w0 * i * dt); }
    else if (th0 <= 15) {
        double zeta = b / (2*w0);
        for (int i = 0; i < s.length; i++) { double t = i * dt; s.data[i] = th * exp(-b * t) * cos(w0 * sqrt(1 - zeta*zeta) * t); }
    } else {
        double thv = th, w = 0;
        for (int i = 0; i < s.length; i++) { s.data[i] = thv; double a = -(g/L) * sin(thv) - b * w; w += a * dt; thv += w * dt; }
    }
    if (th0 > 15) printf("Nonlinear behavior\n");
    plot_ascii(&s); save_to_file(&s); save_to_lab(&s, "Pendulum data saved"); go_back();
}

void mechanical_suite(void) {
    int c;
    while (1) {
        print_header("Mechanical Calculator");
        printf("1. Beam Deflection\n2. Spring-Mass-Damper\n3. Simple Pendulum\n4. Unit Converter\n5. Quick Calculator\n6. Back\n\nSelect: ");
        if (scanf("%d", &c) != 1 || c < 1 || c > 6) {
            flush_input();
            printf("\n*** Invalid - choose 1-6 ***\n");
            go_back();
            continue;
        }
        flush_input();
        if (c == 1) cantilever_beam_deflection();
        else if (c == 2) spring_mass_damper_sim();
        else if (c == 3) simple_pendulum_sim();
        else if (c == 4) unit_converter();
        else if (c == 5) quick_calc();
        else if (c == 6) return;
    }
}

DataSeries* get_global_series(int *count) { *count = global_count; return global_series; }