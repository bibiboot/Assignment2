#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "my402list.h"

#define MICRO 1000000
#define MILLI 1000
#define ILL "????????.???"

unsigned long long AVAILABLE, MAX_TOKEN ;

struct timeval TKN_BEFORE, TKN_BEFORE_PREV;
struct timeval PKT_BEFORE, PKT_BEFORE_PREV;

My402List *Q1, *Q2;

pthread_t PACKET, SERVER, TOKEN;
 
sigset_t NEW;
struct sigaction ACT, ACT_TKN;

struct timeval TIME_AT_Q1, TIME_AT_Q2, TIME_AT_S, SPENT, SERVICE_TIME, PKT_INTV_ARV_TIME;
double SQ_SERVICE_TIME;
int NUM_PACKETS ;
int SERVER_DIE ;

int DROPPED, DROPPED_PKT, TOTAL, PACKET_DEAD, TOKEN_DEAD;

struct timeval pkt_service_time;
struct timeval pkt_system_spent_time;

pthread_mutex_t m;
struct timeval START_TIMEVAL;

pthread_cond_t cond_t;

typedef struct packet_data {
    char *lambda;
    char *mu ;
    char *r ;
    char *B ;
    char *P ;
    char *n ;
    char *FILENAME ;
} MyPacketData;

typedef struct dataElem {
    unsigned long long count;
    struct timeval q1start;
    struct timeval q2start;
    struct timeval q1duration;
    unsigned long long token;
    unsigned long long service_time;
} My402dataElem;

unsigned long long to_ll(char *temp);

unsigned long long sec_to_microsec(char* sec);

unsigned long long millisec_to_microsec(char* millisec);

unsigned long long millisec_llto_microsec(unsigned long long millisec);

unsigned long long toMicroSeconds(struct timeval a);

unsigned long long toMilliSeconds(struct timeval a);

struct timeval micro_to_timeval(unsigned long long usec);

int check_format(int result,
                 int line,
                 unsigned long long inter_time,
                 unsigned long long token,
                 unsigned long long service_time);

struct timeval add_timeval(struct timeval a,
                           struct timeval b);

struct timeval diff_timeval(struct timeval a,
                           struct timeval b);

unsigned long long time_to_sleep(unsigned long long inter_time,
                           struct timeval b);

struct timeval copy_time_val(struct timeval source, struct timeval dest);

double toSeconds(struct timeval a);

double square(double a);

