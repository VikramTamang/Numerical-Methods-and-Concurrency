#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

// Structure to store range of iteration
typedef struct
{
    int start;
    int end;
    double sum;
} Range;

// Function to calculate Pi
void *calculate_pi(void *arg)
{
    Range *r = (Range *)arg;
    double local_sum = 0;

    for (int i = r->start; i < r->end; i++)
    {
        double term = (i % 2 == 0) ? 1.0 : -1.0;
        local_sum += 4.0 * (term / (2 * i + 1));
    }

    r->sum = local_sum;
    return NULL;
}

int main()
{
    long total;
    int no_of_threads;

    // Get user input for iterations
    printf("Enter the number of iterations: ");
    if (scanf("%ld", &total) != 1 || total <= 0)
    {
        printf("Invalid input. Please enter a positive integer for iterations.\n");
        return 1;
    }

    // Get user input for threads
    printf("Enter the number of threads: ");
    if (scanf("%d", &no_of_threads) != 1 || no_of_threads <= 0)
    {
        printf("Invalid input. Please enter a positive integer for threads.\n");
        return 1;
    }

    pthread_t threads[no_of_threads];
    Range ranges[no_of_threads];
    int slice = total / no_of_threads;
    int remainder = total % no_of_threads;

    // Creating threads
    for (int i = 0; i < no_of_threads; i++)
    {
        ranges[i].start = i * slice;
        ranges[i].end = (i == no_of_threads - 1) ? (i + 1) * slice + remainder : (i + 1) * slice;
        ranges[i].sum = 0; // Initialize sum for each thread
        pthread_create(&threads[i], NULL, calculate_pi, &ranges[i]);
    }

    // Joining threads and summing results
    double pi = 0;
    for (int i = 0; i < no_of_threads; i++)
    {
        pthread_join(threads[i], NULL);
        pi += ranges[i].sum;
    }

    printf("\nEstimated value of pi: %.15lf\n", pi);
    printf("Note: Precision improves with more iterations.\n");

    return 0;
}
