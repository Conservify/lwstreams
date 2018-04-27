#include <cstdio>

#include "debug.h"

namespace lws {

void __lws_assert(const char *msg, const char *file, int lineno) {
    #ifdef ARDUINO
    #else
    fprintf(stderr, "ASSERTION: %s:%d '%s'", file, lineno, msg);
    #endif
}

}
