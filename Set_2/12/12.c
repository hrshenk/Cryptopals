#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

#define Input_Size 5000 //limits number of chars read by fgets, then buffers are defined via this value.

unsigned int oracle(unsigned char *user_input);
int base64_decode(unsigned char *encoded, unsigned char *decoded);
unsigned int compute_max_int(); //computes the maximum unsigned integer value.
int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext);
void handleErrors(void);
int detect_block_cipher_mode(unsigned char *array, unsigned int length);
unsigned char key[16], key_set_flag = 0;
unsigned char *ciphertext = NULL;


void main()
{
    unsigned int length,block_size, i, j, k=0, block_fill, target_length, input_length, decrypt_success, target_flag =0;
    FILE *fp_pt;
    unsigned char array[Input_Size] = {0}, user_input[Input_Size] = {0};
    unsigned char *target_ciphertext;
    int mode;
    

    //next we'll determine the blocksize
    length = oracle(user_input);
    target_length = length;
    i = 0;
    while(oracle(user_input) <= length) //once the ciphertext length returned by oracle increases we know we required another full block for encryption..
    {
        user_input[i] = 'a';
        i++;
    }
    block_fill = i;  //block fill is the number of bytes required to align the plaintext encrypted 
                    //by oracle with the block size.  So, if we submit i+1 bytes of text to oracle
                    //we'll get back one more block of ciphertext than if we submit i bytes of pt.
    
    block_size = oracle(user_input)-length;
    printf("block size is:  %u\n", block_size);
    
    for(i=0; i<2*block_size; i++)
    {
        user_input[i] = 'a';
    }
    length = oracle(user_input);
    //here we'll detect ECB.  Note detect_block_cipher_mode was written
    //with an assumed block size of 16.  To be more rigorous we should
    //have the function accept the block size as an argument allowing us
    //to detect ECB with any block size.
    mode = detect_block_cipher_mode(ciphertext, length);
    //if ECB isn't detected then quit
    if(mode == 1)
    {
        printf("ECB not detected...attack not valid");
        return;
    }
    
    
     //clear the input array
    while(k<Input_Size)
    {
        //clear the user_input array
        for(i=0; i < Input_Size; i++)
        {
            user_input[i] = 0;
        }
        //construct user_input so plaintext in oracle will hold the next unknown byte
        for(i=0; i<(block_size-1)-(k%block_size); i++)
        {
             user_input[i] = 'a';
        }

        length = oracle(user_input);
        //quick check to avoid integer wraps
        if(length >= length + Input_Size + block_size)
        {
            return;
        }
        
        if(target_flag ==0)
        {
            target_ciphertext = (unsigned char*)calloc((length + Input_Size + block_size), sizeof(char));
            //puts("calloc ran");
            if(target_ciphertext == NULL)
            {
                printf("calloc failed\n");
                return;
            }
            target_flag = 0;
        }
        else
        {
            target_ciphertext = realloc(target_ciphertext, (length + Input_Size + block_size));
            if(target_ciphertext == NULL)
            {
                printf("realloc failed \n");
                return;
            }
        }
        
        
        
        for(i=0; i<length; i++)
        {
            target_ciphertext[i] = ciphertext[i];
        }
        
        
        for(i = ((block_size - 1) - (k % block_size)); i<Input_Size; i++ )
        {
            user_input[i] = array[i-((block_size - 1) - k%block_size)];

        }

        //make a dictionary and test against target ciphertext.
        decrypt_success = 0;
        for(i= 1 ; i< 255; i++) //we can skip non-printable chars since we're assuming printable chars. 
        {
            input_length = strlen(user_input);
            //puts(user_input);
            //printf("\ninput_len %d\n", input_length);
            if(i==1)
            {
                input_length++;
            }

            user_input[input_length-1]=i;

            length = oracle(user_input);
            
            for(j=0; j< input_length; j++)
            {
                if(ciphertext[j] != target_ciphertext[j])
                {
                    break;
                }
            }
            if(j == input_length)
            {
                //printf("%c", user_input[input_length-1]);
                array[k] = user_input[input_length-1];
                k++;
                decrypt_success = 1;
                //printf("decrypt success %d\n", decrypt_success);
                
                break;
            }
            
           

            
        }
        
        if(decrypt_success != 1)
        {
            printf("failed to decrypt byte %u\n", k);
            break;
        }


    }
    printf("\n");
    puts(array);
    free(ciphertext);
    free(target_ciphertext);
    return;
}

