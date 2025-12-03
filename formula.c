#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "formula.h"
#include "utils.h"
#include "colors.h"

static Formula formulas[] = {
    {"Ohm's Law", "V = I * R", "Electrical", "V, A, Ohms"},
    {"Power (DC)", "P = V * I", "Electrical", "W, V, A"},
    {"Power (resistive)", "P = I^2 * R", "Electrical", "W, A, Ohms"},
    {"Capacitor Energy", "E = 0.5 * C * V^2", "Electrical", "J, F, V"},
    {"Inductor Energy", "E = 0.5 * L * I^2", "Electrical", "J, H, A"},
    {"RC Time Constant", "tau = R * C", "Electrical", "s, Ohms, F"},
    {"RL Time Constant", "tau = L / R", "Electrical", "s, H, Ohms"},
    {"Resonant Freq", "f = 1 / (2*pi*sqrt(L*C))", "Electrical", "Hz, H, F"},
    {"Impedance (RLC)", "Z = sqrt(R^2 + (XL-XC)^2)", "Electrical", "Ohms"},
    {"Capacitive React", "XC = 1 / (2*pi*f*C)", "Electrical", "Ohms, Hz, F"},
    {"Inductive React", "XL = 2*pi*f*L", "Electrical", "Ohms, Hz, H"},
    {"Voltage Divider", "Vout = Vin * R2/(R1+R2)", "Electrical", "V, Ohms"},
    
    {"Newton's 2nd Law", "F = m * a", "Mechanical", "N, kg, m/s^2"},
    {"Kinetic Energy", "KE = 0.5 * m * v^2", "Mechanical", "J, kg, m/s"},
    {"Potential Energy", "PE = m * g * h", "Mechanical", "J, kg, m"},
    {"Work", "W = F * d * cos(theta)", "Mechanical", "J, N, m"},
    {"Power", "P = W / t", "Mechanical", "W, J, s"},
    {"Momentum", "p = m * v", "Mechanical", "kg*m/s"},
    {"Hooke's Law", "F = k * x", "Mechanical", "N, N/m, m"},
    {"Spring Energy", "E = 0.5 * k * x^2", "Mechanical", "J, N/m, m"},
    {"Stress", "sigma = F / A", "Mechanical", "Pa, N, m^2"},
    {"Strain", "epsilon = dL / L", "Mechanical", "dimensionless"},
    {"Young's Modulus", "E = stress / strain", "Mechanical", "Pa"},
    {"Beam Deflection", "d = F*L^3 / (3*E*I)", "Mechanical", "m"},
    {"Moment of Inertia", "I = m * r^2", "Mechanical", "kg*m^2"},
    {"Torque", "T = F * r", "Mechanical", "N*m"},
    {"Angular Velocity", "omega = 2*pi*f", "Mechanical", "rad/s, Hz"},
    {"Centripetal Force", "F = m*v^2 / r", "Mechanical", "N"},
    {"Pendulum Period", "T = 2*pi*sqrt(L/g)", "Mechanical", "s, m"},
    {"Friction Force", "F = mu * N", "Mechanical", "N"},
    
    {"Natural Frequency", "wn = sqrt(k/m)", "Control", "rad/s"},
    {"Damping Ratio", "zeta = c / (2*sqrt(k*m))", "Control", "dimensionless"},
    {"Rise Time", "tr = 1.8 / wn", "Control", "s"},
    {"Settling Time", "ts = 4 / (zeta*wn)", "Control", "s"},
    {"Peak Time", "tp = pi / wd", "Control", "s"},
    {"Overshoot", "Mp = exp(-pi*zeta/sqrt(1-zeta^2))", "Control", "%"},
    {"Damped Frequency", "wd = wn*sqrt(1-zeta^2)", "Control", "rad/s"},
    {"Transfer Function", "G(s) = wn^2/(s^2+2*zeta*wn*s+wn^2)", "Control", ""},
    {"PID Output", "u = Kp*e + Ki*int(e) + Kd*de/dt", "Control", ""},
    {"Closed Loop Gain", "T = G / (1 + G*H)", "Control", ""},
    
    {"Pip Value (USD)", "pip = lot * 0.0001 * 100000", "Forex", "$"},
    {"Position Size", "lots = risk / (SL_pips * pip)", "Forex", "lots"},
    {"Risk Amount", "risk = account * risk%", "Forex", "$"},
    {"R:R Ratio", "RR = TP_pips / SL_pips", "Forex", "ratio"},
    {"Margin (1:100)", "margin = lots * 100000 / 100", "Forex", "$"},
    {"Profit/Loss", "PnL = pips * pip_value * lots", "Forex", "$"},
    
    {"Mean", "mean = sum(x) / n", "Statistics", ""},
    {"Std Deviation", "std = sqrt(sum((x-mean)^2)/n)", "Statistics", ""},
    {"Variance", "var = sum((x-mean)^2) / n", "Statistics", ""},
    
    {"Velocity", "v = d / t", "Physics", "m/s"},
    {"Acceleration", "a = (v2 - v1) / t", "Physics", "m/s^2"},
    {"Displacement", "s = ut + 0.5*a*t^2", "Physics", "m"},
    {"Final Velocity", "v^2 = u^2 + 2*a*s", "Physics", "m/s"}
};

static Formula custom[MAX_CUSTOM];
static int custom_count = 0;
static int num_formulas = 54;

extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);

