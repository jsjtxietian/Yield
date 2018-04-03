#include <ucontext.h>
#include <vector>

#define DEFAULT_STACK_SZIE (1024*128)
#define MAX_UTHREAD_SIZE   1024

enum ThreadState{FREE,RUNNABLE,RUNNING,SUSPEND};

typedef void (*Fun)(void *arg);

typedef struct uthread_t {
    ucontext_t ctx; //协程上下文

    Fun func;   
    void *arg;

    enum ThreadState state;
    char stack[DEFAULT_STACK_SZIE];
} uthread_t;


typedef std::vector<uthread_t> Thread_vector;

typedef struct schedule_t{
    ucontext_t main;
    int running_thread;
    Thread_vector threads;

    schedule_t()
        :running_thread(-1)
    {}

} schedule_t;

int uthread_create(schedule_t &schedule , Fun func , void * arg);
void uthread_yield(schedule_t &schedule);
void uthread_resume(schedule_t &schedule , int id);
int schedule_finished(const schedule_t &schedule);
void uthread_body(schedule_t *ps);




