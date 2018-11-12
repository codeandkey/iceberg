#include "util.h"

int ib_util_col_aabb(ib_ivec2 apos, ib_ivec2 asize, ib_ivec2 bpos, ib_ivec2 bsize) {
    if (apos.x + asize.x < bpos.x) return 0;
    if (apos.x > bpos.x + bsize.x) return 0;
    if (apos.y + asize.y < bpos.y) return 0;
    if (apos.y > bpos.y + bsize.y) return 0;
    return 1;
}
