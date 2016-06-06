//
//  semaphore.h
//  checkmate
//
//  Created by zhao shuan on 12-3-19.
//  Copyright (c) 2012年 hoolai. All rights reserved.
//

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <mach/semaphore.h>
#include <mach/task.h>
#include <mach/mach.h>

typedef struct sem_t {
    semaphore_t msem;
    int value;
}  sem_t;

inline int sem_init(sem_t *sem, int pshared, int initialValue)
{
    int result;
    result = semaphore_create(mach_task_self(), &(sem->msem), SYNC_POLICY_FIFO, initialValue);
    sem->value = initialValue;
    return result;
}

inline int sem_destroy(sem_t *sem)
{
    int result;
    result = semaphore_destroy(mach_task_self(), sem->msem);
    sem->value = -1;
    return result;
}

inline int sem_wait(sem_t *sem)
{
    int result;
    result = semaphore_wait(sem->msem);
    if(result == 0)
        --(sem->value);
    return result;
}

inline int sem_trywait(sem_t *sem)
{
    int result;
    mach_timespec_t wait_time = {0, 0};
    result = semaphore_timedwait(sem->msem, wait_time);
    if(result == 0)
        --(sem->value);   
    return result;
}

inline int sem_post(sem_t *sem)
{
    int result;
    result = semaphore_signal(sem->msem);
    if(result == 0)
        ++(sem->value);
	return result;
}

inline int sem_getvalue(sem_t *sem, int *sval)
{
    if(sem == NULL)
        return -1;
    return *sval = sem->value;
}

#endif
