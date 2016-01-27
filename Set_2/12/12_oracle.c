#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

#define Input_Size 5000 //limits number of chars read by fgets, then buffers are defined via this value.

int base64_decode(unsigned char *encoded, unsigned char *decoded);
unsigned int compute_max_int(); //computes the maximum unsigned integer value.
int encrypt_ecb(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *ciphertext);
void handleErrors(void);
unsigned char key[16], key_set_flag =0;

int main()
{
    unsigned int i,file_size, array_size, user_input_size, plaintext_length, ciphertext_length; //array_size will be used to calculate how much memory must be allocated.
    FILE *fpin, *fpurand;
    unsigned char *buf, *base64, *ciphertext;
    
    
    fpin = fopen("12_test.txt", "r");
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
    
    //read in the user input and put it in buf
    printf("Enter string\n");
    fgets(buf, Input_Size, stdin);
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
        fclose(fpurand);
    }
    printf("\n");
    
    //prep for encryption and encrypt
    ciphertext_length = (plaintext_length + (16 - (plaintext_length % 16)));
    if(ciphertext_length < plaintext_length)
    {
        printf("wrapped int\n");
        return -1;
    }
    
    ciphertext = (unsigned char*)malloc(ciphertext_length);
    if(ciphertext == NULL)
    {
        printf("malloc failed \n");
        return -1;
    }
    
    encrypt_ecb(buf, plaintext_length, key, ciphertext);

    if(ciphertext_length != (plaintext_length + (16 - (plaintext_length % 16))))
    {
        printf("cipher length differs from expected value\n");
        return -1;
    }
    for(i=0; i<ciphertext_length; i++)
    {
        printf("%02x",ciphertext[i]);
    }
    printf("\n");
    
    

    return 0;
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
    int i, j=0, input_length, flag = 0;
    unsigned int temp = 0;
    
    
    input_length = strlen(input);
    //perform rudimentary input check.  
    if(input_length %4 != 0)
    {
        printf("invalid input.  Input length %u\n", input_length);
        printf("%02x\t%02x\n", input[input_length -2], input[input_length-1]);
        return 0;
    }
    
    for(i=0; i<input_length; i++)
    {
        if(temp != 0)
        {
            temp = (temp << 6);
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
                printf("invalid base64: %c\n%d\n", input[i], i);
                return 0;
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

