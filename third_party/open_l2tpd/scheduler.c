/*
 * Layer Two Tunnelling Protocol Daemon
 * Copyright (C) 1998 Adtran, Inc.
 * Copyright (C) 2002 Jeff McAdams
 *
 * Mark Spencer
 *
 * This software is distributed under the terms
 * of the GPL, which you should have received
 * along with this source.
 *
 * Scheduler code for time based functionality
 *
 */

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "l2tp.h"
#include "scheduler.h"

struct schedule_entry *events;
static struct timespec zero;
static sigset_t alarm;


struct timeval convert_timeformat(struct timespec origtime)
{
	struct timeval tmp;
	tmp.tv_sec = origtime.tv_sec;
	tmp.tv_usec = origtime.tv_nsec / 1000;

	return tmp;
}

void init_scheduler (void)
{
    struct sigaction act;
    act.sa_handler = alarm_handler;
#if defined (LINUX) && (__i386__)
    act.sa_restorer = NULL;
#endif
    act.sa_flags = 0;
    sigemptyset (&act.sa_mask);
    sigaddset (&act.sa_mask, SIGALRM);
    sigaction (SIGALRM, &act, NULL);
    events = NULL;
    zero.tv_nsec = 0;
    zero.tv_sec = 0;
    sigemptyset (&alarm);
    sigaddset (&alarm, SIGALRM);
}

void alarm_handler (int signal)
{
    /* Check queue for events which should be
       executed right now.  Execute them, then
       see how long we should set the next timer
     */
    struct schedule_entry *p = events;
    struct timespec now;
    struct timespec then;
    struct itimerval itv;
    static int cnt = 0;
    cnt++;
    if (cnt != 1)
    {
        /* Whoa, we got called from within ourselves! */
        log (LOG_DEBUG, "%s : Whoa... cnt = %d\n", __FUNCTION__, cnt);
        return;
    }
    while (events)
    {
        clock_gettime(CLOCK_MONOTONIC, &now);
        p = events;
        if (TVLESSEQ (p->tv, now))
        {
            events = events->next;
            /* This needs to be executed, as it has expired.
               It is expected that p->func will free p->data
               if it is necessary */
            (*p->func) (p->data);
            free (p);
        }
        else
            break;
    }
    /* When we get here, either there are no more events
       in the queue, or the remaining events need to happen
       in the future, so we should schedule another alarm */
    if (events)
    {
        then.tv_sec = events->tv.tv_sec - now.tv_sec;
        then.tv_nsec = events->tv.tv_nsec - now.tv_nsec;
        if (then.tv_nsec < 0)
        {
            then.tv_sec -= 1;
            then.tv_nsec += 1000000000;
        }
        if ((then.tv_sec <= 0) && (then.tv_nsec <= 0))
        {
            log (LOG_WARN, "%s: Whoa...  Scheduling for <=0 time???\n",
                 __FUNCTION__);
        }
        else
        {
            itv.it_interval = convert_timeformat(zero);
            itv.it_value = convert_timeformat(then);
            setitimer (ITIMER_REAL, &itv, NULL);
        }
    }
    cnt--;
}

void schedule_lock ()
{
    while (sigprocmask (SIG_BLOCK, &alarm, NULL));
};

void schedule_unlock ()
{
    /* See if we missed any events */
/*	alarm_handler(0); */
    while (sigprocmask (SIG_UNBLOCK, &alarm, NULL));
    raise (SIGALRM);
};

struct schedule_entry *schedule (struct timespec tv, void (*func) (void *),
                                 void *data)
{
    /* Schedule func to be run at relative time tv with data
       as arguments.  If it has already expired, run it 
       immediately.  The queue should be in order of
       increasing time */
    struct schedule_entry *p = events, *q = NULL;
    int need_timer = 0;
    struct timespec diff;
    struct itimerval itv;
    diff = tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    tv.tv_sec += diff.tv_sec;
    tv.tv_nsec += diff.tv_nsec;
    if (tv.tv_nsec > 1000000000)
    {
        tv.tv_sec++;
        tv.tv_nsec -= 1000000000;
    }
    while (p)
    {
        if (TVLESS (tv, p->tv))
            break;
        q = p;
        p = p->next;
    };
    if (q)
    {
        q->next =
            (struct schedule_entry *) malloc (sizeof (struct schedule_entry));
        q = q->next;
    }
    else
    {
        q = (struct schedule_entry *) malloc (sizeof (struct schedule_entry));
        events = q;
        need_timer = -1;
    }
    q->tv = tv;
    q->func = func;
    q->data = data;
    q->next = p;
    if (need_timer)
    {
        itv.it_interval = convert_timeformat(zero);
        itv.it_value = convert_timeformat(diff);
        setitimer (ITIMER_REAL, &itv, NULL);

    }
    return q;

}

inline struct schedule_entry *aschedule (struct timespec tv,
                                         void (*func) (void *), void *data)
{
    /* Schedule func to be run at absolute time tv in the future with data
       as arguments */
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    tv.tv_nsec -= now.tv_nsec;
    if (tv.tv_nsec < 0)
    {
        tv.tv_nsec += 1000000000;
        tv.tv_sec--;
    }
    tv.tv_sec -= now.tv_sec;
    return schedule (tv, func, data);
}

void deschedule (struct schedule_entry *s)
{
    struct schedule_entry *p = events, *q = NULL;
    if (!s)
        return;
    while (p)
    {
        if (p == s)
        {
            if (q)
            {
                q->next = p->next;
            }
            else
            {
                events = events->next;
            }
            free (p);
            break;
        }
        q = p;
        p = p->next;
    }
}
