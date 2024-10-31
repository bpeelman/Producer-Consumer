// Brody Peelman
// 10/29/2024
// producer.cpp
// Producer Process
// Generates items and puts them into table
// Waits upon Consumer if table is full (2 items)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>



int main() {

    // Cleanup from previous
    // Just incase
    sem_unlink("lock"); 
    shm_unlink("memoryToShare");

    int sharedMemory = shm_open("memoryToShare", O_CREAT | O_RDWR, 0666);
    if (sharedMemory < 0) {
        perror("shm_open failed");
        exit(1);
    }

    ftruncate(sharedMemory, sizeof(int));

    int *table = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory, 0);
    if (table == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // Initialize Table
    *table = 0;

    // Initialize Lock Semaphore
    sem_t  *lock = sem_open("lock", O_CREAT, 0666, 1);
    if (lock == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    // Wait for Consumer setup
    sleep(2);

    std::cout << std::endl;

    int producer_consumer_count = 0;
    while (producer_consumer_count < 8) {

        // Requests lock and waits
        sem_wait(lock);
        sleep(4);

        if (*table < 2) {
            // Randomness aspect to respect Producer filling Table
            int randProd = rand() % 2;
            if (randProd || *table == 1) {
                ++(*table);
                std::cout << "Producer generated item[" << producer_consumer_count+1 << "]." << std::endl;
                ++producer_consumer_count;
            } else {
                (*table) += 2;
                std::cout << "Producer generated item[" << producer_consumer_count+1 << "]." << std::endl;
                std::cout << "Producer generated item[" << producer_consumer_count+2 << "]." << std::endl;
                producer_consumer_count += 2;
            }
            
        } 

        if (*table == 2) std::cout << "Table is full." << std::endl;
        std::cout << std::endl << std::endl;

        // Releases lock for Consumer and waits
        sem_post(lock);
        sleep(rand() % 2 +2);
    }

    // Cleanup
    sem_close(lock);
    sem_unlink("lock");
    munmap(table, sizeof(int));
    close(sharedMemory);
    shm_unlink("memoryToShare");
    std::cout << "Producer has finished execution." << std::endl;
    std::cout << std::endl << std::endl;
    return 0;

}

