#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

unsigned long long to_ll(char *temp)
{
    /* Convert char pointer to long long */
    unsigned long long value = atoll(temp);
    return value;
}

unsigned long long sec_to_microsec(char* sec)
{
    /* Convert sec to microsec */
    unsigned long long micro = to_ll(sec)*MICRO;
    return micro;
}

unsigned long long millisec_to_microsec(char* millisec)
{
    /* Convert millisec to microsec */
    unsigned long long micro = to_ll(millisec)*MILLI;
    return micro;
}

unsigned long long millisec_llto_microsec(unsigned long long millisec)
{
    /* Convert millisec to microsec */
    unsigned long long micro = millisec*MILLI;
    return micro;
}

int check_format(int result,
                 int line,
                 unsigned long long inter_time,
                 unsigned long long token, 
                 unsigned long long service_time)
{
    /* Check that first line is one integer and others are three */
    if( !((result == 3 && line!=0) || (result == 1 && line == 0)))
        return -1;
    /* Check if the inter_time, token and service_time are integers */

    return 0;
}

struct timeval add_timeval(struct timeval a,
                           struct timeval b)
{
    /* Addition of two time vals */
    a.tv_sec = a.tv_sec + b.tv_sec;
    a.tv_usec = a.tv_usec + b.tv_usec;
    a.tv_sec = a.tv_sec + a.tv_usec/MICRO;
    a.tv_usec = a.tv_usec%MICRO;
    return a;
}
    
struct timeval diff_timeval(struct timeval a,
                           struct timeval b)
{
    /* Subtraction of two time vals */
    /* a is greater then b */
    if((a.tv_sec*MICRO+a.tv_usec) < (b.tv_sec*MICRO+b.tv_usec)){
        printf("Long value\n");
        a.tv_sec = 0;
        a.tv_usec = 0;
        return a;
    }
    unsigned long long  interval = (a.tv_sec*MICRO+a.tv_usec) - (b.tv_sec*MICRO+b.tv_usec);
    a.tv_sec = interval/MICRO;
    a.tv_usec = interval%MICRO;
    return a;
}

unsigned long long toMilliSeconds(struct timeval a)
{
    return a.tv_sec*MILLI + (a.tv_usec/MILLI);
}

unsigned long long toMicroSeconds(struct timeval a)
{
    return (a.tv_sec*MICRO) + a.tv_usec;
}

struct timeval micro_to_timeval(unsigned long long usec)
{
    struct timeval *a = malloc(sizeof(struct timeval));
    a->tv_sec = usec/MICRO;
    a->tv_usec = usec%MICRO;
    return *a;
}

struct timeval print_emulation_time(char *mesg)
{
    //struct timeval current;
    //gettimeofday(&current, NULL);
    //printf("Seconds = %llu\t%llu\n", PKT_BEFORE.tv_sec, PKT_BEFORE.tv_usec);
    //printf("PKT_BEFORE = %llu\n", toMicroSeconds(PKT_BEFORE));
    struct timeval etime = diff_timeval(PKT_BEFORE, START_TIMEVAL);
    fprintf(stdout, "%08llu.%03ldms: ", toMilliSeconds(etime), etime.tv_usec%1000); 
    fprintf(stdout,"%s\n", mesg);
    return etime;
}



unsigned long long time_to_sleep(int thread_type, unsigned long long inter_time)
{
   /* If thread_type == 0: Packet thread */
   /* If thread_type == 1: Server thread */
   /* If thread_type == 0: Token thread */
   struct timeval temp;
   /* Packet thread */
   /* Convert interval time in micro to time val */
   struct timeval inter_time_val = micro_to_timeval(inter_time);

   /* Copy current to PACKET AFTER */
   //PKT_AFTER = copy_time_val(temp, PKT_AFTER);
   gettimeofday(&PKT_AFTER, NULL);

   /* Calculate the time wasted */
   struct timeval wasted = diff_timeval(PKT_AFTER, PKT_BEFORE);
   //printf("BEFORE = %llu and AFTER=%llu\n", toMicroSeconds(PKT_BEFORE), toMicroSeconds(PKT_AFTER));
   //printf("wasted = %llu\n", toMicroSeconds(wasted));
   /* Calculate the actual inter time in timeval terms. */
   struct timeval actual_inter_timeval = diff_timeval(inter_time_val, wasted);

   /* Get the next PCKT BEFORE */
   /* Get current time */
   gettimeofday(&temp, NULL);
   /* Get to get the correct before time */
   /* Irrespective of when the thread will be awake. */ 
   PKT_BEFORE = add_timeval(temp, actual_inter_timeval);
   /* Convert the actual time val to microseconds and return */
   return toMicroSeconds(actual_inter_timeval); 
}

struct timeval copy_time_val(struct timeval source, struct timeval dest)
{
    dest.tv_sec = source.tv_sec;
    dest.tv_usec = dest.tv_usec;
    return dest;
}
