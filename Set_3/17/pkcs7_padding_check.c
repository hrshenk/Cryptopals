int pkcs7_unpad(char *padded_input, unsigned int input_length)
{
    char pad_count, i;
    pad_count = padded_input[input_length-1];
    if(input_length < pad_count)
    {
        return 0;
    }
    
    if(0<pad_count && pad_count<=16)
    {
        for(i=0; i<pad_count; i++)
        {
            if(padded_input[(input_length-1)-i] != pad_count)
            {
                return 0;
            }
        }
    }
    else 
    {
        return 0;
    }
    
    for(i=0; i<pad_count; i++)
    {
        padded_input[(input_length-1)-i] = 0;
    }
    return 1;   
}