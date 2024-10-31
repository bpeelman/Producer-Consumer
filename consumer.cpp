// Brody Peelman
// 10/29/2024
// consumer.cpp
// Consumer Process
// Takes items off table and operates
// Waits upon Producer if table is empty

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <thread>


int main() {

    // Wait a sec for Producer to initialize
    sleep(1);

    int sharedMemory = shm_open("memoryToShare", O_RDWR, 0666);
    if (sharedMemory < 0) {
        perror("shm_open failed");
        return 1;
    }

    int *table = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory, 0);
    if (table == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    *table = 0;

    // Read Lock Semaphore
    sem_t  *lock = sem_open("lock", O_RDWR);
    if (lock == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    int producer_consumer_count = 0;
    while (producer_consumer_count < 8) {

        // Waits to request lock
        sleep(rand() % 2 +2);
        sem_wait(lock);
        sleep(4);


        if (*table > 0) {

            // Randomness aspect to respect Consumer emptying Table
            int randCons = rand() % 2;
            if (randCons || *table == 1) {
                --(*table);
                std::cout << "Consumer took item[" << producer_consumer_count+1 << "]." << std::endl;
                ++producer_consumer_count;
            } else {
                (*table) -= 2;
                std::cout << "Consumer took item[" << producer_consumer_count+1 << "]." << std::endl;
                std::cout << "Consumer took item[" << producer_consumer_count+2 << "]." << std::endl;
                producer_consumer_count += 2;
            }
            
        }
        if (!*table) std::cout << "Table is empty." << std::endl;
        std::cout << std::endl << std::endl;


        // Releases lock for Producer and waits
        sem_post(lock);
        sleep(rand() % 2 +2);
    }

    // Cleanup
    sem_close(lock);
    munmap(table, sizeof(int));
    close(sharedMemory);
    std::cout << "Consumer has finished execution." << std::endl;
    std::cout << std::endl << std::endl;
    return 0;

}