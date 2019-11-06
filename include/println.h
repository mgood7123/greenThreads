//
// Created by brothercomplex on 2/11/19.
//

#ifndef FOREGROUND_SERVICE_PRINTLN_H
#define FOREGROUND_SERVICE_PRINTLN_H

#include <stdio.h>
#include <stdarg.h>

#define println printf("%s:%d:%s: ", __FILE__, __LINE__, __FUNCTION__); println_
#define boolToString(boolean) boolean == true ? "true" : "false"

void println_( const char* format, ... );
#endif //FOREGROUND_SERVICE_PRINTLN_H
