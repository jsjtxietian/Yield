#include "uthread.h"

void uthread_yield(schedule_t &schedule)
{
    if (schedule.running_thread != -1)
    {
        uthread_t *t = &(schedule.threads[schedule.running_thread]);
        t->state = SUSPEND;
        schedule.running_thread = -1;

        swapcontext(&(t->ctx), &(schedule.main));
    }
}

void uthread_resume(schedule_t &schedule, int id)
{
    if (id < 0 || id > schedule.threads.size() - 1)
    {
        return;
    }

    auto t = &(schedule.threads[id]);

    if (t->state == SUSPEND)
    {
        swapcontext(&(schedule.main), &(t->ctx));
    }
}

void uthread_body(schedule_t *ps)
{
    int id = ps->running_thread;

    if (id != -1)
    {
        auto t = &(ps->threads[id]);
        t->func(t->arg);
        t->state = FREE;
        ps->running_thread = -1;
    }
}

int schedule_finished(const schedule_t &schedule)
{
    if (schedule.running_thread != -1)
    {
        return 0;
    }
    else
    {
        for (int i = 0; i < schedule.threads.size(); i++)
        {
            if(schedule.threads[i].state != FREE)
            {
                return 0;
            }
        }
    }

    return 1;
}

int uthread_create(schedule_t &schedule, Fun func, void *arg)
{
    int id = schedule.threads.size();

    uthread_t temp;
    schedule.threads.push_back(temp);
    uthread_t *t = &(schedule.threads[id]);

    t->state = RUNNABLE;
    t->func = func;
    t->arg = arg;

    getcontext(&(t->ctx));

    t->ctx.uc_stack.ss_sp = t->stack;
    t->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
    t->ctx.uc_stack.ss_flags = 0;
    t->ctx.uc_link = &(schedule.main);
    schedule.running_thread = id;

    makecontext(&(t->ctx), (void (*)(void))(uthread_body), 1, &schedule);
    swapcontext(&(schedule.main), &(t->ctx));

    return id;
}
