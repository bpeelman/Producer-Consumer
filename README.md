# Producer-Consumer
Implementation of the Producer-Consumer Problem detailed in Process Synchonization.

Producer generates items and inputs them into a shared memory table. At most there can be 2 items in the table. Consumer takes the items off the table. 
If table is full, the Producer will wait. If the table is empty, the Consumer waits.

Takes advantage of semaphores to synchonize the two processes with consideration in mutual exclusion.


