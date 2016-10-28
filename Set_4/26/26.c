#include "26.h"
int main()
{
    token_t token;
    create_token(&token);
    token_param token_params;
    token_params.xor_in = ";admin=true;aaaa";
    token_params.xor_out = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    token_params.index = 40;
    compute_replacement_token(token.tkn, &token_params);
    accept_token(&token);
    return 0;
}