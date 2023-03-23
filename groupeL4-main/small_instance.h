#include <stdint.h>

#define NB_NODES 4
#define NB_LINKS 5
#define LEPL1503_USE_HEADER_INSTANCE

int64_t links[NB_LINKS][3] = {
    {0, 1, 3}, 
    {0, 2, -1}, 
    {2, 3, 1}, 
    {3, 0, 1503}, 
    {3, 2, 4}
};