// this program hits a deadlock

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct cookie_jar {
    int cookies;
    int empty; // 0 or 1
} cookie_jar_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
cookie_jar_t* cookiejar;

void* homer(void* arg) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutex);

        while (cookiejar->empty == 1) {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("Homer ate cookie %d\n", cookiejar->cookies);
        cookiejar->empty = 1;
        cookiejar->cookies = -1; // No cookie available
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* marge(void* arg) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutex);

        while (cookiejar->empty == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("Marge baked cookie %d\n", i);
        cookiejar->cookies = i;
        cookiejar->empty = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    cookiejar = (cookie_jar_t*)malloc(sizeof(cookie_jar_t));
    cookiejar->empty = 1;
    cookiejar->cookies = -1; // No initial cookie
    pthread_t homer_tid, marge_tid;
    pthread_create(&homer_tid, NULL, homer, NULL);
    pthread_create(&marge_tid, NULL, marge, NULL);
    pthread_join(homer_tid, NULL);
    pthread_join(marge_tid, NULL);
    free(cookiejar); // Free allocated memory
    return 0;
}