static void load_custom(void) {
    FILE *f = fopen("custom_formulas.txt", "r");
    if (!f) return;
    custom_count = 0;
    while (custom_count < MAX_CUSTOM) {
        Formula *c = &custom[custom_count];
        if (fscanf(f, "%49[^|]|%79[^|]|%19[^|]|%29[^\n]\n",
            c->name, c->formula, c->category, c->units) != 4) break;
        custom_count++;
    }
    fclose(f);
}

static void save_custom(void) {
    FILE *f = fopen("custom_formulas.txt", "w");
    if (!f) return;
    for (int i = 0; i < custom_count; i++) {
        Formula *c = &custom[i];
        fprintf(f, "%s|%s|%s|%s\n", c->name, c->formula, c->category, c->units);
    }
    fclose(f);
}

static void display_formula(Formula *f) {
    printf(COLOR_CYAN "%-25s" COLOR_RESET, f->name);
    printf(COLOR_BOLD " %s" COLOR_RESET, f->formula);
    printf("  [%s]\n", f->units);
}

static void browse_all(void) {
    print_header("All Formulas");
    char last_cat[20] = "";
    
    for (int i = 0; i < num_formulas; i++) {
        if (strcmp(formulas[i].category, last_cat) != 0) {
            printf("\n" COLOR_YELLOW "=== %s ===" COLOR_RESET "\n", formulas[i].category);
            strcpy(last_cat, formulas[i].category);
        }
        display_formula(&formulas[i]);
    }
    
    if (custom_count > 0) {
        printf("\n" COLOR_YELLOW "=== Custom ===" COLOR_RESET "\n");
        for (int i = 0; i < custom_count; i++) {
            display_formula(&custom[i]);
        }
    }
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void search_formulas(void) {
    print_header("Search Formulas");
    char query[30];
    printf("Search term: ");
    scanf(" %29[^\n]", query);
    flush_input();
    
    printf("\nResults:\n\n");
    int found = 0;
    
    for (int i = 0; i < num_formulas; i++) {
        if (strstr(formulas[i].name, query) || strstr(formulas[i].formula, query) ||
            strstr(formulas[i].category, query)) {
            display_formula(&formulas[i]);
            found++;
        }
    }
    
    for (int i = 0; i < custom_count; i++) {
        if (strstr(custom[i].name, query) || strstr(custom[i].formula, query)) {
            display_formula(&custom[i]);
            found++;
        }
    }
    
    if (found == 0) printf("No formulas found.\n");
    else printf("\n%d formula(s) found.\n", found);
    
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void by_category(void) {
    print_header("Browse by Category");
    printf("1. Electrical\n2. Mechanical\n3. Control\n4. Forex\n5. Statistics\n6. Physics\n7. Back\nChoice: ");
    int c = get_int("", 1, 7);
    if (c == 7) return;
    
    char *cats[] = {"", "Electrical", "Mechanical", "Control", "Forex", "Statistics", "Physics"};
    printf("\n" COLOR_YELLOW "=== %s Formulas ===" COLOR_RESET "\n\n", cats[c]);
    
    for (int i = 0; i < num_formulas; i++) {
        if (strcmp(formulas[i].category, cats[c]) == 0) {
            display_formula(&formulas[i]);
        }
    }
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

static void add_custom(void) {
    if (custom_count >= MAX_CUSTOM) {
        PRINT_ERROR("Custom formula limit reached");
        printf("\nPress Enter to continue...");
        flush_input();
        getchar();
        return;
    }
    
    print_header("Add Custom Formula");
    Formula *c = &custom[custom_count];
    
    printf("Formula name: ");
    scanf(" %49[^\n]", c->name);
    flush_input();
    
    printf("Formula (e.g. F = m * a): ");
    scanf(" %79[^\n]", c->formula);
    flush_input();
    
    printf("Category: ");
    scanf(" %19[^\n]", c->category);
    flush_input();
    
    printf("Units: ");
    scanf(" %29[^\n]", c->units);
    flush_input();
    
    custom_count++;
    save_custom();
    PRINT_SUCCESS("Formula added!");
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}

void formula_library(void) {
    load_custom();
    int c;
    while (1) {
        print_header("Formula Library");
        printf("%d formulas + %d custom\n\n", num_formulas, custom_count);
        printf("1. Browse All\n");
        printf("2. Search\n");
        printf("3. By Category\n");
        printf("4. Add Custom\n");
        printf("0. Back\n\n");
        printf("Choice: ");
        
        if (scanf("%d", &c) != 1 || c < 0 || c > 4) {
            flush_input();
            printf(COLOR_RED "Invalid input. Please enter 0-4.\n" COLOR_RESET);
            continue;
        }
        flush_input();
        
        if (c == 1) browse_all();
        else if (c == 2) search_formulas();
        else if (c == 3) by_category();
        else if (c == 4) add_custom();
        else return;
    }
}

void formula_library_filtered(const char *category) {
    print_header("Related Formulas");
    printf(COLOR_YELLOW "=== %s Formulas ===" COLOR_RESET "\n\n", category);
    
    int found = 0;
    for (int i = 0; i < num_formulas; i++) {
        if (strcmp(formulas[i].category, category) == 0) {
            display_formula(&formulas[i]);
            found++;
        }
    }
    
    if (found == 0) printf("No formulas found for this category.\n");
    else printf("\n%d formula(s) shown.\n", found);
    
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}