#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "th_packet.h"
#include "utility.h"

void interrupt();
My402dataElem* create(int count, unsigned long long token,
                      unsigned long long service_time);

void* packet_init(void *val)
{
    /* Unblock Signal */

    ACT_PKT.sa_handler = interrupt;
    sigaction(SIGUSR2, &ACT_PKT, NULL);

    /* Handler for thread */
    /* Decision to go to deter or trace */
    TOTAL_PACKETS = 0;
    //unsigned long long inter_time = MILLI/to_ll(((MyPacketData*)val)->lambda);
    //unsigned long long inter_time_milli = MILLI/atof(((MyPacketData*)val)->lambda);
    unsigned long long inter_time_micro = MICRO/atof(((MyPacketData*)val)->lambda);
    if (inter_time_micro == 0)
        inter_time_micro = 1;
    //printf("%llu Value = \n", inter_time);
    if(inter_time_micro > (10*MICRO))
        inter_time_micro = 10*MICRO;
    
    unsigned long long token = to_ll(((MyPacketData*)val)->P);
    //unsigned long long service_time = to_ll(((MyPacketData*)val)->mu);
    //unsigned long long service_time_milli = MILLI/atof(((MyPacketData*)val)->mu);
    unsigned long long service_time_micro = MICRO/atof(((MyPacketData*)val)->mu);
    if (service_time_micro == 0)
        service_time_micro = 1;
    if(service_time_micro > (10*MICRO))
        service_time_micro = 10*MICRO;

    unsigned long long num = to_ll(((MyPacketData*)val)->n);
    unsigned long long B = to_ll(((MyPacketData*)val)->B);
    char *FILENAME = NULL;
    FILENAME = ((MyPacketData*)val)->FILENAME;
    
    /*Check if deterministic or trace*/
    if (FILENAME == NULL)
        deter_packet(inter_time_micro, token, service_time_micro, num);
    else
        trace_packet(FILENAME, B);

    SERVER_DIE = 1;

    pthread_exit(0);

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
        packet_engine(inter_time, 
                      token, 
                      service_time,
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
    unsigned long long inter_time, token, service_time;
    char inter_time_s[20], token_s[20], service_time_s[20], num[20];
    unsigned long long count = 1;

    fp = fopen(FILENAME, "r");
    while(fgets(buffer, 1000, fp)!=NULL){
        /* First line is the number of packets */
        if(line==0)
            result = sscanf(buffer, "%s", num);
        else{
            result = sscanf(buffer, "%[^ ] %[^ ] %s", inter_time_s, token_s, service_time_s);
            //printf("%d-%s-%s-%s\n", result, inter_time_s, token_s, service_time_s);
        }

        //printf("%d--%d\n", result, line);
        /* Check for file format */
        if (check_format(result, line, inter_time_s, token_s, service_time_s,num) == -1){
            fprintf(stderr, "Error in tsfile format\n");
            exit(0);
        }
        inter_time = atoll(inter_time_s);
        token = atoll(token_s);
        service_time = atoll(service_time_s);

        /* Call Engine */
        if(line++==0)
            continue;
        packet_engine(millisec_llto_microsec(inter_time), 
                      token, 
                      millisec_llto_microsec(service_time),
                      count++);

    }
    fclose(fp);
   
}


void print_packet_arrived(unsigned long long token)
{
    gettimeofday(&PKT_BEFORE, NULL);

    /* Calculate arrival time for stats */
    struct timeval inter_timeval = diff_timeval(PKT_BEFORE, PKT_BEFORE_PREV);

    struct timeval etime = diff_timeval(PKT_BEFORE, START_TIMEVAL);
    memcpy(&PKT_BEFORE_PREV, &PKT_BEFORE, sizeof(PKT_BEFORE));
    PKT_INTV_ARV_TIME = add_timeval(PKT_INTV_ARV_TIME, inter_timeval);

    /* Print */
    fprintf(stdout,"%08llu.%03ldms: p%llu arrives, needs %llu , inter-arrival time = %llu.%03ldms\n", 
            toMilliSeconds(etime), etime.tv_usec%MILLI, TOTAL_PACKETS, token, 
            toMilliSeconds(inter_timeval), inter_timeval.tv_usec%MILLI);
    
}

void print_packet_dropped(unsigned long long token)
{
    gettimeofday(&PKT_BEFORE, NULL);

    /* Calculate arrival time for stats */
    struct timeval inter_timeval = diff_timeval(PKT_BEFORE, PKT_BEFORE_PREV);

    struct timeval etime = diff_timeval(PKT_BEFORE, START_TIMEVAL);
    memcpy(&PKT_BEFORE_PREV, &PKT_BEFORE, sizeof(PKT_BEFORE));

    PKT_INTV_ARV_TIME = add_timeval(PKT_INTV_ARV_TIME, inter_timeval);

    fprintf(stdout, "%08llu.%03ldms: packet p%llu arrives, needs %llu tokens, dropped\n", 
            toMilliSeconds(etime), etime.tv_usec%MILLI, TOTAL_PACKETS, token);
}

void pendQ1_stats(struct timeval q1start,
                 unsigned long long pack_num)
{
    /* Add statistics */
    struct timeval current;
    gettimeofday(&current, NULL);
    struct timeval q1end = diff_timeval(current, START_TIMEVAL);
    struct timeval q1duration = diff_timeval(q1end, q1start);
    TIME_AT_Q1 = add_timeval(TIME_AT_Q1, q1duration);

    fprintf(stdout,
            "%08llu.%03ldms: p%llu leaves Q1, time in Q1 = %llu.%03ldms, token bucket has %llu token\n",
            toMilliSeconds(q1end), q1end.tv_usec%1000, pack_num,
            toMilliSeconds(q1duration), q1duration.tv_usec%MILLI, AVAILABLE);
}


void packet_engine(unsigned long long inter_time, 
                   unsigned long long token, 
                   unsigned long long service_time,
                   unsigned long long count)
{
        My402ListElem *elem;
        My402dataElem *topEle;
        struct timeval etime, current;
        int broadcast = 0;

        /* Calculate the actual sleep time. */
        unsigned long long actual_inter_time = time_to_sleep(inter_time, PKT_BEFORE);
        /* Sleep */
        usleep(actual_inter_time);

        /* Create a packet */
        My402dataElem *dataE = create(count, token, service_time);

        /*LOCK*/
        pthread_mutex_lock(&m);

        /* Start time */
        gettimeofday(&current, NULL);
        struct timeval q1start = diff_timeval(current, START_TIMEVAL);
        dataE->q1start = q1start;
          
        TOTAL_PACKETS++;

        if(token < MAX_TOKEN){
            print_packet_arrived(token);
        } else {
            DROPPED_PKT++;
            print_packet_dropped(token);
        }

        /* Add in Q1 */
        My402ListAppend(Q1, (void*)dataE);

        /* Check if move from Q1 to Q2 is allowed */
        /* Get top of the Q1 */
        elem = My402ListFirst(Q1);
        topEle = (My402dataElem*)(elem->obj);

        /* Check if their is enough token */
        if (topEle->token <= AVAILABLE){
            /* Remove it from Q1 */
            My402ListUnlink(Q1, elem);

            /* Decrement token */
            AVAILABLE-=topEle->token;

            /* Add stats */
            pendQ1_stats(topEle->q1start, topEle->count);


            /* Check if Q2 is empty */
            if(My402ListEmpty(Q2))
                broadcast = 1;

            gettimeofday(&current, NULL);
            etime = diff_timeval(current, START_TIMEVAL);

            /* Set start time in Q2 */
            topEle->q2start = etime;

            /* Add to Q2 */
            My402ListAppend(Q2, (void*)topEle);

            /* PRINT */
            fprintf(stdout, "%08llu.%03ldms: p%llu enters Q2\n",
                    toMilliSeconds(etime), etime.tv_usec%MILLI, topEle->count);

           if(broadcast == 1)
                pthread_cond_broadcast(&cond_t);
        }
        pthread_mutex_unlock(&m);
}

void interrupt()
{
    //printf("PACKET HANDLER\n");   
    /* Denote server to die*/
    pthread_mutex_unlock(&m);
    //printf("--------------Packet thread is terminating----------------\n");   
    pthread_exit(0);
}

My402dataElem* create(int count, unsigned long long token, 
                      unsigned long long service_time)
{
       /* Create memory for the structure*/
       My402dataElem *dataE = malloc(sizeof(My402dataElem));
       dataE->count = count;
       dataE->token = token;
       dataE->service_time = service_time;
       return dataE;
}
