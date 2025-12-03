#ifndef FILE_IMPORT_H
#define FILE_IMPORT_H

int load_beam_params(double *L, double *E, double *I, int *type, double *load);
int load_rlc_params(double *R, double *L, double *C);
int load_rc_params(double *R, double *C);

#endif