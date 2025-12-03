#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "colors.h"

extern void mechanical_suite(void);
extern void electrical_suite(void);
extern void finance_suite(void);
extern void analytics_suite(void);
extern void productivity_suite(void);
extern void control_suite(void);
extern void formula_library(void);
extern void csv_analyzer(void);
extern void utilities_menu(void);
extern void show_overview(void);

int main(void) {
    int choice;
    while (1) {
        print_header("Personal Engineering Environment");
        
        printf("  0. Overview\n\n");
        
        printf(COLOR_YELLOW "ENGINEERING:" COLOR_RESET "\n");
        printf("  1. Mechanical\n");
        printf("  2. Electrical\n");
        printf("  3. Control Systems\n\n");
        
        printf(COLOR_YELLOW "PRODUCTIVITY:" COLOR_RESET "\n");
        printf("  4. Forex Trading\n");
        printf("  5. Study Tools\n\n");
        
        printf(COLOR_YELLOW "DATA & ANALYSIS:" COLOR_RESET "\n");
        printf("  6. Data Lab\n");
        printf("  7. CSV Analyzer\n");
        printf("  8. Formula Library\n\n");
        
        printf("  9. Utilities\n");
        printf("  X. Exit\n\n");
        
        printf("Select: ");
        
        char input[10];
        if (scanf("%s", input) != 1) {
            flush_input();
            continue;
        }
        flush_input();
        
        if (input[0] == 'x' || input[0] == 'X') {
            print_header("Exit");
            return 0;
        }
        
        choice = input[0] - '0';
        
        if (choice == 0) show_overview();
        else if (choice == 1) mechanical_suite();
        else if (choice == 2) electrical_suite();
        else if (choice == 3) control_suite();
        else if (choice == 4) finance_suite();
        else if (choice == 5) productivity_suite();
        else if (choice == 6) analytics_suite();
        else if (choice == 7) csv_analyzer();
        else if (choice == 8) formula_library();
        else if (choice == 9) utilities_menu();
        else {
            printf(COLOR_RED "\nInvalid input. Please enter 0-9 or X.\n" COLOR_RESET);
            go_back();
        }
    }
}