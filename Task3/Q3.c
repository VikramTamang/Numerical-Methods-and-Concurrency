#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t semaphore;
FILE *filePointer;
int *pointer;
int count = 0;

struct prime
{
    int start;
    int end;
};

// function to check whether the number is prime or not
int checkPrime(int num)
{
    int i;
    if (num == 0 || num == 1)
    {
        return 0;
    }
    for (i = 2; i <= num / 2; i++)
    {
        if (num % i == 0)
        {
            return 0;
        }
    }

    return 1;
}

// function to be executed by the threads
void *threadFunc(void *p)
{
    filePointer = fopen("output.txt", "w");
    int i;
    struct prime *pr = (struct prime *)p;

    // loop to check the prime numbers in the array
    for (i = pr->start; i <= pr->end; i++)
    {
        int check = checkPrime(pointer[i]);
        sem_wait(&semaphore);
        if (check)
        {
            fprintf(filePointer, "%d\n", pointer[i]);
            count++;
        }
        sem_post(&semaphore);
    }
}
void main()
{
    // initializing the variables
    int sum = 0;
    int numThread;
    int chars, j;

    // opening the files and reading the data from the files
    char *f[] = {"PrimeData1.txt", "PrimeData2.txt", "PrimeData3.txt"};
    for (j = 0; j < 3; j++)
    {
        FILE *fp = fopen(f[j], "r");
        while (fscanf(fp, "%d", &chars) != EOF)
        {
            sum++;
        }
        fclose(fp);
    }
    int i = 0;
    sem_init(&semaphore, 0, 1);
    printf("Enter the number of threads : ");
    scanf("%d", &numThread);
    pointer = (int *)malloc(sum * sizeof(int));
    for (j = 0; j < 3; j++)
    {
        FILE *fp = fopen(f[j], "r");
        while (fscanf(fp, "%d", &pointer[i]) != EOF)
        {
            i++;
        }
        fclose(fp);
    }
    // calculating the number of elements to be processed by each thread
    int temp, slice[numThread];
    temp = sum % numThread;
    int startThread[numThread];
    int endThread[numThread];
    for (i = 0; i < numThread; i++)
    {
        slice[i] = sum / numThread;
    }
    for (i = 0; i < temp; i++)
    {
        slice[i] = slice[i] + 1;
    }
    // calculating the start and end index of the array to be processed by each thread
    for (i = 0; i < numThread; i += 1)
    {
        if (i == 0)
        {
            startThread[i] = 0;
        }
        else
        {
            startThread[i] = endThread[i - 1] + 1;
        }
        endThread[i] = startThread[i] + slice[i] - 1;
    }
    // array of structure to store the start and end index of the array to be processed by each thread
    struct prime ptrTh[numThread];

    for (i = 0; i < numThread; i++)
    {
        ptrTh[i].start = startThread[i];
        ptrTh[i].end = endThread[i];
    }

    pthread_t threads[numThread];
    for (i = 0; i < numThread; i++)
    {
        pthread_create(&threads[i], NULL, threadFunc, &ptrTh[i]);
    }
    for (i = 0; i < numThread; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("Total Prime Numbers :  %d", count);
    free(pointer);
    fclose(filePointer);
    sem_destroy(&semaphore);
}