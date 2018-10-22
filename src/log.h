#ifndef IB_LOG
#define IB_LOG

#include <stdio.h>

#define ib_log(c, x, ...) printf("[%s] : " c x "\e[0m\n", __func__, ##__VA_ARGS__)

#define ib_ok(x, ...) (ib_log("\e[0;36m", x, ##__VA_ARGS__), 0)
#define ib_err(x, ...) (ib_log("\e[0;31m", "(ERROR) " x, ##__VA_ARGS__), 1)
#define ib_warn(x, ...) (ib_log("\e[0;33m", "(WARNING) " x, ##__VA_ARGS__), 0)
#define ib_debug(x, ...) (ib_log("\e[0;32m", "(DEBUG) " x, ##__VA_ARGS__), 0)

#endif
