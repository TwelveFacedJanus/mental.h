#ifndef ASYNCHRONOUS_H
#define ASYNCHRONOUS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <uv.h>
#include <assert.h>

typedef struct {
    void (*func)(void*, void*, int*, uv_timer_t*);
    void* arg;
    bool is_completed;
    int state;
    uv_timer_t timer;
    uint64_t delay_ms;
} async_task;

typedef struct {
    async_task* tasks;
    size_t count;
    size_t capacity;
    uv_loop_t loop;
} async_scheduler;

typedef void (*async_task_func)(void*, void*, int*, uv_timer_t*);

static inline void close_cb(uv_handle_t* handle) {
    // No need to free handle; it's embedded in task
}

static inline async_scheduler* async_create_scheduler(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 4;
    
    size_t alloc_size = sizeof(async_scheduler) + initial_capacity * sizeof(async_task);
    async_scheduler* sched = malloc(alloc_size);
    if (!sched) return NULL;
    
    sched->tasks = (async_task*)((char*)sched + sizeof(async_scheduler));
    sched->count = 0;
    sched->capacity = initial_capacity;
    
    if (uv_loop_init(&sched->loop) != 0) {
        free(sched);
        return NULL;
    }
    
    return sched;
}

static inline void async_destroy_scheduler(async_scheduler* sched) {
    if (!sched) return;
    
    for (size_t i = 0; i < sched->count; i++) {
        if (!sched->tasks[i].is_completed) {
            uv_timer_stop(&sched->tasks[i].timer);
            uv_close((uv_handle_t*)&sched->tasks[i].timer, close_cb);
        }
    }
    
    uv_loop_close(&sched->loop);
    free(sched);
}

static inline bool async_add_task(async_scheduler* sched, async_task_func func, void* arg, uint64_t delay_ms) {
    if (!sched || !func) return false;
    
    if (sched->count >= sched->capacity) {
        size_t new_capacity = sched->capacity ? sched->capacity * 2 : 4;
        size_t new_alloc_size = sizeof(async_scheduler) + new_capacity * sizeof(async_task);
        async_scheduler* new_sched = realloc(sched, new_alloc_size);
        if (!new_sched) return false;
        
        new_sched->tasks = (async_task*)((char*)new_sched + sizeof(async_scheduler));
        sched = new_sched;
        sched->capacity = new_capacity;
    }
    
    async_task* task = &sched->tasks[sched->count++];
    task->func = func;
    task->arg = arg;
    task->is_completed = false;
    task->state = 0;
    task->delay_ms = delay_ms;
    uv_timer_init(&sched->loop, &task->timer);
    task->timer.data = task;
    
    return true;
}

static inline void task_cb(uv_timer_t* handle) {
    async_task* task = (async_task*)handle->data;
    if (!task->is_completed) {
        task->func(task->arg, NULL, &task->state, &task->timer);
        if (task->is_completed) {
            uv_timer_stop(&task->timer);
            uv_close((uv_handle_t*)&task->timer, close_cb);
        }
    }
}

static inline void async_run(async_scheduler* sched) {
    if (!sched || sched->count == 0) return;
    
    for (size_t i = 0; i < sched->count; i++) {
        async_task* task = &sched->tasks[i];
        if (!task->is_completed) {
            uv_timer_start(&task->timer, task_cb, 0, task->delay_ms);
        }
    }
    
    uv_run(&sched->loop, UV_RUN_DEFAULT);
    
    size_t write_idx = 0;
    for (size_t i = 0; i < sched->count; i++) {
        if (!sched->tasks[i].is_completed) {
            if (write_idx != i) {
                sched->tasks[write_idx] = sched->tasks[i];
            }
            write_idx++;
        }
    }
    sched->count = write_idx;
}

static inline void task1(void* arg, void* sched, int* state, uv_timer_t* timer) {
    (void)sched;
    int* counter = (int*)arg;
    if (*state < 5) {
        printf("Task 1: %d (iteration %d)\n", ++(*counter), *state);
        (*state)++;
    } else {
        ((async_task*)timer->data)->is_completed = true;
    }
}

static inline void task2(void* arg, void* sched, int* state, uv_timer_t* timer) {
    (void)sched;
    int* counter = (int*)arg;
    if (*state < 3) {
        printf("Task 2: %d (iteration %d)\n", ++(*counter), *state);
        (*state)++;
    } else {
        ((async_task*)timer->data)->is_completed = true;
    }
}

static inline void task3(void* arg, void* sched, int* state, uv_timer_t* timer) {
    (void)sched;
    int* counter = (int*)arg;
    if (*state < 3) {
        printf("Task 3: %d (iteration %d)\n", ++(*counter), *state);
        (*state)++;
    } else {
        ((async_task*)timer->data)->is_completed = true;
    }
}

static inline void my_custom_task(void* arg, void* sched, int* state, uv_timer_t* timer) {
    (void)sched;
    char* message = (char*)arg;
    if (*state < 4) {
        printf("Custom Task: %s (iteration %d)\n", message, *state);
        (*state)++;
    } else {
        ((async_task*)timer->data)->is_completed = true;
    }
}

#endif // ASYNCHRONOUS_H