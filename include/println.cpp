//
// Created by brothercomplex on 3/11/19.
//

#include "println.h"

void println_(const char* format, ... ) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}