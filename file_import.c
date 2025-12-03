#include <stdio.h>
#include "file_import.h"

int load_beam_params(double *L, double *E, double *I, int *type, double *load) {
    FILE *f = fopen("beam_input.txt", "r");
    if (!f) return 0;
    
    if (fscanf(f, "%lf", L) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%lf", E) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%lf", I) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%d", type) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%lf", load) != 1) { fclose(f); return 0; }
    
    fclose(f);
    return 1;
}

int load_rlc_params(double *R, double *L, double *C) {
    FILE *f = fopen("rlc_input.txt", "r");
    if (!f) return 0;
    
    if (fscanf(f, "%lf", R) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%lf", L) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%lf", C) != 1) { fclose(f); return 0; }
    
    fclose(f);
    return 1;
}

int load_rc_params(double *R, double *C) {
    FILE *f = fopen("rc_input.txt", "r");
    if (!f) return 0;
    
    if (fscanf(f, "%lf", R) != 1) { fclose(f); return 0; }
    if (fscanf(f, "%lf", C) != 1) { fclose(f); return 0; }
    
    fclose(f);
    return 1;
}