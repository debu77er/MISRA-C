#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

// gcc -o advancedmisrac advancedmisrac.c -lpthread



// Configuration parameters
#define BUFFER_SIZE 1024
#define NUM_PRODUCERS 4
#define NUM_CONSUMERS 4
#define ITEMS_PER_PRODUCER 1000

// Ring buffer structure
typedef struct {
    int *buffer;
    size_t size;        // total size
    size_t start;       // read index
    size_t end;         // write index
    size_t count;       // number of items in buffer
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} ring_buffer_t;

// Initialize ring buffer
int ring_buffer_init(ring_buffer_t *rb, size_t size) {
    rb->buffer = malloc(sizeof(int) * size);
    if (!rb->buffer) {
        perror("Failed to allocate buffer");
        return -1;
    }
    rb->size = size;
    rb->start = 0;
    rb->end = 0;
    rb->count = 0;
    if (pthread_mutex_init(&rb->mutex, NULL) != 0) {
        perror("Mutex init failed");
        free(rb->buffer);
        return -1;
    }
    if (pthread_cond_init(&rb->not_full, NULL) != 0) {
        perror("Cond init failed");
        pthread_mutex_destroy(&rb->mutex);
        free(rb->buffer);
        return -1;
    }
    if (pthread_cond_init(&rb->not_empty, NULL) != 0) {
        perror("Cond init failed");
        pthread_cond_destroy(&rb->not_full);
        pthread_mutex_destroy(&rb->mutex);
        free(rb->buffer);
        return -1;
    }
    return 0;
}

// Destroy ring buffer
void ring_buffer_destroy(ring_buffer_t *rb) {
    free(rb->buffer);
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->not_full);
    pthread_cond_destroy(&rb->not_empty);
}

// Add item to buffer
int ring_buffer_push(ring_buffer_t *rb, int item) {
    pthread_mutex_lock(&rb->mutex);
    while (rb->count == rb->size) {
        // Buffer is full
        if (pthread_cond_wait(&rb->not_full, &rb->mutex) != 0) {
            pthread_mutex_unlock(&rb->mutex);
            return -1;
        }
    }
    rb->buffer[rb->end] = item;
    rb->end = (rb->end + 1) % rb->size;
    rb->count++;
    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->mutex);
    return 0;
}

// Remove item from buffer
int ring_buffer_pop(ring_buffer_t *rb, int *item) {
    pthread_mutex_lock(&rb->mutex);
    while (rb->count == 0) {
        // Buffer is empty
        if (pthread_cond_wait(&rb->not_empty, &rb->mutex) != 0) {
            pthread_mutex_unlock(&rb->mutex);
            return -1;
        }
    }
    *item = rb->buffer[rb->start];
    rb->start = (rb->start + 1) % rb->size;
    rb->count--;
    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->mutex);
    return 0;
}

// Shared data for producers and consumers
typedef struct {
    ring_buffer_t buffer;
    int total_items;
    int produced_count;
    int consumed_count;
    pthread_mutex_t count_mutex;
} shared_data_t;

// Producer thread function
void *producer(void *arg) {
    shared_data_t *data = (shared_data_t *)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = rand();
        if (ring_buffer_push(&data->buffer, item) != 0) {
            fprintf(stderr, "Producer failed to push item");
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&data->count_mutex);
        data->produced_count++;
        pthread_mutex_unlock(&data->count_mutex);
        // Optional: simulate variable workload
        usleep(rand() % 100);
    }
    pthread_exit(NULL);
}

// Consumer thread function
void *consumer(void *arg) {
    shared_data_t *data = (shared_data_t *)arg;
    int item;
    while (1) {
        if (ring_buffer_pop(&data->buffer, &item) != 0) {
            fprintf(stderr, "Consumer failed to pop item");
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&data->count_mutex);
        data->consumed_count++;
        int total_produced = data->produced_count;
        int total_consumed = data->consumed_count;
        pthread_mutex_unlock(&data->count_mutex);
        // Process item (here we just simulate processing)
        // Exit condition: stop after all items are processed
        if (total_consumed >= data->total_items) {
            break;
        }
        // Optional: simulate variable workload
        usleep(rand() % 150);
    }
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    shared_data_t shared;
    shared.total_items = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    shared.produced_count = 0;
    shared.consumed_count = 0;
    if (ring_buffer_init(&shared.buffer, BUFFER_SIZE) != 0) {
        fprintf(stderr, "Failed to initialize ring buffer");
        return EXIT_FAILURE;
    }
    if (pthread_mutex_init(&shared.count_mutex, NULL) != 0) {
        perror("Mutex init failed");
        ring_buffer_destroy(&shared.buffer);
        return EXIT_FAILURE;
    }

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        if (pthread_create(&producers[i], NULL, producer, &shared) != 0) {
            perror("Failed to create producer thread");
            return EXIT_FAILURE;
        }
    }

    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        if (pthread_create(&consumers[i], NULL, consumer, &shared) != 0) {
            perror("Failed to create consumer thread");
            return EXIT_FAILURE;
        }
    }

    // Wait for producers to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    // Wait for consumers to process all items
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("Production complete: %d items produced", shared.produced_count);
    printf("Consumption complete: %d items consumed", shared.consumed_count);

    // Cleanup
    pthread_mutex_destroy(&shared.count_mutex);
    ring_buffer_destroy(&shared.buffer);

    return EXIT_SUCCESS;
}
