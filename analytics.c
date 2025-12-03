#include <stdio.h>
#include <math.h>
#include <string.h>
#include "analytics.h"
#include "types.h"
#include "utils.h"

extern DataSeries global_series[3];
extern int global_count;
extern double get_double(char *p, int pos);
extern int get_int(char *p, int mn, int mx);
extern void plot_ascii(DataSeries *d);
extern void save_to_file(DataSeries *d);

void plot_multiple(void) {
    double max_val = global_series[0].data[0], min_val = global_series[0].data[0];
    
    for (int s = 0; s < global_count; s++) {
        for (int i = 0; i < global_series[s].length; i++) {
            if (global_series[s].data[i] > max_val) max_val = global_series[s].data[i];
            if (global_series[s].data[i] < min_val) min_val = global_series[s].data[i];
        }
    }
    
    printf("\nOverlay plot\n");
    char symbols[] = {'*', '#', 'o'};
    
    for (int row = 15; row >= 0; row--) {
        double level = min_val + (max_val - min_val) * row / 15;
        printf("%7.2f |", level);
        
        for (int col = 0; col < 50; col++) {
            char print_char = ' ';
            for (int s = 0; s < global_count; s++) {
                int idx = col * global_series[s].length / 50;
                if (fabs(global_series[s].data[idx] - level) < (max_val - min_val) / 15) {
                    print_char = symbols[s];
                }
            }
            printf("%c", print_char);
        }
        printf("\n");
    }
    printf("        +"); 
    for (int i = 0; i < 50; i++) printf("-"); 
    printf("\n\nLegend: ");
    for (int i = 0; i < global_count; i++) {
        printf("%c = %s   ", symbols[i], global_series[i].label);
    }
    printf("\n");
}

void analytics_suite(void) {
    print_header("Data Analysis");
    
    if (global_count == 0) { 
        printf("No data stored.\n");
        printf("Run simulations and select 'Send to Analytics Lab'.\n");
        printf("\nPress Enter to continue...");
        getchar();
        return; 
    }
    
    printf("Stored: %d/3\n\n", global_count);
    for (int i = 0; i < global_count; i++) {
        printf("%d. %s\n", i+1, global_series[i].label);
    }
    
    printf("\n1. Plot single  2. Plot all  3. Export  4. Clear  5. Back\nSelect: ");
    int c;
    if (scanf("%d", &c) != 1 || c < 1 || c > 5) {
        flush_input();
        printf("\nInvalid input, try 1-5\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    flush_input();
    
    if (c == 5) return;
    
    if (c == 1) { 
        int s = get_int("Plot which? ", 1, global_count); 
        plot_ascii(&global_series[s-1]); 
        printf("\nPress Enter to continue...");
        getchar();
    } else if (c == 2) {
        if (global_count < 2) {
            printf("\nNeed at least 2 series\n");
            printf("Press Enter to continue...");
            getchar();
        } else {
            plot_multiple();
            printf("\nPress Enter to continue...");
            getchar();
        }
    } else if (c == 3) { 
        int s = get_int("Export which? ", 1, global_count); 
        save_to_file(&global_series[s-1]); 
        printf("\nPress Enter to continue...");
        getchar();
    } else if (c == 4) { 
        global_count = 0; 
        printf("\nData cleared\n"); 
        printf("Press Enter to continue");
        getchar();
    }
}