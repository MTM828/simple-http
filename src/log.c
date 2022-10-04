#include "log.h"

// i need some sort of log_debug function as well

void log_info(const char *info_message) {
    printf("%sINFO%s: %s\n\n", ansi.blue, ansi.reset, info_message);
}

void log_warn(const char *warning_name, const char *warning_message) {
    if (warning_message != NULL) {
        printf("%sWARNING%s: %s: %s\n\n", ansi.yellow, ansi.reset, warning_name, warning_message);
    } else {
        printf("%sWARNING%s: %s\n\n", ansi.yellow, ansi.reset, warning_name);
    }
}


void log_error(const char *error_name, const char *error_message) {
    if (error_message != NULL) {
        printf("%sERROR%s: %s: %s\n\n", ansi.red, ansi.reset, error_name, error_message);
    } else {
        printf("%sERROR%s: %s\n\n", ansi.red, ansi.reset, error_name);
    }
}
