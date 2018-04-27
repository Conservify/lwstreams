#ifndef LWS_DEBUG_H_INCLUDED
#define LWS_DEBUG_H_INCLUDED

namespace lws {

#define lws_assert(EX) (void)((EX) || (lws::__lws_assert(#EX, __FILE__, __LINE__), 0))

void __lws_assert(const char *msg, const char *file, int lineno) __attribute__ ((weak));

}

#endif
