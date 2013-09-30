#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "th_packet.h"
#include "utility.h"


void* packet_init(void *val)
{
    /* Handler for thread */
    /* Decision to go to deter or trace */
    PKT_INTV_ARV_TIME = 0;
    NUM_PACKETS = 0;
    unsigned long long inter_time = MILLI/to_ll(((MyPacketData*)val)->lambda);
    unsigned long long token = to_ll(((MyPacketData*)val)->P);
    unsigned long long service_time = to_ll(((MyPacketData*)val)->mu);
    unsigned long long num = to_ll(((MyPacketData*)val)->n);
    unsigned long long B = to_ll(((MyPacketData*)val)->B);
    char *FILENAME = NULL;
    FILENAME = ((MyPacketData*)val)->FILENAME;
    
    /*Check if deterministic or trace*/
    if (FILENAME == NULL)
        deter_packet(inter_time, token, service_time, num);
    else
        trace_packet(FILENAME, B);

    return((void*)0);        
}

void deter_packet(unsigned long long inter_time, 
                  unsigned long long token, 
                  unsigned long long service_time,
                  unsigned long long num)
{
    /* DETERMINISTIC */
    unsigned long long count = 1;
    for(count=1; count<=num;count++){
        packet_engine(millisec_llto_microsec(inter_time), 
                      token, 
                      millisec_llto_microsec(service_time),
                      count);
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
    unsigned long long count = 1;

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
        if(line++==0)
            continue;
        packet_engine(millisec_llto_microsec(inter_time), 
                      token, 
                      millisec_llto_microsec(service_time),
                      count++);

    }

}

void packet_engine(unsigned long long inter_time, 
                   unsigned long long token, 
                   unsigned long long service_time,
                   unsigned long long count)
{
    struct timeval etime, current, q1start, q1end, q1duration, end;
    int token_left = 0;
    char MESG[1024];
    /* Calculate the actual sleep time. */
    unsigned long long actual_inter_time = time_to_sleep(1, inter_time);
    /* Calculate arrival time for stats */
    usleep(actual_inter_time);
    gettimeofday(&end, NULL);
    unsigned long long inter_arr_time = toMicroSeconds(diff_timeval(end, PKT_BEFORE_PREV));
    struct timeval inter_timeval = diff_timeval(end, PKT_BEFORE_PREV);
    /* Add to statistics */
    PKT_INTV_ARV_TIME+=inter_arr_time;
    /* Create message. */
    sprintf(MESG, "p%llu arrives, needs %llu , inter-arrival time = ", 
            count, token);

    NUM_PACKETS++;
    print_emulation_time(1, MESG, inter_timeval, 1 );
    /* Create a packet */
    //TODO
    /* Lock the lutex */
    pthread_mutex_lock(&m);
    /* Add in Q1 */
    gettimeofday(&current, NULL);
    q1start = diff_timeval(current, START_TIMEVAL);
    fprintf(stdout, "%08llu.%03ldms: ", toMilliSeconds(q1start), q1start.tv_usec%1000);
    fprintf(stdout, "p%llu enters Q1\n", count);
    //TODO
    //usleep(actual_inter_time);
    /* Packet leaves Q1 */
    gettimeofday(&current, NULL);
    q1end = diff_timeval(current, START_TIMEVAL);
    fprintf(stdout, "%08llu.%03ldms: ", toMilliSeconds(q1end), q1end.tv_usec%1000);
    q1duration = diff_timeval(q1end, q1start);
    TIME_AT_Q1 = add_timeval(TIME_AT_Q1, q1duration);

    fprintf(stdout, "p%llu leaves Q1, time in Q1 = %llu.%03ldms, token bucket has %d token\n", 
            count, toMilliSeconds(q1duration), q1duration.tv_usec%1000, token_left);
    //TODO
    /* Check for threshold tokens in bucket */
    //TODO
    /* Drop the packet if MAX otherwise shift to Q2 */
    //TODO
    gettimeofday(&current, NULL);
    etime = diff_timeval(current, START_TIMEVAL);
    fprintf(stdout, "%08llu.%03ldms: ", toMilliSeconds(etime), etime.tv_usec%1000);
    fprintf(stdout, "p%llu enters Q2\n", count);
    /* Check if Q2 was empty before */
    //TODO
    /* If yes then broadcast signal to server */
    //TODO
    /* Unlock mutex */
    pthread_mutex_unlock(&m);
}

void cleanup(FILE *fp)
{
    /* When the packet thread dies */
    /* Free the resources used by thread_packet */
    if (fp!=NULL){
        fclose(fp);
    }
}
