#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define MICRO 1000000
#define MILLI 1000
#define ILL "????????.???"

struct timeval TKN_BEFORE, TKN_AFTER, TKN_BEFORE_PREV;
struct timeval PKT_BEFORE, PKT_AFTER, PKT_BEFORE_PREV;

unsigned long long PKT_INTV_ARV_TIME /*TIME_AT_Q1;*/;
struct timeval TIME_AT_Q1;
int NUM_PACKETS ;
struct timeval pkt_service_time;
unsigned long long numQ1;
unsigned long long numQ2;
unsigned long long numS;
struct timeval pkt_system_spent_time;

pthread_mutex_t m;
struct timeval START_TIMEVAL;

typedef struct packet_data {
    char *lambda;
    char *mu ;
    char *r ;
    char *B ;
    char *P ;
    char *n ;
    char *FILENAME ;
} MyPacketData;


unsigned long long to_ll(char *temp);

unsigned long long sec_to_microsec(char* sec);

unsigned long long millisec_to_microsec(char* millisec);

unsigned long long millisec_llto_microsec(unsigned long long millisec);

unsigned long long toMicroSeconds(struct timeval a);

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

struct timeval print_emulation_time(int i, char *mesg, 
                                    struct timeval inter_timeval,
                                    int allow_timeval);

unsigned long long time_to_sleep(int thread_type, 
                                 unsigned long long inter_time);

struct timeval copy_time_val(struct timeval source, struct timeval dest);

unsigned long long toMilliSeconds(struct timeval a);

