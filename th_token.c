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
    unsigned long long count = 1;

    while(1){
         char MESG[1024];
        /* Create message. */
        sprintf(MESG, "token t%llu arrives, token bucket now has 1 token, arrival-time = %llu", count++, token_rate);
        /* Calculate the actual sleep time. */
        unsigned long long actual_inter_time = time_to_sleep(2, token_rate);
        /* Sleep for actual inter-arrival time. */
        printf("Actual time to sleep = %llu\n", actual_inter_time);
        usleep(actual_inter_time);
        print_emulation_time(2, MESG);
        pthread_mutex_lock(&m);
        //usleep(actual_inter_time);
        /* Working code for packet goes here */
        pthread_mutex_unlock(&m);

    }
}
