#include <stdio.h>
#include <stdlib.h>
#include "th_server.h"
#include "utility.h"

void* server_init(void *val)
{
    /* Handler for thread */
    unsigned long long server_rate = to_ll(((MyPacketData*)val)->mu);
    server_engine(1000000/server_rate);

    return((void*)0);        
}

void server_engine(unsigned long long server_rate)
{
    while(1){
        char *mesg = (char *)malloc(sizeof(char*));
        sprintf(mesg, "Server thread");
        pthread_mutex_lock(&m);
        /* Working code for packet goes here */
        print_emulation_time(mesg);
        pthread_mutex_unlock(&m);
        break;
    }
}
