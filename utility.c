#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
                 char * inter_time,
                 char *token, 
                 char *service_time, char *num)
{
    /* Check that first line is one integer and others are three */
    if( !((result == 3 && line!=0) || (result == 1 && line == 0)))
       return -1;
    if (line ==0)
        return 0;
    if(isNum(num) == -1)
        return -1;
    if(isNum(inter_time) == -1)
        return -1;
    if(isNum(token) == -1)
        return -1;
    if(isNum(service_time)==-1)
        return -1;
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
        //printf("%ld\n", a.tv_sec);
        //printf("%ld\n", b.tv_sec);
        //printf("Long value\n");
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
   //printf("Real = %llu\n", toMicroSeconds(inter_time_val));
   //printf("Wasted = %llu\n", toMicroSeconds(wasted));
   actual_inter_timeval = diff_timeval(inter_time_val, wasted);
   //printf("Seleep = %llu\n", toMicroSeconds(actual_inter_timeval));

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

unsigned long long usquare(unsigned long long a)
{
    return a*a;
}

void block_SIGINT()
{
    sigemptyset(&NEW);
    sigaddset(&NEW, SIGINT);

    pthread_sigmask(SIG_BLOCK, &NEW, NULL);
}

void print_stats()
{
    struct timeval end;
    gettimeofday(&end, NULL);
    /* Find the emulation of the program */
    struct timeval emulation_time = diff_timeval(end, START_TIMEVAL);

    fprintf(stdout, "\nStatistics:\n\n");
    if (TOTAL_PACKETS == 0)
        fprintf(stdout, 
               "%4saverage packet inter-arrival time = NA ( No packet arrived at this facility )\n", 
               "");
    else
        fprintf(stdout, "%4saverage packet inter-arrival time = %.6gsec\n", 
            "", (toSeconds(PKT_INTV_ARV_TIME)/TOTAL_PACKETS));

    if (TOTAL_SERVED == 0)
        fprintf(stdout, 
               "%4saverage packet service time = NA ( No packet is still serviced )\n", 
               "");
    else
        fprintf(stdout, "%4saverage packet service time = %.6gsec\n\n", "", 
                toSeconds(SERVICE_TIME)/TOTAL_SERVED);

    fprintf(stdout, "%4saverage number of packets in Q1 = %.6g\n", "",
            ((double)(toMicroSeconds(TIME_AT_Q1)))/(toMicroSeconds(emulation_time))); 
    fprintf(stdout, "%4saverage number of packets in Q2 = %.6g\n", "",
            ((double)(toMicroSeconds(TIME_AT_Q2)))/(toMicroSeconds(emulation_time))); 
    fprintf(stdout, "%4saverage number of packets at S = %.6g\n", "",
            ((double)(toMicroSeconds(TIME_AT_S)))/(toMicroSeconds(emulation_time))); 

    if (TOTAL_SERVED == 0)
        fprintf(stdout, 
               "%4saverage time a packet spent in system = NA ( No packet rechead the server )\n", 
               "");
    else
        fprintf(stdout, "%4saverage time a packet spent in system = %.6gsec\n", "",
            toSeconds(SPENT)/TOTAL_SERVED); 
    if (TOTAL_SERVED == 0 || SQ_SPENT_TIME == 0)
        fprintf(stdout, 
               "%4sstandard deviation for time spent in system = NA ( No packet reached the server )\n", 
               "");
    else{
        fprintf(stdout, "%4sstandard deviation for time spent in system = %.6gsec\n\n", 
            "", sqrt(((double)SQ_SPENT_TIME/TOTAL_SERVED) - square((double)toSeconds(SPENT)/TOTAL_SERVED)));
    }

    if(TOTAL == 0)
        fprintf(stdout, "%4stoken drop probability = NA ( No tokens were produced )\n", ""); 
    else
        fprintf(stdout, "%4stoken drop probability = %.6g\n", "", (double)DROPPED/TOTAL); 

    if(TOTAL_PACKETS == 0)
        fprintf(stdout, "%4spacket drop probability = NA ( No packets were produced )\n", "" ); 
    else 
        fprintf(stdout, "%4spacket drop probability = %.6g\n", "", (double)DROPPED_PKT/TOTAL_PACKETS); 
}

int check_num(char *amt)
{
    char int_part[10];
    char frac_part[10];
    sscanf(amt, "%[^.].%s", int_part, frac_part);
    if(isNum(int_part)){
        return -1;
    }
    if(isNum(frac_part)){
        return -1;
    }
    return 0;
}


int isNum(char *str)
{
    char *temp = str;
    while(*temp!='\0'){
        if((*temp)>='0' && (*temp)<='9' )
            temp++;
        else
            return -1;
    }
    return 0;
}
