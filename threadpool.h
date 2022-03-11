#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include "common.h"
typedef struct {
    void *(*function)(void *);
    void *arg;
} task_t;
class Thread_pool {
private:
    pthread_mutex_t lock;            // Lock the whole object  
    pthread_mutex_t thread_counter;  // 
    pthread_cond_t queue_not_full;
    pthread_cond_t queue_not_empty;

    pthread_t *threads;              // Vector of pids
    pthread_t admin_tid;             
    task_t *task_queue;    
    int min_thr_num;                 // Minimum number of threads
    int max_thr_num;                 // Maximum number of threads
    int live_thr_num;
    int busy_thr_num;
    int wait_exit_thr_num;           // Need to be destroyed

    int queue_front;
    int queue_rear;
    int queue_size;

    int queue_max_size;
    int shutdown;                    // state

    void Thread_pool::Free();
    
public:
    Thread_pool::Thread_pool(int min_thread_num, 
                             int max_thread_num, int max_queue_size);
    Thread_pool::~Thread_pool();
    bool Thread_pool::Is_thread_alive(pthread_t tid);
    int Thread_pool::Add_task(void *(*function)(void *arg), void *arg);

};

void *Admin_thread(void *thread_pool);
void *Work_thread(void *thread_pool);
#endif