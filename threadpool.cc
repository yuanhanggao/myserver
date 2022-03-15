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

    task_queue = new Task_t[queue_max_size];
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

    pthread_create(&admin_tid, NULL, Admin, static_cast<void*>(this));
    for (int i = 0; i < min_thr_num; i++){
        pthread_create(&(threads[1]), NULL, 
                       Work, static_cast<void*>(this));
    }
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

int Thread_pool::Add_task(void *(*function)(void *arg), void *arg){
    pthread_mutex_lock(&lock);
    while ((queue_size == queue_max_size) && (!shutdown))
        pthread_cond_wait(&queue_not_full, &lock);
    if (shutdown) {
        pthread_mutex_unlock(&lock);
        return -1;
    }

    if (task_queue[queue_rear].arg != NULL){
        delete task_queue[queue_rear].arg;
        task_queue[queue_rear].arg = NULL;
    }

    task_queue[queue_rear].function = function;
    task_queue[queue_rear].arg = arg;
    queue_rear = (queue_rear + 1) % queue_max_size;
    queue_size++;    
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&lock);
    return 0;
}

void *Thread_pool::Work(void *thread_pool){
    Thread_pool *pool = static_cast<Thread_pool *>(thread_pool);
    pool->Work_run();
    return pool;
}

void *Thread_pool::Admin(void *thread_pool){
    Thread_pool *pool = static_cast<Thread_pool *>(thread_pool);
    pool->Admin_run();
    return pool;
}

void Thread_pool::Work_run(){
    Task_t task;
    while(true) {
        pthread_mutex_lock(&lock);
        while (!queue_size && !shutdown){
            printf("thread 0x%x is waiting \n", 
                   static_cast<unsigned int>(pthread_self()));
            pthread_cond_wait(&queue_not_empty, &lock);

            if (wait_exit_thr_num > 0){
                wait_exit_thr_num--;
                if (live_thr_num > min_thr_num){
                    printf("thread 0x%x is exiting\n",
                           static_cast<unsigned int>(pthread_self()));
                    live_thr_num--;
                    pthread_mutex_unlock(&lock);
                    pthread_exit(NULL);
                }
            }
        }

        if (shutdown){
            pthread_mutex_unlock(&lock);
            printf("thread 0x%x is exiting\n",
                   static_cast<unsigned int>(pthread_self()));
            pthread_exit(NULL);
        }

        task.function = task_queue[queue_front].function; 
        task.arg = task_queue[queue_front].arg;

        queue_front = (queue_front + 1) % queue_max_size;
        queue_size--;

        pthread_cond_broadcast(&queue_not_full);

        pthread_mutex_unlock(&lock);

        printf("thread 0x%x start working\n", 
               static_cast<unsigned int>(pthread_self()));
        pthread_mutex_lock(&thread_counter);
        busy_thr_num++;
        pthread_mutex_unlock(&thread_counter);

        (*(task.function))(task.arg);
        
        printf("thread 0x%x end working\n", 
               static_cast<unsigned int>(pthread_self()));

        pthread_mutex_lock(&thread_counter);
        busy_thr_num--;
        pthread_mutex_unlock(&thread_counter);
    }
    pthread_exit(NULL);
}

void Thread_pool::Admin_run(){
    while (!shutdown){
        printf("admin -------------\n");
        sleep(DEFAULT_TIME); 
        pthread_mutex_lock(&lock);
        int tmp_queue_size = queue_size;
        int tmp_live_thr_num = live_thr_num;
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&thread_counter);
        int tmp_busy_thr_num = busy_thr_num;
        pthread_mutex_unlock(&thread_counter);

        printf("admin busy live -%d--%d-\n", 
                tmp_busy_thr_num, tmp_live_thr_num);
        if (tmp_queue_size >= MIN_WAIT_TASK_NUM &&
            tmp_live_thr_num <= max_thr_num){
                printf("admin add -------------\n");
                pthread_mutex_lock(&lock);
                int add = 0;
                for (int i = 0; i < max_thr_num && add < DEFAULT_THREAD_NUM &&
                    tmp_live_thr_num < max_thr_num; i++){
                    if (threads[i] == 0 || !Is_thread_alive(threads[i])){
                        pthread_create(&(threads[i]), NULL, Work, 
                            static_cast<void *>(this));
                        add++;
                        live_thr_num++;
                        printf("new thread -------------\n");
                    }
                }
                pthread_mutex_unlock(&lock);
        }

        if ((tmp_busy_thr_num * 2) < tmp_live_thr_num &&
            tmp_live_thr_num > min_thr_num){

            pthread_mutex_lock(&lock);
            wait_exit_thr_num = DEFAULT_THREAD_NUM;
            pthread_mutex_unlock(&lock);

            for (int i = 0; i < DEFAULT_THREAD_NUM; i++){
                pthread_cond_signal(&queue_not_empty);
                printf("admin cler --\n");
            }
        }
    }
}
