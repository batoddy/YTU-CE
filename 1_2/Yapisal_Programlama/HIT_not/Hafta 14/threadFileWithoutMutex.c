#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Global variables
int sharedValue = 0;

// Function to be executed by the threads
void *updateFile(void *threadID) {
    long tid = (long)threadID;
    // Read the current value from the file
    FILE *file = fopen("shared_file.txt", "r+");
    if (file == NULL) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    fscanf(file, "%d", &sharedValue);

    // Increment the shared value
    sharedValue++;

    // Write the updated value back to the file
    rewind(file);
    fprintf(file, "%d", sharedValue);

    fclose(file);

    printf("Thread #%ld: Incremented value: %d\n", tid, sharedValue);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;
    int result;


    // Create thread 1
    result = pthread_create(&thread1, NULL, updateFile, (void *)1);
    if (result) {
        fprintf(stderr, "Error - pthread_create() return code: %d\n", result);
        exit(EXIT_FAILURE);
    }

    // Create thread 2
    result = pthread_create(&thread2, NULL, updateFile, (void *)2);
    if (result) {
        fprintf(stderr, "Error - pthread_create() return code: %d\n", result);
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main thread: Final value after all updates: %d\n", sharedValue);

    // Destroy the mutex
  //  pthread_mutex_destroy(&mutex);

    return 0;
}
