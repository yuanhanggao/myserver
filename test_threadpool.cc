#include "threadpool.h"

void *aha(void *arg){
    printf("aha function finish!\n");
    sleep(20);
    return NULL;
}

int main(){
    Thread_pool thread_pool(10, 100, 100);
    while(1){
        printf("the return is %d\n",thread_pool.Add_task(aha, NULL));
        printf("the return is %d\n",thread_pool.Add_task(aha, NULL));
        printf("the return is %d\n",thread_pool.Add_task(aha, NULL));
        printf("the return is %d\n",thread_pool.Add_task(aha, NULL));
        sleep(1);
    }
}
