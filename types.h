#ifndef TYPES_H
#define TYPES_H

#define MAX_POINTS 2000
#define MAX_STRING 64

typedef struct {
    double data[MAX_POINTS];
    int length;
    char label[MAX_STRING];
} DataSeries;

typedef struct {
    double zeta;
    double omega_n;
    double settling_time;
    double overshoot;
} SecondOrderResult;

#endif