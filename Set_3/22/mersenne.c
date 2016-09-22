//implements MT19937 prng.  user must create and properly initialize an 
//mt_state struct before calling mt_get_rand()
#include <stdio.h>
#include "mersenne.h"

void mt_init(mt_state *state, unsigned int seed)
{
    int i;
    state->array[0] = seed;
    state->seed = seed; //for convenience of reproduction.
    
    for(i=1; i<RECURRENCE_DEGREE; ++i)
    {
        state->array[i] = (1812433253 * (state->array[i-1] ^ (state->array[i-1] >> 30)) + i);
    }
    state->iterator = RECURRENCE_DEGREE;
    return;
}

unsigned int mt_get_rand(mt_state *state)
{
    //we assume the state structure has been initiated
    
    //if we've gotten all we can out of the current state array
    //then renew the state array
    unsigned int x, k;
    if(state->iterator == RECURRENCE_DEGREE)
    {
        for(k=0; k<RECURRENCE_DEGREE-MIDDLE_WORD; k++)
        {
            x = ( state->array[k] & UPPER_MASK ) | ( state->array[k+1] & LOWER_MASK );
            //multiplication by MATRIX_A and addition of middleword
            if(x & 0x1) state->array[k] = state->array[k+MIDDLE_WORD] ^ (x >> 1) ^ MATRIX_A;
            else state->array[k] = state->array[k+MIDDLE_WORD] ^ (x >> 1);
        }
        for(; k<RECURRENCE_DEGREE-1; k++)
        {
            x = ( state->array[k] & UPPER_MASK ) | ( state->array[k+1] & LOWER_MASK );
            //multiplication by MATRIX_A and addition of middleword
            if(x & 0x1) state->array[k] = state->array[k-(RECURRENCE_DEGREE-MIDDLE_WORD)] ^ (x >> 1) ^ MATRIX_A;
            else state->array[k] = state->array[k-(RECURRENCE_DEGREE-MIDDLE_WORD)] ^ (x >> 1);            
        }
        
        x = ( state->array[k] & UPPER_MASK ) | ( state->array[0] & LOWER_MASK );
        //multiplication by MATRIX_A and addition of middleword
        if(x & 0x1) state->array[k] = state->array[MIDDLE_WORD - 1] ^ (x >> 1) ^ MATRIX_A;
        else state->array[k] = state->array[MIDDLE_WORD-1] ^ (x >> 1);
        state->iterator = 0;
    }
    
    //grab a number from the state array and temper it
    x = state->array[state->iterator];
    state -> iterator += 1;
    x ^= (x >> 11);
    x ^= (x << 7) & 0x9d2c5680UL;
    x ^= (x << 15) & 0xefc60000UL;
    x ^= (x >> 18);
    
    return x;
}
