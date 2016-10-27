#include <b64/cencode.h>
#include <b64/cdecode.h>
#include <string.h>

int base64_decode(char *encoded, char* decoded)
{
    base64_decodestate state;
    base64_init_decodestate(&state);
    int count = base64_decode_block(encoded, strlen(encoded), decoded, &state);
    return count;
}
