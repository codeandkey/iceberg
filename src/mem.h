#ifndef IB_MEM
#define IB_MEM

#include <stdlib.h>

#define ib_malloc(n) malloc(n)
#define ib_free(p) (free(p), 0)
#define ib_realloc(p, n) realloc(p, n)
#define ib_zero(p, n) memset(p, 0, n)

#endif