unsigned int oracle(unsigned char *user_input)
{
    unsigned int i,file_size, array_size, user_input_size, plaintext_length, ciphertext_length; //array_size will be used to calculate how much memory must be allocated.
    FILE *fpin, *fpurand;
    unsigned char *buf, *base64;
    
    //puts("oracle entered");
    fpin = fopen("12.txt", "r");
    if(fpin == NULL)
    {
        printf("file open failed\n");
        return 1;
    }
    
    if(fseek(fpin, 0, SEEK_END))
    {
        printf("failure in seek\n");
        return 1;
    }
      
    file_size = ftell(fpin); //grab the position indicator..this should be the number of chars in our file
    rewind(fpin);
    
    /*Check to make sure we won't wrap an integer if the contents of a huge file are appended to our
    *user supplied input.  Note: this is a little silly since we know the length of the data we're appending and
    *we're restricting the size of the user input.  But, it illustrates a recognition that when users control
    *inputs problems can arise.  In this case malloc will allocate memory that depends on the size of a read in 
    *file and the maximum size of the user input.  If the sum of these two numbers exceeds the greatest value
    *that can be represented by an unsigned integer, then we could find ourselves in a buffer overflow condition.
    */
    
    
    if(file_size >= (compute_max_int() - Input_Size)) //we could allow file_size to be bigger since conversion
                                                    //from base 64 will compress the data, but meh...close enough.
    {
        printf("File exceeds allowable size.\n");
        return -1;
    }
    
    base64 = (unsigned char*)malloc(file_size);
    if(base64 == NULL)
    {
        printf("Error in malloc.\n");
        return -1;
    }
    base64[file_size]=0;
    
    //read file into an array.  this content will be base64 decoded and appended to user input.
    i=0;
    while(fscanf(fpin, "%c", &base64[i]) != EOF)
    {
        i++;
    }
    //array_size will not wrap an int due to above checks
    array_size = Input_Size + file_size;
    buf = (unsigned char*)malloc(array_size);
    if(buf == NULL)
    {
        printf("Error in malloc\n");
        return -1;
    }
    
    //transfer user_input to buf so we can append decoded file contents.
    for(i=0; i<Input_Size - 1; i++)
    {
        buf[i] = user_input[i];
        if(user_input[i]==0)
        {
            break;
        }
    }
    buf[i] = 0;
    user_input_size = strlen(buf);

    //decode the contents of the file and append them to the user input.
    //base64_decode returns the length of the decoded content.  Should
    //be roughly 3/4 of the input size (to within 2 bytes)
    
    plaintext_length = base64_decode(base64, buf+user_input_size);
    if(plaintext_length == 0)
    {
        puts("base 64 conversion error");
        return 0;
    }
    //puts("buf now holds userinput||decode");
    plaintext_length += user_input_size;  //determine length of userinput||decoded file input for encryption
    
    //check to see if a key has been generated.  If not, then generate one.
    if(key_set_flag == 0)
    {
        fpurand = fopen("/dev/urandom", "r");
        for(i=0; i<16; i++)
        {
            fscanf(fpurand, "%c", &key[i]);
            printf("%02x", key[i]);
        }
        key_set_flag = 1;
        fclose(fpurand);
        printf("\n");
    }
    
    //prep for encryption and encrypt
    ciphertext_length = (plaintext_length + (16 - (plaintext_length % 16)));
    if(ciphertext_length < plaintext_length)
    {
        printf("wrapped int\n");
        return -1;
    }
    
    if(ciphertext == NULL)
    {
        ciphertext = (unsigned char*)malloc(ciphertext_length);
        if(ciphertext == NULL)
        {
            printf("malloc failed \n");
            return -1;
        }
    }
    else
    {
        ciphertext = (unsigned char*)realloc(ciphertext, ciphertext_length);
        if(ciphertext == NULL)
        {
            printf("realloc failed \n");
            return -1;
        }
    }
    
    encrypt_ecb(buf, plaintext_length, key, ciphertext);

    if(ciphertext_length != (plaintext_length + (16 - (plaintext_length % 16))))
    {
        printf("cipher length differs from expected value\n");
        return -1;
    }
    free(base64);
    free(buf);
    fclose(fpin);

    return ciphertext_length;
}

/************************************************************************/
unsigned int compute_max_int()
{
    unsigned int i, ret = 0;
    
    for(i=0; i<(8*sizeof(int)); i++)
    {
        ret = ret + (1<<i);
    }
    return ret;
}

