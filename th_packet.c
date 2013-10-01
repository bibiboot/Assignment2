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
    ACT.sa_handler = interrupt;
    sigaction(SIGINT, &ACT, NULL);
    pthread_sigmask(SIG_UNBLOCK, &NEW, NULL);

    /* Handler for thread */
    /* Decision to go to deter or trace */
    NUM_PACKETS = 0;
    //unsigned long long inter_time = MILLI/to_ll(((MyPacketData*)val)->lambda);
    unsigned long long inter_time = MILLI/atof(((MyPacketData*)val)->lambda);
    if(inter_time > (10*MILLI))
        inter_time = 10*MILLI;
    
    unsigned long long token = to_ll(((MyPacketData*)val)->P);
    //unsigned long long service_time = to_ll(((MyPacketData*)val)->mu);
    unsigned long long service_time = MILLI/atof(((MyPacketData*)val)->mu);
    if(service_time >= (10*MILLI))
        service_time = 10*MILLI;

    unsigned long long num = to_ll(((MyPacketData*)val)->n);
    unsigned long long B = to_ll(((MyPacketData*)val)->B);
    char *FILENAME = NULL;
    FILENAME = ((MyPacketData*)val)->FILENAME;
    
    /*Check if deterministic or trace*/
    if (FILENAME == NULL)
        deter_packet(inter_time, token, service_time, num);
    else
        trace_packet(FILENAME, B);

    /* Kill Token Thread and Server thread */
    pthread_mutex_lock(&m);
    PACKET_DEAD = 1;
    pthread_mutex_unlock(&m);

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
            //fp.fclose();
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
    int broadcast = 0;
    My402dataElem *topEle;
    struct timeval etime, current, q1start, q1end, q1duration;

    /* Calculate the actual sleep time. */
    unsigned long long actual_inter_time = time_to_sleep(inter_time, PKT_BEFORE);

    /* Sleep */
    usleep(actual_inter_time);

    /* Get Current time */
    gettimeofday(&PKT_BEFORE, NULL);
    etime = diff_timeval(PKT_BEFORE, START_TIMEVAL);

    /* Calculate arrival time for stats */
    struct timeval inter_timeval = diff_timeval(PKT_BEFORE, PKT_BEFORE_PREV);
    //unsigned long long inter_arr_time = toMicroSeconds(inter_timeval);

    /* Add to statistics */
    PKT_INTV_ARV_TIME = add_timeval(PKT_INTV_ARV_TIME, inter_timeval);
    memcpy(&PKT_BEFORE_PREV, &PKT_BEFORE, sizeof(PKT_BEFORE));

    /* Check if packet has to be dropped or not */
    if(token > MAX_TOKEN){
        /* PRINT */
        fprintf(stdout, "%08llu.%03ldms: packet p%llu arrives, needs %llu tokens, dropped\n", 
                toMilliSeconds(etime), etime.tv_usec%MILLI, count, token);
        DROPPED_PKT++;
    }
    else{
        /* Print */
        fprintf(stdout,"%08llu.%03ldms: p%llu arrives, needs %llu , inter-arrival time = %llu.%03ldms\n", 
            toMilliSeconds(etime), etime.tv_usec%MILLI, count, token, 
            toMilliSeconds(inter_timeval), inter_timeval.tv_usec%MILLI);
   }

    NUM_PACKETS++;

    /* Create a packet */
    My402dataElem *dataE = create(count, token, service_time);

    /* Lock the lutex */
    pthread_mutex_lock(&m);

    /* Start time */
    gettimeofday(&current, NULL);
    q1start = diff_timeval(current, START_TIMEVAL);
    dataE->q1start = q1start;

    /* Add in Q1 */
    My402ListAppend(Q1, (void*)dataE);

    /* PRINT */
    fprintf(stdout, "%08llu.%03ldms: p%llu enters Q1\n", 
            toMilliSeconds(q1start), q1start.tv_usec%1000, count);

    /* Get top element */
    My402ListElem *elem = My402ListFirst(Q1);
    topEle = (My402dataElem*)(elem->obj);

    /* Check for threshold tokens in bucket */
    if (topEle->token<= AVAILABLE){

       /* Check if Q2 is empty */
       if(My402ListEmpty(Q2) == 0){
          /* Broadcast server once the process is done */
          broadcast = 1;
       }

       /* Packet leaves Q1 */
       /* Remove top element */
       My402ListUnlink(Q1, elem);

       /* Add to statistics */
       gettimeofday(&current, NULL);
       q1end = diff_timeval(current, START_TIMEVAL);
       q1duration = diff_timeval(q1end, topEle->q1start);
       TIME_AT_Q1 = add_timeval(TIME_AT_Q1, q1duration);

       /* Reduce token */
       AVAILABLE-=topEle->token;

       /* PRINT */
       fprintf(stdout, 
              "%08llu.%03ldms: p%llu leaves Q1, time in Q1 = %llu.%03ldms, token bucket has %llu token\n", 
              toMilliSeconds(q1end), q1end.tv_usec%1000, topEle->count,
              toMilliSeconds(q1duration), q1duration.tv_usec%MILLI, AVAILABLE);

       /* Move from Q1 to Q2 */
       /* Add to statistics */
       gettimeofday(&current, NULL);
       etime = diff_timeval(current, START_TIMEVAL);

       /* Set start time in Q2 */
       dataE->q2start = etime;
       dataE->q1duration = q1duration;

       /* Add to Q2 */
       My402ListAppend(Q2, (void*)dataE);

       /* PRINT */
       fprintf(stdout, "%08llu.%03ldms: p%llu enters Q2\n", 
              toMilliSeconds(etime), q1start.tv_usec%MILLI, topEle->count);

       if(broadcast == 1){
           /* If yes then broadcast signal to server */
           pthread_cond_broadcast(&cond_t);
       }
    }
        
    /* Unlock mutex */
    pthread_mutex_unlock(&m);
}

void interrupt()
{
    /*Clean up resources of packet thread*/
    printf("--------------Ctrl+C Recieved----------------\n");   
    /*Start alarm to server and token thread*/
    SERVER_DIE = 1;
    pthread_cond_broadcast(&cond_t);
    pthread_kill(TOKEN, SIGUSR1);

    while(!My402ListEmpty(Q2)){
        My402ListElem *elem = My402ListFirst(Q2);
        My402dataElem *topEle = (My402dataElem*)(elem->obj);
        My402ListUnlink(Q2, elem);

        TIME_AT_Q1 = diff_timeval(TIME_AT_Q1, topEle->q1duration);
    }
    
    /* Clear the list */
    My402ListUnlinkAll(Q1);

    printf("--------------Packet thread is terminating----------------\n");   
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
