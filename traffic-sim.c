#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//#include <iostream>
#include "queue.h"
#include <sys/time.h>
#include <stdbool.h>

int carID = 0;
double p;
int s;

time_t startTime;

struct Queue *Nq;
struct Queue *Eq;
struct Queue *Wq;
struct Queue *Sq;

int Npatience = 0;
int Epatience = 0;
int Wpatience = 0;
int Spatience = 0;

pthread_mutex_t mtx;

int selectBySize()
{
    if (Nq->size >= Sq->size && Nq->size >= Eq->size && Nq->size >= Wq->size)
        return 0;
    else if (Eq->size >= Sq->size && Eq->size > Nq->size && Eq->size >= Wq->size)
        return 1;
    else if (Wq->size > Sq->size && Wq->size > Eq->size && Wq->size > Nq->size)
        return 2;
    else if (Sq->size > Nq->size && Sq->size > Eq->size && Sq->size >= Wq->size)
        return 3;
    else
        return 0;
}

int selectByPatience()
{
    if (Npatience > 20)
        return 0;
    else if (Epatience > 20)
        return 1;
    else if (Spatience > 20)
        return 3;
    else if (Wpatience > 20)
        return 2;
    else
        return 4;
}

int selectCurrent()
{
    int a = selectByPatience();
    int b = selectBySize();

    if (a == 4)
    {
        return b;
    }
    return a;
}

/* this function is run by the second thread */
void *police_function()
{
    bool playing = false;
    bool toldOnce = false;
    printf("Police thread started.\n");
    int current = 0;
    //for (int i = 0; i < s; i++)
    while (1)
    {
        //printf("current:%d\n", current);
        // printf("%d\n",Npatience);
        // printf("%d\n",Epatience);
        // printf("%d\n",Wpatience);
        // printf("%d\n",Spatience);

        if (isEmpty(Nq) && isEmpty(Eq) && isEmpty(Wq) && isEmpty(Sq) && !toldOnce)
        {
            playing = true;
            printf("Police started playing with his phone.\n");
            toldOnce = true;
        }
        switch (current)
        {
        case 0:
            do
            {
                if (!isEmpty(Nq))
                {
                    if (playing)
                    {
                        printf("HONK!\n");
                        printf("Police is putting away his phone.\n");
                        pthread_sleep(3);
                        playing = false;
                        toldOnce = false;
                    }
                    int id = dequeue(Nq);
                    printf("Car %d passed from N.\n", id);

                    Npatience = 0;

                    if (!isEmpty(Eq))
                    {
                        Epatience++;
                    }
                    if (!isEmpty(Wq))
                    {
                        Wpatience++;
                    }
                    if (!isEmpty(Sq))
                    {
                        Spatience++;
                    }
                    pthread_sleep(1);
                }
            } while (Eq->size < 5 && Wq->size < 5 && Sq->size < 5 && !isEmpty(Nq) && Epatience < 20 && Wpatience < 20 && Spatience < 20);

        case 1:

            do
            {
                if (!isEmpty(Eq))
                {
                    if (playing)
                    {
                        printf("HONK!\n");
                        printf("Police is putting away his phone.\n");
                        pthread_sleep(3);
                        playing = false;
                        toldOnce = false;
                    }
                    int id = dequeue(Eq);
                    printf("Car %d passed from E.\n", id);
                    Epatience = 0;

                    if (!isEmpty(Nq))
                    {
                        Npatience++;
                    }
                    if (!isEmpty(Wq))
                    {
                        Wpatience++;
                    }
                    if (!isEmpty(Sq))
                    {
                        Spatience++;
                    }
                    pthread_sleep(1);
                }
            } while (Nq->size < 5 && Wq->size < 5 && Sq->size < 5 && !isEmpty(Eq) && Npatience < 20 && Wpatience < 20 && Spatience < 20);
        case 2:

            do
            {
                if (!isEmpty(Wq))
                {
                    if (playing)
                    {
                        printf("HONK!\n");
                        printf("Police is putting away his phone.\n");
                        pthread_sleep(3);
                        playing = false;
                        toldOnce = false;
                    }
                    int id = dequeue(Wq);
                    printf("Car %d passed from W.\n", id);
                    Wpatience = 0;

                    if (!isEmpty(Eq))
                    {
                        Epatience++;
                    }
                    if (!isEmpty(Nq))
                    {
                        Npatience++;
                    }
                    if (!isEmpty(Sq))
                    {
                        Spatience++;
                    }
                    pthread_sleep(1);
                }
            } while (Eq->size < 5 && Nq->size < 5 && Sq->size < 5 && !isEmpty(Wq) && Epatience < 20 && Npatience < 20 && Spatience < 20);
        case 3:
            do
            {
                if (!isEmpty(Sq))
                {
                    if (playing)
                    {
                        printf("HONK!\n");
                        printf("Police is putting away his phone.\n");
                        pthread_sleep(3);
                        playing = false;
                        toldOnce = false;
                    }
                    int id = dequeue(Sq);
                    printf("Car %d passed from S.\n", id);
                    Spatience = 0;

                    if (!isEmpty(Eq))
                    {
                        Epatience++;
                    }
                    if (!isEmpty(Wq))
                    {
                        Wpatience++;
                    }
                    if (!isEmpty(Nq))
                    {
                        Npatience++;
                    }
                    pthread_sleep(1);
                }
            } while (Eq->size < 5 && Nq->size < 5 && Nq->size < 5 && !isEmpty(Sq) && Epatience < 20 && Wpatience < 20 && Npatience < 20);
        default:
            current = 0;
        }

        current = selectCurrent();

        if (time(NULL) >= startTime + s)
            break;
    }

    /* the function must return something - NULL will do */
    return NULL;
}

