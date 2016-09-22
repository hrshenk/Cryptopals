#include <stdio.h>
#include <time.h>
#include "mersenne.h"
#define WAIT_MAX 960

static int serve_seed;

int main()
{
    int rando = serve_random_number();
    unsigned int crack_seed = time(NULL);
    unsigned int end = crack_seed;
    crack_seed = crack_seed - (2 * (WAIT_MAX + 40) + 100);  //starting point for our tests.  In real life we'd have to guess at this.
    mt_state state;
    while(crack_seed <= end)
    {
        mt_init(&state, crack_seed);
        if(rando == mt_get_rand(&state))
        {
            break;
        }
        crack_seed++;
    }
    
    if(serve_seed == crack_seed)
    {
        printf("WINNER!\nCracked seed: %d\n",crack_seed);
    }
    
    return crack_seed;
    
}

int serve_random_number()
{
    
    int i;
    srand(time(NULL));
    unsigned int rando_seconds = (rand() % WAIT_MAX) + 40;  //between 40 and 100 for now
    sleep(rando_seconds);
    mt_state state;
    mt_init(&state, time(NULL)); //between 40 and 100 for now
    serve_seed = state.seed;
    rando_seconds = (rand() % WAIT_MAX) + 40;
    sleep(rando_seconds);
    return mt_get_rand(&state);
}