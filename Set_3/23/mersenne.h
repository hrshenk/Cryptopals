#include <stdio.h>

#define RECURRENCE_DEGREE 624
#define MIDDLE_WORD 397
#define MATRIX_A 0x9908b0df //this is the last row of matrix A
#define UPPER_MASK 0x80000000
#define LOWER_MASK 0x7fffffff

typedef struct mt_state
{
    unsigned int array[RECURRENCE_DEGREE];
    unsigned int seed;  //saved for convenience of reproducibility
    unsigned int iterator;
} mt_state;


void mt_init(mt_state *state, unsigned int seed);
unsigned int mt_get_rand(mt_state *state);