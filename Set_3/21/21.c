#include "mersenne.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    mt_state *state = (mt_state *)malloc(sizeof(mt_state));
    mt_init(state, 5489);
    int i;
    for(i=0; i<1000; ++i)
    {
        printf("%u\n",mt_get_rand(state));
    }
    return 0;
    
}