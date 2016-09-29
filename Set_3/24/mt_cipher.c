#include "mersenne.h"

unsigned int  mt_encrypt(char *pt, unsigned int len, char *ct, unsigned short key);

unsigned int  mt_encrypt(char *pt, unsigned int len, char *ct, unsigned short key)
{
    mt_state state;
    mt_init(&state, key);
    unsigned int i, stream;
    for(i=0; i<len; ++i)
    {
        if(i%4==0)
        {
            stream = mt_get_rand(&state);
        }
        ct[i] = pt[i] ^  ((char *) &stream)[i];
    }
    
}