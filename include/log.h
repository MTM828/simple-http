#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "ansi-colors.h"

void log_info(const char *info_message);

void log_warn(const char *warning_name, const char *warning_message);

void log_error(const char *error_name, const char *error_message);

#endif
