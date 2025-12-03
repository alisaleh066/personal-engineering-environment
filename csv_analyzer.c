#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "csv_analyzer.h"
#include "utils.h"
#include "colors.h"

static double data[MAX_ROWS];
static int row_count = 0;

extern int get_int(char *p, int mn, int mx);
extern char get_yn(char *p);

static void sort_data(double *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

static void analyze_column(void) {
    if (row_count == 0) {
        PRINT_ERROR("No data loaded");
        return;
    }
    
    double sum = 0, min = data[0], max = data[0];
    for (int i = 0; i < row_count; i++) {
        sum += data[i];
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    double mean = sum / row_count;
    
    double var_sum = 0;
    for (int i = 0; i < row_count; i++) {
        var_sum += (data[i] - mean) * (data[i] - mean);
    }
    double std_dev = sqrt(var_sum / row_count);
    
    double sorted[MAX_ROWS];
    for (int i = 0; i < row_count; i++) sorted[i] = data[i];
    sort_data(sorted, row_count);
    double median = row_count % 2 == 0 ? 
        (sorted[row_count/2 - 1] + sorted[row_count/2]) / 2 : sorted[row_count/2];
    
    printf("\nStatistics:\n\n");
    printf("Count:      %d\n", row_count);
    printf("Sum:        %.4f\n", sum);
    printf("Mean:       %.4f\n", mean);
    printf("Median:     %.4f\n", median);
    printf("Std Dev:    %.4f\n", std_dev);
    printf("Min:        %.4f\n", min);
    printf("Max:        %.4f\n", max);
    printf("Range:      %.4f\n", max - min);
    
    printf("\nOutliers (>2 std dev):\n");
    int outliers = 0;
    for (int i = 0; i < row_count; i++) {
        if (fabs(data[i] - mean) > 2 * std_dev) {
            printf("Row %d: %.4f\n", i + 1, data[i]);
            outliers++;
        }
    }
    if (outliers == 0) printf("None\n");
}

void csv_analyzer(void) {
    print_header("CSV Analyzer");
    
    char filename[50];
    printf("Filename: ");
    if (scanf(" %49s", filename) != 1) {
        flush_input();
        printf("\nInvalid input\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    flush_input();
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        PRINT_ERROR("File not found");
        printf("\nPress Enter to continue...");
        getchar();
        return;
    }
    
    char header[200];
    fgets(header, sizeof(header), f);
    
    char *cols[MAX_COLS];
    int num_cols = 0;
    char *token = strtok(header, ",\n");
    while (token && num_cols < MAX_COLS) {
        cols[num_cols] = token;
        num_cols++;
        token = strtok(NULL, ",\n");
    }
    
    printf("\nColumns:\n");
    for (int i = 0; i < num_cols; i++) {
        printf("  %d. %s\n", i + 1, cols[i]);
    }
    
    int col = get_int("\nAnalyze which? ", 1, num_cols) - 1;
    
    row_count = 0;
    char line[200];
    while (fgets(line, sizeof(line), f) && row_count < MAX_ROWS) {
        char *val = strtok(line, ",\n");
        for (int i = 0; i < col && val; i++) {
            val = strtok(NULL, ",\n");
        }
        if (val) {
            data[row_count] = atof(val);
            row_count++;
        }
    }
    fclose(f);
    
    printf("\nLoaded %d rows\n", row_count);
    analyze_column();
    
    if (get_yn("\nExport stats? (y/n): ") == 'y') {
        char outfile[60];
        strcpy(outfile, filename);
        char *dot = strrchr(outfile, '.');
        if (dot) *dot = '\0';
        strcat(outfile, "_stats.txt");
        
        FILE *out = fopen(outfile, "w");
        if (out) {
            fprintf(out, "Analysis of %s\n", filename);
            fprintf(out, "Count: %d\n", row_count);
            double sum = 0;
            for (int i = 0; i < row_count; i++) sum += data[i];
            fprintf(out, "Mean: %.4f\n", sum / row_count);
            fclose(out);
            printf("Saved to %s\n", outfile);
        }
    }
    
    printf("\nPress Enter to continue...");
    flush_input();
    getchar();
}