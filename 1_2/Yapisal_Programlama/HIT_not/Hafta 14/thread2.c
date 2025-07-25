#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t tid[2];

void *doSomeThing(void *arg)
{
    unsigned long i = 0;
    pthread_t id = pthread_self();

    if (pthread_equal(id, tid[0]))
        printf("\n First thread processing\n");
    else
        printf("\n Second thread processing\n");
    for (i = 0; i < (0xFFFFFFFF); i++)
    {
        printf("%d\n", id);
    };
    return NULL;
}
int main(void)
{
    int i = 0;
    int err;

    while (i < 30)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread: %s", strerror(err));
        else
            printf("\n Thread created successfully\n");
        i++;
    }
    // sleep(5); // this is important!!
    return 0;
}
