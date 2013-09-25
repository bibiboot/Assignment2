#include <stdio.h>
#include <stdlib.h>
#include "th_token.h"
#include "utility.h"

void* token_init(void *val)
{
    /* Handler for thread */
    unsigned long long token_rate = to_ll(((MyPacketData*)val)->r);
    token_engine(1000000/token_rate);
        

    return((void*)0);        
}

void token_engine(unsigned long long token_rate)
{
    while(1){
        pthread_mutex_lock(&m);
        /* Working code for packet goes here */
        printf("Token thread: %llu\n", token_rate);
        pthread_mutex_unlock(&m);
        break;
    }
}