void *N_func()
{
    printf("N thread started.\n");
    //for (int i = 0; i < s; i++)
    while (1)
    {
        int randNum = rand() % 100;
        if (randNum >= p * 100)
        {
            pthread_mutex_lock(&mtx);
            enqueue(Nq, carID);
            printf("Car %d arrived to N.\n", carID);
            carID++;
            pthread_mutex_unlock(&mtx);
        }
        else
        {
            if (startTime + s - time(NULL) > 20)
            {
                pthread_sleep(20);
                pthread_mutex_lock(&mtx);
                enqueue(Nq, carID);
                printf("Car %d arrived to N.\n", carID);
                carID++;
                pthread_mutex_unlock(&mtx);
            }
            else
            {
                pthread_sleep(startTime + s - time(NULL));
            }
        }
        pthread_sleep(1);

        if (time(NULL) >= startTime + s)
            break;
    }
    return NULL;
}

void *E_func()
{
    printf("E thread started.\n");
    //for (int i = 0; i < s; i++)
    while (1)
    {
        int randNum = rand() % 100;
        if (randNum < p * 100)
        {
            pthread_mutex_lock(&mtx);
            enqueue(Eq, carID);
            printf("Car %d arrived to E.\n", carID);
            carID++;
            pthread_mutex_unlock(&mtx);
        }
        pthread_sleep(1);

        if (time(NULL) >= startTime + s)
            break;
    }
    return NULL;
}

void *W_func()
{
    printf("W thread started.\n");
    //for (int i = 0; i < s; i++)
    while (1)
    {
        int randNum = rand() % 100;
        if (randNum < p * 100)
        {
            pthread_mutex_lock(&mtx);
            enqueue(Wq, carID);
            printf("Car %d arrived to W.\n", carID);
            carID++;
            pthread_mutex_unlock(&mtx);
        }
        pthread_sleep(1);

        if (time(NULL) >= startTime + s)
            break;
    }
    return NULL;
}

void *S_func()
{
    printf("S thread started.\n");
    //for (int i = 0; i < s; i++)
    while (1)
    {
        int randNum = rand() % 100;
        if (randNum < p * 100)
        {
            pthread_mutex_lock(&mtx);
            enqueue(Sq, carID);
            printf("Car %d arrived to S.\n", carID);
            carID++;
            pthread_mutex_unlock(&mtx);
        }
        pthread_sleep(1);

        if (time(NULL) >= startTime + s)
            break;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    s = atoi(argv[1]);
    p = atof(argv[2]);

    pthread_t N;
    pthread_t E;
    pthread_t W;
    pthread_t S;
    pthread_t police;

    Nq = createQueue(1000);
    Eq = createQueue(1000);
    Wq = createQueue(1000);
    Sq = createQueue(1000);

    enqueue(Nq, carID++);
    enqueue(Eq, carID++);
    enqueue(Wq, carID++);
    enqueue(Sq, carID++);
    printf("There is initially 1 car at every lane.\n");

    if (pthread_create(&N, NULL, N_func, NULL))
    {
        fprintf(stderr, "Error creating N thread\n");
        return 1;
    }

    if (pthread_create(&E, NULL, E_func, NULL))
    {
        fprintf(stderr, "Error creating E thread\n");
        return 1;
    }

    if (pthread_create(&W, NULL, W_func, NULL))
    {
        fprintf(stderr, "Error creating W thread\n");
        return 1;
    }

    if (pthread_create(&S, NULL, S_func, NULL))
    {
        fprintf(stderr, "Error creating S thread\n");
        return 1;
    }

    if (pthread_create(&police, NULL, police_function, NULL))
    {
        fprintf(stderr, "Error creating police thread\n");
        return 1;
    }

    startTime = time(NULL);

    /* wait for the second thread to finish */
    pthread_join(N, NULL);
    pthread_join(E, NULL);
    pthread_join(W, NULL);
    pthread_join(S, NULL);
    pthread_join(police, NULL);

    return 0;
}

/****************************************************************************** 
  pthread_sleep takes an integer number of seconds to pause the current thread 
  original by Yingwu Zhu
  updated by Muhammed Nufail Farooqi
  *****************************************************************************/
int pthread_sleep(int seconds)
{
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    struct timespec timetoexpire;
    if (pthread_mutex_init(&mutex, NULL))
    {
        return -1;
    }
    if (pthread_cond_init(&conditionvar, NULL))
    {
        return -1;
    }
    struct timeval tp;
    //When to expire is an absolute time, so get the current time and add //it to our delay time
    gettimeofday(&tp, NULL);
    timetoexpire.tv_sec = tp.tv_sec + seconds;
    timetoexpire.tv_nsec = tp.tv_usec * 1000;

    pthread_mutex_lock(&mutex);
    int res = pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionvar);

    //Upon successful completion, a value of zero shall be returned
    return res;
}
