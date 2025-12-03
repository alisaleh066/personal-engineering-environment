#!/bin/bash

echo "Compiling..."

gcc -Wall -Wextra -o apex \
    main.c \
    overview.c \
    mechanical.c \
    electrical.c \
    finance.c \
    analytics.c \
    utils.c \
    file_import.c \
    productivity.c \
    control.c \
    formula.c \
    csv_analyzer.c \
    settings.c \
    -lm

if [ $? -eq 0 ]; then
    echo "Build successful"
    echo "Run with: ./apex"
else
    echo "Build failed"
fi