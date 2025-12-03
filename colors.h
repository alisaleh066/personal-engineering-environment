#ifndef COLORS_H
#define COLORS_H

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

#define PRINT_SUCCESS(msg) printf(COLOR_GREEN "%s" COLOR_RESET "\n", msg)
#define PRINT_ERROR(msg) printf(COLOR_RED "%s" COLOR_RESET "\n", msg)
#define PRINT_WARNING(msg) printf(COLOR_YELLOW "%s" COLOR_RESET "\n", msg)
#define PRINT_INFO(msg) printf(COLOR_CYAN "%s" COLOR_RESET "\n", msg)

#endif