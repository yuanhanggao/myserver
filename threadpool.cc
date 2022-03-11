#include "threadpool.h"

Thread_pool::Thread_pool(int min_thread_num, 
                         int max_thread_num, int max_queue_size){
    min_thr_num = min_thread_num;
    max_thr_num = max_thread_num;
    busy_thr_num = 0;
    live_thr_num = min_thread_num;
    wait_exit_thr_num = 0;
    queue_front = 0;
    queue_rear = 0;
    queue_size = 0;
    queue_max_size = max_queue_size;
    shutdown = false;

    threads = new pthread_t[max_thr_num]();
    if (threads == NULL){
        printf("new threads false!\n");
        exit(1);
    }

    task_queue = new task_t[queue_max_size];
    if (task_queue == NULL){
        printf("new task_queue false!\n");
        exit(1);
    }

    if (pthread_mutex_init(&lock, NULL) != 0 ||
        pthread_mutex_init(&thread_counter, NULL) != 0 ||
        pthread_cond_init(&queue_not_empty, NULL) != 0 ||
        pthread_cond_init(&queue_not_full, NULL) != 0) {
        printf("init lock or cond false, due to %d\n", errno);
        exit(1);
    }

    for (int i = 0; i < min_thr_num; i++){
        pthread_create(&(threads[1]), NULL, 
                       Work_thread, static_cast<void*>(this));
        printf("start thread 0x%x...\n", 
               static_cast<unsigned int>(threads[i]));
    }
    pthread_create(&admin_tid, NULL, Admin_thread, static_cast<void*>(this));
}

Thread_pool::~Thread_pool(){
    shutdown = true;
    pthread_join(admin_tid, NULL);

    for (int i = 0; i< live_thr_num; i++){
        pthread_cond_broadcast(&(queue_not_empty));
    }

    for (int i = 0; i < live_thr_num; i++){
        pthread_join(threads[i], NULL);
    }

    Free();

}

void Thread_pool::Free(){
    if (task_queue)
        delete [] task_queue;
    if (threads) {
        delete [] threads; 
        pthread_mutex_lock(&lock);
        pthread_mutex_destroy(&lock);   //why 
        pthread_mutex_lock(&thread_counter);
        pthread_mutex_destroy(&thread_counter);
        pthread_cond_destroy(&queue_not_empty);
        pthread_cond_destroy(&queue_not_full);
    }
}

bool Thread_pool::Is_thread_alive(pthread_t tid){
    int kill_rc = pthread_kill(tid, 0);
    if (kill_rc == ESRCH)
        return false;
    return true;
}

void *Work_thread(Thread_pool &thread_pool){

}