#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        printf("%ld\n", a.tv_sec);
        printf("%ld\n", b.tv_sec);
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

double toSeconds(struct timeval a)
{
    return (a.tv_sec) + (double)a.tv_usec/MICRO;
}

struct timeval micro_to_timeval(unsigned long long usec)
{
    struct timeval *a = malloc(sizeof(struct timeval));
    a->tv_sec = usec/MICRO;
    a->tv_usec = usec%MICRO;
    return *a;
}

unsigned long long time_to_sleep(unsigned long long inter_time,
                                struct timeval before)
{
   struct timeval current, wasted, actual_inter_timeval;
   /* Convert interval time in micro to time val */
   struct timeval inter_time_val = micro_to_timeval(inter_time);

   gettimeofday(&current, NULL);
   wasted = diff_timeval(current, before);
   
   /* Calculate the actual inter time in timeval terms. */
   actual_inter_timeval = diff_timeval(inter_time_val, wasted);

   /* Convert the actual time val to microseconds and return */
   return toMicroSeconds(actual_inter_timeval); 
}

struct timeval copy_time_val(struct timeval source, struct timeval dest)
{
    dest.tv_sec = source.tv_sec;
    dest.tv_usec = dest.tv_usec;
    return dest;
}

double square(double a)
{
    return a*a;
}


