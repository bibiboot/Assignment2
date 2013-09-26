#include <stdio.h>
#include <stdlib.h>
#include "th_packet.h"
#include "utility.h"

void* packet_init(void *val)
{
    /* Handler for thread */
    /* Decision to go to deter or trace */
    unsigned long long inter_time = 0;
    unsigned long long token = 0;
    unsigned long long service_time = 0;
    //unsigned long long r = 0;
    unsigned long long B = to_ll(((MyPacketData*)val)->B);
    char *FILENAME = NULL;
    FILENAME = ((MyPacketData*)val)->FILENAME;
    
    /*Check if deterministic or trace*/
    if (FILENAME == NULL)
        deter_packet(inter_time, token, service_time);
    else
        trace_packet(FILENAME, B);

    return((void*)0);        
}

void deter_packet(unsigned long long inter_time, 
                  unsigned long long token, 
                  unsigned long long service_time)
{
    /* DETERMINISTIC */
    while(1){
        printf("Hello Determisistic here\n"); 
        break;
    }
}

void trace_packet(char *FILENAME, 
                  unsigned long long B)
{
    /* TRACE_DRIVEN*/
    /* Gather data from file and call packet engine */
    FILE *fp = NULL;
    char buffer[1024];
    int line = 0;
    int result;
    unsigned long long inter_time, token, service_time, num;

    fp = fopen(FILENAME, "r");
    while(fgets(buffer, 1000, fp)!=NULL){
        /* First line is the number of packets */
        if(line==0)
            result = sscanf(buffer, "%llu", &num);
        else
            result = sscanf(buffer, "%llu\t%llu\t%llu", &inter_time, &token, &service_time);

        /* Check for file format */
        if (check_format(result, line, inter_time, token, service_time) == -1){
            fprintf(stderr, "Error in tsfile format\n");
            cleanup(fp);
            exit(0);
        }
        //printf("%s,%d", buffer,result);
        //printf("%llu-%llu-%llu\n", inter_time, token, service_time);

        /* Call Engine */
        packet_engine(millisec_llto_microsec(inter_time), 
                      token, 
                      millisec_llto_microsec(service_time));

        line++;
    }

}

void packet_engine(unsigned long long inter_time, 
                   unsigned long long token, 
                   unsigned long long service_time)
{
    pthread_mutex_lock(&m);
    /* Working code for packet goes here */
    print_emulation_time();
    printf("Packet Thread: %llu\t%llu\t%llu\n", inter_time, token, service_time);
    pthread_mutex_unlock(&m);
    usleep(inter_time);

}

void cleanup(FILE *fp)
{
    /* When the packet thread dies */
    /* Free the resources used by thread_packet */
    if (fp!=NULL){
        fclose(fp);
    }
}
