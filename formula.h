#ifndef FORMULA_H
#define FORMULA_H

#define MAX_FORMULAS 60
#define MAX_CUSTOM 10

typedef struct {
    char name[50];
    char formula[80];
    char category[20];
    char units[30];
} Formula;

void formula_library(void);
void formula_library_filtered(const char *category);

#endif