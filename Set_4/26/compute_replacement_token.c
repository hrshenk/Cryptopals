#include "26.h"
#include <string.h>
void compute_replacement_token(unsigned char *token, token_param *token_params)
{
    unsigned int i, index = token_params->index;
    for(i=0; i<strlen(token_params->xor_in); ++i)
    {
        token[i+index] ^= ( (token_params->xor_in)[i]^(token_params->xor_out)[i] );
    }
}