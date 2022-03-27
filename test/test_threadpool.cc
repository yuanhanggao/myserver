#include "threadpool.h"

void *aha(void *arg){
    int *i = static_cast<int *>(arg);
    printf("aha function finish, *arg is %d!\n", *i);
    sleep(20);
    return NULL;
}

int main(){
    Thread_pool thread_pool(10, 100, 100);
    int i = 0;
    while(1){
        i++;
        printf("the return is %d\n",thread_pool.Add_task(aha, &i));
        i++;
        printf("the return is %d\n",thread_pool.Add_task(aha, &i));
        i++;
        printf("the return is %d\n",thread_pool.Add_task(aha, &i));
        i++;
        printf("the return is %d\n",thread_pool.Add_task(aha, &i));
        sleep(1);
    }
}