/*************************************************************************/
/*
    base64_decode decodes null terminated base 64 encoded array input and puts results in output.
    output should be sufficiently large to hold the decoded output
    input length should be a multiple of 4.
*/
int base64_decode(unsigned char *input, unsigned char *output)
{
    int i, j=0, input_length, flag = 0, invalid_flag = 0;
    unsigned int temp = 0;

    
    input_length = strlen(input);
    //perform rudimentary input check.  
/*    if(input_length %4 != 0)
    {
        printf("invalid input.  Input length %u\n", input_length);
        return 0;
    }
*/
    
    for(i=0; i<input_length; i++)
    {
        if((temp != 0) && (invalid_flag !=1))
        {
            temp = (temp << 6);
        }
        if(invalid_flag == 1)
        {
            invalid_flag =0;
        }
        switch(input[i])
        {
            case 'A':
                temp += 0;
                break;
            case 'B':
                temp += 1;
                break;
            case 'C':
                temp += 2;
                break;
            case 'D':
                temp += 3;
                break;
            case 'E':
                temp += 4;
                break;
            case 'F':
                temp += 5;
                break;
            case 'G':
                temp += 6;
                break;
            case 'H':
                temp += 7;
                break;
            case 'I':
                temp += 8;
                break;
            case 'J':
                temp += 9;
                break;
            case 'K':
                temp += 10;
                break;
            case 'L':
                temp += 11;
                break;
            case 'M':
                temp += 12;
                break;
            case 'N':
                temp += 13;
                break;
            case 'O':
                temp += 14;
                break;
            case 'P':
                temp += 15;
                break;
            case 'Q':
                temp += 16;
                break;
            case 'R':
                temp += 17;
                break;
            case 'S':
                temp += 18;
                break;
            case 'T':
                temp += 19;
                break;
            case 'U':
                temp += 20;
                break;
            case 'V':
                temp += 21;
                break;
            case 'W':
                temp += 22;
                break;
            case 'X':
                temp += 23;
                break;
            case 'Y':
                temp += 24;
                break;
            case 'Z':
                temp += 25;
                break;
            case 'a':
                temp += 26;
                break;
            case 'b':
                temp += 27;
                break;
            case 'c':
                temp += 28;
                break;
            case 'd':
                temp += 29;
                break;
            case 'e':
                temp += 30;
                break;
            case 'f':
                temp += 31;
                break;
            case 'g':
                temp += 32;
                break;
            case 'h':
                temp += 33;
                break;
            case 'i':
                temp += 34;
                break;
            case 'j':
                temp += 35;
                break;
            case 'k':
                temp += 36;
                break;
            case 'l':
                temp += 37;
                break;
            case 'm':
                temp += 38;
                break;
            case 'n':
                temp += 39;
                break;
            case 'o':
                temp += 40;
                break;
            case 'p':
                temp += 41;
                break;
            case 'q':
                temp += 42;
                break;
            case 'r':
                temp += 43;
                break;
            case 's':
                temp += 44;
                break;
            case 't':
                temp += 45;
                break;
            case 'u':
                temp += 46;
                break;
            case 'v':
                temp += 47;
                break;
            case 'w':
                temp += 48;
                break;
            case 'x':
                temp += 49;
                break;
            case 'y':
                temp += 50;
                break;
            case 'z':
                temp += 51;
                break;
            case '0':
                temp += 52;
                break;
            case '1':
                temp += 53;
                break;
            case '2':
                temp += 54;
                break;
            case '3':
                temp += 55;
                break;
            case '4':
                temp += 56;
                break;
            case '5':
                temp += 57;
                break;
            case '6':
                temp += 58;
                break;
            case '7':
                temp += 59;
                break;
            case '8':
                temp += 60;
                break;
            case '9':
                temp += 61;
                break;
            case '+':
                temp += 62;
                break;
            case '/':
                temp += 63;
                break;
            case '=':
                temp += 0;
                flag += 1;
                break;
            default :
                //printf("invalid base64: input %c\ncharacter num%d\n", input[i], i);
                invalid_flag = 1;
        }
        //if 24 bits are added to tem, then convert to bytes
        if(i%4 == 3)
        {
            
            switch(flag)
            {
                case 1:
                    output[j] = (temp >> 16) & 0xff;
                    j++;
                    output[j] = (temp >> 8) & 0xff;
                    j++;
                    break;
                case 2:
                    output[j] = (temp >> 16) & 0xff;
                    j++;
                    break;
                default:
                    output[j] = (temp >> 16) & 0xff;
                    j++;
                    output[j] = (temp >> 8) & 0xff;
                    j++;
                    output[j] = temp & 0xff;
                    j++;
                
            }
            temp =0;
        }
    }
 
    return j;
}
/* End base64_decode*/

/**************************************************************************************/
int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len, cipher_len = 0;
  

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation.
   */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
  {
      handleErrors();
  }
  
  /* Provide the message to be encrypted, and obtain the encrypted output.
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  
  cipher_len = cipher_len + len;
  /* Finalise the encryption.
   * EVP_EncryptFinal_ex will handle the padding, then encrypt the last block.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + cipher_len, &len)) handleErrors();

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return cipher_len;
}

/*************************************************************************************/
void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}

int detect_block_cipher_mode(unsigned char *array, unsigned int length)
{
    int i, j,k, block_count;
    
    //test our input has valid length
    if(length % 16 != 0)
    {
        printf("invalid length");
        return -1;
    }
    block_count = length/16;
    if(block_count < 2)
    {
        printf("Insufficient number of blocks to accurately determing mode\n");
        return -1;
    }

    for(i=0; i<block_count-1; i++)
    {
        for(j=i+1; j<block_count; j++)
        {
            k=0;
            while(array[i*16+k] == array[j*16+k])
            {
                if(k==15)
                {
                    printf("ECB mode detected\n");
                    return 0;
                }
                k++;
            }

        }
    }
    printf("CBC mode seems to have been used\n");
    return 1;
    
}

