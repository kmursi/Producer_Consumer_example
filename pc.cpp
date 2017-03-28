#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
/*********************************************************************************************************************/

#define BUF_SIZE 8                                      //buffer size
int buffArray [BUF_SIZE], buff_counter=0;               //create a buffer array and its counter
pthread_cond_t can_produce=PTHREAD_COND_INITIALIZER;    //create and initialize producer condition
pthread_cond_t  can_consume = PTHREAD_COND_INITIALIZER; //create and initialize consumer condition
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      //create and initialize mutex
int c_counter=0;                                        //this variable help to determine when to stop the consumer loop
int p_number, c_number;                                 //declare the number of producers & consumers
int number_of_needed_consuming;

/*********************************************************************************************************************/

void* producer(void* ID) {                              //producer function receive thread ID
   int p_counter=0;                                    //each thread going to have its own producer counter
   while(p_counter<64) {                               //stop when producer produced 64 items starting from 0 to 63
       pthread_mutex_lock(&mutex);                     //lock the critical section
       while(buff_counter == BUF_SIZE) {               //enter the loop and wait till the buffer is not full
           printf("\nProducer %d found the buffer is full and waiting for a consumer to consume!\n",*((int *) ID));
           pthread_cond_wait(&can_produce, &mutex);    //wait till consumer consume an iteme
       }
       int item = rand();                              //generate a random item
       buffArray[buff_counter] = item;                 //store the item in the buffer with the corresponding counter
       buff_counter++;                                 //increase the counter by one
       printf("\nProducer %d produced item: %d.\n" ,*((int *) ID),item,p_counter);
       p_counter++;                                    //increase the producer counter by one
       pthread_cond_signal(&can_consume);              //awake the producer when it is called by the consumer wait condition
       pthread_mutex_unlock(&mutex);                   //unlock the critical section
   }
   printf("\nproducer thread: %d ***********************exit**************************.\n",*((int *) ID));
   pthread_exit(0);                                    //exit the current thread
}

/*********************************************************************************************************************/

void* consumer(void* ID) {                              //consumer function receive thread ID
   while(c_counter++< number_of_needed_consuming) {      //stop when expected number of produced items has been consumed
       pthread_mutex_lock(&mutex);                     //lock the critical section
           while (buff_counter == 0) {                 //enter when buffer is empty
               printf("\nConsumer %d found the buffer is empty and waiting for a producer to produce!\n",*((int *) ID));
               pthread_cond_wait(&can_consume, &mutex);//wait till producer produce an item
           }
           buff_counter--;                             //reduce the buffer counter that has been increased while producing
           printf("\nConsumer %d consumed item: %d.\n", *((int *) ID), buffArray[buff_counter]);
           pthread_cond_signal(&can_produce);          //awake the consumer when producer is waiting
           pthread_mutex_unlock(&mutex);               //unlock the critical section
   }
   printf("\nconsumer thread: %d ***********************exit**************************.\n",*((int *) ID));
   pthread_exit(0);                                    //exit the current thread
}

/*********************************************************************************************************************/
/* this method check the validity of the user input when entering the producer & consumer numbers */
int userInputChecker(std::string type)
{
   int myNumber = 0,counter=0;
   std::string input = "";
   while (true) {
       if(type=="producers")
           printf("\nEnter the producers number:\n");
       if(type=="consumers")
           printf("\nEnter the consumers number:\n");
       getline(std::cin, input);
       std::stringstream myStream(input);                              // This code converts from string to number safely.
       if (myStream >> myNumber && !(myNumber<=0)&& !(myNumber>500))   // the input should be  0 < int < 500
           return myNumber;                                            // return the valid input
       std::cout << "\nInvalid number, please try again\n" << std::endl;
       if(counter++==2)                                                // check number of trials which should be three
       {
           printf("\nYou exceeded the limit of the trials! Goodbye..\n");
           exit(0);                                                    // exit the program after three trials
       }
   }
}

/*********************************************************************************************************************/

int main(int argc, char *argv[]) {

   p_number= userInputChecker("producers");
   c_number= userInputChecker("consumers");
//-------------------------------------------------------------------------------------------------//
   pthread_t prod[p_number];                   //create producers array
   pthread_t cons[c_number];                   //create consumers array
   number_of_needed_consuming=p_number*64;     //calculate the number of needed consuming to limitize the consumers' loop
   int pID[p_number], cID[c_number];           //create producers & consumers arrays to hold their IDs
//-------------------------------------------------------------------------------------------------//
   for(int i=0;i<p_number;i++)
   {
       pID[i]=i;                               //store the thread id into pID array
       pthread_create(&prod[i], NULL, producer, &pID[i]); //create thread, call producer function, and pass thread's id
       printf("\nProducer thread number %d has been created\n", i);
   }
   for(int i=0;i<c_number;i++)
   {
       cID[i]=i;                               //store the thread id into cID array
       pthread_create(&cons[i], NULL, consumer, &cID[i]); //create thread, call consumer function, and pass thread's id
       printf("\nConsumer thread number %d has been created\n", i);
   }
   for(int i=0;i<p_number;i++)
   {
       pthread_join(prod[i], NULL); //joining the producers thread with the main thread
   }
   for(int i=0;i<c_number;i++)
   {
       pthread_join(cons[i], NULL); //joining the producers thread with the main thread
   }
//-------------------------------------------------------------------------------------------------//
   pthread_cond_destroy(&can_produce); //destroy the condition can_produce
   pthread_cond_destroy(&can_consume); //destroy the condition can_produce
   pthread_mutex_destroy(&mutex);      //destroy the mutex
   pthread_exit(0);                    // exit the main thread
}

/*********************************************************************************************************************/



