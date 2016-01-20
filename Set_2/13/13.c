#include <stdio.h>

typedef struct profile{
    unsigned char email[50];
    unsigned int uid;
    unsigned char role[6];
    
}profile;

profile profile_for(unsigned char *input[]);

int main()
{
   puts("Please enter your email address");
    //Get user input
    char user_input[50] = {0};
    if(fgets(user_input, 50, stdin))
    {
        puts(user_input);
    }
    else
    {
        puts("Error");
        return 0;
    }

    
    return 0;
}