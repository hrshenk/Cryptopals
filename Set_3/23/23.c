#include <stdio.h>
#include <time.h>
#include "mersenne.h"

unsigned int inverse_temper(unsigned int x);

int main()
{
    mt_state server, reproduced;
    mt_init(&server, time(NULL));
    unsigned int rando, i;
    
    //just to show we can start observing the server output at any time,
    //as opposed to being restricted to capturing the output from the start.
    rando = mt_get_rand(&server) % 1000;
    for(i=0; i<rando; ++i)
    {
        mt_get_rand(&server);
    }
    
    //we'll grab 624 consecutive randos and reproduce the state array of the server.
    for(i=0; i<624; ++i)
    {
        rando = mt_get_rand(&server); //get a random number from the 'server'
        reproduced.array[i] = inverse_temper(rando); //fill the state array with untempered values
    }
    reproduced.iterator = 624;
    
    if( mt_get_rand(&server) == mt_get_rand(&reproduced) )
    {
        puts("We have a weener!");
    }
    
    
}

unsigned int inverse_temper(unsigned int x)
{
    x ^= (x >> 18);  
    x ^= (x << 15) & 0xefc60000; 
    
    x ^= (x << 7) & (0x3f80 & 0x9d2c5680); //14
    x ^= (x << 7) & (0x1fc000 & 0x9d2c5680);//21
    x ^= (x << 7) & (0xfe00000 & 0x9d2c5680);//28
    x ^= (x << 7) & (0xf0000000 & 0x9d2c5680);//32 bits restored
   
    x ^= (x >> 11) & 0x1ffc00; //gets us 22 bits back
    x ^= (x >> 11) & 0x3ff;//gets us last 10
    
    return x;
    
}